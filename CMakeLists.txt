include_directories(../wiiuse/src)
add_executable(llmtester.o tester.c)
target_link_libraries(llmtester.o wiiuse)
