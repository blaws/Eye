// blaws, eye.c

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include "font.h"

int wsize=600;
int c[3];   // base eyecolor is (c[0],c[1],c[2]); default: random
int pupil,white;  // radius of beginning and end of iris
int currentIndex;  // for color selection
int circular[600][6300][3];
float square[600][600][3];
int vbands[600][3],crypts[600][6300][3];
int isLidded=0;

void v_band(){//int r,int mv,int dir){
  int r,i,j,k,inc;
  double theta;
  for(r=0;r<600;r++){
    if(vbands[r][0] || vbands[r][1] || vbands[r][2]){
      inc = 0;
      for(theta=0,i=0;theta<2*M_PI;theta+=.0025,i++){
	if(inc<1 && rand()%20==0) inc++;
	if(inc>-1 && rand()%20==0) inc--;
	for(j=0;j<5;j++){
	  for(k=0;k<3;k++){
	    if(rand()%4 && r+inc+j<white){
	      circular[r+inc+j][i][k] += vbands[r][k]/(j+1);
	      circular[r+inc+j][i][k] *= (j+1.0)/(j+2.0);
	    }
	  }
	}
      }
    }
  }
}

void crypt(){
  int r,i,j,x,y;
  double theta;
  for(r=0;r<600;r++){
    for(theta=0,i=0;theta<2*M_PI;theta+=.0025,i++){

      if(crypts[r][i][0]){
	for(x=-crypts[r][i][1];x<crypts[r][i][1];x++){
	  for(y=-crypts[r][i][2];y<crypts[r][i][2];y++){
	    if((hypot(x,y)<crypts[r][i][1] || hypot(x,y)<crypts[r][i][2]) && rand()%4){
	      if((r+y<white && r+y>pupil) && (i+x>=0 || i+x<6300)){
		for(j=0;j<3;j++){
		  circular[r+y][i+x][j]-=crypts[r][i][0];
		  if(circular[r+y][i+x][j]<0) circular[r+y][i+x][j]=0;
		}
	      }
	    }
	  }
	}
      }

    }
  }
}

void r_colors(int size){
  int r,i;
  pupil = 50 + rand()%20;
  white = size-110 + rand()%21;
  int start = (white + pupil) / 2;
  int chanceofvband = rand()%5;

  for(r=0;r<pupil;r++) circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = 0;  // inside pupil
  circular[start][0][0] = c[0];
  circular[start][0][1] = c[1];
  circular[start][0][2] = c[2];

  for(r=start;r>pupil;r--){  // middle in
    circular[r-1][0][0] = circular[r][0][0] + pow(-1,rand()%2)*(rand()%3);
    circular[r-1][0][1] = circular[r][0][1] + pow(-1,rand()%2)*(rand()%3);
    circular[r-1][0][2] = circular[r][0][2] + pow(-1,rand()%2)*(rand()%3);
    if(circular[r][0][0]>255) circular[r-1][0][0]=255;
    else if(circular[r-1][0][0]<0) circular[r-1][0][0]=0;
    if(circular[r-1][0][1]>255) circular[r-1][0][1]=255;
    else if(circular[r-1][0][1]<0) circular[r-1][0][1]=0;
    if(circular[r-1][0][2]>255) circular[r-1][0][2]=255;
    else if(circular[r-1][0][2]<0) circular[r-1][0][2]=0;
    if(rand()%200<chanceofvband && r<white-5){    // random bands of high-variance colors
      for(i=0;i<3;i++) vbands[r][i]=circular[r][0][i]+rand()%100*pow(-1,rand()%2);
      //v_band(r-1,50,-1);
      //r-=5;
    }
  }
  for(r=start+1;r<white;r++){  // middle out
    circular[r][0][0] = circular[r-1][0][0] + pow(-1,rand()%2)*(rand()%3);
    circular[r][0][1] = circular[r-1][0][1] + pow(-1,rand()%2)*(rand()%3);
    circular[r][0][2] = circular[r-1][0][2] + pow(-1,rand()%2)*(rand()%3);
    if(circular[r][0][0]>255) circular[r][0][0]=255;
    else if(circular[r][0][0]<0) circular[r][0][0]=0;
    if(circular[r][0][1]>255) circular[r][0][1]=255;
    else if(circular[r][0][1]<0) circular[r][0][1]=0;
    if(circular[r][0][2]>255) circular[r][0][2]=255;
    else if(circular[r][0][2]<0) circular[r][0][2]=0;
    if(rand()%200<chanceofvband){    // random bands of high-variance colors
      for(i=0;i<3;i++) vbands[r][i]=circular[r][0][i]+rand()%100*pow(-1,rand()%2);
      //v_band(r,50,1);
      //r+=5;
    }
  }

  for(;r<size-10;r++){  // white space (sclera)
    circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = 255 - rand()%20;
  }
}

void theta_colors(int size){
  int r,c1,c2,c3,brightness[2600],variance[2600][3],i=0,j=0;
  int chanceofstreak = rand()%15;
  int chanceofcrypt = rand()%15;
  double theta;

  int sinamp = rand()%16 * pow(-1,rand()%2) + 10;
  int sinshift = rand()%300;

  for(theta=0;theta<2*M_PI;theta+=.0025){  // set brightness for each angle, starting and ending with the same value
    brightness[i] = sinamp*sin(i/200.0+sinshift);
    i++;
  }

  i = 0;
  for(theta=0;theta<2*M_PI;theta+=.0025){  // add streaks
    if(rand()%15 < chanceofstreak){
      for(j=0;j<3;j++)
	variance[i][j] = pow(-1,rand()%2)*(rand()%15);
    }
    else variance[i][0] = variance[i][1] = variance[i][2] = 0;
    i++;
  }

  for(r=0;r<white;r++){  // color each ring with variance
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      circular[r][i][0]=circular[r][0][0]+variance[i][0]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
      circular[r][i][1]=circular[r][0][1]+variance[i][1]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
      circular[r][i][2]=circular[r][0][2]+variance[i][2]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
      if(circular[r][i][0]<0) circular[r][i][0]=0;
      if(circular[r][i][0]>255) circular[r][i][0]=255;
      if(circular[r][i][1]<0) circular[r][i][1]=0;
      if(circular[r][i][1]>255) circular[r][i][1]=255;
      if(circular[r][i][2]<0) circular[r][i][2]=0;
      if(circular[r][i][2]>255) circular[r][i][2]=255;
      i++;

      // add crypts
      if(rand()%100000<chanceofcrypt){
	crypts[r][i][0] = rand()%30+1;
	crypts[r][i][1] = rand()%20+1;
	crypts[r][i][2] = rand()%10+1;
      }
    }
  }

  int veins[2600][2] = {{0}};
  for(;r<size-10;r++){   // white space
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      // veins
      if(veins[i][0]){  // continuing vein
	circular[r][i][0] = veins[i][0];
	circular[r][i][1] = circular[r][i][2] = 100;

	if(veins[i][1]==1 && rand()%3==0){  // move right
	  veins[i+2][0] = veins[i+1][0] = veins[i][0];
	  veins[i-1][0] = veins[i][0] = 0;
	  veins[i+1][1] = veins[i+2][1] = veins[i][1];
	  i++;
	  circular[r][i][0] = veins[i][0];
	  circular[r][i][1] = circular[r][i][2] = 100;
	  i++;
	  circular[r][i][0] = veins[i][0];
	  circular[r][i][1] = circular[r][i][2] = 100;
	}
	else if(veins[i][1]==-1 && rand()%3==0){  // move left
	  veins[i-2][0] = veins[i-1][0] = veins[i][0];
	  veins[i+1][0] = veins[i][0] = 0;
	  veins[i-1][1] = veins[i-2][1] = veins[i][1];
	}
	else if(rand()%50==0){  // switch direction
	  veins[i-1][1] *= -1;
	  veins[i][1] *= -1;
	  veins[i+1][1] *= -1;
	}
      }
      else if(rand()%25000==0){  // new vein
	veins[i][0] = veins[i-1][0] = circular[r][i][0] = 200 + rand()%56;
	circular[r][i][1] = circular[r][i][2] = 100;
	veins[i][1] = pow(-1,rand()%2);
      }
      // no vein
      else circular[r][i][0] = circular[r][i][1] = circular[r][i][2] = 255 - rand()%20;
      i++;
    }
  }
}

void circle_to_square(void){
  int r,i,j,x,y;
  double theta;
  int lidRadius = 2*white + rand()%40-20;

  for(x=0;x<wsize;x++){
    for(y=0;y<wsize;y++){
      square[x][y][0] = square[x][y][1] = square[x][y][2] = 0.0;
    }
  }

  for(r=0;r<wsize/2-10;r++){
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      x = wsize/2 + cos(theta)*r;
      y = wsize/2 + sin(theta)*r;
      if(!isLidded || (hypot(wsize/2-white-x,wsize/2-y)<lidRadius && hypot(wsize/2+white-x,wsize/2-y)<lidRadius)){
	square[x][y][0] = circular[r][i][0] / 255.0;
	square[x][y][1] = circular[r][i][1] / 255.0;
	square[x][y][2] = circular[r][i][2] / 255.0;

	if(r>white){  // eliminate holes in sclera
	  for(j=0;j<3;j++) square[x+1][y][j] = square[x][y+1][j] = square[x+1][y+1][j] = square[x][y][j];
	}
      }
      //else square[x][y][0] = square[x][y][1] = square[x][y][2] = 0.0;

      i++;
      //printf("x=%d, y=%d; r=%d, theta=%f  =  (%f,%f,%f)\n",x,y,r,theta,square[x][y][0],square[x][y][1],square[x][y][2]);
    }
  }
}

void clear_matrices(){
  int r,i,k;
  for(r=0;r<600;r++){
    for(k=0;k<3;k++){
      for(i=0;i<600;i++){
	circular[r][i][k] = crypts[r][i][k] = 0;
	square[r][i][k] = 0.0;
      }
      for(;i<6300;i++) circular[r][i][k]=crypts[r][i][k]=0;
      vbands[r][k] = 0;
    }
  }
}

void makeEye(void){
  clear_matrices();
  r_colors(wsize/2);
  theta_colors(wsize/2);
  v_band();
  crypt();
}

void drawEye(void){
  circle_to_square();
  glutPostRedisplay();
}

void reshape(int w,int h){
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,w,h,0,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glutPostRedisplay();
}

void keyboard(unsigned char key,int x,int y){
  switch(key){
  case 'q':
  case 27:  // ESC key
    exit(0);
    break;
  case 'l':
  case 'L':
    isLidded = !isLidded;
    drawEye();
    break;
  default:
    makeEye();
    drawEye();
    break;
  }
}

void keyboardSpecials(int key,int x,int y){
  switch(key){
  case GLUT_KEY_UP:
    c[currentIndex] += 5;
    if(c[currentIndex]>255) c[currentIndex]=255;
    break;
  case GLUT_KEY_DOWN:
    c[currentIndex] -= 5;
    if(c[currentIndex]<0) c[currentIndex]=0;
    break;
  case GLUT_KEY_LEFT:
    currentIndex = (currentIndex+2) % 3;
    break;
  case GLUT_KEY_RIGHT:
    currentIndex = (currentIndex+1) % 3;
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void keyboardSpecialsUp(int key,int x,int y){
  switch(key){
  case GLUT_KEY_UP:
  case GLUT_KEY_DOWN:
    makeEye();
    drawEye();
    break;
  default:
    break;
  }
}

void display(void){
  int i,offset;
  char value[100];
  glClear(GL_COLOR_BUFFER_BIT);

  // eye
  glRasterPos2i(0,wsize);
  glDrawPixels(wsize,wsize,GL_RGB,GL_FLOAT,square);

  // color bars
  for(i=0;i<3;i++){
    glColor3f(i==0,i==1,i==2);
    glRectf(wsize+50+50*i,wsize-40,wsize+60+50*i,10+(1-c[i]/255.0)*(wsize-50));
    sprintf(value,"%d",c[i]);
    glColor3f(i==0,i==1,i==2);

    if(c[i]>99) offset=wsize+40;
    else if(c[i]>9) offset=wsize+45;
    else offset=wsize+50;
    glRasterPos2i(offset+50*i,wsize-25);
    printString(value);
  }
  // current index indicator
  glColor3f(1,1,0);
  glBegin(GL_TRIANGLES);
  glVertex2f(wsize+50*currentIndex+45,wsize-10);
  glVertex2f(wsize+50*currentIndex+55,wsize-20);
  glVertex2f(wsize+50*currentIndex+65,wsize-10);
  glEnd();

  glutSwapBuffers();
}

int main(int argc,char* argv[]){
  int i;
  srand(time(NULL));

  for(i=0;i<3;i++) c[i]=rand()%52 * 5; // random multiple of 5 from 0 to 255
  //printf("Base eyecolor (r,g,b): ");
  //scanf("%d,%d,%d",&c[0],&c[1],&c[2]);

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(wsize+200,wsize);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  makeRasterFont();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecials);
  glutSpecialUpFunc(keyboardSpecialsUp);

  makeEye();
  drawEye();
  glutMainLoop();

  return 0;
}
