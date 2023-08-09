add_subdirectory(interpreter)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp code_editor.cpp interpreter/interpreter.cpp interpreter/evaluator.cpp interpreter/repl.cpp interpreter/parser.cpp
                               interpreter/ast.cpp interpreter/lexer.cpp interpreter/object.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets fmt::fmt)
target_compile_options(${PROJECT_NAME} PRIVATE ${CPP_FLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${CPP_LINKING_OPTS})
