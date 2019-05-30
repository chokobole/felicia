"""Repository rule for Python autoconfiguration.

`python_configure` depends on the following environment variables:

  * `PYTHON2_BIN_PATH`: location of python2 binary.
  * `PYTHON_BIN_PATH`: location of python binary.
  * `PYTHON_LIB_PATH`: location of python libraries.
"""

load(
    "//third_party:util.bzl",
    "execute",
    "is_windows",
    "norm_path",
    "red",
    "symlink_genrule_for_dir",
)

_BAZEL_SH = "BAZEL_SH"
_PYTHON2_BIN_PATH = "PYTHON2_BIN_PATH"
_PYTHON_BIN_PATH = "PYTHON_BIN_PATH"
_PYTHON_LIB_PATH = "PYTHON_LIB_PATH"
_TF_PYTHON_CONFIG_REPO = "TF_PYTHON_CONFIG_REPO"

def _tpl(repository_ctx, tpl, substitutions = {}, out = None):
    if not out:
        out = tpl
    repository_ctx.template(
        out,
        Label("//third_party/py:%s.tpl" % tpl),
        substitutions,
    )

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("Python Confiugation Error:"), msg))

def _get_python2_bin(repository_ctx):
    """Gets the python2 bin path."""
    python2_bin = repository_ctx.os.environ.get(_PYTHON2_BIN_PATH)
    if python2_bin != None:
        return python2_bin
    python2_bin_path = repository_ctx.which("python2")
    if python2_bin_path != None:
        return str(python2_bin_path)
    _fail("Cannot find python2 in PATH, please make sure " +
          "python2 is installed and add its directory in PATH, or --define " +
          "%s='/something/else'.\nPATH=%s" % (
              _PYTHON2_BIN_PATH,
              repository_ctx.os.environ.get("PATH", ""),
          ))

def _get_python_bin(repository_ctx):
    """Gets the python bin path."""
    python_bin = repository_ctx.os.environ.get(_PYTHON_BIN_PATH)
    if python_bin != None:
        return python_bin
    python_bin_path = repository_ctx.which("python")
    if python_bin_path != None:
        return str(python_bin_path)
    _fail("Cannot find python in PATH, please make sure " +
          "python is installed and add its directory in PATH, or --define " +
          "%s='/something/else'.\nPATH=%s" % (
              _PYTHON_BIN_PATH,
              repository_ctx.os.environ.get("PATH", ""),
          ))

def _get_bash_bin(repository_ctx):
    """Gets the bash bin path."""
    bash_bin = repository_ctx.os.environ.get(_BAZEL_SH)
    if bash_bin != None:
        return bash_bin
    else:
        bash_bin_path = repository_ctx.which("bash")
        if bash_bin_path != None:
            return str(bash_bin_path)
        else:
            _fail("Cannot find bash in PATH, please make sure " +
                  "bash is installed and add its directory in PATH, or --define " +
                  "%s='/path/to/bash'.\nPATH=%s" % (
                      _BAZEL_SH,
                      repository_ctx.os.environ.get("PATH", ""),
                  ))

def _get_python_lib(repository_ctx, python_bin):
    """Gets the python lib path."""
    python_lib = repository_ctx.os.environ.get(_PYTHON_LIB_PATH)
    if python_lib != None:
        return python_lib
    print_lib = ("<<END\n" +
                 "from __future__ import print_function\n" +
                 "import site\n" +
                 "import os\n" +
                 "\n" +
                 "try:\n" +
                 "  input = raw_input\n" +
                 "except NameError:\n" +
                 "  pass\n" +
                 "\n" +
                 "python_paths = []\n" +
                 "if os.getenv('PYTHONPATH') is not None:\n" +
                 "  python_paths = os.getenv('PYTHONPATH').split(':')\n" +
                 "try:\n" +
                 "  library_paths = site.getsitepackages()\n" +
                 "except AttributeError:\n" +
                 " from distutils.sysconfig import get_python_lib\n" +
                 " library_paths = [get_python_lib()]\n" +
                 "all_paths = set(python_paths + library_paths)\n" +
                 "paths = []\n" +
                 "for path in all_paths:\n" +
                 "  if os.path.isdir(path):\n" +
                 "    paths.append(path)\n" +
                 "if len(paths) >=1:\n" +
                 "  print(paths[0])\n" +
                 "END")
    cmd = "%s - %s" % (python_bin, print_lib)
    result = repository_ctx.execute([_get_bash_bin(repository_ctx), "-c", cmd])
    return result.stdout.strip("\n")

def _check_python_lib(repository_ctx, python_lib):
    """Checks the python lib path."""
    cmd = 'test -d "%s" -a -x "%s"' % (python_lib, python_lib)
    result = repository_ctx.execute([_get_bash_bin(repository_ctx), "-c", cmd])
    if result.return_code == 1:
        _fail("Invalid python library path: %s" % python_lib)

def _check_python_bin(repository_ctx, python_bin):
    """Checks the python bin path."""
    cmd = '[[ -x "%s" ]] && [[ ! -d "%s" ]]' % (python_bin, python_bin)
    result = repository_ctx.execute([_get_bash_bin(repository_ctx), "-c", cmd])
    if result.return_code == 1:
        _fail("--define %s='%s' is not executable. Is it the python binary?" % (
            _PYTHON_BIN_PATH,
            python_bin,
        ))

def _get_python_include(repository_ctx, python_bin):
    """Gets the python include path."""
    result = execute(
        repository_ctx,
        [
            python_bin,
            "-c",
            "from __future__ import print_function;" +
            "from distutils import sysconfig;" +
            "print(sysconfig.get_python_inc())",
        ],
        error_msg = "Problem getting python include path.",
        error_details = ("Is the Python binary path set up right? " +
                         "(See ./configure or " + _PYTHON_BIN_PATH + ".) " +
                         "Is distutils installed?"),
    )
    return result.stdout.splitlines()[0]

def _get_python_import_lib_name(repository_ctx, python_bin):
    """Get Python import library name (pythonXY.lib) on Windows."""
    result = execute(
        repository_ctx,
        [
            python_bin,
            "-c",
            "import sys;" +
            'print("python" + str(sys.version_info[0]) + ' +
            '      str(sys.version_info[1]) + ".lib")',
        ],
        error_msg = "Problem getting python import library.",
        error_details = ("Is the Python binary path set up right? " +
                         "(See ./configure or " + _PYTHON_BIN_PATH + ".) "),
    )
    return result.stdout.splitlines()[0]

def _get_numpy_include(repository_ctx, python_bin):
    """Gets the numpy include path."""
    return execute(
        repository_ctx,
        [
            python_bin,
            "-c",
            "from __future__ import print_function;" +
            "import numpy;" +
            " print(numpy.get_include());",
        ],
        error_msg = "Problem getting numpy include path.",
        error_details = "Is numpy installed?",
    ).stdout.splitlines()[0]

def _create_local_python_repository(repository_ctx):
    """Creates the repository containing files set up to build with Python."""
    python2_bin = _get_python2_bin(repository_ctx)
    _check_python_bin(repository_ctx, python2_bin)
    python_bin = _get_python_bin(repository_ctx)
    _check_python_bin(repository_ctx, python_bin)
    python_lib = _get_python_lib(repository_ctx, python_bin)
    _check_python_lib(repository_ctx, python_lib)
    python_include = _get_python_include(repository_ctx, python_bin)
    numpy_include = _get_numpy_include(repository_ctx, python_bin) + "/numpy"
    python_include_rule = symlink_genrule_for_dir(
        repository_ctx,
        python_include,
        "python_include",
        "python_include",
    )
    python_import_lib_genrule = ""

    # To build Python C/C++ extension on Windows, we need to link to python import library pythonXY.lib
    # See https://docs.python.org/3/extending/windows.html
    if is_windows(repository_ctx):
        python_include = norm_path(python_include)
        python_import_lib_name = _get_python_import_lib_name(repository_ctx, python_bin)
        python_import_lib_src = python_include.rsplit("/", 1)[0] + "/libs/" + python_import_lib_name
        python_import_lib_genrule = symlink_genrule_for_dir(
            repository_ctx,
            None,
            "",
            "python_import_lib",
            [python_import_lib_src],
            [python_import_lib_name],
        )
    numpy_include_rule = symlink_genrule_for_dir(
        repository_ctx,
        numpy_include,
        "numpy_include/numpy",
        "numpy_include",
    )

    _tpl(repository_ctx, "py.bzl", {
        "%{PYTHON_BIN}": "\"%s\"" % (python_bin),
        "%{PYTHON2_BIN}": "\"%s\"" % (python2_bin),
    })

    _tpl(repository_ctx, "BUILD", {
        "%{PYTHON_INCLUDE_GENRULE}": python_include_rule,
        "%{PYTHON_IMPORT_LIB_GENRULE}": python_import_lib_genrule,
        "%{NUMPY_INCLUDE_GENRULE}": numpy_include_rule,
    })

def _create_remote_python_repository(repository_ctx, remote_config_repo):
    """Creates pointers to a remotely configured repo set up to build with Python.
    """
    repository_ctx.template("BUILD", Label(remote_config_repo + ":BUILD"), {})

def _python_autoconf_impl(repository_ctx):
    """Implementation of the python_autoconf repository rule."""
    if _TF_PYTHON_CONFIG_REPO in repository_ctx.os.environ:
        _create_remote_python_repository(
            repository_ctx,
            repository_ctx.os.environ[_TF_PYTHON_CONFIG_REPO],
        )
    else:
        _create_local_python_repository(repository_ctx)

python_configure = repository_rule(
    implementation = _python_autoconf_impl,
    environ = [
        _BAZEL_SH,
        _PYTHON2_BIN_PATH,
        _PYTHON_BIN_PATH,
        _PYTHON_LIB_PATH,
        _TF_PYTHON_CONFIG_REPO,
    ],
)
"""Detects and configures the local Python.

Add the following to your WORKSPACE FILE:

```python
python_configure(name = "local_config_python")
```

Args:
  name: A unique name for this workspace rule.
"""
