cmake_minimum_required(VERSION 3.12)
project(FounzoLF)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenMP REQUIRED)
find_package(RapidJSON REQUIRED)
include_directories(${RAPIDJSON_INCLUDE_DIRS})

find_package(MPI REQUIRED)

include_directories(src)
include_directories(src/Tracker)
include_directories(src/Aruka)
include_directories(src/Aruka/Mono)
include_directories(src/Aruka/OpenMP)
include_directories(src/Aruka/MPI)
include_directories(src/Aruka/Tracker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

if(NOT DEFINED ${BASE_OUTPUT_DIRECTORY})
    set(BASE_OUTPUT_DIRECTORY "output")
endif()

add_library(founzo_lf_lib INTERFACE)
target_include_directories(founzo_lf_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Aruka

    ${MPI_INCLUDE_PATH}
)
target_link_libraries(founzo_lf_lib INTERFACE 
    nlohmann_json::nlohmann_json 
    OpenMP::OpenMP_CXX
    MPI::MPI_CXX
)

add_executable(fullMain fullMain.cpp)
target_link_libraries(fullMain PRIVATE founzo_lf_lib)

add_executable(lightMain lightMain.cpp)
target_link_libraries(lightMain PRIVATE founzo_lf_lib)

add_executable(noMain noMain.cpp)
target_link_libraries(noMain PRIVATE founzo_lf_lib)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

if (NOT Python3_FOUND)
    message(FATAL_ERROR "Python3 n'est pas trouvé sur le système. Veuillez installer Python3.")
endif()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/teste_openmp_multiple_worker.sh
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/teste_openmp_multiple_worker.sh ${CMAKE_BINARY_DIR}/teste_openmp_multiple_worker.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/teste_openmp_multiple_worker.sh
    COMMENT "Copie du script bash vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/teste_mpi_multiple_worker.sh
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/teste_mpi_multiple_worker.sh ${CMAKE_BINARY_DIR}/teste_mpi_multiple_worker.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/teste_mpi_multiple_worker.sh
    COMMENT "Copie du script bash vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/analyse_no_graph.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/analyse_no_graph.py ${CMAKE_BINARY_DIR}/analyse_no_graph.py
    DEPENDS ${CMAKE_SOURCE_DIR}/analyse_no_graph.py
    COMMENT "Copie du script Python vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/compare.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/compare.py ${CMAKE_BINARY_DIR}/compare.py
    DEPENDS ${CMAKE_SOURCE_DIR}/compare.py
    COMMENT "Copie du script Python vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/analyse.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/analyse.py ${CMAKE_BINARY_DIR}/analyse.py
    DEPENDS ${CMAKE_SOURCE_DIR}/analyse.py
    COMMENT "Copie du script Python vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/classSrc.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/classSrc.py ${CMAKE_BINARY_DIR}/classSrc.py
    DEPENDS ${CMAKE_SOURCE_DIR}/classSrc.py
    COMMENT "Copie du script Python vers le répertoire de build"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/template
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/template ${CMAKE_BINARY_DIR}/template
    DEPENDS ${CMAKE_SOURCE_DIR}/template ${CMAKE_SOURCE_DIR}/template/index.jinja2.html ${CMAKE_SOURCE_DIR}/template/index_compare.jinja2.html
    COMMENT "Copie du dossier vers le répertoire de build"
)

add_custom_target(clean_base_output_directory
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BASE_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_OUTPUT_DIRECTORY}/light
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_OUTPUT_DIRECTORY}/full
    COMMENT "Delete everything in the output directory"
)

add_custom_target(run_light_tracker
    DEPENDS lightMain ${CMAKE_BINARY_DIR}/classSrc.py ${CMAKE_BINARY_DIR}/analyse.py ${CMAKE_BINARY_DIR}/template
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/light
    COMMAND $<TARGET_FILE:lightMain> ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/light
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_BINARY_DIR}/analyse.py ${BASE_OUTPUT_DIRECTORY}/light
    COMMENT "Execute les differents programmes avec recuperation des informations"
    VERBATIM 
)

add_custom_target(run_full_no_graph_tracker
    DEPENDS fullMain ${CMAKE_BINARY_DIR}/classSrc.py ${CMAKE_BINARY_DIR}/analyse_no_graph.py ${CMAKE_BINARY_DIR}/template
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/full
    COMMAND $<TARGET_FILE:fullMain> ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/full
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_BINARY_DIR}/analyse_no_graph.py ${BASE_OUTPUT_DIRECTORY}/full
    COMMENT "Execute les differents programmes avec recuperation des informations"
    VERBATIM 
)

add_custom_target(create_output_dirs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output/light
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output/full
)

add_custom_target(all_required_depends
    DEPENDS ${CMAKE_BINARY_DIR}/classSrc.py 
            ${CMAKE_BINARY_DIR}/analyse.py 
            ${CMAKE_BINARY_DIR}/template 
            ${CMAKE_BINARY_DIR}/compare.py 
            ${CMAKE_BINARY_DIR}/teste_openmp_multiple_worker.sh
            ${CMAKE_BINARY_DIR}/teste_mpi_multiple_worker.sh
            create_output_dirs
)

add_custom_target(run_full_tracker
    DEPENDS fullMain ${CMAKE_BINARY_DIR}/classSrc.py ${CMAKE_BINARY_DIR}/analyse.py ${CMAKE_BINARY_DIR}/template
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/full
    COMMAND $<TARGET_FILE:fullMain> ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY}/full
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_BINARY_DIR}/analyse.py ${BASE_OUTPUT_DIRECTORY}/full
    COMMENT "Execute les differents programmes avec recuperation des informations"
    VERBATIM 
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/${BASE_OUTPUT_DIRECTORY})
