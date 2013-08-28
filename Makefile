all: eye

eye: eye.c
	gcc eye.c font.c -o eye -framework GLUT -framework OpenGL -Wall

clean:
	rm -f eye *~
