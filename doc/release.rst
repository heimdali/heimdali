Releasing a new Heimdali version
=================================

Preliminary checks
------------------

Check the conda package can build (on both GNU/Linux and OS X):

.. code-block:: bash

    conda build conda-recipe

Update CHANGELOG
-------------------

Update the changes log file.

Bump version number
-------------------

Update version number X.X.X in `conda-recipe/meta.yaml` and `libheimdali/heimdali/version.hxx`
and commit:

.. code-block:: bash

    git add conda-recipe/meta.yaml libheimdali/heimdali/version.hxx
    git commit -m 'Update version number

Tag the new version:

.. code-block:: bash

    git tag X.X.X

Merge into master
-------------------

Merge develop into master:

.. code-block:: bash

    git checkout master
    git merge develop

Push to GitHub, with tags:

.. code-block:: bash

    git push origin --tags develop
    git push origin master

Build conda package
---------------------

Build and upload the Conda package.

.. code-block:: bash

    conda build conda-recipe
    binstar upload -u heimdali /path/to/heimdali-X.X.X-X.tar.bz2 # See output of previous command

.. note::

    For portability, GNU/Linux conda packages are built on Centos OS 5.11, like
    official conda packages. A virtual machine is used managed by `Vagrant`_,
    using this `box`_ . Conda packages on anaconda.org are tagged with the
    box version (for example: 0.4.0).

.. _Vagrant: https://www.vagrantup.com/
.. _box: https://github.com/dfroger/conda-build-env
