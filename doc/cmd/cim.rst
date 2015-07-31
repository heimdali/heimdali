cim
====================================


Example
------------------------------------

Fixed point number on 1 byte
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To write a file using fixed point representation on 1 byte (``char`` or
``unsigned char``), one enter the ``fixed_point_factor`` values:

For example, to write in a file ``fixed_point_uchar.inr`` the pixel values
``0.1``, ``0.2`` and ``0.3`` with fixed point representation on 1 byte
(``uchar``):

+-------------+----------------------+-------------------+--------------------+
| pixel value | fixed_point_exponent | pixed_point_nbits | fixed_point_factor |
+=============+======================+===================+====================+
| 0.1         | 0                    | 8                 | 25                 |
+-------------+----------------------+-------------------+--------------------+
| 0.2         | 0                    | 8                 | 51                 |
+-------------+----------------------+-------------------+--------------------+
| 0.3         | 0                    | 8                 | 76                 |
+-------------+----------------------+-------------------+--------------------+

.. code-block:: bash

    echo 25 51 76 | cim -f -x 3 > fixed_point_uchar.inr

Fixed point number on 2 or 4 bytes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To write a file using fixed point representation on 2 byte (``short`` or
``unsigned short``), or 4 bytes (``int`` or ``unsigned int``),  one enter the
``pixel_value`` values.

For example, to write in a file ``fixed_point_ushort.inr`` the pixel values
``0.1``, ``0.2`` and ``0.3`` with fixed point representation on 2 byte
(``ushort``):

.. code-block:: bash

    echo 0.1 0.2 0.3 | cim -f -o 2 -x 3 > fixed_point_ushort.inr
