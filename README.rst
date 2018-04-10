Houseguest
==========
|codacy|

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


Using
-----
The simplest way to include houseguest in a project is using CMake_.  First,
add the appropriate :code:`find_package` command, then include
:code:`houseguest` in any :code:`target_link_libraries` declarations.

.. code:: CMake

    find_package(Houseguest REQUIRED)
    add_executable(foo foo.cpp)
    target_link_libraries(foo PRIVATE Houseguest::houseguest)
    add_library(bar bar.cpp)
    target_link_libraries(bar PRIVATE Houseguest::houseguest)

Provided :code:`find_package` worked, :code:`target_link_libraries` will handle
the necessary include paths.  It *will not* provide any necessary standard
flags (e.g., :code:`std=c++17`) since houseguest is designed to support
multiple standards; add those if your compiler defaults to a standard earlier
than C++14.


Supported Standards
-------------------
Right now, I'm only targetting C++14 and later.


.. |codacy| image:: https://api.codacy.com/project/badge/Grade/af6869e775b64090aa59821a47baa7a5
   :target: https://www.codacy.com/app/snewell/houseguest?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=snewell/houseguest&amp;utm_campaign=Badge_Grade
.. _CMake: https://www.cmake.org
