#include "Arduino.h"
inline uint8_t geth(uint8_t r, uint8_t g, uint8_t b){
  uint8_t maxnum, minnum, h;
  if(r>g){
    if(r>b){
      maxnum = r;
      minnum = g>b?b:g;
    }
    else{
      maxnum = b;
      minnum = g;
    }
  }
  else{
    if(g>b){
      maxnum =g;
      minnum = b>r?r:b;
    }
    else{
      maxnum = b;
      minnum = r;
    }
  }
  if(maxnum == minnum){
    h=0;
  }
  else{
    if(r==maxnum ){
      if(g>=b){
        h = 60*(g-b)/(maxnum-minnum);
      }
      else{
        h = 60*(g-b)/(maxnum-minnum)+360;
      }
    }
    else if(g == maxnum){
      h = 60*(b-r)/(maxnum-minnum)+120;
    }
    else{
      h = 60*(r-g)/(maxnum-minnum)+240;
    }
  }
  return h;
}

void rgb2h(short width, short height, uint8_t*rgbuf, uint8_t *H_resbuf){
 for(int i=0; i<height; i++){
      for(int j=0; j<width; j++){
        int index = i*width*3+j*3;
        H_resbuf[width*i+j]=geth(rgbuf[index+2],rgbuf[index+1],rgbuf[index]) ;
      }
 }
}

uint8_t gfilter(short width, short height, uint8_t*gbuf, uint8_t *resbuf, short*desx, short*desy){
  const int fsx=8, fsy = 8, pixelnum = fsx*fsy; //it should be odd
  int i=0,j=0;
  int index=0;
  short xmin=width,xmax=0,ymin=height,ymax=0;
  for( i=0; i<height-fsy; i++){
      for( j=0; j<width-fsx; j++){
        float add = 0;
        for(int m=i,index=i*width+j; m<i+fsy; m++, index=m*width+j){
          for(int n = j; n<j+fsx; n++, index++){
           add+=gbuf[index];
          }
        }
        add/=pixelnum;
        if(add>0.7){
          if( i>ymax) ymax=i;
          if(i<ymin)ymin=i;
          if(j<xmin) xmin=j;
          if(j>xmax) xmax=j;
          //resbuf[i*width+j]=255;
        }
       // else resbuf[i*width+j]=0;*/
      }
  }
  if( xmin<xmax && ymin<ymax){
    *desx = (xmax-xmin)/2;
    *desy = (ymax-ymin)/2;
    return 1;
  }
  return 0;
}
void binfilter(short width, short height, uint8_t*binbuf, uint8_t *resbuf){
  const int maxsx = 4*8;
  const int fsx=9, fsy = 9, pixelnum = fsx*fsy; //it should be odd

 /* int i=0,j=0;
  int index=0;
  for( i=0; i<height-fsy; i++){
      for( j=0; j<width-fsx; j++){
        float add = 0;
        for(int m=i,index=i*width+j; m<i+fsy; m++, index=m*width+j){
          for(int n = j; n<j+fsx; n++, index++){
           add+=gbuf[index];
          }
        }
        add/=pixelnum;
        if(add>0.8)resbuf[i*width+j]=255;
        else resbuf[i*width+j]=0;
      }
 }*/
}

//width must be multipul 8.
void gray2bin(short width, short height, uint8_t*gbuf, uint8_t*binbuf){
  int indexg=0, indexb=0;
  for(int i=0; i<height; i++){
    for(int j=0; j<width/8; j++, indexb++){
      for(int k=7; k>=0; k--, indexg++){
        binbuf[indexb] |= (gbuf[indexg]<<k);
      }
    }
  }
}

uint8_t color_process(short width, short height, uint8_t*rgbuf, uint8_t*graybuf, uint8_t * hbuf, uint8_t *resbuf){
  int k=0,index=0,i=0,j=0;
  for( i=0; i<height; i++){
      for( j=0; j<width; j++, index+=3){
        uint8_t h = geth(rgbuf[index+2],rgbuf[index+1],rgbuf[index]) ;
       // graybuf[k]=h;     //here can show the h channel in hsv.
       if( h>14 && h<60){ hbuf[k]=1; }   //;graybuf[k]=255;here just binarilize the pic
        else { hbuf[k]=0; }              //; graybuf[k]=0;
        resbuf[k] = 0;
        k++;
      }
  }
  const int fsx=9, fsy = 9, pixelnum = fsx*fsy; //it should be odd
  short xmin=width,xmax=0,ymin=height,ymax=0, desx, desy, num=0;
  uint32_t xadd=0, yadd=0 ;
  static short lastnum=0 ;
  
  for( i=0; i<height-fsy; i++){
      for( j=0; j<width-fsx; j++){
        float add = 0;
        for(int m=i,index=i*width+j; m<i+fsy; m++, index=m*width+j){
          for(int n = j; n<j+fsx; n++, index++){
           add+=hbuf[index];
          }
        }
        add/=pixelnum;
        if(add>0.95){
          if( i>ymax) ymax=i;
          if(i<ymin)ymin=i;
          if(j<xmin) xmin=j;
          if(j>xmax) xmax=j;
          xadd+=j; yadd+=i;num++;
        }
      }
  }
  if( xmin<xmax && ymin<ymax &&num>20){
    desx = xadd/num + fsx/2; desy = yadd/num + fsy/2;
    short r1 = (xmax - xmin)/2;
    short r2 = (ymax - ymin)/2;
    r1 = (r1+r2)/2;
    //Serial.printf("x %d y %d r %d\r\n",desx, desy, (r1+r2)/2);
    Serial.write((uint8_t*)"x=",2);
    Serial.write((uint8_t*)&desx,2);
       Serial.write((uint8_t*)"y=",2);
       Serial.write((uint8_t*)&desy,2);
       Serial.write((uint8_t*)"r=",2);
       Serial.write((uint8_t*)&r1,2);
       Serial.write((uint8_t*)"\r\n",2);
    return 1;
  }
  return 0;
  if(num>20){
    /*if(lastnum ==0){ lastnum=num;return 0;}
    if(abs(lastnum-num)>70){ lastnum=0; return 0;}
    else {
      //num = lastnum*0.3+num*0.7;
      lastnum = num;
    }*/
    desx = xadd/num + fsx/2; desy = yadd/num + fsy/2;
    /*resbuf[desy*width+desx]=255;
    resbuf[desy*width+desx+1]=255;
    resbuf[desy*width+desx-1]=255;
    resbuf[(desy-1)*width+desx]=255;
    resbuf[(desy+1)*width+desx]=255;*/
    //Serial.printf("x %d y %d n %d\r\n",desx, desy, num);
    Serial.write((uint8_t*)"x=",2);
       Serial.write((uint8_t*)&desx,2);
       Serial.write((uint8_t*)"y=",2);
       Serial.write((uint8_t*)&desy,2);
       Serial.write((uint8_t*)"n=",2);
       Serial.write((uint8_t*)&num,2);
       Serial.write((uint8_t*)"\r\n",2);
    return 1;
  }
  //lastnum=0;
  return 0;
}
