cmake_minimum_required(VERSION 3.4.1)

add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        src/main/cpp/native-lib.cpp )

target_link_libraries(
        native-lib
        log
)