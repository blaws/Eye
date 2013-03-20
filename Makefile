all: eye

eye: eye.c gfx.c gfx.h
	gcc eye.c gfx.c -o eye -lm -lX11

clean:
	rm -f eye
