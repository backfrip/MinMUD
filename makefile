.PHONY: clean run

all:
	@printf "\033[33m> Compiling MinMUD...\n"
	gcc -o minmud main.c client/* server/* -I.
	@printf "\033[0m"

clean:
	@rm -f minmud
	@printf "\033[32m> Directory cleaned.\033[0m\n"

run: all
	@./minmud
