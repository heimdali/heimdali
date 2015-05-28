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

@before.all
def setup_directories():
    world.heimdali_root = heimdali.get_heimdali_root()
    world.data_dir = heimdali.get_data_dir()
    world.example_dir = heimdali.get_example_dir()
    world.build_dir = 'build_debug'

@world.absorb
def example_build_dir(name):
    return join(world.example_dir, name, world.build_dir)

@world.absorb
def examples_is_configured(name):
    """ Is heidamli/example/name configured with CMakefile? """
    build_dir = world.example_build_dir(name)
    makefile = join(world.example_build_dir(name), 'Makefile')
    return isfile(makefile)

@world.absorb
def assert_examples_is_configured(name):
    if not world.examples_is_configured(name):
        raise IOError, "heimdali/example/%s is not configured with CMake" % name

def configure_example(name):
    """Configure heimdali/example/name with CMake"""
    if world.examples_is_configured(name):
        return
    build_dir = world.example_build_dir(name)
    if not isdir(build_dir):
        mkdir(world.build_dir)
    conda_env_path = heimdali.get_active_conda_env_path()
    args = ['cmake',
            '-DCMAKE_BUILD_TYPE=Debug',
            '-DCMAKE_PREFIX_PATH=%s' % (conda_env_path,),
            '-DHEIMDALI_DIR=%s/%s' % (world.heimdali_root, world.build_dir),
            '..'
            ]
    check_call(args, cwd=build_dir)

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
def setup_root_workdir():
    """Clean and/or create root workdir for all tests"""
    heimdali_workdir = heimdali.get_workdir()
    root_workdir = join(heimdali_workdir, 'lettuce')
    heimdali.setup_clean_directory(root_workdir)
    world.root_workdir = root_workdir

@before.each_scenario
def move_to_workdir(scenario):
    chdir(world.workdir)
    world.child = None

@before.each_feature
def setup_feature_workdir(feature):
    """Create workdir for this feature"""
    world.workdir = join(world.root_workdir, feature.name)
    mkdir(world.workdir)
