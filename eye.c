// blaws, eye.c

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include "font.h"

int wsize=600;
int c[3];   // base eyecolor is (c[0],c[1],c[2]); default: random
int pupil,white;  // radius of beginning and end of iris
int currentIndex;  // for color selection
int circular[600][6300][3];
GLfloat square[1024][1024][3];
int vbands[600][3],crypts[600][6300][3];
int isLidded=0;
GLfloat fourOnes[]={1.0,1.0,1.0,1.0},transparent[]={1.0,1.0,1.0,0.1};
GLuint texture;
GLUquadricObj* sphere=NULL;

// adds bands of highly-varying color
void v_band(){
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

// adds dark spots
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

// sets the beginning color for each radial section
void r_colors(int size){
  int r,i;
  pupil = (50 + rand()%20) /2;
  white = (size-110 + rand()%21) /2;
  int start = ((white + pupil) / 2) /2;
  int chanceofvband = rand()%5;

  for(r=0;r<pupil;r++) circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = rand()%2;  // inside pupil

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
    }
  }

  for(;r<(size-10)/2;r++){  // white space (sclera)
    circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = 255 - rand()%20;
  }
}

// sets the color for each position on the eye based on the base color for that radius
void theta_colors(int size){
  int r,variance[2600][3],i=0,j=0;
  int chanceofstreak = rand()%15;
  int chanceofcrypt = rand()%15;
  double theta;

  i = 0;
  for(theta=0;theta<2*M_PI;theta+=.005){  // add streaks
    if(rand()%15 < chanceofstreak){
      for(j=0;j<3;j++)
	variance[i][j] = pow(-1,rand()%2)*(rand()%15);
    }
    else variance[i][0] = variance[i][1] = variance[i][2] = 0;
    i++;
  }

  for(r=0;r<white;r++){  // color each ring with variance
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.005){
      circular[r][i][0]=circular[r][0][0]+variance[i][0]+pow(-1,rand()%2)*(rand()%10);
      circular[r][i][1]=circular[r][0][1]+variance[i][1]+pow(-1,rand()%2)*(rand()%10);
      circular[r][i][2]=circular[r][0][2]+variance[i][2]+pow(-1,rand()%2)*(rand()%10);
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
  for(;r<(size-10)/2;r++){   // white space
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.005){
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

void init(void){
  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D,texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,600,600,0,GL_RGB,GL_FLOAT,square);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

  glEnable(GL_LIGHT0);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
  //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

  sphere = gluNewQuadric();
  gluQuadricDrawStyle(sphere,GLU_FILL);
  gluQuadricTexture(sphere,GL_TRUE);
  gluQuadricNormals(sphere,GL_TRUE);
}

void set_texture_and_light(void){
  int i,x,y;
  int lidRadius = 2*white + rand()%40-20;

  for(x=0;x<1024;x++){
    for(y=0;y<1024;y++){
      if(!isLidded || (hypot(white-x,y)<lidRadius && hypot(white-x,y)<lidRadius)){
        square[x][y][0] = circular[x][y][0] / 255.0;
        square[x][y][1] = circular[x][y][1] / 255.0;
        square[x][y][2] = circular[x][y][2] / 255.0;
      }
    }
  }

  glBindTexture(GL_TEXTURE_2D,texture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,600,600,0,GL_RGB,GL_FLOAT,square);

  // position light
  float light_position[4];
  for(i=0;i<2;i++) light_position[i]=rand()%wsize;
  light_position[2] = 5*wsize;
  light_position[3] = 0.0;
  glLightfv(GL_LIGHT0,GL_POSITION,light_position);
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,rand()%40+10.0);
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
  set_texture_and_light();
  glutPostRedisplay();
}

void reshape(int w,int h){
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,w,h,0,-wsize/2,wsize);
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
    /*case 'l':
  case 'L':
    isLidded = !isLidded;
    drawEye();
    break;*/
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // eye
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,fourOnes);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,fourOnes);
  glBindTexture(GL_TEXTURE_2D,texture);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(wsize/2,wsize/2,0);
  glRotatef(180.0,0.0,1.0,0.0);
  gluSphere(sphere,wsize/2-10,60,60);
  glDisable(GL_TEXTURE_2D);
  /*glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,transparent);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,fourOnes);
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100.0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0,1.0,1.0,0.1);
  gluSphere(sphere,wsize/2-5,60,60);  // cornea*/
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

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

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(wsize+200,wsize);
  glutInitWindowPosition((1080-wsize)/2,(800-wsize)/2);
  glutCreateWindow("Eye");
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  makeRasterFont();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecials);
  glutSpecialUpFunc(keyboardSpecialsUp);

  init();
  makeEye();
  drawEye();
  glutMainLoop();

  return 0;
}
