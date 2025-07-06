all: main.c ./include/ini.c ./include/utils.c 
	gcc -o durpkg main.c ./include/ini.c ./include/utils.c -Wall -Wextra -std=c99
clear:
	rm -rf durpkg main 

