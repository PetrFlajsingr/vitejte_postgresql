cmake_minimum_required(VERSION 3.17)
project(vitejte_postgresql_service)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(FMT_HEADER_ONLY)

set(SOURCES
        lib_interface/Communicator.cpp
        main.cpp
        savers.cpp
        Service.cpp
        DeviceFinder.cpp
        )

set(HEADERS
        lib_interface/Communicator.h
        lib_interface/Finally.h
        lib_interface/LibInterface.h
        lib_interface/VitejteLibTypes.h
        global_logger.h
        utils.h
        savers.h
        Service.h
        DeviceFinder.h
        )

include_directories(include)

set(PQXX include/pqxx)

find_library(PQXX_LIB pqxx)




add_executable(vitejte_postgresql_service ${SOURCES})
#set_target_properties(vitejte_postgresql_service PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(vitejte_postgresql_service ${PQXX_LIB})