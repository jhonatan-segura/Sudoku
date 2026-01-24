# =========================
# Project configuration
# =========================
APP_NAME = app
SRC_DIR  = sudoku
SRC      = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)

BIN_DIR  = bin
TARGET   = $(BIN_DIR)/$(APP_NAME)

# =========================
# Detect compiler
# =========================
ifeq ($(findstring .cpp,$(SRC)),.cpp)
    CC = g++
    STD = -std=c++17
else
    CC = gcc
    STD = -std=c99
endif

# =========================
# Flags
# =========================
CFLAGS   = -Wall -Wextra -O2 $(STD)
CDEBUGFLAGS   = -Wall -Wextra -g -O0 $(STD)
INCLUDES = -I/usr/local/include
LIBS     = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt

# =========================
# Targets
# =========================
all: $(TARGET)

# Create bin directory if it does not exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build binary into bin/
$(TARGET): $(SRC) | $(BIN_DIR)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f $(TARGET)

debug:
	$(CC) $(SRC) -o $(TARGET) $(CDEBUGFLAGS) $(INCLUDES) $(LIBS)

.PHONY: all clean
