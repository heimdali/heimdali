Releasing a new Heimdali version
=================================

Preliminary checks
------------------

Check the conda package can build (on both ``GNU/Linux`` and ``OS X``):

.. code-block:: bash

    conda build conda-recipe/heimdali

Update CHANGELOG
-------------------

Update the changes log file.

Bump version number
-------------------

Update version number ``X.X.X`` in files:

  - `conda-recipe/heimdali/meta.yaml` 
  - `libheimdali/heimdali/version.hxx`

and commit:

.. code-block:: bash

    git add conda-recipe/heimdali/meta.yaml libheimdali/heimdali/version.hxx
    git commit -m 'Bumped version number to X.X.X'

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

Build conda packages
---------------------

Build and upload the Conda packages for ``GNU/Linux`` and ``OS X``.

.. code-block:: bash

    conda build conda-recipe
    binstar upload -u heimdali /path/to/heimdali-X.X.X-X.tar.bz2 # See output of previous command

.. note::

    For portability, ``GNU/Linux`` conda packages are built on ``Centos 5.11``,
    like official conda packages. This can be done on a virtual machine managed
    by `Vagrant`_, using this `box`_ . Conda packages on ``anaconda.org`` are tagged
    with the box version (for example: 0.4.0).

.. _Vagrant: https://www.vagrantup.com/
.. _box: https://github.com/dfroger/conda-build-env
