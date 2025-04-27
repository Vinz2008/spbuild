CXX = g++

CXXFLAGS = -Wall -c -g -std=c++17 -fno-exceptions 
#CXXFLAGS += -fsanitize=undefined,address

LDFLAGS = 
#LDFLAGS += -fsanitize=undefined,address

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
	cd test && ../spbuild


release: set_release all

set_release: 
	$(eval CXXFLAGS += -O3 -flto -DNDEBUG) 
	$(eval LDFLAGS += -s -flto) 