build:
	cc -o bin/builder src/main.c src/parser.c src/log.c

dev: build
	./bin/builder -noclean -Dhello
