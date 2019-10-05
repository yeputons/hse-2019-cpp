# '-g' means 'add debug information'

main: main.o foo.o
	gcc -g main.o foo.o -o main

main.o: main.c foo.h
	gcc -c -Wall -Wextra -Werror -g main.c

foo.o: foo.c foo.h
	gcc -c -Wall -Wextra -Werror -g foo.c

clean:
	rm main main.o foo.o
