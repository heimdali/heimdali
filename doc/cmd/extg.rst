.. _cmd-extg:

extg
====================================

**extg** extract a image subregion

Synopsis
------------------------------------

.. code-block:: bash

   extg  [-x <NX>] [-y <NY>] [-z <NZ>] [-i <IX>] [-j <IY>] [-k <IZ>] [--]
            [--version] [-h] <FILE-IN> <FILE-OUT>

Description
------------------------------------

**extg** extract a subregion in `FILE-IN` and write it in `FILE-OUT`.

+----------------+-----------------------------------------------------------+
| option         | description                                               |
+================+===========================================================+
| -ix, -iy, -iz  | Index (counted from 0) of the subregion                   |
+----------------+-----------------------------------------------------------+
| -x, -y, -z     | Size (counted from 0) of the subregion                    |
+----------------+-----------------------------------------------------------+
