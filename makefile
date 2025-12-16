build:
	cc -o bin/builder src/main.c src/parser.c src/log.c src/file.c -Wall

dev: build
	./bin/builder -noclean -input srcr
