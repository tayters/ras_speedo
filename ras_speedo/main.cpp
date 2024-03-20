#include "pch.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

#define ORANGE Scalar(0,165,255)

#include <stdio.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

using namespace cv;
using namespace std;

bool mouseClick_Flag = false;

class PointData {
    public:
        Point p;
};


void onMouseClick(int event, int x, int y, int flags, void* userdata) { //function to track mouse movement and click//
    
    
    PointData* pd = static_cast<PointData*>(userdata);

    if (event == EVENT_LBUTTONDOWN) { //when left button clicked//
       
        mouseClick_Flag = true;
        pd->p.x = x;
        pd->p.y = y;
    }
  
    


}

double calculateLength(const vector<Point>& points) {
    double totalLength = 0.0;
    for (size_t i = 1; i < points.size(); ++i) {
        double dx = points[i].x - points[i - 1].x;
        double dy = points[i].y - points[i - 1].y;
        double distance = sqrt(dx * dx + dy * dy);
        totalLength += distance;
    }
    return totalLength;
}


int main()
{

     cout << "hello there...";

     waitKey(1);

    Mat frame, track_frame;
    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_FFMPEG;      // 0 = autodetect default API
    string filename = "C:\\Users\\jrap017\\Videos\\testvid_01_reduced.mp4";
    PointData pd;
    vector <Point> points;
    double length;
    
    // open selected camera using selected API
    cap.open(filename,apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open video file\n";
        //return -1;
    }
    namedWindow(filename, WINDOW_NORMAL);
    resizeWindow(filename, 1080, 720);
   

    cap >> frame;
    imshow(filename, frame);

    setMouseCallback(filename, onMouseClick, &pd);

     while (true) 
     {
         
         // Read a frame from the video
         
         if (mouseClick_Flag)
         {
             mouseClick_Flag = false;
             imshow(filename, frame);
             cap >> frame;
             imshow(filename, frame);

             // Check if the video has ended
             if (frame.empty()) {
                 std::cout << "End of video." << std::endl;
                 break;
             }
             //cout << "Left click has been made, Position:(" << pd.p.x << "," << pd.p.y << ")" << endl;
             points.push_back(pd.p);
             circle(frame, pd.p, 2, cv::Scalar(0, 0, 255), cv::FILLED);
             line(frame, pd.p, points[points.size()-2], cv::Scalar(0, 0, 255), 1);

             // Draw each point on the frame
             if (points.size() > 2)
             {
                 for (size_t i = 1; i < points.size()-1; ++i) {
                     cv::circle(frame, points[i], 3, cv::Scalar(0, 0, 255), cv::FILLED);
                     line(frame, points[i], points[i - 1], cv::Scalar(0, 0, 255), 2);
                 }
             }

             imshow(filename, frame);

             length = calculateLength(points);

             cout << "Trace length:" << length << endl;


         }
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


