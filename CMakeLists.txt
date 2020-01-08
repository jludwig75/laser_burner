cmake_minimum_required(VERSION 3.5)

# set the project name
project(parking_sensor LANGUAGES CXX VERSION 2.11.0)

enable_testing()

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