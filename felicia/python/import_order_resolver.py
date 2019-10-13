import sys
import platform

if platform.system() == 'Windows':
    # When you run py_binary using bazel, it constructs output layout like below
    # runfiles
    # | __init__.py
    # | - felicia
    #     | __init__.py
    #     | - felicia
    #         | - core
    #         | - examples
    # | - com_google_protobuf
    # | - six_archive
    #
    # On Windows, because PYTHONPATH contains [..., .../runfiles, .../runfiles/felicia]
    # so it is confused when python tries to import felicia, because in layout order,
    # the parent felicia will be imported first.
    for i in range(len(sys.path)):
        if sys.path[i].endswith('runfiles'):
            del sys.path[i]
            break