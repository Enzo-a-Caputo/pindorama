CXX      = g++
CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -Iinclude
RAYLIB   = $(shell pkg-config --libs --cflags raylib)
SRC      = src/main.cpp
BIN      = build/pindorama

.PHONY: all run clean

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(RAYLIB)

run: all
	./$(BIN)

clean:
	rm -f $(BIN)