.RECIPEPREFIX = >
CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2
SRC = main.c
OUT = build/CainiaoStation

$(OUT): $(SRC)
> mkdir -p build
> $(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
> ./$(OUT)

clean:
> rm -rf build data

.PHONY: run clean
