par
====================================

**par** prints format parameters of images.

Synopsis
------------------------------------

.. code-block:: bash

   par  [--wr <output.txt>] [--x0] [--y0] [--z0] [-o] [-x] [-y] [-z] [--]
        [--version] [-h] <INPUT> ...

Description
------------------------------------

**par** print on `stdout` in the file `outout.txt`, the format parameters of iamges given as arguments.

The `--wr` options can be given the special file names `stdout` and `stderr`. If the file name `output.txt`
starts with `>>`, result are written at the end of the file.

If one or more options `--x0 --y0 --z0 -x -y -z` is given, **par** print the corresponding parameters, in
the :ref:`format_options`. This allow to use **par** in command subsitution, as for example:

.. code-block:: bash

    create image-copy.h5 `par -x -y image.h5` -r

If no options are given, all format parameters are printed for all images on argument.
