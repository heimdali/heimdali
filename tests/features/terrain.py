from lettuce import *

from os.path import dirname, realpath, join, expanduser, isdir, isfile, normpath
from os import getenv, mkdir
from subprocess import check_output, check_call
import json

def get_active_conda_env_path():
    """get path to a conda environment"""

    args = 'conda info --json'.split()

    try:
        json_str = check_output(args)
    except OSError:
        raise OSError, "Command 'conda info --json' failed, " \
                       "is conda installed and in PATH?"

    json_data = json.loads(json_str)
    active_env_path = json_data['default_prefix']

    return realpath(normpath(active_env_path))

def get_data_dir():
    """ Heimdali-data githrepository """
    data_dir = getenv('HEIMDALI_DATA_DIR')
    must_contain = 'imtest_z5_y4_x3_c2.h5'

    # Check environment variable exists.
    if data_dir == None or not data_dir.strip():
        msg = "HEIMDALI_DATA_DIR environment variable " \
              "is required. Export it to the directory containing " \
              "for example the file %s ." % must_contain
        raise OSError, msg

    # Check directory exists.
    data_dir = expanduser(data_dir)
    if not isdir(data_dir):
        msg = "Directory $HEIMDALI_DATA_DIR=%s " \
              "does not exists" % data_dir
        raise OSError, msg

    # Check directory is correct, ie it contains the 'must_contain' file.
    must_contain_path = join(data_dir, must_contain)
    if not isfile(must_contain_path):
        msg = "Directory $HEIMDALI_DATA_DIR=%s " \
              "does not contain the file %s ." % (data_dir, must_contain)
        raise OSError, msg

    return data_dir

def get_invocation_dir():
    """ Directory from which lettuce is invoked, ie heimdali/tests """
    this_script_dir = dirname(realpath(__file__))
    fullpath = lambda path: realpath(join(this_script_dir,path))
    # Directory heimdali/tests from where lettuce is invoked.
    invocation_dir = fullpath("../")
    return invocation_dir

def get_heimdali_root():
    """ heimdali git repository root directory """
    invocation_dir = get_invocation_dir()
    return realpath(join(invocation_dir, '..'))

def get_example_dir():
    """ heimdali/example directory """
    heimdali_root = get_heimdali_root()
    return realpath(join(heimdali_root, 'example'))

@before.all
def setup_directories():
    world.data_dir = get_data_dir()
    world.cwd = get_invocation_dir()
    world.example_dir = get_example_dir()
    world.example_build_dir = join(world.example_dir, 'build')

@world.absorb
def examples_are_configured():
    """ Is heidamli/example configured with CMakefile? """
    makefile = join(world.example_build_dir, 'Makefile')
    return isfile(makefile)

@world.absorb
def assert_examples_are_configured():
    if not world.examples_are_configured():
        raise IOError, "heimdali/example is not configured with CMake"

@before.all
def configure_example():
    """Configure heimdali/example with CMake"""
    if world.examples_are_configured():
        return
    if not isdir(world.example_build_dir):
        mkdir(world.example_build_dir)
    conda_env_path = get_active_conda_env_path()
    args = 'cmake -DCMAKE_PREFIX_PATH=%s ..' % (conda_env_path,)
    check_call(args.split(), cwd=world.example_build_dir)

@before.each_scenario
def setup_cwd_to_invocation_dir(scenario):
    world.cwd = get_invocation_dir()
