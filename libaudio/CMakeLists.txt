cmake_minimum_required(VERSION 3.4.1)
file(GLOB native_sound_touch "src/main/cpp/soundtouch/SoundTouch/*.cpp")

include_directories(src/main/cpp/include)
include_directories(src/main/cpp/soundtouch/include)
include_directories(src/main/cpp/soundtouch/SoundTouch)

message(" add   native-lib")
message(${ANDROID_ABI})
message(${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavcodec.so)

add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        src/main/cpp/native-lib.cpp
        src/main/cpp/JavaListener.cpp
        src/main/cpp/WlAudio.cpp
        src/main/cpp/WlCallJava.cpp
        src/main/cpp/WlFFmpeg.cpp
        src/main/cpp/WlQueue.cpp
        src/main/cpp/WlPlayState.cpp
        src/main/cpp/TestSLES.cpp
        src/main/cpp/WlPcmBean.cpp
        src/main/cpp/WlBufferQueue.cpp

        src/main/cpp/WlVideo.cpp

        ${native_sound_touch}
        )


add_library(avcodec-58
        SHARED
        IMPORTED)
set_target_properties(avcodec-58
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavcodec-58.so)

add_library(avfilter-7
        SHARED
        IMPORTED)
set_target_properties(avfilter-7
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavfilter-7.so)
add_library(avformat-58
        SHARED
        IMPORTED)
set_target_properties(avformat-58
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavformat-58.so)
add_library(avutil-56
        SHARED
        IMPORTED)
set_target_properties(avutil-56
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavutil-56.so)
add_library(swresample-3
        SHARED
        IMPORTED)
set_target_properties(swresample-3
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswresample-3.so)
add_library(swscale-5
        SHARED
        IMPORTED)
set_target_properties(swscale-5
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswscale-5.so)
add_library(postproc-55
        SHARED
        IMPORTED)
set_target_properties(postproc-55
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-55.so)


target_link_libraries(
        native-lib
        avcodec-58
        avfilter-7
        avformat-58
        avutil-56
        swresample-3
        swscale-5
        postproc-55
        # 链接 android ndk 自带的一些库
        android
        log
        OpenSLES
        ${log-lib}  ${android-lib}
)