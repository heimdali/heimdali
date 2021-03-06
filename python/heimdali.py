#!/usr/bin/env python

from os import getenv, mkdir
from os.path import dirname, realpath, join, expanduser, isdir, isfile, \
    normpath, basename
from subprocess import check_output
import shutil
import json

def get_heimdali_conda_dir():
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

def get_directory_from_envvar(name, must_contain=None):
    directory = getenv(name)

    # Check environment variable exists.
    if directory == None or not directory.strip():
        msg = "$%s environment variable " \
              "is required." % (name,)
        if must_contain:
            msg += " Export it to the directory containing " \
                   "for example the file %s ." % (must_contain,)
        raise OSError, msg

    # Check directory exists.
    directory = expanduser(directory)
    if not isdir(directory):
        msg = "Directory $%s=%s " \
              "does not exists" % (name,directory)
        raise OSError, msg

    # Check directory is correct, ie it contains the 'must_contain' file.
    if must_contain:
        must_contain_path = join(directory, must_contain)
        if not isfile(must_contain_path):
            msg = "Directory $%s=%s " \
                  "does not contain the file %s ." % (name, directory, must_contain)
            raise OSError, msg

    return directory

def get_heimdali_data_dir():
    """Path to heimdali-data git repository"""
    return get_directory_from_envvar('HEIMDALI_DATA_DIR', must_contain='imtest_z5_y4_x3_c2.h5')

def get_heimdali_work_dir():
    """Path to working directory for test and benchmark"""
    return get_directory_from_envvar("HEIMDALI_WORK_DIR")

def get_heimdali_src_dir():
    """Path to heimdali Git repository root directory"""
    here = dirname(__file__)
    heimdali_src_dir = join(here, '..')
    return realpath(heimdali_src_dir)

def get_example_src_dir(name):
    """heimdali/example directory"""
    heimdali_src_dir = get_heimdali_src_dir()
    return join(heimdali_src_dir, 'example', name)

def setup_clean_directory(directory):
    """Create a directory, removing it before if existing"""
    if isdir(directory):
        shutil.rmtree(directory)
    mkdir(directory)
