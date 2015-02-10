.. _local_arithmetic:

Local arithmetic operations
====================================

Synopsis
------------------------------------

.. code-block:: bash

  bi -n value [image-in] [image-out]
  sc -n coeff [image-in] [image-out]
  sd -n coeff [image-in] [image-out]
  lo value [image-in] [image-out]
  exp [image-in] [image-out]
  ra [image-in] [image-out]
  sba -n threshold [image-in] [image-out]
  sha -n threshold [image-in] [image-out]
  mb -n threshold [image-in] [image-out]
  mh -n threshold [image-in] [image-out]
  mo [image-in] [image-out]
  car [image-in] [image-out]
  vb -n threshold value [image-in] [image-out]
  vh -n threshold value [image-in] [image-out]

Description
------------------------------------

All of these commands perform an operation on `image-in` (which can be of any
type), and write result in `image-out`.

`image-in` and `image-out` must have the same dimension.

If argument `image-in` is absent or egal to `-`, the command reads on standard
input.

If argument `image-out` is absent, the command writes on standard output.

Values passed with `-n` are `float`.

+----------------+-----------------------------------------------------------+
| command        | description                                               |
+================+===========================================================+
| bi             | Add `value` to each pixel.                                |
+----------------+-----------------------------------------------------------+
| sc             | Multiply each pixel by `coeff`.                           |
+----------------+-----------------------------------------------------------+
| sd             | Divied each pixel by `coeff`.                             |
+----------------+-----------------------------------------------------------+
| lo             | Compute logarithm of each pixel.                          |
+----------------+-----------------------------------------------------------+
| exp            | Compute exponential of each pixel.                        |
+----------------+-----------------------------------------------------------+
| ra             | Compute square root of each pixel.                        |
+----------------+-----------------------------------------------------------+
| sba            | Every pixel inferior or egual to `threshold`              |
|                | is replaced by `threshold`.                               |
+----------------+-----------------------------------------------------------+
| sha            | Every pixel greater or egual to `threshold`               |
|                | is replaced by `threshold`.                               |
+----------------+-----------------------------------------------------------+
| mb             | Every pixel inferior or egual to `threshold`              |
|                | is replaced by 1 and others by 0.                         |
+----------------+-----------------------------------------------------------+
| mh             | Every pixel greater or egual to `threshold`               |
|                | is replaced by 1 and others by 0.                         |
+----------------+-----------------------------------------------------------+
| mo             | Compute each pixel modulo                                 |
+----------------+-----------------------------------------------------------+
| car            | Compute each pixel square                                 |
+----------------+-----------------------------------------------------------+
| vb             | Every pixel inferior or egual to `threshold`              |
|                | is replaced by `value`.                                   |
+----------------+-----------------------------------------------------------+
| vh             | Every pixel greater or egual to `threshold`               |
|                | is replaced by `value`.                                   |
+----------------+-----------------------------------------------------------+
