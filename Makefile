all: eye

eye: eye.c
	gcc eye.c -o eye -framework GLUT -framework OpenGL

clean:
	rm -f eye *~
