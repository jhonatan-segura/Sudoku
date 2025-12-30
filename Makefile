# =========================
# Project configuration
# =========================
APP_NAME = app
SRC_DIR  = src
SRC      = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)

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
CFLAGS = -Wall -Wextra -O2 $(STD)
INCLUDES = -I/usr/local/include
LIBS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt

# =========================
# Targets
# =========================
all: $(APP_NAME)

$(APP_NAME):
	$(CC) $(SRC) -o $(APP_NAME) $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f $(APP_NAME)

.PHONY: all clean
