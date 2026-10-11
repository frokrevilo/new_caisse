CXX ?= g++
CXXFLAGS ?= -std=c++20 -Os -flto=auto -ffunction-sections -fdata-sections -Wall -Wextra -MMD -MP
LDFLAGS ?= -flto=auto -Wl,--gc-sections -Wl,--as-needed -Wl,-s

GTKMM_CFLAGS := $(shell pkg-config --cflags gtkmm-4.0 2>/dev/null)
GTKMM_LIBS := $(shell pkg-config --libs gtkmm-4.0 2>/dev/null)

TARGET := caisse
BUILD_DIR := build
SRCS := src/main.cpp src/main_window.cpp src/article_button.cpp src/article_config.cpp src/cash_input_dialog.cpp src/scale_helper.cpp src/sales_logger.cpp
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all run clean check-deps

all: $(TARGET)

check-deps:
	@pkg-config --exists gtkmm-4.0 || { \
		echo "FEHLER: gtkmm-4.0 wurde nicht gefunden!"; \
		echo "Bitte installiere das Paket mit:"; \
		echo "    sudo apt update && sudo apt install -y libgtkmm-4.0-dev"; \
		exit 1; \
	}

$(TARGET): check-deps $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(GTKMM_LIBS)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GTKMM_CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(DEPS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) src/*.o src/*.d
