from subprocess import Popen, PIPE
from os.path import realpath, expandvars

from lettuce import *

def failed():
    return world.returncode != 0 or world.stderr != ''

def cmd_error():
    raise AssertionError, "returncode is %r, stdout is %r, stderr is %r" % (
            world.returncode, world.stdout, world.stderr)

@step('I am in the heimdali data directory')
def in_the_data_directory(step):
    world.cwd = world.data_dir

@step('I am in directory (.*)')
def in_the_directory(step,cwd):
    world.cwd = realpath( expandvars(cwd) )

@step('I run the command: (.*)')
def run_the_command(step,cmd):
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE, cwd=world.cwd)
    world.stdout, world.stderr = p.communicate()
    world.returncode = p.returncode

@step('I see the line in standard output: (.*)')
def i_see_the_line(step,line):
    if failed() or not line in world.stdout:
        cmd_error()

@step(u'Then I see the standard output:')
def then_i_get_the_standard_output(step):
    stdout = world.stdout.strip()
    stderr = world.stderr.strip()
    if failed() or step.multiline != stdout:
        cmd_error()

def factorial(number):
    return 1
