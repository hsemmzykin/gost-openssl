CXX=clang++
CPPFLAGS=-Wall -std=c++17
LDLIBS=-lcrypto -lssl
RM=rm -f
LDFLAGS=

SRCS=main.cpp
OBJS=$(subst .cpp,.o, $(SRCS))

all: main
main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

main.o: main.cpp



clean: 
	$(RM) $(OBJS)
distclean: clean
	$(RM)  main

