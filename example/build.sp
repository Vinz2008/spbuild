# exe("demo", ["src/test.c", "src/test2.c"])
cc("clang")
exe("demo", wildcard("src/*.c"), [])
exe("demo2", wildcard("src/*.c"), [])