Check the conda package can build:

.. code-block:: bash

    conda build conda-recipe

Update version number X.X.X in `conda-recipe/meta.yaml` and `libheimdali/heimdali/version.hxx`

Commit:

.. code-block:: bash

    git add conda-recipe/meta.yaml libheimdali/heimdali/version.hxx
    git commit -m 'Update version number

Tag.

.. code-block:: bash

    git tag X.X.X

Merge develop into master

.. code-block:: bash

    git checkout master
    git merge develop

Push with tags.

.. code-block:: bash

    git push origin --tags develop
    git push origin master


In `conda-recipe/meta.yaml` source section, comment `path` and uncomment `fn`
and `url` (do not commit).

Build and upload the conda package.

.. code-block:: bash

    conda build conda-recipe
    binstar upload -u heimdali /path/to/heimdali-X.X.X-X.tar.bz2 # See output of previous command
