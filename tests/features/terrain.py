from lettuce import *

import sys
from os import mkdir, chdir
from os.path import join, isdir, isfile, realpath, dirname
from subprocess import check_call
import platform

here = dirname(__file__)
heimdali_python = join(here, '..', '..', 'python')
sys.path.append(realpath(heimdali_python))
import heimdali

BUILD_DIR_NAME = 'build_debug'
LETTUCE_DIR_NAME = 'lettuce'

@before.all
def setup_directories():
    world.heimdali_src_dir = heimdali.get_heimdali_src_dir()
    world.heimdali_data_dir = heimdali.get_heimdali_data_dir()
    world.heimdali_example_dir = heimdali.get_heimdali_example_dir()
    world.heimdali_work_dir = heimdali.get_heimdali_work_dir()
    world.lettuce_work_dir = join(world.heimdali_work_dir, LETTUCE_DIR_NAME)

@world.absorb
def example_build_dir(name,check_config=False):
    build_dir = join(world.heimdali_work_dir, BUILD_DIR_NAME, name)
    if check_config:
        makefile = join(build_dir, 'Makefile')
        is_configured = isfile(makefile)
        return build_dir, is_configured
    else:
        return build_dir

def configure_example(name):
    """Configure heimdali/example/name with CMake"""
    build_dir,is_configured = world.example_build_dir(name, check_config=True)
    if not is_configured:
        if not isdir(build_dir):
            mkdir(build_dir)
        args = 'cmake -DCMAKE_BUILD_TYPE=Debug ..'
        check_call(args.split(), cwd=build_dir)

@before.all
def configure_all_examples():
    for name in ['create_input_image', 'inrimage_read', 'inrimage_write']:
        configure_example(name)

@before.all
def get_reference_inr_commands():
    if platform.system() == "Darwin":
        inrimage_path = "/usr/local/"
    else:
        inrimage_path = heimdali.get_conda_env_path('inrimage')
    world.par = join(inrimage_path, 'bin', 'par')
    world.so = join(inrimage_path, 'bin', 'so')
    world.ical = join(inrimage_path, 'bin', 'ical')

@before.all
def setup_lettuce_workdir():
    """Clean and/or create root workdir for all tests"""
    heimdali.setup_clean_directory(world.lettuce_work_dir)

@before.each_scenario
def move_to_workdir(scenario):
    chdir(world.feature_workdir)

@before.each_feature
def setup_feature_workdir(feature):
    """Create workdir for this feature"""
    world.feature_workdir = join(world.lettuce_work_dir, feature.name)
    mkdir(world.feature_workdir)
