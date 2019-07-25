load("@bazel_tools//tools/cpp:lib_cc_configure.bzl", "get_cpu_value")
load(
    "@bazel_tools//tools/build_defs/repo:utils.bzl",
    "patch",
    "update_attrs",
    "workspace_and_buildfile",
)
load("//bazel:felicia_util.bzl", "norm_path")

def failed_to_find_bin_path(repository_ctx, bin, env_var = None):
    fmt_str = "Cannot find %s in PATH, please make sure %s is installed and add its directory in PATH"
    if env_var != None:
        return fmt_str + ", or --define %s='/path/to/%s'.\nPATH=%s" % (bin, bin, bin, env_var, repository_ctx.os.environ("PATH"))
    else:
        return fmt_str % (bin, bin)

def failed_to_find_bash_bin_path(repository_ctx):
    return failed_to_find_bin_path(repository_ctx, "bash", "BAZEL_SH")

def get_bin_path(repository_ctx, bin, env_var = None):
    bin_path = None
    if env_var != None:
        bin_path = repository_ctx.os.environ.get(env_var)
    if bin_path == None:
        bin_path = repository_ctx.which(bin)
    return bin_path

def get_bash_bin_path(repository_ctx):
    """Gets the bash bin path."""
    return get_bin_path(repository_ctx, "bash", "BAZEL_SH")

def is_executable(repository_ctx, bin, env_var = None):
    """Check bin whether exists and is executable."""
    bin_path = get_bin_path(repository_ctx, bin, env_var)
    if bin_path == None:
        return False
    cmd = "test -x \"%s\"" % bin_path
    bash_bin = get_bash_bin_path(repository_ctx)
    if bash_bin == None:
        return False
    result = repository_ctx.execute([bash_bin, "-c", cmd])
    return result.return_code == 0

def is_windows(repository_ctx):
    """Returns true if the host operating system is windows."""
    os_name = repository_ctx.os.name.lower()
    if os_name.find("windows") != -1:
        return True
    return False

def is_linux(repository_ctx):
    """Returns true if the host operating system is linux."""
    os_name = repository_ctx.os.name.lower()
    if os_name.find("linux") != -1:
        return True
    return False

def is_unix(repository_ctx):
    """Returns true if the host operating system is unix."""
    return not is_windows(repository_ctx)

def execute(
        repository_ctx,
        cmdline,
        error_msg = None,
        error_details = None,
        empty_stdout_fine = False):
    """Executes an arbitrary shell command.

    Args:
      repository_ctx: the repository_ctx object
      cmdline: list of strings, the command to execute
      error_msg: string, a summary of the error if the command fails
      error_details: string, details about the error or steps to fix it
      empty_stdout_fine: bool, if True, an empty stdout result is fine, otherwise
        it's an error
    Return:
      the result of repository_ctx.execute(cmdline)
    """
    result = repository_ctx.execute(cmdline)
    if result.stderr or not (empty_stdout_fine or result.stdout):
        fail("\n".join([
            error_msg.strip() if error_msg else "Repository command failed",
            result.stderr.strip(),
            error_details if error_details else "",
        ]))
    return result

def _read_dir(repository_ctx, src_dir):
    """Returns a string with all files in a directory.

    Finds all files inside a directory, traversing subfolders and following
    symlinks. The returned string contains the full path of all files
    separated by line breaks.
    """
    if is_windows(repository_ctx):
        src_dir = src_dir.replace("/", "\\")
        find_result = execute(
            repository_ctx,
            ["cmd.exe", "/c", "dir", src_dir, "/b", "/s", "/a-d"],
            empty_stdout_fine = True,
        )

        # src_files will be used in genrule.outs where the paths must
        # use forward slashes.
        result = find_result.stdout.replace("\\", "/")
    else:
        find_result = execute(
            repository_ctx,
            ["find", src_dir, "-follow", "-type", "f"],
            empty_stdout_fine = True,
        )
        result = find_result.stdout
    return result

def _genrule(src_dir, genrule_name, command, outs):
    """Returns a string with a genrule.

    Genrule executes the given command and produces the given outputs.
    """
    return (
        "genrule(\n" +
        '    name = "' +
        genrule_name + '",\n' +
        "    outs = [\n" +
        outs +
        "\n    ],\n" +
        '    cmd = """\n' +
        command +
        '\n   """,\n' +
        ")\n"
    )

def symlink_genrule_for_dir(
        repository_ctx,
        src_dir,
        dest_dir,
        genrule_name,
        src_files = [],
        dest_files = []):
    """Returns a genrule to symlink(or copy if on Windows) a set of files.

    If src_dir is passed, files will be read from the given directory; otherwise
    we assume files are in src_files and dest_files
    """
    if src_dir != None:
        src_dir = norm_path(src_dir)
        dest_dir = norm_path(dest_dir)
        files = "\n".join(sorted(_read_dir(repository_ctx, src_dir).splitlines()))

        # Create a list with the src_dir stripped to use for outputs.
        dest_files = files.replace(src_dir, "").splitlines()
        src_files = files.splitlines()
    command = []
    outs = []
    for i in range(len(dest_files)):
        if dest_files[i] != "":
            # If we have only one file to link we do not want to use the dest_dir, as
            # $(@D) will include the full path to the file.
            dest = "$(@D)/" + dest_dir + dest_files[i] if len(dest_files) != 1 else "$(@D)/" + dest_files[i]

            # Copy the headers to create a sandboxable setup.
            cmd = "cp -f"
            command.append(cmd + ' "%s" "%s"' % (src_files[i], dest))
            outs.append('        "' + dest_dir + dest_files[i] + '",')
    genrule = _genrule(
        src_dir,
        genrule_name,
        " && ".join(command),
        "\n".join(outs),
    )
    return genrule

_http_archive_attrs = {
    "url_fmt": attr.string(mandatory = True),
    "fmt_dict": attr.string_list_dict(mandatory = True),
    "sha256": attr.string(),
    "strip_prefix_dict": attr.string_dict(),
    "type": attr.string(),
    "build_file": attr.label(allow_single_file = True),
    "build_file_content": attr.string(),
    "patches": attr.label_list(default = []),
    "patch_tool": attr.string(default = "patch"),
    "patch_args": attr.string_list(default = ["-p0"]),
    "patch_cmds": attr.string_list(default = []),
    "workspace_file": attr.label(allow_single_file = True),
    "workspace_file_content": attr.string(),
}

# Please refer to https://github.com/bazelbuild/bazel/blob/0.24.0/tools/build_defs/repo/http.bzl
def _http_archive_per_os_impl(repository_ctx):
    if repository_ctx.attr.build_file and repository_ctx.attr.build_file_content:
        fail("Only one of build_file and build_file_content can be provided.")

    cpu_value = get_cpu_value(repository_ctx)
    if cpu_value not in repository_ctx.attr.fmt_dict:
        fail("You should add fmt_dict for %s" % cpu_value)
    url = repository_ctx.attr.url_fmt % tuple(repository_ctx.attr.fmt_dict[cpu_value])

    if cpu_value not in repository_ctx.attr.strip_prefix_dict:
        fail("You should add strip_prefix_dict for %s" % cpu_value)
    strip_prefix = repository_ctx.attr.strip_prefix_dict[cpu_value]

    download_info = repository_ctx.download_and_extract(
        url,
        "",
        repository_ctx.attr.sha256,
        repository_ctx.attr.type,
        strip_prefix,
    )
    patch(repository_ctx)
    workspace_and_buildfile(repository_ctx)

    return update_attrs(repository_ctx.attr, _http_archive_attrs.keys(), {"sha256": download_info.sha256})

http_archive_per_os = repository_rule(
    implementation = _http_archive_per_os_impl,
    attrs = _http_archive_attrs,
)
