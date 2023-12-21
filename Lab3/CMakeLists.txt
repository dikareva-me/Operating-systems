cmake_minimum_required(VERSION 3.10)
project(Lab3)

set(INCLUDE include)
set(SRC src)
set(TEST test)

add_compile_options(-Wall -Werror)

include_directories(${INCLUDE})

add_executable(${PROJECT_NAME} ${TEST}/main.cpp ${INCLUDE}/iset.h
                               ${INCLUDE}/fine_grained_set.h ${SRC}/fine_grained_set.cpp
                               ${INCLUDE}/set_tester.h ${SRC}/set_tester.cpp)

target_link_libraries(${PROJECT_NAME} rt pthread)
