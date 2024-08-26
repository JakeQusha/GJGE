set(EXEC_NAME "main")
set(SOURCE_FILES src/main.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp)
add_executable(${EXEC_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${EXEC_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
target_compile_definitions(${EXEC_NAME} PRIVATE RESOURCES_DIR="${RESOURCES_DIR}")
target_link_libraries(${EXEC_NAME} GJGE)
set(MAIN_FLAGS ${COMPILE_FLAGS})
add_custom_command(TARGET ${EXEC_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCES_DIR} $<TARGET_FILE_DIR:${EXEC_NAME}>/resources)


target_compile_options(${EXEC_NAME} PRIVATE ${MAIN_FLAGS})