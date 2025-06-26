add_library(lexer lexer.c tokens.c)

target_include_directories(lexer PUBLIC ../../include/lexer)
