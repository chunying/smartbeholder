#include "captst.h"

using namespace std; 
using namespace cv;
// mux_enable1: header11, mux_enable2: header12
int mux_select = 7, mux_enable1 = 0, mux_enable2 = 1;
//camera 1 2 3 mux configurations
int select_arr[3] = {0,1,0}, enable1_arr[3] = {0,0,1}, enable2_arr[3] = {1,1,0};
raspicam::RaspiCam_Cv Camera;     

int
captst_init(){
  /*set output*/
  if(PiSetup() == -1){
    cout << "init wiringPi error" << endl;
    return -1;
  }
  else
    cout << "Success setup camera" << endl;
  pinMode(mux_select, OUTPUT);
  pinMode(mux_enable1, OUTPUT);
  pinMode(mux_enable2, OUTPUT);
  return 0;
}

void
captst_deinit(){
  Camera.release();
  return;
}

void
captst_perform(char *buf, int buflen){
  Mat img[3];
  vector<Mat> vImg;
  cv::Mat image;
  Mat stImg;
  //do camera shots
  int nCount=10;
  //set camera params
  Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
  //3 cameras taker turns to shot
  for (int count = 0 ; count < CameraNum ; count ++){

    digitalWrite(mux_select, select_arr[count]);    
    digitalWrite(mux_enable1, enable1_arr[count]);    
    digitalWrite(mux_enable2, enable2_arr[count]);    

    Camera.set(CV_CAP_PROP_FRAME_WIDTH ,640);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT ,480);

    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {
      cerr<<"Error opening the camera"<<endl;
      return;
    }

    //Start capture
    cout<<"Capturing"<<endl;
    for ( int i = 1; i < nCount; i++ ) {
      Camera.grab();
      Camera.retrieve (image);
      if ( i%5 == 0 )
        cout<<"\r captured "<<i<<" images"<<std::flush;
      } 
      cout<<"Stop camera..."<<endl;
      Camera.release();
      //save image 
      string a = "raspicam_cv_image_";
      string b = ".jpg";
      char c = count + '0';
      string tmp = a + c + b;
      imwrite(tmp,image);
      cout<<"Image saved at raspicam_cv_image_" << count << ".jpg"  <<endl;

    }
    digitalWrite(mux_select, 0);    
    digitalWrite(mux_enable1, 0);    
    digitalWrite(mux_enable2, 0);    

    
    vImg.push_back( imread("raspicam_cv_image_0.jpg"));
    vImg.push_back( imread("raspicam_cv_image_1.jpg"));
    vImg.push_back( imread("raspicam_cv_image_2.jpg"));
  
    Stitcher stitcher = Stitcher::createDefault();

    Stitcher::Status status = stitcher.stitch(vImg, stImg);
  
    if (Stitcher::OK == status)
      cout << "stitch sucess\n" << endl;
    if(status == 1)
      cout <<"error 1:: size=" << vImg.size() << endl;
    else
      cout << status << endl;
    
    if(status == 0){
      Mat yuvImage, ansImg;
      //resize(rrrImg, ansImg, Size(1280,720));
      resize(stImg, ansImg, Size(1280,480));
      int bufLen = 1280*480* 3 /2;
      unsigned char* yImage = new unsigned char[bufLen];
      
      cvtColor(ansImg, yuvImage, CV_BGR2YUV_I420);
      memcpy(yImage, yuvImage.data, bufLen*sizeof(unsigned char));
      bcopy(yImage, buf, buflen/*buflen*/);
    }   
    else{
      cout << "Stitch error" <<endl;
    }
 
  return;
}
