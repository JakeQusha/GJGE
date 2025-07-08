set(EXEC_NAME "Demo")
set(SOURCE_FILES src/main.cpp src/templates.cpp src/scenes.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp)
add_executable(${EXEC_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${EXEC_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(${EXEC_NAME} GJGE)
set(MAIN_FLAGS ${COMPILE_FLAGS})
add_custom_command(TARGET ${EXEC_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${EXEC_NAME}>/resources)


target_compile_options(${EXEC_NAME} PRIVATE ${MAIN_FLAGS})
