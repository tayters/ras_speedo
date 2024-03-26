#include "pch.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

#define BLACK Scalar(0,0,0)
#define WHITE Scalar(255,255,255)
#define GREEN Scalar(0,255,0)
#define BLUE Scalar(255,0,0)
#define RED Scalar(0,0,255)
#define ORANGE Scalar(0,165,255)
#define RED2 Scalar(0,20,255)
#define FONT FONT_HERSHEY_PLAIN

#include <stdio.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

using namespace cv;
using namespace std;

bool mouseClick_Flag = false;
bool mouseClickRelease_Flag = false;
Point startpoint, endpoint;
bool drawing = false;

class PointData {
    public:
        Point p;
        int frame;
};
     

class Fish
{
    public:
        int index;
        vector <PointData> points;
        Scalar path_colour = RED;
        
        void drawPath(Mat frame) {

            if (points.size() > 0)
            {
                circle(frame, points.back().p, 2, path_colour, cv::FILLED);
                for (size_t i = 1; i < points.size(); ++i) {
                    circle(frame, points[i].p, 3, path_colour, cv::FILLED);
                    line(frame, points[i].p, points[i - 1].p, path_colour, 2);
                }
            }


        }

        //Constructor
        Fish(int i){
            index = 1;
        }
};


void onMouseClick(int event, int x, int y, int flags, void* userdata) { //function to track mouse movement and click//
     
    PointData* pd = static_cast<PointData*>(userdata);

    if (event == EVENT_LBUTTONDOWN) { //when left button clicked//
       
        mouseClick_Flag = true;
        pd->p.x = x;
        pd->p.y = y;
        startpoint = Point(x, y);
        endpoint = Point(x, y);
        drawing = true;
    }
    else if (event == EVENT_MOUSEMOVE && drawing) {
        endpoint = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP) {
        endpoint = Point(x, y);
        drawing = false;
        mouseClickRelease_Flag = true;
    }

 
}

double calculateLength(const Fish f) {
    double totalLength = 0.0;
    
    for (size_t i = 1; i < f.points.size(); ++i) {
        double dx = f.points[i].p.x - f.points[i - 1].p.x;
        double dy = f.points[i].p.y - f.points[i - 1].p.y;
        double distance = sqrt(dx * dx + dy * dy);
        totalLength += distance;
    }
    return totalLength;
}

double calculateBL(Point p1, Point p2) {
    double totalLength = 0.0;

        double dx = abs(p1.x - p2.x);
        double dy = abs(p1.y - p2.y);
        double length = sqrt(dx * dx + dy * dy);
       
    
    return length;
}

void updateVideoData(VideoCapture cap, Mat frame)
{
    rectangle(frame, Rect(Point(0, 0), Size(200, 50)), BLUE, FILLED);
    putText(frame, "Frame: " + to_string((int)cap.get(CAP_PROP_POS_FRAMES)), Point(10, 15), FONT, 1, WHITE, 2, 1);
    putText(frame, "Time: " + to_string((float)cap.get(CAP_PROP_POS_MSEC) / 1000) + "ms", Point(10, 30), FONT, 1, WHITE, 2, 1);

}

int main()
{
    Mat frame, track_frame, start_frame;
    VideoCapture cap;
    PointData mouse_pd;
    string filename = "C:\\Users\\jrap017\\Videos\\testvid_01_reduced.mp4";
    int deviceID = 0, apiID = cv::CAP_FFMPEG;      
    double length;
    int frame_number, n = 0;
    double bodylength = 0;

    Fish fish(0);
    vector <Fish> school;
    
    // open selected camera using selected API
    cap.open(filename,apiID);
    
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open video file\n";
        //return -1;
    }

    //Initialize window for unprocessed data
    namedWindow(filename, WINDOW_NORMAL);
    resizeWindow(filename, 1080, 720);
 
   
    //Capture first frame
    cap >> frame;
    start_frame = frame.clone();
    cv::imshow(filename, frame);
     
    updateVideoData(cap, frame);

    //Set up mouse callback
    setMouseCallback(filename, onMouseClick, &mouse_pd);

    //Measure body length
    /*
    cout << "Meaure BL" << endl;
    cout << "Press enter to confirm" << endl;

    while (true)
    {
        if (drawing) {
            frame = start_frame.clone();
            line(frame, startpoint, endpoint, GREEN, 2);
            cv::imshow(filename, frame);              
        }

        if (mouseClickRelease_Flag)
        {
            cout << "startpoint: " << startpoint << endl;
            cout << "endpoint: " << endpoint << endl;
            bodylength = calculateBL(startpoint, endpoint);
            cout << "Body Length (px): " << bodylength << endl;
            mouseClickRelease_Flag = false;
        }
        
        char key = waitKey(10);
        if (key == 27) // Esc key to exit
            break;
        if (key == 13) {
            cout << "BL selected!" << endl;
            mouseClick_Flag = false;
            break;
        } 
    }
    */

    //Tracking

     while (true) 
     {
         if(mouseClick_Flag)
         {
             mouseClick_Flag = false;
             
             updateVideoData(cap, frame);

             mouse_pd.frame = cap.get(CAP_PROP_POS_FRAMES);
             fish.points.push_back(mouse_pd);

             fish.drawPath(frame);
             imshow(filename, frame);
             
             length = calculateLength(fish);
             cout << "Trace length:" << length << endl;

         }

         // Wait for a key press (30 milliseconds delay)
         int key = waitKey(30);

         //Frame step forward or backwards
         if ((key == 46) || (key == 44))
         {
             if (key == 44)
             {
                int tmp = cap.get(CAP_PROP_POS_FRAMES);
                cap.set(CAP_PROP_POS_FRAMES, tmp - 2);
             }
             
             cap >> frame;

             // Check if the video has ended
             if (frame.empty()) {
                 std::cout << "End of video." << std::endl;
                 break;
             }
                        
             updateVideoData(cap, frame);
             fish.drawPath(frame);
             imshow(filename, frame);
         }

         // Break the loop if 'Esc' key is pressed
         if (key == 27) {
             cout << "Video playback terminated by user." << std::endl;
             break;
         }
         
         if (key > 0x29 && key < 0x40) {
             n = key - 48;
             cout << n << " was pressed!" << endl;
         }

     }

     // Release the video capture object and close the window
     cap.release();
     destroyAllWindows();
     
     return 0;
        
}


