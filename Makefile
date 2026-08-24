CXX      := g++
CXXFLAGS := -std=c++17 -O3 -march=native -DNDEBUG -fopenmp -Wall
ENGINE_DIR := engine
INCLUDES := -Iinclude -I$(ENGINE_DIR)/include -I$(ENGINE_DIR)/third_party/imgui \
            -I$(ENGINE_DIR)/third_party/imgui/backends \
            -I/usr/include/jsoncpp -I/usr/include/eigen3
LDFLAGS  := -lGL -lGLEW -lglfw -ljsoncpp -lgomp -ldl

TARGET    := ./sph_engine
BUILD_DIR := build

SPH_SRCS    := $(wildcard src/*.cpp)
ENGINE_SRCS := $(wildcard $(ENGINE_DIR)/src/*.cpp)
IMGUI_SRCS  := $(ENGINE_DIR)/third_party/imgui/imgui.cpp \
               $(ENGINE_DIR)/third_party/imgui/imgui_draw.cpp \
               $(ENGINE_DIR)/third_party/imgui/imgui_tables.cpp \
               $(ENGINE_DIR)/third_party/imgui/imgui_widgets.cpp \
               $(ENGINE_DIR)/third_party/imgui/imgui_demo.cpp \
               $(ENGINE_DIR)/third_party/imgui/backends/imgui_impl_glfw.cpp \
               $(ENGINE_DIR)/third_party/imgui/backends/imgui_impl_opengl3.cpp

# Object paths are keyed off each source's path relative to ENGINE_DIR/src, not $(ENGINE_DIR)
# itself, so build/ stays self-contained regardless of where ENGINE_DIR points.
SPH_OBJS    := $(patsubst src/%.cpp,$(BUILD_DIR)/sph/%.o,$(SPH_SRCS))
ENGINE_OBJS := $(patsubst $(ENGINE_DIR)/src/%.cpp,$(BUILD_DIR)/engine/%.o,$(ENGINE_SRCS))
IMGUI_OBJS  := $(patsubst $(ENGINE_DIR)/third_party/imgui/%.cpp,$(BUILD_DIR)/imgui/%.o,$(IMGUI_SRCS))

OBJS := $(SPH_OBJS) $(ENGINE_OBJS) $(IMGUI_OBJS)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/sph/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/engine/%.o: $(ENGINE_DIR)/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/imgui/%.o: $(ENGINE_DIR)/third_party/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	XDG_SESSION_TYPE=x11 ./$(TARGET) inputs/fluid.json

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
