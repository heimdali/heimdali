Build Heimdali in development mode
====================================

Development mode means build Heimdali from its source code, typically the
`develop` branch of heimdali_ Git repository.

Working in development mode consists in iterating in the cycle:

  - Modify source code.
  - Build.
  - Run the test.

without having to run the `make install` step.

Install dependencies
--------------------

Create a `conda` enviromnent named `heimdali-dev` containing all dependencies:

.. code-block:: bash

    conda config --add channels http://conda.binstar.org/dfroger
    conda create -n heimdali-dev h5unixpipe itk-heimdali-dbg libinrimage-dbg tclap cmake pip pexpect

.. warning::

    Do not install the `heimdali` package in the `heimdali-dev` environment,
    as it would conflicts with sources files (from your heimdali git
    repository) you are building.

For the rest of the section, we need to activate the conda environment, and
set the CONDA_ENV_PATH environment variable:

.. code-block:: bash

    source activate heimdali-dev
    hash -r
    CONDA_ENV_PATH=$(conda info -e | grep '*' | tr -s ' ' | cut -d" " -f3)
   
Install lettuce:

.. code-block:: bash

    pip install lettuce

Build Heimdali
--------------------

Build heidmali, asking CMake to search dependances in the Conda environment:

+------------------------+----------------------------------------------------+
| variable               |    meaning                                         |
+========================+====================================================+
| `CONDA_ENV_PATH`       | For example, `~/miniconda/envs/heimdali-dev`       |
+------------------------+----------------------------------------------------+
| `CMAKE_PREFIX_PATH`    | Where `CMake` will search for dependent libraries  |
+------------------------+----------------------------------------------------+
| `..`                   | Path to Heimdali main CMakeLists.txt               |
+------------------------+----------------------------------------------------+

On Mac OS X your will need to install `/Developer/SDKs/MacOSX10.5`, and use it:

.. code-block:: bash

    export MACOSX_DEPLOYMENT_TARGET=10.5

`CMAKE_INSTALL_PREFIX` is optinal. You may want to install `Heimdali` to test
that `find_package(Heimdali)` works.

.. code-block:: bash

    cd heimdali
    mkdir build; cd build
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH=$CONDA_ENV_PATH \
        -DCMAKE_INSTALL_PREFIX=~/tmp/heimdali-install \
        ..
    make

Configure examples
--------------------

As before, the Conda environment is used. Moreover, because Heimdali has been
built in `heimdali/build` and is not installed (development mode), we need to
specified all paths to CMake.

.. code-block:: bash

    cd heimdali
    HEIMDALI_ROOT=$PWD
    [ `uname` == 'Darwin' ] && EXT=dylib || EXT=so
    cd example
    mkdir build; cd build
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH=$CONDA_ENV_PATH \
        -DHEIMDALI_INCLUDE=$HEIMDALI_ROOT/libheimdali \
        -DITKINRIMAGEIO_INCLUDE=$HEIMDALI_ROOT/itkINRimageIO/include \
        -DHEIMDALI_LIBRARY=$HEIMDALI_ROOT/build/libheimdali/libheimdali.$EXT \
        -DITKINRIMAGEIO_LIBRARY=$HEIMDALI_ROOT/build/itkINRimageIO/libitkINRImageIO.$EXT \
        ..

Run functional tests
--------------------

Get Heimdali data files, and set `HEIMDALI_DATA_DIR`:

.. code-block:: bash

    git clone https://github.com/dfroger/heimdali-data
    export HEIMDALI_DATA_DIR=/path/to/heimdali-data

Add path to the built executables:

.. code-block:: bash

    cd heimdali
    export PATH=$PWD/build/cmd:$PATH

Specifiy a directory where test files will be created:

.. code-block:: bash

    export HEIMDALI_WORK_DIR=/path/to/heimdali/work/dir


Run the functional tests:

.. code-block:: bash

    cd tests
    lettuce

Writting documentation
====================================


Install Sphinx_ and Doxygen_:

.. code-block:: bash

    sudo apt-get install doxygen
    conda create -n heimdali-doc python=2 sphinx sphinx_rtd_theme
    source activate heimdali-doc

Build the documentation:

.. code-block:: bash
    
    cd doc
    make html

View the documentation:

.. code-block:: bash

    cd doc
    firefox _build/html/index.html

Note that breathe_, a Sphinx extension, is already provided in
`heimdali/doc/ext/breathe`.

Dependencies
====================================

Here is a summary of Heimdali dependencies, if you want to apply modifications on
it:

+-----------------------------+------------------------+
| sources or homepage         | conda recipe           |
+=============================+========================+
| heimdali_                   | `heimdali recipe`_     |
+-----------------------------+------------------------+
| itk-heimdali_               | `itk-heimdali recipe`_ |
+-----------------------------+------------------------+
| tclap_                      | `tclap recipe`_        |
+-----------------------------+------------------------+
| h5unixpipe_                 | `h5unixpipe recipe`_   |
+-----------------------------+------------------------+
| libinrimage_                | `libinrimage recipe`_  |
+-----------------------------+------------------------+

Conda packages are hosted on `binstar dfroger channel`_.

.. _Sphinx: http://sphinx-doc.org/
.. _Doxygen: www.doxygen.org/
.. _breathe: https://breathe.readthedocs.org
.. _heimdali: https://github.com/dfroger/heimdali
.. _heimdali recipe: https://github.com/dfroger/heimdali/tree/master/conda-recipe
.. _itk-heimdali: https://github.com/dfroger/itk/tree/heimdali
.. _itk-heimdali recipe: https://github.com/dfroger/df-conda-recipe/tree/master/itk-heimdali
.. _tclap: http://tclap.sourceforge.net/
.. _tclap recipe: https://github.com/dfroger/df-conda-recipe/tree/master/tclap
.. _h5unixpipe: https://github.com/dfroger/h5unixpipe
.. _h5unixpipe recipe: https://github.com/dfroger/h5unixpipe/tree/master/conda
.. _libinrimage: http://inrimage.gforge.inria.fr
.. _libinrimage recipe: https://github.com/dfroger/df-conda-recipe/tree/master/libinrimage
.. _binstar dfroger channel: https://binstar.org/dfroger 
