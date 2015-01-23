from lettuce import *

from os.path import dirname, realpath, join

this_script_dir = dirname(realpath(__file__))
fullpath = lambda path: realpath(join(this_script_dir,path))

# Directory climedev/cmd/tests from where lettuce is invoked.
invocation_dir = fullpath("../")

@before.each_scenario
def setup_cwd_to_invocation_dir(scenario):
    world.cwd = invocation_dir
