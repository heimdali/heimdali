Converting INRimage simulation code to ITK
==========================================

------------------------------------------------------
Building
------------------------------------------------------

------------------------------------------------------
Proving command line interface
------------------------------------------------------

------------------------------------------------------
Creating image
------------------------------------------------------

.. doxygenfunction:: Heimdali::CreateImage

The :cpp:func:`CreateImage <Heimdali::CreateImage>` function create an image in
the temporary program memory, but does not do any persistant operation on the disk.

------------------------------------------------------
Read and writting image
------------------------------------------------------

Working with the whole image
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Working line by line
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Working plane by plane
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

------------------------------------------------------
Modifiyting image
------------------------------------------------------

Using C pointer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Using wrapped C pointer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Using GetPixel, SetPixel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Using ITK iterators
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Using ITK convolution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Using ITK filter
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
