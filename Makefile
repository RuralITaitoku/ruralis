

CPP=g++
CPPFLAGS= -g -Wall -DDEBUG -fPIC -I.

LIB_SRCS = $(wildcard lib/*.cpp)
LIB_OBJS = $(LIB_SRCS:.cpp=.o)

SRCS = $(wildcard ./*.cpp)
OBJS = $(SRCS:.cpp=.o)


all: ruralis fp2sql

libruralis.so : $(LIB_OBJS)
	ar rcs $@ $^

#####################  実行ファイル生成
RURA_OBJS = ruralis.o zubolite.o
ruralis: $(RURA_OBJS) libruralis.so
	g++ -o $@ $(RURA_OBJS) -L. -lruralis -lsqlite3 

FP2SQL_OBJS = fp2sql.o
fp2sql: $(FP2SQL_OBJS) libruralis.so
	g++ -o $@ $(FP2SQL_OBJS) -L. -lruralis -lsqlite3 

#####################

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ -c $<

clean:
	rm $(RURA_OBJS)
	rm $(LIB_OBJS)
	rm ruralis libruralis.so
	
