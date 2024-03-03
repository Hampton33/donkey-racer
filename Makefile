# Detect OS
OS := $(shell uname -s 2>/dev/null || echo not)

# Common settings
CXX=g++
CXXFLAGS=-std=c++17 -g -Wall
SOURCES=$(wildcard src/*.cpp src/renderer/*.cpp)
TARGET=app

# Platform specific settings
ifeq ($(OS),not)
    # Windows settings
    RM = del /Q /F
    MKDIR = if not exist "$(@D)" mkdir "$(@D)"
    RMDIR = if exist build rmdir /s /q build
    TARGET := $(TARGET).exe
    CXXFLAGS += -I./include -I./external/tinyobjloader -I./external/glfw/include -I./external/glm/include -IC:/VulkanSDK/1.3.275.0/Include
    LDFLAGS = -L./external/glfw/lib-mingw-w64 -LC:/VulkanSDK/1.3.275.0/Lib
    LDLIBS = -lglfw3 -lvulkan-1 -lgdi32
else
    # Linux settings
    RM = rm -f
    MKDIR = @mkdir -p $(@D)
    RMDIR = rm -rf build
    TARGET := $(TARGET)
    CXXFLAGS += -I./external/tinyobjloader -g -O2 $(shell find ./include -type d | sed 's/^/-I/')
    LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
endif

OBJECTS=$(SOURCES:src/%.cpp=build/%.o)

# Default target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Rule to compile object files
build/%.o: src/%.cpp
	$(MKDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	$(RMDIR) build
	$(RM) $(TARGET)

.PHONY: all clean
