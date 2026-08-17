all:liba.a libb.a

liba.a:a.c
	gcc $^ -o a.o -c
	ar crs $@ a.o

libb.a:b.c
	gcc $^ -o b.o -c
	ar crs $@ b.o

clean:
	-@$(RM) *.o lib*.a