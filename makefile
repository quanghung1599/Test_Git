#compiler name
cc :=gcc

#remove command
RM := rm -rf

#source files
SOURCES := hung.c test.c

#object files
OBJS :=$(SOURCES:.c=.o)

#main target
main: $(OBJS)
    $(CC) $^ -o $@

%.o: %.c
    $(CC) -c $< -o $@
 

.PHONY: clean
clean:
    $(RM) *.o main