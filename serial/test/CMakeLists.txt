find_package(GTest REQUIRED)
add_executable(
        unitTests
        matrix.cpp
        main.cpp
)
target_link_libraries(
        unitTests
        PRIVATE ${GTEST_LIBRARIES}
)

add_test(NAME testOne COMMAND unitTests)
enable_testing()
