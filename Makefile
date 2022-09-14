CXX=clang++
CPPFLAGS=-Wall -std=c++17
LDLIBS=-lcrypto -lssl
RM=rm -f
LDFLAGS=
CC=clang
SRCS=main.cpp

CIPHER_REG := ^.*\_cipher$
OBJS=$(subst .cpp,.o, $(SRCS))

all: main
main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

main.o: main.cpp

clean: 
	$(RM) $(OBJS)
distclean: clean
	$(RM)  main
