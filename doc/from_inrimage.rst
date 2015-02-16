Converting INRimage simulation code to ITK
==========================================

------------------------------------------------------
Image definition
------------------------------------------------------

The file :libheimdali:`heimdali/itkhelper.hxx` defines the type of an image.

.. doxygentypedef:: Heimdali::PixelFloat

.. doxygenvariable:: Heimdali::ImageDimension

.. doxygentypedef:: Heimdali::ImageFloat

Images are 3-dimensionial, of size `(nz, ny, nx)`.

Images contains `nx * ny * nx` pixels, and each pixel
is a vector of `nv` value of type `float`.

+-------------+----------------------------+
| `nz`        | Number of planes           |
+-------------+----------------------------+
| `ny`        | Number of rows             |
+-------------+----------------------------+
| `nx`        | Number of columns          |
+-------------+----------------------------+
| `nv`        | Number of values per pixel |
+-------------+----------------------------+

Internally, the image is stored in a continous block
of memory, ie is a `float*`. 

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

`itkhelper.hxx` defines the :cpp:func:`CreateImage <Heimdali::CreateImage>`
function to create an image in the temporary program memory. It does not do any
persistant operation on the disk.

For example:

.. code-block:: bash

    Heimdali::ImageFloat::Pointer image = Heimdali::CreateImage(5,5,5,2);

create a image of 3-dimensional image with 5 planes, 5 rows and 5 columns,
where is value is a vector of 2 floats. 

The following code:

.. code-block:: bash

    Heimdali::ImageFloat::Pointer image = Heimdali::CreateImage(5,5);

create a image of 3-dimensional image with 1 planes, 5 rows and 5 columns,
where is value is a vector of 1 float.

See the file :example:`createInputImage.cxx` for a running example.


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
