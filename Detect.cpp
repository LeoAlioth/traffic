#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opencv2/text.hpp"

using namespace cv;
using namespace cv::text;
using namespace std;


Mat src; Mat src_gray;
Mat src_split[3];
int thresh = 80;
int max_thresh = 255;
RNG rng(12345);

int lowThreshold = 110;
int const max_lowThreshold = 255;
int ratio = 3;
int kernel_size = 3;
Mat dst, detected_edges;


Ptr<OCRTesseract> ocr = OCRTesseract::create(NULL, NULL, "0123456789", 3, 6);


/// Function header
void thresh_callback(int, void* );
void blob_detector();
void recognize_chars(Mat img);

/** @function main */
int main( int argc, char** argv ){
    /// Load source image and convert it to gray
    //cout << "opening video" << endl;
    VideoCapture capture("/media/pi/VID/vids.mp4");
    //VideoCapture capture(0);
    //cout << "video open" << endl;
    //src = imread( argv[1], CV_LOAD_IMAGE_COLOR);
    
    /// Convert image to gray and blur it
    
    //blur( src_gray, src_gray, Size(3,3) );


    /// Create Window
    //namedWindow( "RGB", CV_WINDOW_AUTOSIZE );
    //imshow( "RGB", src );
    //cout << "reading frame" << endl;


    if(!capture.read(src)) {
        cerr << "Unable to read next frame." << endl;
        cerr << "Exiting..." << endl;
        exit(EXIT_FAILURE);
    }
    //cout << "before callback" << endl;
    thresh_callback( 0, 0 );
    //blob_detector();
    //cout << "tresh, callback" << endl;


    namedWindow( "src", WINDOW_NORMAL );
    //resizeWindow("src", 400, 280);


    //cout << "window" << endl;
    createTrackbar( "Min Threshold:", "src", &lowThreshold, max_lowThreshold);

    //cout << "trackbar created" << endl;
    int time, complete;

    dst.create( src.size(), src.type() );
 

    while(true){
        //cout << "reading frame" << endl;
        time = clock();
        complete = clock();
        if(!capture.read(src)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //cout << "elapsed for reading: "  << ((clock() - time) / (float) (float) 800) << endl;

        imshow( "src", src);
        thresh_callback(0, 0);
        //cout << "elapsed ALL TOGETHER: "  << ((clock() - complete) / (float) (float) 800) << endl << endl << endl;

        waitKey(1);
        
        //cout << "frame read" << endl;
    }
    return(0);
}

/** @function thresh_callback */


void thresh_callback(int, void* ){

    /// Using Canny's output as a mask, we display our result
    //dst = Scalar::all(0);

    //src.copyTo( dst, detected_edges);
    //imshow( "canny", dst );
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //cout << "in callback" << endl;
    int time = clock();
    split(src, src_split);
    //cvtColor( src_split[0], src_gray, CV_BGR2GRAY );
    src_gray= src_split[0];
    //cout << "elapsed for grayscale: "  << (clock() - time) / (float) 800 << endl;
    //time = clock();
    equalizeHist(src_gray, src_gray);
    //cout << "elapsed for equalizing: "  << (clock() - time) / (float) 800 << endl;
    //time = clock();

    int blur_size = 2; 

    blur( src_gray, detected_edges, Size(blur_size,blur_size) );

    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

    /// Detect edges using Threshold
    //threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    //cout << "elapsed for canny: "  << (clock() - time) / (float) 800 << endl;
    //time = clock();

    /// Find contours

    /*Mat element = getStructuringElement( MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );
    

    erode( detected_edges, detected_edges, element );
    /*
    float factor = 1.5;
    element = getStructuringElement( MORPH_ELLIPSE,
                                        Size( 2*erosion_size*factor + 1, 2*erosion_size*factor+1 ),
                                        Point( erosion_size*factor, erosion_size*factor) );

    erode( detected_edges, detected_edges, element );
    */
    
    findContours( detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //cout << "elapsed for contours: "  << (clock() - time) / (float) 800 << endl;
    //time = clock();

    /// Find the rotated rectangles and ellipses for each contour
    vector<RotatedRect> minEllipse;

    Mat img = Mat::zeros( src.size(), CV_8UC3 );

    for( int i = 0; i < contours.size(); i++ ){ 
        if( contours[i].size() > 100 ){
            if (contourArea(contours[i]) > 1000){ 
                RotatedRect temp = fitEllipse(Mat(contours[i]));
                if( 0.80 < abs(temp.size.width/temp.size.height) && abs(temp.size.width/temp.size.height) < 1.2){
                    if (temp.size.width*temp.size.height/4*3.14 <= 1.1 * contourArea(contours[i])) {
                        //cout << area(temp) << " < 1.1* " << contourArea(contours[i]) << endl;
                        minEllipse.push_back(temp);
                        //drawContours(img, contours, i, Scalar(0,255,0), -1, 8);
                        //ellipse(img, temp, Scalar(0,255,0), 2, 8);
                    } else {
                        //cout << "Reject ellipse " << i << endl;
                        //drawContours(img, contours, i, Scalar(0,0,100), -1, 8);
                        //ellipse(img, temp, Scalar(0,0,100), 2, 8);
                    }    
                }
            }    
        }
    }
    //namedWindow("Ellipses", WINDOW_NORMAL);
    //imshow("Ellipses", detected_edges);
    //waitKey(1);
    //cout << "elapsed for fitting elipses: "  << (clock() - time) / (float) 800 << endl;
    //time = clock();
    /// Draw contours + rotated rects + ellipses

    /*
    Mat drawing = Mat::zeros( src.size(), CV_8UC3 );

    for( int i = 0; i< contours.size(); i++ ){
        if(contours[i].size()> 100){
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            // contour
            drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        }   
    }
    
    cout << "elapsed for drawing contours :"  << (clock() - time) / (float) 800 << endl;
    time = clock();

    */

    /*for (int i = 0; i < minEllipse.size(); ++i)
    {
        // ellipse
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        ellipse( drawing, minEllipse[i], color, 2, 8 );
    }*/

    //printf("%d\n", matches.size());
    /*
    Scalar color;
    for(int i = 0; i < minEllipse.size(); i++){
        ellipse( drawing, minEllipse[i], color, 2, 8 );
        ellipse( threshold_output, minEllipse[i], 170, 2, 8 );

    }

    cout << "elapsed for drawing ellipses:"  << (clock() - time) / (float) 800 << endl;
    time = clock();
    */

    Mat cropped;
    float crop = 0.6;
    for(int i = 0; i < minEllipse.size(); i++){
        int cx = minEllipse[i].center.x;
        int cy = minEllipse[i].center.y;

        int x = minEllipse[i].size.width * crop;
        int y = minEllipse[i].size.height * crop;
        if(0 < cx - x && 0 < cy - y && cx+x < src.cols && cy+y < src.rows){
            float crop = 0.65;
            Rect myROI(cx-x*crop, cy-y*crop, 2*x*crop, 2*y*crop);
            cropped = src(myROI);
            
            recognize_chars(cropped);
        }
    }


    

    // Show in a window
    //namedWindow( "tresholded", WINDOW_NORMAL );
    //resizeWindow("tresholded", 400, 300);
    //namedWindow( "Contours", WINDOW_NORMAL );
    //resizeWindow("Contours", 400, 300);
    //imshow( "Contours", drawing );
    //namedWindow( "src", WINDOW_NORMAL );
    //resizeWindow("src", 400, 280);
    //imshow("src", src);
    //imshow( "tresholded", threshold_output );
    //cout << "elapsed for displaying:"  << (clock() - time) / (float) 800 << endl;
    //time = clock();
}

void recognize_chars(Mat img){
    cvtColor( img, img, CV_BGR2GRAY );
    equalizeHist(img, img);
    threshold( img, img, 100, 255, THRESH_BINARY );
    int erosion_size = img.cols/30;
    //cout << "es " << erosion_size << endl;
    Mat element = getStructuringElement( MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );
    

    dilate( img, img, element );

    namedWindow( "sign", CV_WINDOW_AUTOSIZE );
    imshow( "sign", img );
    waitKey(1); 
    //printf("%s\n", "ocr started");    
    
    std::string speed;
    speed = ocr -> run(img, 60);
    if(speed.length() > 0){
        int spd = stoi(speed);
        switch(spd){
            case 30:
            case 40:
            case 50:
            case 60:
            case 70:
            case 90:
            case 100:
                cout << "SPEED: " << speed << endl;
        }
    }
}