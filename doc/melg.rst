.. _cmd-melg:

melg
====================================

**melg** mix two images, testing on pixel values.

Synopsis
------------------------------------

.. code-block:: bash

   melg  [--idv <IDV>] [--idx <IDX>] [--idy <IDY>] [--idz <IDZ>] [--ivo
         <IXO>] [--ixo <IXO>] [--iyo <IYO>] [--izo <IZO>] [--ivi <IXI>]
         [--ixi <IXI>] [--iyi <IYI>] [--izi <IZI>] [--] [--version] [-h]
         <IMAGE-IN> <IMAGE-OUT

Description
------------------------------------

**melg** replace pixels of a subregion `IMAGE-OUT` with pixels of `IMAGE-IN`.

+-------------------+-----------------------------------------------------------+
| option            | description                                               |
+===================+===========================================================+
| -ixi, -iyi, -izi  | Index (counted from 0) of `IMAGE-IN` subregion            |
+-------------------+-----------------------------------------------------------+
| -ixo, -iyo, -izo  | Index (counted from 0) of `IMAGE-OUT` subregion           |
+-------------------+-----------------------------------------------------------+
| -idx, -idy, -idz  | Size of subregion                                         |
+-------------------+-----------------------------------------------------------+
