from subprocess import Popen, PIPE
from os.path import realpath, expandvars

from lettuce import *

@step('I am in directory (.*)')
def in_the_directory(step,cwd):
    world.cwd = realpath( expandvars(cwd) )

@step('I run the command: (.*)')
def run_the_command(step,cmd):
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE, cwd=world.cwd)
    world.stdoutdata, world.stderrdata = p.communicate()
    world.returncode = p.returncode

@step('I see the line in standard output: (.*)')
def i_see_the_line(step,line):
    assert line in world.stdoutdata

@step(u'Then I see the standard output:')
def then_i_get_the_standard_output(step):
    stdout = world.stdoutdata.strip()
    assert step.multiline == stdout, "Got %r" % stdout

def factorial(number):
    return 1
