build:
	cc -o bin/builder src/main.c src/parser.c src/log.c src/file.c src/define.c -Wall

dev: build
	./bin/builder -input srcr -DDEBUG
