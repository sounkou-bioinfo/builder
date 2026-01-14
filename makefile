CC = $(shell R CMD config CC)
CFLAGS = $(shell R CMD config --cppflags)
LDFLAGS = $(shell R CMD config --ldflags)
FLS = -Wall -Iinclude -s
CMD = ./bin/builder -input srcr -DDEBUG -DTEST '"a string"' -DXXX 42

FILES = src/main.c \
  src/r.c \
	src/parser.c \
	src/log.c \
	src/file.c \
	src/define.c \
	src/include.c \
	src/fstring.c \
	src/deconstruct.c \
	src/test.c \
	src/preflight.c \
	src/const.c

build: $(FILES)
	$(CC) $(CFLAGS) $(FLS) $^ -o bin/builder $(LDFLAGS)

dev: build
	$(CMD)

debug: build
	valgrind --leak-check=full ${CMD}
