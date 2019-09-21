import requests
import cv2
import numpy as np
from PIL import Image 
import io
import time
from PIL import ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True

pic = "http://192.168.43.145/capture"
binpic = "http://192.168.43.145/pic"

import socket 
socket.setdefaulttimeout(1) 


#pimg = Image.open(io.BytesIO(r.content))
while 1:
    r = requests.get(pic, 'timeout=(1,1)');
    
    if len(r.content)!=160*120:
        continue;
    pimg = Image.frombytes("L",(160,120),r.content)
    cimg = cv2.cvtColor( np.array(pimg), cv2.COLOR_RGB2BGR) ;
    cv2.imshow("im1",cimg);
    r = requests.get(binpic);
    pimg = Image.frombytes("L",(160,120),r.content)
    cimg = cv2.cvtColor( np.array(pimg), cv2.COLOR_RGB2BGR) ;
    cv2.imshow("im2",cimg);
    if cv2.waitKey(100)==27: break;
"""
r = requests.get(pic);
pimg = Image.frombytes("L",(160,120),r.content)
pimg

r = requests.get(binpic);
pimg = Image.frombytes("L",(160,120),r.content)
pimg
"""