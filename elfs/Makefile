NAMES	:= \
	none\
	none.o\
	none_g\
	libft_malloc.so\
	webserv\
	# libft.a\

CC		:= cc
CFLAGS	:= 

.PHONY:	all
all:	tall

none:	none.c
	$(CC) $(CFLAGS) none.c -o $@

none.o:	none.c
	$(CC) $(CFLAGS) none.c -c

none_g:	none.c
	$(CC) $(CFLAGS) -g3 -fsanitize=address -fsanitize=undefined none.c -o $@

libft.a:
	cp ../$@ .

libft_malloc.so: malloc
	$(MAKE) -C $^
	cp -L $^/$@ $@

malloc:
	git clone https://github.com/corvvs/allocyou.git $@

webserv:	ws
	$(MAKE) -C $^
	cp $^/$@ $@

ws:
	git clone https://github.com/corvvs/webserv.git $@


.PHONY:	clean
clean:
	$(RM) $(NAMES)

.PHONY:	tall
tall:	$(NAMES)
	@$(RM) -r results
	@mkdir -p results
	@mkdir -p results/diff
	ruby test.rb $^
