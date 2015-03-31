#!/usr/bin/env python

import os
from os.path import basename, dirname, join, realpath, isfile, expanduser
import re
import sys
import argparse
import subprocess
import glob
import shlex

here = dirname(__file__)
heimdali_python = join(here, '..', 'python')
sys.path.append(realpath(heimdali_python))
import heimdali

def which(program):
    """Copied from http://stackoverflow.com/a/377028"""
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def extract_dims_from_filename(filename):
    s = "imtest_z(?P<z>\d+)_y(?P<y>\d+)_x(?P<x>\d+)_c(?P<c>\d+)\.*"
    regexp = re.compile(s)
    d = re.search(regexp, filename).groupdict()
    nz,ny,nx,nc = [int(d[name]) for name in 'z y x c'.split()]
    return nz,ny,nx,nc

def extract_dims_from_cmd(line):
    exe, input_filename, output_filename = line.split()
    nz,ny,nx,nc = extract_dims_from_filename(input_filename)
    return nz,ny,nx,nc

def extract_times(line):
    s = "real: 0:(?P<real>.*) user: (?P<user>.+?) sys: (?P<sys>.+?)$"
    regexp = re.compile(s)
    d = re.search(regexp, line.strip()).groupdict()
    return [float(d[name]) for name in "real user sys".split()]

def init(workdir,args):
    """ Create input images with WriteHDF5Imtest executable"""
    import numpy as np

    print workdir

    def compare(expected,actual):
        # Print header only one time.
        if not hasattr(compare, 'header_printed'):
            print 'expected size,  actual size,  percentage'
            compare.header_printed = True
        percent = (float(actual) / expected) * 100
        print expected, actual, percent

    # Executable to create images.
    data_dir = heimdali.get_data_dir()
    exe = os.path.join(data_dir, "build", "WriteHDF5Imtest")

    nv = args.number_of_components

    for nzyxv_approx in np.linspace(args.first_image_size, args.last_image_size, args.number_of_images):
        nzyxv = int(nzyxv_approx)
        nzyx = nzyxv / nv
        nz = ny = nx = int(nzyx**(1./3.))
        if args.no_action:
            compare(nzyxv_approx, nz*ny*nx*nv)
        else:
            cmd = "%s %i %i %i %i" % (exe, nz, ny, nx, nv)
            subprocess.check_call(cmd.split(),cwd=workdir)

def get_ext(inr_to_h5):
    if inr_to_h5:
        src_ext = 'inr'
        dst_ext = 'h5'
    else:
        src_ext = 'h5'
        dst_ext = 'inr'
    return src_ext, dst_ext

def get_inrimage_exe_name(src_ext, dst_ext):
    return '%s2%s' % (src_ext, dst_ext)

def get_exe_path(with_inrimage, inrimage_path, src_ext, dst_ext):
    if with_inrimage:
        exe = get_inrimage_exe_name(src_ext, dst_ext)
        if inrimage_path:
            exe = join(expanduser(args.inrimage_path), exe)
    else:
        exe = 'hconv'
    return exe

def get_time_file(exe, src_ext, dst_ext):
    return '%s_%s_%s.txt' % (basename(exe), src_ext, dst_ext)

def bench(workdir, args):
    src_ext, dst_ext = get_ext(args.inr_to_h5)
    exe = get_exe_path(args.with_inrimage, args.inrimage_path, src_ext, dst_ext)
    time_file = get_time_file(exe, src_ext, dst_ext)

    if not which(exe):
        raise OSError, "No such executable: %s" % exe

    time_path = join(workdir, time_file)

    if isfile(time_path):
        os.remove(time_path)

    pattern = join(workdir, 'imtest_z*_y*_x*_c*.%s' % src_ext)
    for src in glob.glob(pattern):
        dst = os.path.splitext(src)[0] + '.' + dst_ext
        cmd = ['/usr/bin/time',
              r'--format=real: %E user: %U sys: %S',
               '--output=%s' % time_path,
               '--append',
              exe, src, dst]
        info = '%s %s %s' % (basename(exe), basename(src), basename(dst))
        print info
        with open(time_path,'a') as f:
            f.write(info+'\n')
        subprocess.check_call(cmd)

def plot(workdir, args):
    import matplotlib.pyplot as plt
    plt.switch_backend('agg')

    symbols = ['o', 'x']

    src_ext, dst_ext = get_ext(args.inr_to_h5)
    exe_inrimage = get_inrimage_exe_name(src_ext, dst_ext)
    exe_heimdali = 'hconv'

    for exe,symbol in zip([exe_heimdali, exe_inrimage], symbols):

        filename = get_time_file(exe, src_ext, dst_ext)
        filepath = join(workdir, filename)

        # Read result file.
        lines = open(filepath).readlines()
        cmd_lines = lines[::2]
        time_lines = lines[1::2]

        # Extract image sizes.
        sizes = []
        for line in cmd_lines:
            nz,ny,nx,nc = extract_dims_from_cmd(line)
            size = nz*ny*nx
            sizes.append(size)

        # Extract times
        real_times, user_times, sys_times = [], [], []
        for line in time_lines:
            real_time,user_time,sys_time = extract_times(line)
            real_times.append(real_time)
            user_times.append(user_time)
            sys_times.append(sys_time)

        # Plot real time
        plt.plot(sizes, real_times, 'b'+symbol, label = exe + ' real')
        plt.plot(sizes, user_times, 'g'+symbol, label = exe + ' user')
        plt.plot(sizes, sys_times, 'r'+symbol, label = exe + ' sys')

    plt.xlabel('Number of pixels (each has 3 components)')
    plt.ylabel('Time in seconds')
    plt.legend()
    plt.grid()

    filename = '%s_VS_%s.png' % (exe_heimdali, exe_inrimage)
    filepath = join(workdir, filename)
    plt.savefig(filepath)
    print filepath

def clean(workdir, args):
    heimdali.setup_clean_directory(workdir)

def parse_command_line():
    parser = argparse.ArgumentParser(description="Manage hconv benchmarks.")
    subparser = parser.add_subparsers()

    # init
    parser_init = subparser.add_parser('init', help='Create input images')
    parser_init.add_argument('--first-image-size', type=int, default=100)
    parser_init.add_argument('--last-image-size', type=int, default=50000000)
    parser_init.add_argument('--number-of-images', type=int, default=20)
    parser_init.add_argument('--number-of-components', type=int, default=3)
    parser_init.add_argument('--no-action',  action='store_true',
                             help='show size of images that would be created')
    parser_init.set_defaults(func=init)

    # bench
    parser_bench = subparser.add_parser('bench', help='Run benchmarks')
    parser_bench.add_argument('--with-inrimage', action='store_true',
                              help='Use inr2h5/h5toinr instead of hconv')
    parser_bench.add_argument('--inr-to-h5', action='store_true',
                              help='Convert from inr to h5 instead of h5 to inr')
    parser_bench.add_argument('--inrimage-path', type=str,
                              help='Path to inr2h5 and h52inr commands')
    parser_bench.set_defaults(func=bench)

    # plot
    parser_plot = subparser.add_parser('plot', help='Plot time results')
    parser_plot.add_argument('--inr-to-h5', action='store_true',
                             help='Convert from inr to h5 instead of h5 to inr')
    parser_plot.set_defaults(func=plot)

    # clean
    parser_clean = subparser.add_parser('clean', help='Clean working directory')
    parser_clean.set_defaults(func=clean)

    return parser.parse_args()

if __name__ == '__main__':
    
    # Set-up working directory.
    heimdali_workdir = heimdali.get_workdir()
    bench_hconv_workdir = join(heimdali_workdir, 'bench_hconv')

    # Parse command line.
    args = parse_command_line()

    # Do the job.
    args.func(bench_hconv_workdir, args)
