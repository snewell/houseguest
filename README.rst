Houseguest
==========
Houseguest is a header-only library that provides some functionality I keep
writing for every project I touch.


Building/Installing
-------------------
Houseguest's build is managed using CMake_; because its header-only, the build
only manages unit tests.

.. code:: bash

    $ mkdir build_dir
    $ cd build_dir
    $ cmake /path/to/houseguest
    $ make && make test
    $ make install


Supported Standards
-------------------
Right now, I'm only targetting C++14 and later.


.. _CMake: https://www.cmake.org
