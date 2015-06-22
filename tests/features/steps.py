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

def check_command(returncode=0):
    if world.returncode != int(returncode) or \
       (world.returncode == 0 and world.stderr != ''):
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

def check_stderr(actual, expected):
    if actual != expected:
        diff = ndiff(actual.splitlines(True), expected.splitlines(True))
        diff = '\n' + ''.join(diff)
        raise AssertionError, "stderr is not as expected " \
            "(- actual, + expected): %s" % (diff,)

def invoke_from(cmd_string, path):
    """Prefix executable in cmd_string with path"""
    cmd_list = cmd_string.split()
    executable = cmd_list[0]
    executable = join(path, executable)
    cmd_list[0] = executable
    return ' '.join(cmd_list)

def expand_env_var(text):
    text = text.replace('$HEIMDALI_DATA_DIR', world.heimdali_data_dir)
    return text

@step('I run the command: (.*)')
def run_the_command(step,cmd):
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step('I run the command \(with return code (\d+)\): (.*)')
def run_the_command(step, returncode, cmd):
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command(returncode=returncode)

@step("I build the (.*) example")
def build_example(step, name):
    build_dir, is_configured = world.get_example_build_dir(name, check_config=True)
    if not is_configured:
        raise IOError, "%s is not configured with CMake" % (build_dir,)
    check_call(['make', ], cwd=build_dir)

@step('I run the example (.*?): (.*)')
def run_the_example(step,name,cmd):
    build_dir = world.get_example_build_dir(name)
    cmd = invoke_from(cmd, build_dir)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()

@step('I see the line in standard output: (.*)')
def i_see_the_line(step,line):
    line = line.replace('$HEIMDALI_DATA_DIR', world.heimdali_data_dir)
    if not line in world.stdout:
        raise AssertionError, \
            "Expected line: %r in stdout, but stdout is: %r" % (
                line,world.stdout)

@step('I see the standard output:')
def i_see_the_standard_output(step):
    expected_stdout = expand_env_var(step.multiline)
    check_stdout(world.stdout, expected_stdout)

@step('I see on standard error:')
def i_see_the_standard_output(step):
    expected_stderr = expand_env_var(step.multiline)
    check_stderr(world.stderr, expected_stderr)

@step('I see as standard output the content of the file (.*)')
def i_see_as_standard_output_file_content(step,filename):
    expected_stdout = open(expand_env_var(filename)).read()
    check_stdout(world.stdout, expected_stdout)

def invoke_inrimage_par(filename,flags='-F -x -y -f -o'):
    cmd = '%s %s  %s' % (world.par, flags, filename)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()
    params = world.stdout.strip().split()
    params.pop(0) # remove file name
    return ' '.join(params)

def assert_inrimage_same_parameters(fileA, fileB):
    """Execute 'par' on both images, and compare output."""
    paramsA = invoke_inrimage_par(fileA)
    paramsB = invoke_inrimage_par(fileB)
    if paramsA != paramsB:
        raise AssertionError, \
           "Image %s has parameters:\n %r\n, but " \
           "image %s has parameters:\n %r" % \
           (fileA, paramsA, fileB, paramsB)

def assert_inrimage_equal(fileA, fileB):
    """ Execute so file1 fileB | ical, and check output """
    # Execute so file1 fileB | ical, and check output
    cmd = '%s %s %s | %s' % (world.so, fileA, fileB, world.ical)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode
    check_command()
    im_min, im_avg, im_max = [float(v) for v in world.stdout.split()]
    if im_min != 0. or im_avg != 0. or im_max !=0.:
        raise AssertionError, "Images are not equal. " \
            "min, average, and max of the difference are: %f, %f, %f" % \
            (im_min, im_avg, im_max)

def assert_inrimage_almost_equal(fileA, fileB):
    pass

@step("images (.*) and (.*) are equal")
def images_are_equal(step, fileA, fileB):
    extA = os.path.splitext(fileA)[1].strip()
    extB = os.path.splitext(fileB)[1].strip()

    if extA in ['.h5', '.hdf5'] and extB in ['.h5', '.hdf5']:
        cmd = "h5diff -v --compare " \
              "--exclude-path /HDFVersion " \
              "--exclude-path /ITKVersion " \
              "--exclude-path /ITKImage/0/MetaData/sz_sy_iz_iy " \
              "%s %s" % (fileA, fileB)
        p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
        world.stdout, world.stderr = p.communicate()
        world.returncode = p.returncode
        check_command()

    elif extA == '.inr' and extB == '.inr':
        # note that par, ical and so are from the original
        # Inrimage library, not from Heimdali.
        assert_inrimage_same_parameters(fileA, fileB)
        assert_inrimage_equal(fileA, fileB)

    else:
        raise ValueError,  \
            "Expected both HDF5 or both INRimage files, " \
            "but got %r and %r files" % (extA, extB)

@step("images (.*) and (.*) are almost equal with the relative parameter (.*)")
def images_are_almost_equal(step, fileA, fileB, relative):
    extA = os.path.splitext(fileA)[1].strip()
    extB = os.path.splitext(fileB)[1].strip()

    if extA in ['.h5', '.hdf5'] and extB in ['.h5', '.hdf5']:
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

    elif extA == '.inr' and extB == '.inr':
        # note that par, ical and so are from the original
        # Inrimage library, not from Heimdali.
        assert_inrimage_same_parameters(fileA, fileB)
        assert_inrimage_almost_equal(fileA, fileB)

    else:
        raise ValueError,  \
            "Expected both HDF5 or both INRimage files, " \
            "but got %r and %r files" % (extA, extB)
