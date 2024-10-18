cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(GJGE VERSION 0.1 LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)


    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(COMPILE_FLAGS -Wall -Wextra -Wpedantic -Wconversion)
    endif ()

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_definitions(_DEBUG)
    endif ()

    if (MSVC)
        add_compile_options(/W4)
        add_compile_options(/permissive-)
    endif ()
    option(ENABLE_SANITIZERS "Enable sanitizers." ON)
    if (ENABLE_SANITIZER)
        message("- SANITIZERS ENABLED")
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            set(SANITIZER_AVAILABLE_AND_SET
                    TRUE
                    CACHE BOOL "" FORCE)
            mark_as_advanced(SANITIZER_AVAILABLE_AND_SET)
        endif ()

        if (NOT SANITIZER_AVAILABLE_AND_SET)
            message(VERBOSE
                    "The option ENABLE_SANITIZER is set but sanitizers not found.")
        else ()
            set(SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
        endif ()
    endif ()
endif ()
add_subdirectory(external)
add_subdirectory(engine)
add_subdirectory(demo)

