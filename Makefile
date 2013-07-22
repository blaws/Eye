all: eye

eye: eye.c
	gcc eye.c font.c -o eye -framework GLUT -framework OpenGL

clean:
	rm -f eye *~
