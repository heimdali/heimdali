.. _cmd-tpr:

tpr
====================================

**tpr** prints the pixel values of a image subregion

Synopsis
------------------------------------

.. code-block:: bash

   tpr  [-x <NX>] [-y <NY>] [-z <NZ>] [-i <IX>] [-j <IY>] [-k <IZ>] [--]
           [--version] [-h] <IMAGE> ...

Description
------------------------------------

**tpr** prints on standard output pixel values of `IMAGE` given in argument.

+----------------+-----------------------------------------------------------+
| option         | description                                               |
+================+===========================================================+
| -ix, -iy, -iz  | Index (counted from 0) of the subregion                   |
+----------------+-----------------------------------------------------------+
| -x, -y, -z     | Size (counted from 0) of the subregion                    |
+----------------+-----------------------------------------------------------+
