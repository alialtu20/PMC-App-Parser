cmake_minimum_required(VERSION 3.5)

Project(parser)

set(SOURCES
    main.cpp
    src/AppParser.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${SOURCES})

