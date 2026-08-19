CC = aarch64-linux-gnu-gcc
CFLAGS = -fPIC
INC = -I/home/chicken/jpeg-9d
LIBPATH = -L. -L/home/chicken/jpeg-9d/.libs

# 默认目标：两个都要生成
all: libjpg.so main

# 生成动态库：只链接系统jpeg，不要 -ljpg！！
libjpg.so: jpg.c bmp.c jpg.h bmp.h
	$(CC) $(CFLAGS) -shared jpg.c bmp.c -o $@ $(INC) $(LIBPATH) -ljpeg

# 编译应用程序 main，此时才 -ljpg
main: main.c libjpg.so jpg.h bmp.h
	$(CC) main.c -o $@ $(LIBPATH) -ljpg -ljpeg

scp_so: main
	scp libjpg.so root@192.168.53.113:/libjpeg
	scp main root@192.168.53.113:/usr/lib

.PHONY: all clean scp_so

clean:
	rm -f libjpg.so main
