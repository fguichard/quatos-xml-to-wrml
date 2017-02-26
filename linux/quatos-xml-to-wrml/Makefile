CC=g++
CFLAGS=-c -std=c++11 -Wall -fpermissive
LDFLAGS= -lstdc++
SOURCES= quatos-xml-to-wrml.cpp pugixml.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=quatos-xml-to-wrml

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o quatos-xml-to-wrml *~

