set(BINARY ${CMAKE_PROJECT_NAME})
set(LIBRARY ${BINARY}_lib)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})
add_library(${LIBRARY} STATIC ${SOURCES})

find_package(OpenMP REQUIRED)
target_link_libraries(${BINARY} PUBLIC ${LIBRARY} PRIVATE OpenMP::OpenMP_CXX)
