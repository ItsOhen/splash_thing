CORES := $(shell nproc 2>/dev/null || getconf NPROCESSORS_CONF)
BUILD_DIR := build
TARGET := splash_thing

all: release

release:
	cmake -B $(BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) -j$(CORES)
debug:
	cmake -B $(BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) -j$(CORES)

install:
	hyprctl plugin unload ~/.config/hypr/plugins/$(TARGET).so
	cp build/$(TARGET).so ~/.config/hypr/plugins/$(TARGET).so
	hyprctl plugin load ~/.config/hypr/plugins/$(TARGET).so

.PHONY: all release debug install
