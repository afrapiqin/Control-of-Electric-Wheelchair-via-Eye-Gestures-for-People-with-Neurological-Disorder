cmake_minimum_required(VERSION 3.10)
project(eye_tracker)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenCV 4 REQUIRED)
find_package(dlib REQUIRED)

# Create list of source files
set(SOURCES
    src/main.cpp
    src/eye_tracker.cpp
    src/pupil_tracker.cpp
	src/control_instruction.cpp
    src/utils.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    dlib::dlib
    lapack
    blas
    cblas
)

# Add target-specific include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${DLIB_INCLUDE_DIRS}
)

# Copy res folder to build directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    ${CMAKE_BINARY_DIR}/res
)
