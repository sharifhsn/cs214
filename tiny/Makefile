OUTPUT = tiny
CFLAGS = -g -Wall -Wvla -I inc -fsanitize=address -pthread -D_REENTRANT

%: %.c 
	gcc $(CFLAGS) -o $@ csapp.c $^

all: $(OUTPUT)

server:
	gcc $(CFLAGS)

clean:
	rm -f $(OUTPUT) *.tmp