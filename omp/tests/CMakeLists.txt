set(BINARY ${CMAKE_PROJECT_NAME}_tests)
set(LIB ${CMAKE_PROJECT_NAME}_lib)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

find_package(OpenMP REQUIRED)
find_package(GTest REQUIRED)
#target_link_libraries(${BINARY} PUBLIC ${LIB} gtest)
target_link_libraries(${BINARY} PUBLIC ${LIB}
                        PRIVATE ${GTEST_LIBRARIES}
                        PRIVATE OpenMP::OpenMP_CXX)
enable_testing()