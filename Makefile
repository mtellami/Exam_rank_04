NAME = microshell
CFLAGS = -Wall -Wextra -Werror

SRC = microshell.c microshell_utils.c

all :
	cc $(CFLAGS) $(SRC) -o $(NAME)
clean :
	rm -fr $(NAME)
re : clean all