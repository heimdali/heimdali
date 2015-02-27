from lettuce import *

from os.path import dirname, realpath, join, expanduser, isdir, isfile, \
    normpath, basename
from os import getenv, mkdir
from subprocess import check_output, check_call
import json
import shutil
import os

def get_active_conda_env_path():
    """Path to the active conda environment"""

    args = 'conda info --json'.split()

    try:
        json_str = check_output(args)
    except OSError:
        raise OSError, "Command 'conda info --json' failed, " \
                       "is conda installed and in PATH?"

    json_data = json.loads(json_str)
    active_env_path = json_data['default_prefix']

    return realpath(normpath(active_env_path))

def get_conda_env_path(env_name, ensure_is_active=False):
    """get path to a conda environment"""

    normalize = lambda path : str(realpath(normpath(path)))

    args = 'conda info --json'.split()

    try:
        json_str = check_output(args)
    except OSError:
        raise OSError, "Command 'conda info --json' failed, " \
                       "is conda installed and in PATH?"

    json_data = json.loads(json_str)
    env_paths = [normalize(p) for p in json_data['envs']]
    active_env_path = normalize(json_data['default_prefix'])

    for env_path in env_paths:
        if basename(env_path) == env_name:
            break
    else:
        raise OSError, "Can not find %s conda environment." % env_name

    if ensure_is_active and env_path != active_env_path:
        raise OSError, "%s conda environment must be active. " \
            "Please issue the command: 'source activate %s'" \
            % (env_name, env_name)

    return env_path


def get_data_dir():
    """Path to heimdali-data git repository"""
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
    """Directory from where lettuce is invoked, ie heimdali/tests"""
    this_script_dir = dirname(realpath(__file__))
    fullpath = lambda path: realpath(join(this_script_dir,path))
    # Directory heimdali/tests from where lettuce is invoked.
    invocation_dir = fullpath("../")
    return invocation_dir

def get_heimdali_root():
    """Path to heimdali Git repository root directory"""
    invocation_dir = get_invocation_dir()
    return realpath(join(invocation_dir, '..'))

def get_example_dir():
    """heimdali/example directory"""
    heimdali_root = get_heimdali_root()
    return realpath(join(heimdali_root, 'example'))

@before.all
def setup_directories():
    world.data_dir = get_data_dir()
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

@before.all
def get_reference_inr_commands():
    inrimage_path = get_conda_env_path('inrimage')
    world.par = join(inrimage_path, 'bin', 'par')
    world.so = join(inrimage_path, 'bin', 'so')
    world.ical = join(inrimage_path, 'bin', 'ical')

@before.all
def setup_root_workdir():
    """Clean and/or create root workdir for all tests"""
    invocation_dir = get_invocation_dir()
    root_workdir = join(invocation_dir, 'workdir')
    if isdir(root_workdir):
        shutil.rmtree(root_workdir)
    mkdir(root_workdir)
    world.root_workdir = root_workdir

@before.each_scenario
def move_to_workdir(scenario):
    os.chdir(world.workdir)

@before.each_feature
def setup_feature_workdir(feature):
    """Create workdir for this feature"""
    world.workdir = join(world.root_workdir, feature.name)
    mkdir(world.workdir)
