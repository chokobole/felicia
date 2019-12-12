# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("@bazel_tools//tools/cpp:lib_cc_configure.bzl", "get_cpu_value")
load(
    "@bazel_tools//tools/build_defs/repo:utils.bzl",
    "patch",
    "update_attrs",
    "workspace_and_buildfile",
)
load("//bazel:platform.bzl", "is_windows")
load("//bazel:util.bzl", "norm_path")

def failed_to_find_bin_path(repository_ctx, bin, env_var = None):
    fmt_str = "Cannot find %s in PATH, please make sure %s is installed and add its directory in PATH"
    if env_var != None:
        return fmt_str + ", or --define %s='/path/to/%s'.\nPATH=%s" % (bin, bin, bin, env_var, repository_ctx.os.environ["PATH"])
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

def failed_to_get_cflags_and_libs(pkgs):
    return "Failed to get cflags and libs for [%s]" % ", ".join(pkgs)

def _pkg_config_query(repository_ctx, cmd):
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        return None
    ret = []
    for f in result.stdout[:-1].split(" "):
        ret.append("\"%s\"" % f)
    return ret

def _append_to_set(s, element):
    has_element = False
    for e in s:
        if e == element:
            has_element = True
            break
    if has_element == False:
        s.append(element)

def _extend_to_set(s, element):
    for e in element:
        _append_to_set(s, e)

def pkg_config_cflags_and_libs(repository_ctx, pkgs):
    if not is_executable(repository_ctx, "pkg-config"):
        return None
    pkg_config_bin_path = get_bin_path(repository_ctx, "pkg-config")
    cflags = []
    libs = []
    for pkg in pkgs:
        cmd = [pkg_config_bin_path, "--cflags", pkg]
        result = _pkg_config_query(repository_ctx, cmd)
        if result == None:
            return None
        _extend_to_set(cflags, result)
        cmd = [pkg_config_bin_path, "--libs", pkg]
        result = _pkg_config_query(repository_ctx, cmd)
        if result == None:
            return None
        _extend_to_set(libs, result)
    return struct(cflags = cflags, libs = libs)

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

def symlink_dir(repository_ctx, src_dir, dest_dir, src_files = []):
    files = repository_ctx.path(src_dir).readdir()
    for src_file in files:
        if len(src_files) == 0 or src_file.basename in src_files:
            repository_ctx.symlink(src_file, dest_dir + "/" + src_file.basename)
