// Benjamin Laws, eye.c

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "gfx.h"

void v_band(int rcolors[][3],int r,int mv){
  // offset current band to make room
  rcolors[r+5][0] = rcolors[r][0];
  rcolors[r+5][1] = rcolors[r][1];
  rcolors[r+5][2] = rcolors[r][2];
  // create high-variance band
  rcolors[r+1][0] = rcolors[r][0] + pow(-1,rand()%2)*(rand()%mv);
  rcolors[r+1][1] = rcolors[r][1] + pow(-1,rand()%2)*(rand()%mv);
  rcolors[r+1][2] = rcolors[r][2] + pow(-1,rand()%2)*(rand()%mv);
  // blend high-variance band into the rest
  rcolors[r][0] = rcolors[r+3][0] = (rcolors[r+5][0]+rcolors[r+1][0]) / 2;
  rcolors[r][1] = rcolors[r+3][1] = (rcolors[r+5][1]+rcolors[r+1][1]) / 2;
  rcolors[r][2] = rcolors[r+3][2] = (rcolors[r+5][2]+rcolors[r+1][2]) / 2;
  rcolors[r+2][0] = (rcolors[r+1][0]+rcolors[r+3][0]) / 2;
  rcolors[r+2][1] = (rcolors[r+1][1]+rcolors[r+3][1]) / 2;
  rcolors[r+2][2] = (rcolors[r+1][2]+rcolors[r+3][2]) / 2;
  rcolors[r+4][0] = (rcolors[r+3][0]+rcolors[r+5][0]) / 2;
  rcolors[r+4][1] = (rcolors[r+3][1]+rcolors[r+5][1]) / 2;
  rcolors[r+4][2] = (rcolors[r+3][2]+rcolors[r+5][2]) / 2;
}

void r_colors(int c1,int c2,int c3,int rcolors[][3],int size){
  int r;
  rcolors[50][0] = c1;
  rcolors[50][1] = c2;
  rcolors[50][2] = c3;
  for(r=51;r<size-100;r++){
    rcolors[r][0] = rcolors[r-1][0] + pow(-1,rand()%2)*(rand()%3);
    rcolors[r][1] = rcolors[r-1][1] + pow(-1,rand()%2)*(rand()%3);
    rcolors[r][2] = rcolors[r-1][2] + pow(-1,rand()%2)*(rand()%3);
    if(rcolors[r][0]>255) rcolors[r][0]=255;
    if(rcolors[r][1]>255) rcolors[r][1]=255;
    if(rcolors[r][2]>255) rcolors[r][2]=255;
    if(rcolors[r][0]<0) rcolors[r][0]=0;
    if(rcolors[r][1]<0) rcolors[r][1]=0;
    if(rcolors[r][2]<0) rcolors[r][2]=0;

    if(rand()%200 < 1){    // random bands of high-variance colors
      v_band(rcolors,r,50);
      r+=5;
    }

    gfx_color(rcolors[r][0],rcolors[r][1],rcolors[r][2]);
    gfx_line(size,size-r,size,size-r);
    gfx_line(size+r,size,size+r,size);
  }

  for(;r<size-10;r++){  // white space
    rcolors[r][0] = rcolors[r][1] = rcolors[r][2] = 245;
    gfx_color(245,245,245);
    gfx_line(size,size-r,size,size-r);
    gfx_line(size+r,size,size+r,size);
  }
}

void theta_colors(int rcolors[][3],int size){
  int r,c1,c2,c3;
  double theta;
  for(r=50;r<size-100;r++){
    for(theta=0;theta<2*M_PI;theta+=.005){
      c1=rcolors[r][0]+pow(-1,rand()%2)*(rand()%15);
      c2=rcolors[r][1]+pow(-1,rand()%2)*(rand()%15);
      c3=rcolors[r][2]+pow(-1,rand()%2)*(rand()%15);
      if(c1<0) c1=0;
      if(c1>255) c1=255;
      if(c2<0) c2=0;
      if(c2>255) c2=255;
      if(c3<0) c3=0;
      if(c3>255) c3=255;
      gfx_color(c1,c2,c3);
      gfx_line(size+r*cos(theta),size-r*sin(theta),size+r*cos(theta+.008),size-r*sin(theta+.008));

      if(gfx_event_waiting())
      if(gfx_wait()=='q') return;
    }
  }

  for(;r<=size-10;r++){   // white space
    for(theta=0;theta<2*M_PI;theta+=.0025){
      c1=245-rand()%15;
      gfx_color(c1,c1,c1);
      gfx_line(size+r*cos(theta),size-r*sin(theta),size+r*cos(theta+.008),size-r*sin(theta+.008));

      if(gfx_event_waiting())
      if(gfx_wait()=='q') return;
    }
  }
}



int main(void){
  int wsize=600;
  int c1,c2,c3;   // base eyecolor is (c1,c2,c3); default: random
  int rcolors[300][3]={{0}};
  char c;

  srand(time(NULL));

  c1 = rand()%256;
  c2 = rand()%256;
  c3 = rand()%256;
  printf("Base eyecolor (r,g,b): ");
    scanf("%d,%d,%d",&c1,&c2,&c3);

  gfx_open(wsize,wsize,"Eye");
  r_colors(c1,c2,c3,rcolors,wsize/2);
  theta_colors(rcolors,wsize/2);

  while(c!='q') c=gfx_wait();

  return 0;
}
