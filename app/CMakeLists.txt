set(pathToProject C:\\Users\\wuyongxiang\\Documents\\LazySnapping-Android)
set(pathToOpenCv C:\\OpenCV-android-sdk)

cmake_minimum_required(VERSION 3.6.0)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

include_directories(${pathToOpenCv}/sdk/native/jni/include)

add_library(lib_opencv STATIC IMPORTED )

set_target_properties(lib_opencv
                       PROPERTIES
                       IMPORTED_LOCATION
                       ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so
                       )

add_library( LazySnapping_Android
             SHARED
              src/main/cpp/native-lib.cpp
              src/main/cpp/Lazy-Snapping.cpp
              src/main/cpp/SimpleKMeans.cpp
              src/main/cpp/maxflow-v3.01/graph.cpp
              src/main/cpp/maxflow-v3.01/maxflow.cpp
               )

target_link_libraries( LazySnapping_Android log android -ljnigraphics lib_opencv)