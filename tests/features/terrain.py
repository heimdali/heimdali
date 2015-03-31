from lettuce import *

import sys
from os import mkdir, chdir
from os.path import join, isdir, isfile, realpath, dirname
from subprocess import check_call
import shutil
import platform

here = dirname(__file__)
heimdali_python = join(here, '..', '..', 'python')
sys.path.append(realpath(heimdali_python))
import heimdali

@before.all
def setup_directories():
    world.data_dir = heimdali.get_data_dir()
    world.example_dir = heimdali.get_example_dir()
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
    conda_env_path = heimdali.get_active_conda_env_path()
    args = 'cmake -DCMAKE_PREFIX_PATH=%s ..' % (conda_env_path,)
    check_call(args.split(), cwd=world.example_build_dir)

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
    if isdir(root_workdir):
        shutil.rmtree(root_workdir)
    mkdir(root_workdir)
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
