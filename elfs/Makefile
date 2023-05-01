NAMES	:= \
	none\
	none.o\

CC		:= cc
CFLAGS	:= 

all:	tall

none:	none.c
	$(CC) $(CFLAGS) none.c -o $@

none.o:	none.c
	$(CC) $(CFLAGS) none.c -c

clean:
	$(RM) $(NAMES)

tall:	$(NAMES)
	@mkdir -p results
	ruby test.rb $^



