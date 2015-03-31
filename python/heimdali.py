#!/usr/bin/env python

from os import getenv, mkdir
from os.path import dirname, realpath, join, expanduser, isdir, isfile, \
    normpath, basename
from subprocess import check_output
import shutil
import json

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

def get_directory_from_envvar(name, must_contain=None):
    data_dir = getenv(name)

    # Check environment variable exists.
    if data_dir == None or not data_dir.strip():
        msg = "$%s environment variable " \
              "is required." % (name,)
        if must_contain:
            msg += " Export it to the directory containing " \
                   "for example the file %s ." % (must_contain,)
        raise OSError, msg

    # Check directory exists.
    data_dir = expanduser(data_dir)
    if not isdir(data_dir):
        msg = "Directory $%s=%s " \
              "does not exists" % (name,data_dir)
        raise OSError, msg

    # Check directory is correct, ie it contains the 'must_contain' file.
    if must_contain:
        must_contain_path = join(data_dir, must_contain)
        if not isfile(must_contain_path):
            msg = "Directory $%s=%s " \
                  "does not contain the file %s ." % (name, data_dir, must_contain)
            raise OSError, msg

    return data_dir

def get_data_dir():
    """Path to heimdali-data git repository"""
    return get_directory_from_envvar('HEIMDALI_DATA_DIR', must_contain='imtest_z5_y4_x3_c2.h5')

def get_workdir():
    """Path to working directory for test and benchmark"""
    return get_directory_from_envvar("HEIMDALI_WORK_DIR")

def get_heimdali_root():
    """Path to heimdali Git repository root directory"""
    here = dirname(__file__)
    heimdali_root = join(here, '..')
    return realpath(heimdali_root)

def get_example_dir():
    """heimdali/example directory"""
    heimdali_root = get_heimdali_root()
    return realpath(join(heimdali_root, 'example'))

def setup_clean_directory(directory):
    """Create a directory, removing it before if existing"""
    if isdir(directory):
        shutil.rmtree(directory)
    mkdir(directory)
