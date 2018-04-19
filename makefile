OPTIMIZATION=-O2
#DEBUG=-g
FLAGS=-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wfatal-errors -Werror -ansi -pedantic -std=c++11 -fno-rtti
CFLAGS=-c $(DEBUG) $(OPTIMIZATION) $(FLAGS) 
FFLAGS=$(DEBUG) $(OPTIMIZATION) $(FLAGS) 
CC=g++

all: objdir obj/curl_request.o main 

objdir:
	mkdir -p obj;

main: main.cpp obj/curl_request.o
	$(CC) main.cpp obj/curl_request.o  $(FFLAGS) -lcurl

obj/curl_request.o: src/curl_request.h src/curl_request.cpp
	$(CC) $(CFLAGS) src/curl_request.cpp -o obj/curl_request.o

clean:
	rm obj/*; rmdir obj;
