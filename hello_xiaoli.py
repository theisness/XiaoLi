import requests
import cv2
import numpy as np
from PIL import Image 
import io
import time
from PIL import ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True
tt = "http://192.168.43.145:80/";
stream = "http://192.168.43.145:81/stream";
pic = "http://192.168.43.145:80/capture"

def Process(cimg):
    HSV = cv2.cvtColor(cimg, cv2.COLOR_BGR2HSV)  #RGB 转为 HSV
    H, S, V = cv2.split(HSV)    #分离 HSV 三通道
    parr = np.asarray(pimg);
    r = parr[:,:,0];
    g = parr[:,:,1];
    b = parr[:,:,2];
    
    
    res=((r*0.45 + g*0.55)- b*0.7);

    #res[res>145]=0;
    res[res>90]=255;
    res[res<0]=0;
    res[res>255]=255;
    #res[res<254]=0;

    res1 =H;
    m = cv2.inRange(res1, 0,19);
    m = m.astype(bool);
    #res1[m] = 255;
    #res1[res1>10]=255;
    res1[res1<0]=0;
    res1[res1>255]=255;
    res1 = res1.astype(np.uint8)
    res=res.astype(np.uint8)
    
    cv2.imshow("im3", H);
    return res;

#key = 0
#while  key != 27:
"""while 1:
    r = requests.get(pic);
    if len(r.content)==640*480*3:
        pimg = Image.frombytes("RGB", (640,480), r.content);
        cimg = cv2.cvtColor(np.asarray(pimg), cv2.COLOR_RGB2BGR) ; 
        cv2.imshow('im', cimg);
        key = cv2.waitKey(1);
        if key==27:
            break;
cv2.destroyWindow('im');
#cimg = cv2.cvtColor(np.asarray(pimg), cv2.COLOR_RGB2BGR) ; 
"""
garr = np.empty( (176, 240), dtype = np.uint8 );
key=0;
while 1:
    r = requests.get(pic);
    if len(r.content)>5000:#==240*176*3:
        pimg = Image.frombytes("RGB",(240,176),r.content);
        #pimg = Image.open(io.BytesIO( r.content ));
        #cimg = cv2.cvtColor(  np.array(pimg) , cv2.COLOR_RGB2BGR) ;
        cimg = np.asarray(pimg)
        cv2.imshow('im1',cimg);
        garr = Process(cimg);
        cv2.imshow('im2', garr);
        key=cv2.waitKey(10);
        if key==27: break;

cv2.destroyAllWindows();
#cv2.destroyWindow('im2');
"""
r = requests.get(pic);
pimg = Image.frombytes("RGB",(240,176),r.content);
pimg
cimg = np.asarray(pimg)
cv2.imshow('im1',cimg);
key=cv2.waitKey(10);
"""