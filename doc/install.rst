Installation
====================================

The recommanded way to install **Heimdali** is using the
`Conda <http://conda.pydata.org/docs/>`_ package manager.

`Conda` installation 
------------------------------------

Download the Python `2.7` version of `miniconda` corresponding to your plateform from 
`http://conda.pydata.org/miniconda.html <http://conda.pydata.org/miniconda.html>`_

Execute the downloaded file: this will install the
`Conda <http://conda.pydata.org/docs/>`_ package manager.

Add the `heimdali` `binstar` channel to your config:

.. code-block:: bash

  conda config --add channels https://conda.binstar.org/heimdali

**Heimdali** installation
------------------------------------

Install the `heimdali` package in a environment called `heim` or whather name
you want:

.. code-block:: bash

  conda create -n heim heimdali

Activate the environment:

.. code-block:: bash

  source activate heim

Your are ready to use **Heimdali**.

Test **Heimdali** installation
------------------------------------

Download some input data to test:

.. code-block:: bash

  git clone https://github.com/heimdali/heimdali-data

Execute the :ref:`par <cmd-par>` command:

.. code-block:: bash

    par heimdali-data/imtest_z5_y4_x3_c2.h5


**Heimdali** update
------------------------------------

You can update **Heimdali** when a new version is released:

.. code-block:: bash

  conda update heimdali

You may also want to keep the currently installed **Heimdali** version, and
install a new version in another `Conda` environment:

.. code-block:: bash

  conda create -n heim0.1 heimdali==0.1.0

You can now switch between the two version of **Heimdali**:

.. code-block:: bash

  source activate heim

or

.. code-block:: bash

  source activate heim0.1
