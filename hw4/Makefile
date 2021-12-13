OUTPUT = memperf
CFLAGS = -g -Wall -Wvla -fsanitize=address

%: %.c 
	gcc $(CFLAGS) -o $@ mymalloc.c $^

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.tmp