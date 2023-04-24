SRCDIR	:=	srcs
OBJDIR	:=	objs
INCDIR	:=	includes
FILES	:=	\
			main.c\
			file.c\
			structure_mapping.c\
			analysis.c\
			analysis_header.c\
			print_utils.c\
			basic_utils.c\
			names.c\
			printf.c\
			assert.c\
			# nm.c\


SRCS	:=	$(FILES:%.c=$(SRCDIR)/%.c)
OBJS	:=	$(FILES:%.c=$(OBJDIR)/%.o)
NAME	:=	ft_nm

LIBFT		:=	libft.a
LIBFT_DIR	:=	libft
CC			:=	gcc
CCOREFLAGS	=	-Wall -Wextra -Werror -O2 -I$(INCDIR) -I$(LIBFT_DIR)
CFLAGS		=	$(CCOREFLAGS) -D DEBUG -g -fsanitize=address -fsanitize=undefined
RM			:=	rm -rf

all:			$(NAME)
	./$(NAME) a.out

bonus:
	$(MAKE) all WITH_BONUS=1

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
	cp $(LIBFT_DIR)/$(LIBFT) .

.PHONY:	clean
clean:
	$(RM) $(OBJDIR) $(LIBFT)

.PHONY:	fclean
fclean:			clean
	$(RM) $(LIBFT) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

.PHONY:	re
re:				fclean all

.PHONY:	up
up:
	docker-compose up --build -d

.PHONY:	down
down:
	docker-compose down

.PHONY:	it
it:
	docker-compose exec app bash

.PHONY:	t
t:	$(NAME)
	bash test_linux.sh > test_result.linux.txt
	cat test_result.linux.txt

.PHONY:	rt
rt:	fclean t

