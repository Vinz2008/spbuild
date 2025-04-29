// exe("demo", ["src/test.c", "src/test2.c"])
exe("demo", wildcard("src/*.c"))