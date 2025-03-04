add_library(hashx_sass STATIC
    sass/compiler.cpp
    sass/translator.cpp
)

# Test executable
add_executable(miner-tests
    tests/test_sass.cpp
)

target_include_directories(hashx_sass PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/rNdN/include
    ${CMAKE_SOURCE_DIR}/thirdparty/equix/include
    ${CMAKE_SOURCE_DIR}/thirdparty/equix/hashx/include
    ${CMAKE_SOURCE_DIR}/thirdparty/equix/hashx/src 
    ${CUDA_INCLUDE_DIRS}
)

target_link_libraries(hashx_sass PRIVATE
    rNdN_static
    ${CUDA_LIBRARIES}
)

target_link_libraries(miner-tests PRIVATE
    hashx_sass
    equix_static
    hashx_static
    GTest::gtest
    GTest::gtest_main
)

enable_testing()
add_test(NAME miner-tests COMMAND miner-tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 