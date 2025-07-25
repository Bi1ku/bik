add_library(generator generator.c eval.c expr.c stmt.c vars.c)

target_include_directories(generator PUBLIC ../../include/generator)
