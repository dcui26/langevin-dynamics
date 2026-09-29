cmake_minimum_required(VERSION 3.18)
project(langevin_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)
find_package(pybind11 REQUIRED)

add_library(langevin_core src/simulator.cpp)
set_target_properties(langevin_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(langevin_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(langevin_core PUBLIC Eigen3::Eigen)

pybind11_add_module(langevin_sim bindings/bindings.cpp)
target_link_libraries(langevin_sim PRIVATE langevin_core)

add_custom_command(TARGET langevin_sim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:langevin_sim>
        ${CMAKE_SOURCE_DIR}/webapp/
)