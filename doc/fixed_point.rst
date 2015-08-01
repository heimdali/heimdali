Fixed point representation
====================================

``Heimdali`` can write pixel values to ``HDF5`` and ``Inrimage`` file using
their floating point representation or using a fixed point representation.

The conversion is explained by the formulaes:

.. math::

    \text{pixel_value} = 
        \text{fixed_point_factor}
            \times
        \frac
            {2^\text{fixed_point_exponent}}
            {2^\text{fixed_point_nbits} - 1}

.. math::

    \text{fixed_point_factor} = round
        \left(
            \text{pixel_value}
                \times
            \frac
                {2^\text{fixed_point_nbits} - 1}
                {2^\text{fixed_point_exponent}}
        \right)

where:

- ``pixel_value`` is a ``float``.
- ``fixed_point_exponent`` is  a ``char`` of values in :math:`[-100, 100]` It
  is a global value for the image.
- ``fixed_point_factor`` is  of type ``fixed_point_type``, and is defined on all pixel.
- ``fixed_point_nbits`` is the number of bits of ``fixed_point_type`` (so number of bytes
  times 8). As all pixel are of the same type, it is a global value for the
  image.
- ``fixed_point_type`` can be:
    -  ``char`` (1 byte).
    -  ``unsigned char`` (1 byte).
    -  ``short`` (2 bytes).
    -  ``unsigned short`` (2 bytes).
    -  ``int`` (4 bytes).
    -  ``unsigned int`` (4 bytes).
- ``round`` convert a floating point to the nearest lower integer.

A file stores the ``fixed_point_exponent`` (one value), and an array of
``fixed_point_factor`` of type ``T``. When the file is read or written, these
values are converted to an array of ``float`` back and forth, and computation
are performed using ``float``.


+-----------------------------------------------------------------------------------+
|                       Examples (fixed_point_exponent is 0)                        |
+-------------+---------------------+--------------------------+--------------------+
| pixel value | fixed_point_type    | 2^pixed_point_nbits -1   | fixed_point_factor |
+=============+=====================+==========================+====================+
| 0.11        | [unsigned] char     | 2^8 - 1 = 255            |                28  |
+-------------+---------------------+--------------------------+--------------------+
| 0.22        | [unsigned] char     | 2^8 - 1 = 255            |                56  |
+-------------+---------------------+--------------------------+--------------------+
| 0.33        | [unsigned] char     | 2^8 - 1 = 255            |                84  |
+-------------+---------------------+--------------------------+--------------------+
| 0.11        | [unsigned] short    | 2^16 - 1 = 65534         |              7209  |
+-------------+---------------------+--------------------------+--------------------+
| 0.22        | [unsigned] short    | 2^16 - 1 = 65534         |             14417  |
+-------------+---------------------+--------------------------+--------------------+
| 0.33        | [unsigned] short    | 2^16 - 1 = 65534         |             21626  |
+-------------+---------------------+--------------------------+--------------------+
| 0.11        | [unsigned] int      | 2^32 - 1 = 4294967294    |         472446400  |
+-------------+---------------------+--------------------------+--------------------+
| 0.22        | [unsigned] int      | 2^32 - 1 = 4294967294    |         944892800  |
+-------------+---------------------+--------------------------+--------------------+
| 0.33        | [unsigned] int      | 2^32 - 1 = 4294967294    |         1417339264 |
+-------------+---------------------+--------------------------+--------------------+
