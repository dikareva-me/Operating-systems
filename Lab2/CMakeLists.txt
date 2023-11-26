cmake_minimum_required(VERSION 3.15)
project(lab2)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

find_package (Threads)
find_library (LIBRT rt)

file(GLOB CONNECTIONS src/conn/conn_*.cpp)
file(GLOB HOST src/host/*.cpp src/host/*.hpp)
file(GLOB CLIENT src/client/*.cpp src/client/*.hpp)

foreach(PATH_TO_CON ${CONNECTIONS})
        get_filename_component(FILE_NAME ${PATH_TO_CON} NAME_WLE )
        string(REGEX REPLACE "conn_" "" CONN_NAME ${FILE_NAME})
        add_executable(host_${CONN_NAME} 
                ${HOST}   
                src/conn/IConn.hpp
                src/conn/conn_${CONN_NAME}.cpp )

        add_executable(client_${CONN_NAME}
                ${CLIENT}
                src/conn/IConn.hpp 
                src/conn/conn_${CONN_NAME}.cpp )
        target_link_libraries(host_${CONN_NAME} PUBLIC pthread rt)
        target_link_libraries(client_${CONN_NAME} PUBLIC pthread rt)
endforeach()