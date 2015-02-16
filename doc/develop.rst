Build Heimdali in development mode
====================================

Create a `conda` enviromnent named `heimdali-dev` containing all dependencies:

.. code-block:: bash

    conda config --add channels http://conda.binstar.org/dfroger
    conda create -n heimdali-dev h5unixpipe itk-heimdali inrimage tclap cmake pip
   
Install lettuce:

.. code-block:: bash

    source activate heimdali-dev
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
| `..`                   | Path to Heimdali main CMakeLists.txt               |
+------------------------+----------------------------------------------------+


.. code-block:: bash

    mkdir build
    cd build
    source activate heimdali-dev
    CONDA_ENV_PATH=$(conda info -e | grep '*' | tr -s ' ' | cut -d" " -f3)
    cmake -DCMAKE_PREFIX_PATH=$CONDA_ENV_PATH ..
    make

Add path to the built executables:

.. code-block:: bash

    cd build
    export PATH=$PWD/cmd:$PATH

Execute the functional tests:

.. code-block:: bash

    cd tests
    lettuce

Writting documentation
====================================


Install Sphinx_ and Doxygen_, and the breathe_ extension:

.. code-block:: bash

    conda create -n heimdali-doc sphinx pip
    source activate heimdali-doc
    hash -r
    sudo apt-get install doxygen
    pip install breathe

Build the documentation:

.. code-block:: bash
    
    cd doc
    make html

View the documentation:

.. code-block:: bash

    cd doc
    firefox _build/html/index.html

.. _Sphinx: http://sphinx-doc.org/
.. _Doxygen: www.doxygen.org/
.. _breathe: https://breathe.readthedocs.org
