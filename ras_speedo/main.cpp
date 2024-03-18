#include "pch.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
//#include <opencv2/tracking.hpp>
#
#include <stdio.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

using namespace cv;
using namespace std;

int main()
{
       
    cout << "hello there...";
    waitKey(1);

    Mat frame;
    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_FFMPEG;      // 0 = autodetect default API
    string filename = "C:\\Users\\jrap017\\Videos\\testvid_01.mp4";
    
    // open selected camera using selected API
    cap.open(filename,apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open video file\n";
        //return -1;
    }
    namedWindow(filename, WINDOW_NORMAL);
    resizeWindow(filename, 1920, 1080);
        
     
     while (true) 
     {
         
         // Read a frame from the video
         cap >> frame;

         // Check if the video has ended
         if (frame.empty()) {
             std::cout << "End of video." << std::endl;
             break;
         }

         


         // Display the frame
         imshow(filename, frame);

         // Wait for a key press (30 milliseconds delay)
         int key = waitKey(30);

         // Break the loop if 'Esc' key is pressed
         if (key == 27) {
            cout << "Video playback terminated by user." << std::endl;
             break;
         }
     }

     // Release the video capture object and close the window
     cap.release();
     destroyAllWindows();
     


     return 0;

   
}


