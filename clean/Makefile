CXX=g++
CXXFLAGS=-g --std=c++11 -O2
LDFLAGS=-g
LDLIBS=-lboost_program_options
OUT=main

SRC := $(wildcard *.cc)
OBJ := $(patsubst %.cc,%.o,$(filter %.cc,$(SRC)))
DEP := $(addprefix .,$(OBJ:.o=.d))

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

include $(DEP)

.%.d: %.cc
	./depend.sh `dirname $*` $(BUILDDIR) $(CXXFLAGS) $*.cc > $@

clean:
	rm -f *.o
	rm -f .*.d

distclean: clean
	rm -f main

.PHONY: all clean distclean
