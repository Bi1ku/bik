add_library(generator generator.c eval.c)

target_include_directories(generator PUBLIC ../../include/generator)
