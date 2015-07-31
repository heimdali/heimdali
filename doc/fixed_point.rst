Fixed point representation
====================================

``Heimdali`` can write pixel values to ``HDF5`` and ``Inrimage`` file using
their floating point representation or using a fixed point representation.

The conversion is explained by the formulae:

.. math::

    \text{pixel_value} = round 
        \left( 
            2^\text{fixed_point_exponent}
            \times
            \frac
                {\text{fixed_point_factor}}
                {2^\text{fixed_point_nbits}-1}
        \right)


where:

- ``pixel_value`` is a ``float``.
- ``fixed_point_exponent`` is  a ``char`` of values in :math:`[-100, 100]` It
  is a global value for the image.
- ``fixed_point_factor`` is  of type ``T``, and is defined on all pixel.
- ``fixed_point_nbits`` is the number of bits of ``T`` (so number of bytes
  times 8). As all pixel are of the same type, it is a global value for the
  image.
- ``T`` can be:
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
