add_library(parser parser.c ast.c env.c)

target_include_directories(parser PUBLIC ../../include/parser)

