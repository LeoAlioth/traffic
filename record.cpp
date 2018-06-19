#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(){
    VideoCapture vcap(0); 
    if(!vcap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }
    namedWindow("Frame", WINDOW_NORMAL);
    resizeWindow("Frame", 384, 216);
    vcap.set(CV_CAP_PROP_FRAME_WIDTH,800);
    vcap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    //vcap.set(CV_CAP_PROP_FPS, 30);

    int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int fps = vcap.get(CV_CAP_PROP_FPS);

    cout << "FPS: " << fps << endl;
    cout << "W: " << frame_width << endl;
    cout << "H: " << frame_height << endl;
    
    VideoWriter video("/home/pi/Desktop/out.avi",CV_FOURCC('M','J','P','G'),fps, Size(frame_width,frame_height),true);
    cout << "Started recording" << endl;
    for(;;){
        Mat frame;
        vcap >> frame;
        video.write(frame);
        imshow( "Frame", frame );
        char c = (char)waitKey(15);
        if( c == 27 ) break;
    }
    cout << "Stopped recording" << endl;
    return 0;
}