include_directories(../wiiuse/src)
add_executable(llmtester tester.c)
target_link_libraries(llmtester wiiuse)
