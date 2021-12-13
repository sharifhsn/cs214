OUTPUT = tiny
CFLAGS = -g -Wall -Wvla -I inc -fsanitize=address -pthread -D_REENTRANT

%: %.c 
	gcc $(CFLAGS) -o $@ csapp.c $^

all: $(OUTPUT)

server:
	gcc $(CFLAGS) -o tinyserver csapp.c tinyserver.c

clean:
	rm -f $(OUTPUT) *.tmp