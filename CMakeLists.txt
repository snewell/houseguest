cmake_minimum_required(VERSION 3.9)
cmake_policy(VERSION 3.9)

project("houseguest"
    LANGUAGES
        CXX
    VERSION
        0.1.0
)

include(GNUInstallDirs)

option(HOUSEGUEST_BUILD_TESTS "Build houseguest's unit tests" ON)
option(HOUSEGUEST_BUILD_DOCS  "Build houseguest's documentation" ON)

enable_testing()

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)

if(HOUSEGUEST_BUILD_TESTS)
    if(NOT HOUSEGUEST_MAXIMUM_TEST_STANDARD)
        set(HOUSEGUEST_MAXIMUM_TEST_STANDARD 17
            CACHE STRING "The maximum C++ standard to use for tests (default: 17)"
        )
    endif()

    foreach(supported_version IN ITEMS 14 17)
        if(${supported_version} LESS_EQUAL ${HOUSEGUEST_MAXIMUM_TEST_STANDARD})
            message(STATUS "Enabling C++${supported_version} tests")
            list(APPEND test_standards ${supported_version})
        endif()
    endforeach()

    if(NOT test_standards)
        message(WARNING "No supported standards for tests")
    endif()

    find_package(GTest REQUIRED)

    function(create_test_build real_name std_version)
        add_executable(${real_name} ${ARGN})
        target_link_libraries(${real_name} PUBLIC
            houseguest
        )
        target_link_libraries(${real_name} PRIVATE
            GTest::GTest
            GTest::Main
        )
        set_target_properties(${real_name} PROPERTIES
            CXX_EXTENSIONS OFF
        )
        target_compile_features(${real_name} PRIVATE
            cxx_std_${std_version}
        )
    endfunction()

    function(create_test_std test_name std_version)
        set(real_name "${test_name}-${std_version}")
        create_test_build(${real_name} ${std_version} ${ARGN})
        add_test(NAME ${real_name}
            COMMAND ${real_name} "--gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${real_name}_results.xml"
        )
    endfunction()

    function(create_test test_name)
        foreach(std_version IN LISTS test_standards)
            create_test_std(${test_name} ${std_version} ${ARGN})
        endforeach()
    endfunction()

    function(create_failed_build_test_std test_name std_version)
        set(real_name "${test_name}-${std_version}")
        create_test_build(${real_name} ${std_version} ${ARGN})
        add_test(NAME ${real_name}
            COMMAND ${CMAKE_COMMAND} --build . --target ${real_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        set_target_properties(${real_name} PROPERTIES
            EXCLUDE_FROM_ALL TRUE
            EXCLUDE_FROM_DEFAULT_BUILD TRUE
        )
        set_tests_properties(${real_name} PROPERTIES
            WILL_FAIL TRUE
        )
    endfunction()

    function(create_failed_build_test test_name)
        foreach(std_version IN LISTS test_standards)
            create_failed_build_test_std(${test_name} ${std_version} ${ARGN})
        endforeach()
    endfunction()
else()
    function(create_test_std test_name std_version)
    endfunction()
    function(create_test test_name)
    endfunction()
    function(create_failed_build_test_std test_name std_version)
    endfunction()
    function(create_failed_build_test test_name)
    endfunction()
endif()

add_library(houseguest INTERFACE)
target_include_directories(houseguest INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(houseguest INTERFACE
    cxx_std_14
)

set(headers
    bounded_value.hpp
    lock.hpp
    mutex.hpp
    synchronize.hpp
    thread_safe_object.hpp
)
foreach(file IN LISTS headers)
    target_sources(houseguest INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/houseguest/${file}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/houseguest/${file}>
    )
    install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/houseguest/${file}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/houseguest
    )
endforeach()

create_test(synchronize_test
    synchronize_test.cpp
)
create_test(thread_safe_object_test
    thread_safe_object_test.cpp
)
create_test(bounded_value_test
    bounded_value_test.cpp
)

add_subdirectory(bad_bounded_value_tests)

if(HOUSEGUEST_BUILD_DOCS)
    find_program(DOXYGEN "doxygen")
    if(DOXYGEN)
        find_program(DOT "dot")
        if(DOT)
            set(HAVE_DOT "YES")
        else()
            set(HAVE_DOT "NO")
        endif()
        configure_file(doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/doxyfile" @ONLY)

        add_custom_target(docs)
        add_custom_command(TARGET docs
            COMMAND "${DOXYGEN}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        )
    else()
        message(WARNING "Couldn't find doxygen, disabling documentation")
    endif()
endif()

install(TARGETS
        houseguest
    EXPORT HouseguestConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
export(TARGETS
        houseguest
    NAMESPACE Houseguest::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/HouseguestConfig.cmake"
)
install(EXPORT
        HouseguestConfig
    DESTINATION "${CMAKE_INSTALL_DATADIR}/Houseguest/cmake"
    NAMESPACE Houseguest::
)

if(DOXYGEN)
    install(DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/doc/html"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endif()
