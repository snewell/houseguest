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

if(HOUSEGUEST_BUILD_TESTS)
    find_package(GTest REQUIRED)
    function(create_test test_name)
        foreach(std_version IN ITEMS 14 17)
            set(real_name "${test_name}-${std_version}")
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
            add_test(NAME ${real_name}
                COMMAND ${real_name} "--gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${real_name}_results.xml"
            )
        endforeach()
    endfunction()
else()
    function(create_test test_name)
    endfunction()
endif()

add_library(houseguest INTERFACE)
target_include_directories(houseguest INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(headers
    mutex.hpp
    synchronize.hpp
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
