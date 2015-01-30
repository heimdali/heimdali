Build Heimdali in development mode
====================================

Create a `conda` enviromnent named `heimdali` containing all dependencies:

.. code-block:: bash

    conda config --add channels http://conda.binstar.org/dfroger
    conda create -n heimdali h5unixpipe itk tclap cmake pip
   
Install lettuce:

.. code-block:: bash

    source activate heimdali
    hash -r
    pip install lettuce

Get Heimdali data files, and set `HEIMDALI_DATA_DIR`:

.. code-block:: bash

    git clone https://github.com/dfroger/heimdali-data
    export HEIMDALI_DATA_DIR=/path/to/heimdali-data

Build heidmali:


+------------------------+----------------------------------------------------+
| variable               |    meaning                                         |
+========================+====================================================+
| `CONDA_ENV_PATH`       | For example, `~/miniconda/envs/heimdali`           |
+------------------------+----------------------------------------------------+
| `CMAKE_PREFIX_PATH`    | Where `CMake` will search for dependent libraries  |
+------------------------+----------------------------------------------------+
| `CMAKE_INSTALL_PREFIX` | Where `CMake` will install `Heimdali`              |
+------------------------+----------------------------------------------------+
| `..`                   | Path to Heimdali main CMakeLists.txt               |
+------------------------+----------------------------------------------------+


.. code-block:: bash

    mkdir build
    cd build
    source activate heimdali
    CONDA_ENV_PATH=$(conda info -e | grep '*' | tr -s ' ' | cut -d" " -f3)
    cmake \
        -DCMAKE_PREFIX_PATH=$CONDA_ENV_PATH \
        -DCMAKE_INSTALL_PREFIX=$PWD/../install \
        ..
    make
    make install

Set up environment to search executables and libraries installed by `CMake`
in `heimdali/install` (read the few notes in the script
`develop_heimdali_bash.sh`):

.. code-block:: bash

    source script/develop_heimdali_bash.sh

Execute the functional tests:

.. code-block:: bash

    cd tests
    lettuce
