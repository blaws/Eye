// Blaws, eye.c

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GLUT/glut.h>

int wsize=600;
int c[3];   // base eyecolor is (c[0],c[1],c[2]); default: random
int circular[600][6300][3]={{{0}}};
float square[600][600][3]={{{0}}};

void v_band(int r,int mv){
  // offset current band to make room
  circular[r+5][0][0] = circular[r][0][0];
  circular[r+5][0][1] = circular[r][0][1];
  circular[r+5][0][2] = circular[r][0][2];
  // create high-variance band
  circular[r+1][0][0] = circular[r][0][0] + pow(-1,rand()%2)*(rand()%mv);
  circular[r+1][0][1] = circular[r][0][1] + pow(-1,rand()%2)*(rand()%mv);
  circular[r+1][0][2] = circular[r][0][2] + pow(-1,rand()%2)*(rand()%mv);
  // blend high-variance band into the rest
  circular[r][0][0] = circular[r+3][0][0] = (circular[r+5][0][0]+circular[r+1][0][0]) / 2;
  circular[r][0][1] = circular[r+3][0][1] = (circular[r+5][0][1]+circular[r+1][0][1]) / 2;
  circular[r][0][2] = circular[r+3][0][2] = (circular[r+5][0][2]+circular[r+1][0][2]) / 2;
  circular[r+2][0][0] = (circular[r+1][0][0]+circular[r+3][0][0]) / 2;
  circular[r+2][0][1] = (circular[r+1][0][1]+circular[r+3][0][1]) / 2;
  circular[r+2][0][2] = (circular[r+1][0][2]+circular[r+3][0][2]) / 2;
  circular[r+4][0][0] = (circular[r+3][0][0]+circular[r+5][0][0]) / 2;
  circular[r+4][0][1] = (circular[r+3][0][1]+circular[r+5][0][1]) / 2;
  circular[r+4][0][2] = (circular[r+3][0][2]+circular[r+5][0][2]) / 2;
}

void r_colors(int size){
  int r;
  int start = 50 + rand()%20;

  for(r=0;r<start;r++) circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = 0;
  circular[start][0][0] = c[0];
  circular[start][0][1] = c[1];
  circular[start][0][2] = c[2];
  for(r=start+1;r<size-100;r++){
    circular[r][0][0] = circular[r-1][0][0] + pow(-1,rand()%2)*(rand()%3);
    circular[r][0][1] = circular[r-1][0][1] + pow(-1,rand()%2)*(rand()%3);
    circular[r][0][2] = circular[r-1][0][2] + pow(-1,rand()%2)*(rand()%3);
    if(circular[r][0][0]>255) circular[r][0][0]=255;
    else if(circular[r][0][0]<0) circular[r][0][0]=0;
    if(circular[r][0][1]>255) circular[r][0][1]=255;
    else if(circular[r][0][1]<0) circular[r][0][1]=0;
    if(circular[r][0][2]>255) circular[r][0][2]=255;
    else if(circular[r][0][2]<0) circular[r][0][2]=0;

    if(rand()%200 < 1){    // random bands of high-variance colors
      v_band(r,50);
      r+=5;
    }

    // Display vertical and horizontal guide-lines; highlights where v-bands have been added
    /*gfx_color(circular[r][0],circular[r][1],circular[r][2]);
    gfx_line(size,size-r,size,size-r);
    gfx_line(size+r,size,size+r,size);*/
  }

  for(;r<size-10;r++){  // white space
    circular[r][0][0] = circular[r][0][1] = circular[r][0][2] = 255 - rand()%20;
  }
}

void theta_colors(int size){
  int r,c1,c2,c3,brightness[2600],variance[2600][3],i=0,j=0,chanceofstreak;
  double theta;

  int sinamp = rand()%16 * pow(-1,rand()%2);
  int sinshift = rand()%300;
  for(theta=0;theta<2*M_PI;theta+=.0025){  // set brightness for each angle, starting and ending with the same value
    brightness[i] = sinamp*sin(i/200.0+sinshift);
    i++;
  }

  chanceofstreak = rand()%15;
  i = 0;
  for(theta=0;theta<2*M_PI;theta+=.0025){  // add streaks
    if(rand()%15 < chanceofstreak){
      for(j=0;j<3;j++)
	variance[i][j] = pow(-1,rand()%2)*(rand()%15);
    }
    else variance[i][0] = variance[i][1] = variance[i][2] = 0;
    i++;
  }

  for(r=0;r<size-100;r++){  // color each ring with variance
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      /*if(circular[r][0][0]=circular[r][0][1]=circular[r][0][2]=0){
	circular[r][i][0]=circular[r][0][0]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
	circular[r][i][1]=circular[r][0][1]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
	circular[r][i][2]=circular[r][0][2]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
      }
      else{*/
	circular[r][i][0]=circular[r][0][0]+variance[i][0]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
	circular[r][i][1]=circular[r][0][1]+variance[i][1]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
	circular[r][i][2]=circular[r][0][2]+variance[i][2]+brightness[i]+pow(-1,rand()%2)*(rand()%10);
	//}
      if(circular[r][i][0]<0) circular[r][i][0]=0;
      if(circular[r][i][0]>255) circular[r][i][0]=255;
      if(circular[r][i][1]<0) circular[r][i][1]=0;
      if(circular[r][i][1]>255) circular[r][i][1]=255;
      if(circular[r][i][2]<0) circular[r][i][2]=0;
      if(circular[r][i][2]>255) circular[r][i][2]=255;
      i++;
    }
  }

  for(;r<size-10;r++){   // white space
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      circular[r][i][0] = circular[r][i][1] = circular[r][i][2] = 255 - rand()%20;
      i++;
    }
  }
}

void circle_to_square(void){
  int r,i,x,y;
  double theta;
  for(r=0;r<wsize/2-10;r++){
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.0025){
      x = wsize/2 + cos(theta)*r;
      y = wsize/2 + sin(theta)*r;
      square[x][y][0] = circular[r][i][0] / 255.0;
      square[x][y][1] = circular[r][i][1] / 255.0;
      square[x][y][2] = circular[r][i][2] / 255.0;

      if(r>wsize/2-100){  // eliminate holes in white space
	square[x+1][y][0] = square[x][y+1][0] = square[x+1][y+1][0] = square[x][y][0];
	square[x+1][y][1] = square[x][y+1][1] = square[x+1][y+1][1] = square[x][y][1];
	square[x+1][y][2] = square[x][y+1][2] = square[x+1][y+1][2] = square[x][y][2];
      }

      i++;
      //printf("x=%d, y=%d; r=%d, theta=%f  =  (%f,%f,%f)\n",x,y,r,theta,square[x][y][0],square[x][y][1],square[x][y][2]);
    }
  }
}

void drawEye(void){
  r_colors(wsize/2);
  theta_colors(wsize/2);
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
  default:
    drawEye();
    break;
  }
}

void display(void){
  int r,i,x,y;
  double theta;
  glClear(GL_COLOR_BUFFER_BIT);

  /*for(r=50;r<wsize-10;r++){
    i = 0;
    for(theta=0;theta<2*M_PI;theta+=.05){
      x = wsize/2 + cos(theta)*r;
      y = wsize/2 + sin(theta)*r;
      glColor3f(circular[i][0],circular[i][1],circular[i][2]);
      glRectf(x,y,x,y);
      i++;
    }
    }*/
  glRasterPos2i(0,wsize);
  glDrawPixels(wsize,wsize,GL_RGB,GL_FLOAT,square);

  glutSwapBuffers();
}

int main(int argc,char* argv[]){
  int i;
  srand(time(NULL));

  for(i=0;i<3;i++) c[i]=rand()%256;
  printf("Base eyecolor (r,g,b): ");
  scanf("%d,%d,%d",&c[0],&c[1],&c[2]);

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(wsize,wsize);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  drawEye();
  glutMainLoop();

  return 0;
}
