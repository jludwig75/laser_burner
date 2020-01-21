cmake_minimum_required(VERSION 3.5)

# set the project name
project(parking_sensor LANGUAGES CXX VERSION 2.11.0)

include(CTest)
enable_testing()

OPTION( ENABLE_CODECOVERAGE "Enable code coverage testing support" )

if ( ENABLE_CODECOVERAGE )

    if ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
        message( WARNING "Code coverage results with an optimised (non-Debug) build may be misleading" )
    endif ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )

    if ( NOT DEFINED CODECOV_OUTPUTFILE )
        set( CODECOV_OUTPUTFILE cmake_coverage.output )
    endif ( NOT DEFINED CODECOV_OUTPUTFILE )

    if ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )
        set( CODECOV_HTMLOUTPUTDIR coverage_results )
    endif ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )

    if ( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCXX )
        find_program( CODECOV_GCOV gcov )
        find_program( CODECOV_LCOV lcov )
        find_program( CODECOV_GENHTML genhtml )
        add_definitions( -fprofile-arcs -ftest-coverage )
        link_libraries( gcov )
        set( CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} --coverage )
        add_custom_target( coverage_init ALL ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture --initial )
        add_custom_target( coverage ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture COMMAND genhtml -o ${CODECOV_HTMLOUTPUTDIR} ${CODECOV_OUTPUTFILE} )
    endif ( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCXX )

endif (ENABLE_CODECOVERAGE )


# Catch2
add_subdirectory(Catch2)

add_library(unittest STATIC
            test/unittest_main.cpp)
target_link_libraries(unittest Catch2::Catch2)

# laser_burner_sim
add_executable(laser_burner_sim
                test/laser_burner_sim.cpp
                test/sketch.cpp
                test/serial_port.cpp
                test/laser.cpp
                test/head_motors.cpp
                test/serial_interface.cpp
                test/burner_image.cpp

                sketch/burner_protocol_handler.cpp
                sketch/image_piece.cpp
                sketch/image_receiver.cpp
                sketch/image_router.cpp
                sketch/laser_burner.cpp
                sketch/laser_head.cpp
                )

target_include_directories(laser_burner_sim PUBLIC
                           "${PROJECT_SOURCE_DIR}/sketch"
                           "${PROJECT_SOURCE_DIR}/test"
                           "${PROJECT_SOURCE_DIR}/CImg"
                           )

target_compile_definitions(laser_burner_sim PRIVATE
                            cimg_display=0)

target_link_libraries(laser_burner_sim
                        pthread)

# burner_protocol_test
add_executable(burner_protocol_test
                sketch/burner_protocol_handler.cpp

                test/burner_protocol_test.cpp
                test/serial_interface.cpp
                test/serial_port.cpp
                test/exec_process.cpp
                )
target_include_directories(burner_protocol_test PUBLIC
                "${PROJECT_SOURCE_DIR}/sketch"
                "${PROJECT_SOURCE_DIR}/test")
target_link_libraries(burner_protocol_test
                        unittest
                        pthread
                        Catch2::Catch2)
add_test(burner_protocol_test burner_protocol_test)

# image_receiver_test
add_executable(image_receiver_test
                sketch/burner_protocol_handler.cpp
                sketch/image_receiver.cpp
                sketch/image_piece.cpp

                test/image_receiver_test.cpp
                test/mock_serial_interface.cpp
                )
target_include_directories(image_receiver_test PUBLIC
                "${PROJECT_SOURCE_DIR}/sketch"
                "${PROJECT_SOURCE_DIR}/test")
target_link_libraries(image_receiver_test
                        unittest
                        pthread
                        Catch2::Catch2)
add_test(image_receiver_test image_receiver_test)


# burner_script_to_image_burner_test
add_executable(burner_script_to_image_burner_test
                sketch/burner_protocol_handler.cpp
                sketch/image_receiver.cpp
                sketch/image_piece.cpp
                sketch/image_router.cpp

                test/serial_port.cpp
                test/serial_interface.cpp
                test/burner_script_to_image_burner_test.cpp
                test/image_building_burner.cpp
                test/image_builder.cpp
                test/exec_process.cpp
                )
target_include_directories(burner_script_to_image_burner_test PUBLIC
                "${PROJECT_SOURCE_DIR}/sketch"
                "${PROJECT_SOURCE_DIR}/test"
                "${PROJECT_SOURCE_DIR}/CImg"
                )
target_compile_definitions(burner_script_to_image_burner_test PRIVATE
                cimg_display=0)
target_link_libraries(burner_script_to_image_burner_test
                        unittest
                        pthread
                        Catch2::Catch2)
add_test(burner_script_to_image_burner_test burner_script_to_image_burner_test)


add_executable(make-gradient-image
                make-gradient-img.cpp
                )
target_include_directories(make-gradient-image PUBLIC
                "${PROJECT_SOURCE_DIR}/CImg"
                )
target_compile_definitions(make-gradient-image PRIVATE
                cimg_display=0)


# laser_burner_sim_test
add_test(laser_burner_sim_test
            python3 ${PROJECT_SOURCE_DIR}/test/laser_burner_sim_test.py -s ${PROJECT_SOURCE_DIR})