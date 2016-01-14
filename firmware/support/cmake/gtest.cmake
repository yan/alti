#### ########################### GTEST
#### # Enable ExternalProject CMake module
#### INCLUDE(ExternalProject)
#### 
#### # Set default ExternalProject root directory
#### SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party)
#### 
#### # Add gtest
#### # http://stackoverflow.com/questions/9689183/cmake-googletest
#### ExternalProject_Add(
####     googletest
####     URL http://googletest.googlecode.com/files/gtest-1.7.0.zip
####     # TIMEOUT 10
####     # # Force separate output paths for debug and release builds to allow easy
####     # # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
####     # CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
####     #            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
####     #            -Dgtest_force_shared_crt=ON
####     # Disable install step
####     INSTALL_COMMAND ""
####     # Wrap download, configure and build steps in a script to log output
####     LOG_DOWNLOAD ON
####     LOG_CONFIGURE ON
####     LOG_BUILD ON)
#### 
#### # Specify include dir
#### ExternalProject_Get_Property(googletest source_dir)
#### set(GTEST_INCLUDE_DIR ${source_dir}/include)
#### 
#### # Library
#### ExternalProject_Get_Property(googletest binary_dir)
#### set(GTEST_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
#### set(GTEST_LIBRARY gtest)
#### add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
#### set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION
####                 ${GTEST_LIBRARY_PATH} )
#### add_dependencies(${GTEST_LIBRARY} googletest)
# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL ${PROJECT_SOURCE_DIR}/support/release-1.7.0.tar.gz # https://googletest.googlecode.com/files/gtest-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

set(GTEST_LIBRARY libgtest)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

# Download and install GoogleMock
## ExternalProject_Add(
##     gmock
##     URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
##     PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
##     # Disable install step
##     INSTALL_COMMAND ""
## )
## 
## # Create a libgmock target to be used as a dependency by test programs
## add_library(libgmock IMPORTED STATIC GLOBAL)
## add_dependencies(libgmock gmock)
## 
## # Set gmock properties
## ExternalProject_Get_Property(gmock source_dir binary_dir)
## set_target_properties(libgmock PROPERTIES
##     "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
##     "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
## #    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
## )
## # I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
## include_directories("${source_dir}/include")

