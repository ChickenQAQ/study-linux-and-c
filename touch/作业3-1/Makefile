
CC=aarch64-linux-gnu-gcc

CFLAGS += -I ./include
CFLAGS += -I ./jpeg-9d/include

LDFLAGS += -ldl
LDFLAGS += -L ./lib -lLCD
LDFLAGS += -L ./jpeg-9d/lib -ljpeg

all:install
	$(CC) main.c -o main $(CFLAGS) $(LDFLAGS) 
	scp main  root@192.168.55.11:/root/0819
	scp lib/* root@192.168.55.11:/root/mylib

install:
	$(CC) -shared -fPIC src/lcd.c -o lib/libLCD.so $(CFLAGS)
	$(CC) -shared -fPIC src/bmp.c -o lib/libbmp.so $(CFLAGS) $(LDFLAGS)
	$(CC) -shared -fPIC src/jpg.c -o lib/libjpg.so $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf main
	rm -rf lib/libjpg.so
	rm -rf lib/libbmp.so