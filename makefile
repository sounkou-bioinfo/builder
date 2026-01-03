build:
	cc -o bin/builder src/main.c src/parser.c src/log.c src/file.c src/define.c -Wall

cmd := ./bin/builder -input srcr -DDEBUG -DTEST '"a string"'

dev: build
	${cmd}

debug: build
	valgrind --leak-check=full ${cmd}
