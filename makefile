# Source and target
SRCS = main.cpp
TARGET = raylib_app

UNAME_S := $(shell uname -s)

ifeq ($(wildcard CS3113/cs3113.cpp),CS3113/cs3113.cpp)
    SRCS += CS3113/cs3113.cpp
endif

ifeq ($(wildcard CS3113/Entity.cpp),CS3113/Entity.cpp)
    SRCS += CS3113/Entity.cpp
endif

ifeq ($(wildcard CS3113/Map.cpp),CS3113/Map.cpp)
    SRCS += CS3113/Map.cpp
endif

ifeq ($(wildcard CS3113/Scene.cpp),CS3113/Scene.cpp)
    SRCS += CS3113/Scene.cpp
endif

ifeq ($(wildcard CS3113/LevelA.cpp),CS3113/LevelA.cpp)
    SRCS += CS3113/LevelA.cpp
endif

ifeq ($(wildcard CS3113/LevelB.cpp),CS3113/LevelB.cpp)
    SRCS += CS3113/LevelB.cpp
endif

ifeq ($(wildcard CS3113/LevelC.cpp),CS3113/LevelC.cpp)
    SRCS += CS3113/LevelC.cpp
endif


ifeq ($(wildcard CS3113/Menu.cpp),CS3113/Menu.cpp)
    SRCS += CS3113/Menu.cpp
endif


ifeq ($(wildcard CS3113/WinScene.cpp),CS3113/WinScene.cpp)
    SRCS += CS3113/WinScene.cpp
endif


ifeq ($(wildcard CS3113/LoseScene.cpp),CS3113/LoseScene.cpp)
    SRCS += CS3113/LoseScene.cpp
endif

ifeq ($(wildcard CS3113/LevelD.cpp),CS3113/LevelD.cpp)
    SRCS += CS3113/LevelD.cpp
endif


ifeq ($(wildcard CS3113/LevelE.cpp),CS3113/LevelE.cpp)
    SRCS += CS3113/LevelE.cpp
endif

# Default values
CXX = g++
CXXFLAGS = -std=c++11

# Raylib configuration using pkg-config
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else
    # Linux/WSL fallback
    CXXFLAGS +=
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

# Run rule
run: $(TARGET)
	$(EXEC)