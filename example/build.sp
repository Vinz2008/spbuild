# exe("demo", ["src/test.c", "src/test2.c"])
check_header("unistd.h", "_UNISTD_")
check_header("stdio.h")
check_header("stdlib.h")
check_header("stdbool.h")
check_header("assert.h")
check_header("complex.h")
check_header("ctype.h")
check_header("errno.h")
check_header("fenv.h")
check_header("float.h")
check_header("inttypes.h")
check_header("limits.h")
check_header("locale.h")
check_header("math.h")
check_header("setjmp.h")
check_header("signal.h")
check_header("stdarg.h")
check_header("stdatomic.h")
check_header("stddef.h")
check_header("stdint.h")
check_header("string.h")
check_header("tgmath.h")
check_header("time.h")
check_header("wchar.h")
cc("clang")
exe("demo", wildcard("src/*.c"), [])
exe("demo2", wildcard("src/*.c"), [])