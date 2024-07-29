

CPP=g++
CPPFLAGS= -g -Wall -DDEBUG

.o.cpp :
	$(CPP) $(CPPFLAGS) -o $@ -c $<

ruralis: ruralis.o ruralis_http.o ruralis_sql.o zubolite.o ruralis_bytes.o
	$(CPP) -o $@ *.o  -lsqlite3

clean:
	rm *.o *'~'

all: ruralis

ruralis.o: ruralis.cpp zubolite.h ruralis.h
ruralis_bytes.o: ruralis_bytes.cpp ruralis.h
ruralis_http.o: ruralis_http.cpp ruralis.h
ruralis_sql.o: ruralis_sql.cpp ruralis.h
zubolite.o: zubolite.cpp zubolite.h ruralis.h


