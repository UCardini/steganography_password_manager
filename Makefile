CXX = g++
#clang++
CXXFLAGS = -Iinc -Iext -Iext/lodepng
#-Wall -Wextra -std=c++17  # or c++11
LDFLAGS = -lboost_random -lboost_system -lcrypto

#-pedantic -ansi -O3 -Wextra -Wall -std=c++11
#-Wall -Wextra -pedantic -ansi -O3 -Im
#-Wall -Wextra -pedantic -ansi -O3
#--debug -Wall -g -Iinc

OBJ := obj
BIN := bin
SRC := src
EXT := ext

SRCS = $(wildcard $(SRC)/*.cpp) $(wildcard $(EXT)/*.cpp)
# OBJS = $(patsubst $(SRC)/%.cpp, $(EXT)/%.cpp, $(OBJ)/%.o, $(SRCS))
OBJS := $(patsubst %.cpp, $(OBJ)/%.o, $(SRCS))

TARGET = $(BIN)/main

# for clangd
clang:
	@if [ ! -f compile_commands.json ]; then \
		bear -- make clean all; \
		fi
	@make all

all: $(TARGET)
	@./$(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	@$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ) $(BIN) compile_commands.json .cache

rebuild: clean all

.PHONY: all clean rebuild clang
