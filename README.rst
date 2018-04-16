Houseguest
==========
|codacy|

Houseguest is a header-only library that provides some functionality I keep
writing for every project I touch.


Building/Installing
-------------------
Houseguest's build is managed using CMake_; because it's header-only, the build
only manages unit tests and building documentation.

.. code:: bash

    $ mkdir build_dir
    $ cd build_dir
    $ cmake /path/to/houseguest
    $ make && make test
    $ make docs
    $ make install

The following options are supported:

- :code:`HOUSEGUEST_BUILD_TESTS` (defaults to :code:`ON`).  Build unit tests in
  all supported standards.  This option requires GTest_.  GTest (and required
  flags) will be detected using :code:`find_package`.
- :code:`HOUSEGUEST_BUILD_DOCS` (defualts to :code:`ON`).  Add a target
  (:code:`docs`) to generate documentation; documentation will also be
  installed.  This option requires Doxygen_.  Doxygen's path will be detected
  using :code:`find_program`.
- :code:`HOUSEGUEST_MAXIMUM_TEST_STANDARD` (defaults to :code:`17`).  Control
  which C++ standards are used when building tests.  Tests will be built for
  *each* standard supported, up to the maximum standard specified.  Note that
  this option provides no sanity checking, so :code:`14`, :code:`15`, and
  :code:`16` each have the same result (tests will be built for C++14, but no
  later standard).  The default represents the latest standard houseguest
  supports, so setting this option to a higher standard (e.g., :code:`20`) has
  no effect.


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
Right now, I'm only targetting C++14 and later.  Some things will change
depending on the standard (e.g., default types if later standards provide
superior options), but these should be transparent for the most part.  If
compiled using a later standard, additional :code:`static_assert`\ s and
attributes will be leveraged to improve compiler diagnostics.


Supported Compilers
-------------------
Houseguest should be very portable and work with any standards-compliant
compiler.  It's known to work correctly (i.e., compiles without warnings and
all tests pass) with gcc-7.3.0 and clang-6.0.0 (with both libc++ and
libstdc++).  I also run several static analysis tools (cppcheck, clang-tidy,
and clang's static analyzer) and regularly build with clang's sanitizers
(address, thread, and undefined behavior) to try and ferret out portability and
quality issues.

For reference, the warning flags I use are:
:code:`-pipe -Wall -Wextra -Wshadow -Wundef -pedantic`.  These aren't embedded
in :code:`CMakeLists.txt` to avoid portability issues with compilers I don't
have access to (e.g., Visual Studio).


.. |codacy| image:: https://api.codacy.com/project/badge/Grade/af6869e775b64090aa59821a47baa7a5
   :target: https://www.codacy.com/app/snewell/houseguest?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=snewell/houseguest&amp;utm_campaign=Badge_Grade
.. _CMake: https://www.cmake.org
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _GTest: https://github.com/google/googletest
