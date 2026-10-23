cmake_minimum_required(VERSION 3.24)
project(DoABarrelRollNative VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "DoABarrelRollNative must be built for Windows x64.")
endif()

add_library(DoABarrelRoll SHARED
    src/native/dllmain.cpp
    src/native/signature_scanner.cpp
    src/core/rotation.cpp
)

target_include_directories(DoABarrelRoll PRIVATE src)
target_compile_features(DoABarrelRoll PRIVATE cxx_std_20)
target_compile_definitions(DoABarrelRoll PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
    DOABARRELROLL_VERSION=L"0.1.0"
)

set_target_properties(DoABarrelRoll PROPERTIES
    OUTPUT_NAME "DoABarrelRoll"
    CXX_EXTENSIONS OFF
    MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

if(MSVC)
    target_compile_options(DoABarrelRoll PRIVATE /W4 /permissive- /EHsc /utf-8)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(rotation_tests
        tests/rotation_tests.cpp
        src/core/rotation.cpp
    )
    target_include_directories(rotation_tests PRIVATE src)
    target_compile_features(rotation_tests PRIVATE cxx_std_20)
    set_target_properties(rotation_tests PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(rotation_tests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME rotation_math COMMAND rotation_tests)

    add_executable(signature_scanner_tests
        tests/signature_scanner_tests.cpp
        src/native/signature_scanner.cpp
    )
    target_include_directories(signature_scanner_tests PRIVATE src)
    target_compile_features(signature_scanner_tests PRIVATE cxx_std_20)
    set_target_properties(signature_scanner_tests PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(signature_scanner_tests PRIVATE /W4 /permissive- /EHsc /utf-8)
    endif()
    add_test(NAME signature_pattern_parser COMMAND signature_scanner_tests)
endif()
