cmake_minimum_required(VERSION 2.8.9)

project(task1.out)
include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} main1.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(task1.out m)
#----------------------------------------------------------------
project(task1_test.out)
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(${PROJECT_NAME} tests/main1.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(${PROJECT_NAME} check subunit m rt pthread)
#================================================================
project(task2.out)
include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} main2.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(task2.out m)
#----------------------------------------------------------------
project(task2_test.out)
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(${PROJECT_NAME} tests/main2.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(${PROJECT_NAME} check subunit m rt pthread)
#================================================================
project(task3.out)
include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} main3.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(task3.out m)
#----------------------------------------------------------------
project(task3_test.out)
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(${PROJECT_NAME} tests/main3.c ${SOURCES})
SET(CMAKE_C_FLAGS "-g -std=c11 -Wall -pedantic-errors -Werror -Wno-unused")
target_link_libraries(${PROJECT_NAME} check subunit m rt pthread)




