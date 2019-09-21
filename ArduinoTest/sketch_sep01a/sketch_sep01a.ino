#include <Arduino.h>
#include <esp_camera.h>
#include <esp_heap_caps.h>
#include <esp_http_server.h>
#include <WiFi.h>
#include "hsv.hpp"

char* ssid = "theisness";
char* password = "12345678";

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

char led_flag = 0;
uint8_t* rgbuf;
uint8_t* graybuf;
uint8_t* jpgbuf;
uint32_t jpglen;
uint8_t* hbuf=0;
uint8_t* binbuf=0;
camera_fb_t * fb = NULL;
#define CAMON_PIN 12
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define IMAGE_LEN IMAGE_WIDTH*IMAGE_HEIGHT*3

uint8_t ProcessRGB(){
  const float weight[3] = {0.5, 0.5,-0.6};
  //if( fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgbuf)){ 
     uint32_t xx=0, yy=0, num=0;
    for(int i=0; i<IMAGE_HEIGHT; i++){
      for(int j=0; j<IMAGE_WIDTH; j++){
        int index = i*IMAGE_WIDTH*3+j*3;
        float res= (float)rgbuf[index]*weight[2] +(float)rgbuf[index+1]*weight[1] +(float)rgbuf[index+2]*weight[0] ;
        res=(uint8_t)constrain(res,0,255);
        graybuf[IMAGE_WIDTH*i+j]=res;
        if(res> 115){
          xx+=j; yy+=i; num++;
        }
      }
    }
    if(num>5){
      uint16_t x = xx/num, y=yy/num, n=num;
      Serial.write((uint8_t*)"x=",2);
       Serial.write((uint8_t*)&x,2);
       Serial.write((uint8_t*)"y=",2);
       Serial.write((uint8_t*)&y,2);
       Serial.write((uint8_t*)"n=",2);
       Serial.write((uint8_t*)&n,2);
       Serial.write((uint8_t*)"\r\n",2);
      //Serial.printf("x=%dy=%dn=%d\r\n", x, y, num);
    }
 // }
}
void capture(){
  fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
     Serial.println("Camera capture failed");
     return;   
  }
  else{
    //Serial.println("Camera capture success");
  
  }
  color_process(160, 120, fb->buf,graybuf,hbuf, binbuf);
  esp_camera_fb_return(fb);
}
static esp_err_t pic_handler(httpd_req_t *req){
    //fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start ;
    int64_t fr_end ;
   /* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }*/
    
       fr_start = esp_timer_get_time();  
       //ProcessRGB(); 
       //httpd_resp_set_type(req, "image/jpeg");
       //httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
       //color_process(160, 120, fb->buf,graybuf,hbuf, binbuf);
         res = httpd_resp_send(req, (const char *)binbuf, 160*120);
          fr_end = esp_timer_get_time();
         Serial.printf("jpg sent. Time=%dms\n", (uint32_t)((fr_end - fr_start)/1000));
     // esp_camera_fb_return(fb);
  return res;
}
static esp_err_t capture_handler(httpd_req_t *req){
    fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start ;
    int64_t fr_end ;
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
       fr_start = esp_timer_get_time();  
       //ProcessRGB(); 
       //httpd_resp_set_type(req, "image/jpeg");
       //httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
       short x,y;
       if(color_process(160, 120, fb->buf,graybuf,hbuf, binbuf)){
          //here get the target, otherwise didn't.
       }
         res = httpd_resp_send(req, (const char *)graybuf, 120*160);
          fr_end = esp_timer_get_time();
         Serial.printf("jpg sent. Time=%dms\n", (uint32_t)((fr_end - fr_start)/1000));
      esp_camera_fb_return(fb);
  return res;
}
httpd_handle_t camera_httpd = NULL;
void startCameraServer(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_uri_t capture_uri = {
        .uri       = "/capture",
        .method    = HTTP_GET,
        .handler   = capture_handler,
        .user_ctx  = NULL
    };
    httpd_uri_t pic_uri = {
        .uri       = "/pic",
        .method    = HTTP_GET,
        .handler   = pic_handler,
        .user_ctx  = NULL
    };
    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &pic_uri);
    }
   
}

void setup() {
   Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  pinMode(CAMON_PIN, INPUT_PULLUP);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB888;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 0;
    config.fb_count = 4;
    Serial.println("PSRAM found. pixel = QQVGA.");
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  if( rgbuf = (uint8_t*)heap_caps_malloc(160*120*3, MALLOC_CAP_SPIRAM) ){
    //Serial.println("malloc success 160*120*3");
  }
  else{
     //Serial.println("malloc fail 160*120*3");
     while(1);
  }
  if( graybuf = (uint8_t*)heap_caps_malloc(160*120, MALLOC_CAP_SPIRAM) ){
    //Serial.println("malloc success 160*120*3");
  }
  else{
     //Serial.println("malloc fail 160*120*3");
     while(1);
  }
  if( hbuf = (uint8_t*)heap_caps_malloc(160*120, MALLOC_CAP_SPIRAM) ){
    //Serial.println("malloc success 160*120*3");
  }
  else{
     //Serial.println("malloc fail 160*120*3");
     while(1);
  }
  if( binbuf = (uint8_t*)heap_caps_malloc(160*120, MALLOC_CAP_SPIRAM) ){
    //Serial.println("malloc success 160*120");
  }
  else{
     //Serial.println("malloc fail 160*120");
     while(1);
  }
  if( jpgbuf = (uint8_t*)heap_caps_malloc(160*120*3, MALLOC_CAP_SPIRAM) ){
    //Serial.println("malloc success 160*120*3");
  }
  else{
     //Serial.println("malloc fail 160*120*3");
     while(1);
  }
  /*
   WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  */
}
 
void loop() {
  
  //int a=esp_timer_get_time();
 //delay(1);
  if( digitalRead(CAMON_PIN) ==0){
    
    capture();
    //int b=esp_timer_get_time();
    //Serial.printf("ms%d",(b-a)/1000);
  }
}
