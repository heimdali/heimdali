from lettuce import *

from os.path import dirname, realpath, join, expanduser, isdir, isfile
from os import getenv

def get_invocation_dir():
    this_script_dir = dirname(realpath(__file__))
    fullpath = lambda path: realpath(join(this_script_dir,path))
    # Directory heimdali/tests from where lettuce is invoked.
    invocation_dir = fullpath("../")
    return invocation_dir

def get_data_dir():
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

@before.all
def setup_directories():
    world.data_dir = get_data_dir()
    world.cwd = get_invocation_dir()

@before.each_scenario
def setup_cwd_to_invocation_dir(scenario):
    world.cwd = get_invocation_dir()
