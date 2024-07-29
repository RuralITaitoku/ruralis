

CPP=g++
CPPFLAGS= -g -Wall -DDEBUG

.o.cpp :
	$(CPP) $(CPPFLAGS) -o $@ -c $<

libruralis.a: ruralis_http.o ruralis_sql.o zubolite.o ruralis_bytes.o
	ar rcs $@ $^

ruralis: ruralis.o zubolite.o libruralis.a
	$(CPP) -o $@  ruralis.o zubolite.o -lsqlite3 -L. -lruralis


ruralis: ruralis.o zubolite.o 
	$(CPP) -o $@  *.o -lsqlite3 


clean:
	rm *.o ruralis libruralis.a log.log
all: ruralis
lib: libruralis.a

ruralis.o: ruralis.cpp zubolite.h ruralis.h
ruralis_bytes.o: ruralis_bytes.cpp ruralis.h
ruralis_http.o: ruralis_http.cpp ruralis.h
ruralis_sql.o: ruralis_sql.cpp ruralis.h
zubolite.o: zubolite.cpp zubolite.h ruralis.h


