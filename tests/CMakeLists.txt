set(lexer_sources   lexer_test.cpp
                    ../src/interpreter/lexer.cpp)

set(parser_sources  parser_test.cpp
                    ../src/interpreter/lexer.cpp
                    ../src/interpreter/parser.cpp
                    ../src/interpreter/ast.cpp)

set(ast_sources     ast_test.cpp
                    ../src/interpreter/lexer.cpp
                    ../src/interpreter/parser.cpp
                    ../src/interpreter/ast.cpp)

set(eval_sources    evaluator_test.cpp
                    ../src/interpreter/lexer.cpp
                    ../src/interpreter/parser.cpp
                    ../src/interpreter/ast.cpp
                    ../src/interpreter/evaluator.cpp
                    ../src/interpreter/object.cpp)

add_executable(lexer_tests ${lexer_sources})
add_executable(parser_tests ${parser_sources})
add_executable(ast_tests ${ast_sources})
add_executable(eval_tests ${eval_sources})

target_link_libraries(lexer_tests PRIVATE Catch2::Catch2WithMain fmt::fmt)
target_link_libraries(parser_tests PRIVATE Catch2::Catch2WithMain fmt::fmt)
target_link_libraries(ast_tests PRIVATE  Catch2::Catch2WithMain fmt::fmt)
target_link_libraries(eval_tests PRIVATE Catch2::Catch2WithMain fmt::fmt)

target_compile_options(lexer_tests PRIVATE ${CPP_FLAGS})
target_compile_options(parser_tests PRIVATE ${CPP_FLAGS})
target_compile_options(ast_tests PRIVATE ${CPP_FLAGS})
target_compile_options(eval_tests PRIVATE ${CPP_FLAGS})

target_link_options(lexer_tests PRIVATE ${CPP_LINKING_OPTS})
target_link_options(parser_tests PRIVATE ${CPP_LINKING_OPTS})
target_link_options(ast_tests PRIVATE ${CPP_LINKING_OPTS})
target_link_options(eval_tests PRIVATE ${CPP_LINKING_OPTS})

include(CTest)
include(Catch)
catch_discover_tests(lexer_tests)
catch_discover_tests(parser_tests)
catch_discover_tests(ast_tests)
catch_discover_tests(eval_tests)
