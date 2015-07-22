.. _ccache_use:

ccache
====================================


You may want to use `ccache`_ to speed-up re-compiling after cleaning.

The best option is to install it in your system (ie, outside the ``conda``
environment).

Download and extract `ccache``.

.. code-block:: bash

    wget http://samba.org/ftp/ccache/ccache-3.2.2.tar.bz2
    tar xvjf ccache-3.2.2.tar.bz2
    cd ccache-3.2.2

Configure:

.. code-block:: bash

    mkdir -p /usr/local/ccache/3.2.2 # for example
    ./configure --prefix=/usr/local/ccache/3.2.2

Build and install:

.. code-block:: bash

    make
    make install

Create symbolic links:

.. code-block:: bash

    for COMP in gcc g++ cc c++
    do
        ln -s /usr/local/ccache/3.2.2/bin/ccache /usr/local/ccache/3.2.2/bin/$COMP  
    done

And add ``/usr/local/ccache/3.2.2/bin`` to your ``PATH``.


And finally, here is an example of `ccache`_ configuration file, in
``$HOME/.ccache/ccache.conf``:

.. code-block:: ini

    max_size = 5.0G
    cache_dir = /local/froger/ccache

.. _ccache: https://ccache.samba.org/
