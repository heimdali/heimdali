#!/usr/bin/env python
import os
import os.path as P
import tarfile

"""
Create tarballs of cluster usage examples that can later downloaded by users.
"""

def setup_dirs():
    """
    Define directories involving cluser usage example.

    Directory hierarchy is as follow:

        heimdali/                 <- root
            doc/                  <- here (where this file is)
                _static/
                    example/      <- static_dir
            example/              <- examples_root_dir
                inrimage_read/    <- example_dir
                inrimage_write/   <- example_dir
                ...
            write-example-tarballs.py
    """
    here = P.dirname(__file__)
    root = P.realpath( P.join(here, '..') )
    examples_root_dir = P.join(root, 'example')
    if not P.isdir(examples_root_dir):
        raise IOError, "No such example dir: %s" % (examples_root_dir,)
    static_dir = P.join(root, 'doc', '_static', 'example')
    if not P.isdir(static_dir):
        os.makedirs(static_dir)
    return examples_root_dir, static_dir

def walk_example_dirs(examples_root_dir):
    """Iterates on all example directory absolute paths
    
    :param examples_root_dir: Root directory containing example directories
    :type scr_dir: string
    """
    for name in os.listdir(examples_root_dir):
        path = P.join(examples_root_dir, name)
        if P.isdir(path):
            yield path

def write_example_tarball(static_dir, example_dir):
    """Write to the disk an example archive
    
    :param static_dir: Directory where to write archive.
    :type static_dir: string
    
    :param static_dir: Directory to compress.
    :type static_dir: string
    """
    example_name = P.basename(example_dir)
    prefix = 'heimdali-example'
    filename = prefix + '-' + example_name + '.tar.gz'
    filepath = P.join(static_dir, filename)
    with tarfile.open(filepath,'w:gz') as tar:
        tar.add(example_dir, arcname=P.join(example_name))
    return filepath

def main():
    examples_root_dir, static_dir = setup_dirs()
    for example_dir in walk_example_dirs(examples_root_dir):
        filepath = write_example_tarball(static_dir, example_dir)
        print(filepath)

if __name__ == '__main__':
    main()
