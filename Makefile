CXX = g++

CXXFLAGS = -Wall -c -g -std=c++17 -fno-exceptions
#CXXFLAGS += -fsanitize=undefined,address
#CXXFLAGS += -pg

LDFLAGS = 
#LDFLAGS += -fsanitize=undefined,address
#LDFLAGS += -pg

SRCDIR = src

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

all: spbuild

spbuild: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o:$(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS)

run: 
	cd example && ../spbuild


release: set_release all

set_release: 
	$(eval CXXFLAGS += -O3 -flto -DNDEBUG) 
	$(eval LDFLAGS += -s -flto) 


TESTS_DIR = test

TESTS_OBJS = src/file.o src/lexer.o

TESTS_SRCS = $(wildcard $(TESTS_DIR)/*.cpp)
TESTS_OBJS += $(patsubst %.cpp,%.o,$(TESTS_SRCS))

TESTS_LDFLAGS = $(shell pkg-config --libs gmock)


$(TESTS_DIR)/test-runner: $(TESTS_OBJS)
	$(CXX) -o $@ $^ $(TESTS_LDFLAGS)

test: $(TESTS_DIR)/test-runner
	$(TESTS_DIR)/test-runner