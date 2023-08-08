add_executable(${PROJECT_NAME}-interpreter interpreter_main.cpp evaluator.cpp repl.cpp parser.cpp
                               ast.cpp lexer.cpp object.cpp)
target_link_libraries(${PROJECT_NAME}-interpreter PRIVATE fmt::fmt)
target_compile_options(${PROJECT_NAME}-interpreter PRIVATE ${CPP_FLAGS})
target_link_options(${PROJECT_NAME}-interpreter PRIVATE ${CPP_LINKING_OPTS})
