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
   
Install lettuce:

.. code-block:: bash

    pip install lettuce

Activate the conda environment:

.. code-block:: bash

    source activate heimdali-dev
    hash -r

Get test datas
--------------------

Get Heimdali data files:

.. code-block:: bash

    git clone https://github.com/dfroger/heimdali-data

Define directories
--------------------

For convenience, define these directories:

+------------------------+----------------------------------------------------+
| Variable               | Description                                        |
+========================+====================================================+
| `HEIMDALI_SRC_DIR`     | | Heimdali sources (git repo), containing the      |
|                        | | main CMakeLists.txt                              |
+------------------------+----------------------------------------------------+
| `HEIMDALI_DATA_DIR`    | | Heimdali data directory (heimdali-data git repo  |
|                        | | cloned above)                                    |
+------------------------+----------------------------------------------------+
| `HEIMDALI_CONDA_DIR`   | | Where Conda installed dependent libraries,       |
|                        | | for example, `~/miniconda/envs/heimdali-dev`.    |
+------------------------+----------------------------------------------------+
| `HEIMDALI_WORK_DIR`    | | Directory for temporary files (building sources, |
|                        | | building examples, running tests).               |
+------------------------+----------------------------------------------------+

For example:

.. code-block:: bash

    cd heimdali
    export HEIMDALI_SRC_DIR=$PWD

    cd heimdali-data
    export HEIMDALI_DATA_DIR=$PWD

    export HEIMDALI_CONDA_DIR=$(conda info -e | grep '*' | tr -s ' ' | cut -d" " -f3)

    export HEIMDALI_WORK_DIR=/path/to/<heimdali-work-dir>

.. note::

    It may be useful to have ``HEIMDALI_SRC_DIR`` and ``HEIMDALI_WORK_DIR`` if different
    locations. A typical example is having ``HEIMDALI_SRC_DIR`` on a backed-up
    ``NAS`` file system, while ``HEIMDALI_WORK_DIR`` on a local hard disk for speed
    read/write operations.

.. warning::

    The conda environment must be activated and these 4 variables must be
    defined for the sections bellow.

Build Heimdali
--------------------

On Mac OS X your will need to install `/Developer/SDKs/MacOSX10.5`, and use it:

.. code-block:: bash

    export MACOSX_DEPLOYMENT_TARGET=10.5

Build heidmali, asking CMake to search dependances in the Conda environment:

.. note::

    You may want to use ``ccache`` to speed-up re-compiling after cleaning.
    (``conda install ccache``).

+------------------------+----------------------------------------------------+
| Variable               | Description                                        |
+========================+====================================================+
| `CMAKE_PREFIX_PATH`    | | Where `CMake` will search for dependent          |
|                        | | libraries                                        |
+------------------------+----------------------------------------------------+
| `CMAKE_INSTALL_PREFIX` | | Optional. You may want to install `Heimdali` to  |
|                        | | test that `find_package(heimdali)` works.        |
+------------------------+----------------------------------------------------+
| `CMAKE_CXX_COMPILER`   | | If using ``ccache``, points to the symbolic link |
|                        | | to ``ccache``.                                   |
+------------------------+----------------------------------------------------+

.. code-block:: bash

    mkdir -p $HEIMDALI_WORK_DIR/build_debug/src
    cd $HEIMDALI_WORK_DIR/build_debug/src
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH=$HEIMDALI_CONDA_DIR \
        # -DCMAKE_INSTALL_PREFIX=~/tmp/heimdali-install \
        # -DCMAKE_CXX_COMPILER=$HEIMDALI_CONDA_DIR/bin/g++ \ 
        $HEIMDALI_SRC_DIR
    make -j 4

Configure examples
--------------------

As before, the Conda environment is used. Moreover, because Heimdali has been
built in `HEIMDALI_WORK_DIR/build_debug/src` and is not installed (development
mode), we need to specified `Heimdali` path to CMake.

.. code-block:: bash

    for example in create_input_image inrimage_read inrimage_write
    do
        mkdir -p $HEIMDALI_WORK_DIR/build_debug/$example
        cd $HEIMDALI_WORK_DIR/build_debug/$example
        cmake \
            -DCMAKE_BUILD_TYPE=Debug \
            -DHEIMDALI_DIR=$HEIMDALI_WORK_DIR/build_debug/src \
            $HEIMDALI_SRC_DIR/example/$example
    done

Example are built latter by ``lettuce``.

Run functional tests
--------------------

Add path to the built executables:

.. code-block:: bash

    export PATH=$HEIMDALI_WORK_DIR/build_debug/src/cmd:$PATH

Run the functional tests:

.. code-block:: bash

    cd $HEIMDALI_SRC_DIR/tests
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
