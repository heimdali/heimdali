Artithmetic operations
====================================

Arithmetic operations between two images element by element.

Synopsis
------------------------------------

.. code-block:: bash

  ad image0-in image1-in [image-out]
  so image0-in image1-in [image-out]
  mu image0-in image1-in [image-out]
  di image0-in image1-in [image-out]
  min image0-in image1-in [image-out]
  max image0-in image1-in [image-out]

Description
------------------------------------

All of these commands perform an operation between `image0-in` and `image1-in` and write
result to `image-out`. If argument `image0-in` or `image1-in` is equal to `-`, the command
reads on standard input. If argument `image-out` is absent, the command writes to standard
output.

+----------------+-----------------------------------------------------------+
| command        | description                                               |
+================+===========================================================+
| ad             | Add two images                                            |
+----------------+-----------------------------------------------------------+
| so             | Substract two images                                      |
+----------------+-----------------------------------------------------------+
| mu             | Multiply two images                                       |
+----------------+-----------------------------------------------------------+
| div            | Divide two images                                         |
+----------------+-----------------------------------------------------------+
| min            | Compute minimum of two images                             |
+----------------+-----------------------------------------------------------+
| max            | Compute maximum of two images                             |
+----------------+-----------------------------------------------------------+

All operation are performed on pixels element by element.

Description
------------------------------------

See also :ref:`local_arithmetic`.
