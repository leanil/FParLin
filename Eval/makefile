CC=clang++
FPARLIN=../FParLin
CFLAGS=-c -Wall -I/mnt/hgfs/host/Programs/boost_1_60_0 -I$(FPARLIN) -std=c++14 -g
LDFLAGS=-ldl -pthread
LIB=../FParLin/bin/fparlin.a
SOURCES=main.cpp
OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))
EXECUTABLE=eval

all: $(SOURCES) $(EXECUTABLE)
   
$(EXECUTABLE): $(OBJECTS) $(LIB)
	$(CC) $(OBJECTS) $(LIB) $(LDFLAGS) -o $@
	
obj/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(LIB): $(FPARLIN)
	$(MAKE) -j -C $<
	
.PHONY: clean
clean:
	rm -f eval $(OBJECTS)
