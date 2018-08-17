CFLAGS = -g -Iinclude 

all:
	gcc $(CFLAGS)  src/frm.c src/vars.c src/util.c -oreadfrm