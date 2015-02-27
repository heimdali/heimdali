from subprocess import Popen, PIPE, check_call
from os.path import realpath, expandvars, join, isfile
from difflib import ndiff
import os

from lettuce import *

MAX_CHAR_OUTPUT = 10000

def truncate_long_output(output):
    if len(output) > MAX_CHAR_OUTPUT:
        output = output[:MAX_CHAR_OUTPUT]
        if output[-1] == '\n':
            output += '...output truncated...'
        else:
            output += '\n...output truncated...'
    return output

def check_command():
    if world.returncode != 0 or world.stderr != '':
        raise AssertionError, \
            "returncode is: %i, stdout is:\n%s\n, stderr is:\n%s\n" % (
                world.returncode,
                truncate_long_output(world.stdout),
                truncate_long_output(world.stderr))

def check_stdout(actual, expected):
    if actual != expected:
        diff = ndiff(actual.splitlines(True), expected.splitlines(True))
        diff = '\n' + ''.join(diff)
        raise AssertionError, "stdout is not as expected " \
            "(- actual, + expected): %s" % (diff,)

def invoke_from(cmd_string, path):
    """Prefix executable in cmd_string with path"""
    cmd_list = cmd_string.split()
    executable = cmd_list[0]
    executable = join(path, executable)
    cmd_list[0] = executable
    return ' '.join(cmd_list)

@step('I run the command: (.*)')
def run_the_command(step,cmd):
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step('I run the example: (.*)')
def run_the_example(step,cmd):
    cmd = invoke_from(cmd, world.example_build_dir)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step('I see the line in standard output: (.*)')
def i_see_the_line(step,line):
    check_command()
    line = line.replace('$HEIMDALI_DATA_DIR', world.data_dir)
    line = line.replace('$WORKDIR', world.workdir)
    if not line in world.stdout:
        raise AssertionError, \
            "Expected line: %r in stdout, but stdout is: %r" % (
                line,world.stdout)

@step(u'Then I see the standard output:')
def then_i_get_the_standard_output(step):
    expected_stdout = step.multiline
    expected_stdout = expected_stdout.replace('$HEIMDALI_DATA_DIR', 
                                              world.data_dir)
    expected_stdout = expected_stdout.replace('$WORKDIR', 
                                              world.workdir)
    check_command()
    check_stdout(world.stdout, expected_stdout)

@step("images (.*) and (.*) are equal")
def hdf5_files_are_equal(step, fileA, fileB):
    cmd = "h5diff -v --compare " \
          "--exclude-path /HDFVersion " \
          "--exclude-path /ITKVersion " \
          "--exclude-path /ITKImage/0/MetaData/sz_sy_iz_iy " \
          "%s %s" % (fileA, fileB)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step("the HDF5 files (.*) and (.*) are almost equal with the relative parameter (.*)")
def hdf5_files_are_equal(step, fileA, fileB, relative):
    cmd = "h5diff -v --compare " \
          "--relative=%s " % (relative, ) + \
          "--exclude-path /HDFVersion " \
          "--exclude-path /ITKVersion " \
          "--exclude-path /ITKImage/0/MetaData/sz_sy_iz_iy " \
          "%s %s" % (fileA, fileB)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step("I build the (.*) example")
def build_example(step, target):
    world.assert_examples_are_configured()
    check_call(['make', target], cwd=world.example_build_dir)
