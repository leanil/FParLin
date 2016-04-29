CC=clang++
CFLAGS=-c -Wall -I/mnt/hgfs/host/Programs/boost_1_60_0 -std=c++14 -g
LDFLAGS=
SOURCES=codegen_alg.cpp costest_alg.cpp evaluator.cpp expr.cpp typecheck_alg.cpp typeprinter_alg.cpp
OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))
EXECUTABLE=bin/fparlin.a

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	ar -cr $@ $(OBJECTS)
	
obj/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)
	
.PHONY: clean

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
