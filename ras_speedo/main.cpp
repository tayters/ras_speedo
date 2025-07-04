#include "pch.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>

#define RED Scalar(0,0,255)
#define ORANGE Scalar(0,165,255)
#define YELLOW Scalar(0,255,255)
#define GREEN Scalar(0,255,0)
#define BLUE Scalar(200,0,0)
#define CYAN Scalar(255,255,0)
#define PURPLE Scalar(255,0,255)
#define BGREEN Scalar(50,200,0)
#define PRED Scalar(155,0,255)
#define WHITE Scalar(255,255,255)
#define BLACK Scalar(0,0,0)
#define FONT FONT_HERSHEY_PLAIN 

using std::cin;
using std::cout;
using std::endl;

using namespace cv;
using namespace std;

vector <Scalar> colours {RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, PURPLE, BGREEN, PRED, WHITE};
bool mouseClick_Flag = false;
bool mouseClickRelease_Flag = false;
bool showAllPaths_Flag = false;
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
        double bodylength;
        vector <PointData> points;
        Scalar path_colour;
                     

        void addPoint(PointData pd)
        {
            if (points.size() > 0)
            {

                for (int i = 0; i < points.size(); i++)
                {
                    //replace if existing
                    if (points[i].frame == pd.frame)
                    {
                        points[i] = pd;
                        break;
                    }
                    else if (points[i].frame > pd.frame)
                    {
                        points.insert(points.begin() + i, pd);
                        break;
                    }
                    else if (i == points.size() - 1)
                    {
                        points.push_back(pd);
                        
                    }
                }
            }
            else
            {
                points.push_back(pd);
            }

            
        }
        
        void drawPath(Mat frame, int frame_number) {

            

            if (points.size() == 1)
            {
                circle(frame, points.back().p, 2, path_colour, cv::FILLED);
                circle(frame, points.front().p, 2, path_colour, cv::FILLED);
            }
            else if (points.size() > 0)
            {
                circle(frame, points.front().p, 2, path_colour, cv::FILLED);
            
                for (size_t i = 1; i < points.size(); ++i) {
                    
                    if (points[i].frame > frame_number)
                        break;
                                        
                    circle(frame, points[i].p, 3, path_colour, cv::FILLED);
                    line(frame, points[i].p, points[i - 1].p, path_colour, 2);
                    
                }
                
            }
         }

        int getDirection() 
        {
            int d = points.back().p.y - points.front().p.y;
            if (d >= 0)
                return 1; //Clockwise
            else
                return -1; //Anti-Clockwise
        }

        double getPathLength(double bl) 
        {
            double tl = 0;

            for (size_t i = 1; i < points.size(); ++i) {
                double dx = points[i].p.x - points[i - 1].p.x;
                double dy = points[i].p.y - points[i - 1].p.y;
                double distance = sqrt(dx * dx + dy * dy);
                tl += distance;
            }
            return tl/bl;
        }

        double getSwimTime()
        {
            int frame_total = points.back().frame - points.front().frame;
            double swim_time = frame_total * 0.1;

            return swim_time;
        }

        
        //Constructor
        Fish(int i, Scalar s){
            index = 1;
            path_colour = s;
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

double calculatePathLength(const Fish f) {
    double totalLength = 0.0;
    
    for (size_t i = 1; i < f.points.size(); ++i) {
        double dx = f.points[i].p.x - f.points[i - 1].p.x;
        double dy = f.points[i].p.y - f.points[i - 1].p.y;
        double distance = sqrt(dx * dx + dy * dy);
        totalLength += distance;
    }
    return totalLength;
}

double calculateBL(Point p1, Point p2)
{
    double dx = abs(p1.x - p2.x);
    double dy = abs(p1.y - p2.y);
    double length = sqrt(dx * dx + dy * dy);
        
    return length;
}

void updateVideoData(VideoCapture cap, Mat frame, int n, Scalar c)
{
    rectangle(frame, Rect(Point(0, 0), Size(200, 80)), BLUE, FILLED);
    //line(frame, Point(640,0),Point(640,720), WHITE, 2);
    putText(frame, "Frame: " + to_string((int)cap.get(CAP_PROP_POS_FRAMES)), Point(10, 15), FONT, 1, WHITE, 2, 1);
    putText(frame, "Time: " + to_string((float)cap.get(CAP_PROP_POS_MSEC) / 1000) + "s", Point(10, 30), FONT, 1, WHITE, 2, 1);
    putText(frame, "FISH: " + to_string(n), Point(10, 45), FONT, 1, c, 2, 1);
}

void writeSchoolData(vector <Fish> &school, ofstream &outFile, double bl)
{
    cout << "Computing fish speeds and directions..." << endl << endl;
    cout << "Fish# " << "BodyLength(px) " << "PathLength(BL) " << "SwimTime(s) " << "SwimSpeed(BL/s) " << "Direction" << endl;
    outFile << "Fish# " << "BodyLength(px) " << "PathLength(BL) " << "SwimTime(s) " << "SwimSpeed(BL/s) " << "Direction" << endl;

    for (size_t n = 0; n < 10; n++)
    {
        cout << n << " ";
        outFile << n << " ";
        cout << setprecision(3) << school[n].bodylength << " ";
        outFile << setprecision(3) << school[n].bodylength << " ";

        double path_length_bl = school[n].getPathLength(school[n].bodylength);
        cout << setprecision(3) << path_length_bl << " ";
        outFile << setprecision(3) << path_length_bl << " ";

        if (school[n].points.size() > 0)
        {
            double swim_time = school[n].getSwimTime();
            cout << setprecision(3) << swim_time << " ";
            outFile << setprecision(3) << swim_time << " ";

            double swim_speed = path_length_bl / swim_time;
            cout << setprecision(3) << swim_speed << " ";
            outFile << setprecision(3) << swim_speed << " ";

            cout << school[n].getDirection() << endl;
            outFile << school[n].getDirection() << endl;

        }
        else
        {
            cout << 0 << endl;
            outFile << 0 << endl;
        }
    }
    
    outFile << endl << "---Raw Data---" << endl;
    for (size_t n = 0; n < 10; n++)
    {
        outFile << "Fish " << n << " ";
        for (size_t i = 0; i < school[n].points.size(); ++i)
        {
            outFile << "(" << school[n].points[i].frame << "," << school[n].points[i].p.x << "," << school[n].points[i].p.y << ") ";
        }
        outFile << endl;
    }
}

int main()
{
    Mat src, out_frame, start_frame;
    VideoCapture cap;
    PointData mouse_pd;
    ofstream outFile;
    string filename, input_file, out_filename;
    int deviceID = 0, apiID = cv::CAP_FFMPEG, frame_number, n = 0;
    double length, defaultbodylength = 200;
   
    vector <Fish> school;

    cout << "Enter file path:";
    cin >> filename;
    out_filename = filename.substr(0, filename.length() - 4) + ".txt";
    
    //construct school of fish
    for (int i = 0; i < 10; i++)
    {
        school.push_back(Fish(i, colours[i]));
    };
    
    // open selected camera using selected API
    cap.open(filename,apiID);
    
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open video file\n";
        return 0;
    }

    //Initialize window for unprocessed data
    namedWindow(filename, WINDOW_NORMAL);
    resizeWindow(filename, 1920, 1080);
 
    //Capture first src
    cap >> src;
    start_frame = src.clone();
    out_frame = src.clone();
    imshow(filename, src);
     
    updateVideoData(cap, src, n, school[n].path_colour);

    //Set up mouse callback
    setMouseCallback(filename, onMouseClick, &mouse_pd);

    //Measure body length
    cout << "Draw line to measure BL" << endl;
    cout << "Press enter to confirm" << endl;



    while (true)
    {
        if (drawing) {
            src = out_frame.clone();
            line(src, startpoint, endpoint, GREEN, 2);
            cv::imshow(filename, src);              
        }

        if (mouseClickRelease_Flag)
        {
            //cout << "startpoint: " << startpoint << endl;
            //cout << "endpoint: " << endpoint << endl;
            defaultbodylength = calculateBL(startpoint, endpoint);
            cout << "Body Length (px): " << defaultbodylength << endl;
                        mouseClickRelease_Flag = false;
        }
        
        char key = waitKey(10);

        if (key == 27) // Esc key to exit
            break;

        if (key == 13) {
            cout << "BL selected!" << endl;
            mouseClick_Flag = false;
            src = start_frame.clone();
            out_frame = start_frame.clone();
            updateVideoData(cap, out_frame, n, school[n].path_colour);
            imshow(filename, out_frame);

            for (int i = 0; i < 10; i++)
            {
                school[i].bodylength = defaultbodylength;
            }

            break;
        } 

        //Frame step forward or backwards
        if ((key == 46) || (key == 44))
        {
            if (key == 44)
            {
                //go back one frame
                int tmp = cap.get(CAP_PROP_POS_FRAMES);
                cap.set(CAP_PROP_POS_FRAMES, tmp - 2);
            }
            

            cap >> src;

            // Check if the video has ended
            if (src.empty()) {
                std::cout << "End of video." << std::endl;
                cap.set(CAP_PROP_POS_FRAMES, 0);
                cap >> src;
                //break;
            }

            src.copyTo(out_frame);
            imshow(filename, out_frame);
        }
    }
    
    cout << "Click to start tracking..." << endl;
    cout << "Press 'h' to see commands" << endl;

    //Tracking
    while (true)
    {
        if (mouseClick_Flag)
        {
            mouseClick_Flag = false;

            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);

            mouse_pd.frame = cap.get(CAP_PROP_POS_FRAMES);

            school[n].addPoint(mouse_pd);
            school[n].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
            imshow(filename, out_frame);

            length = calculatePathLength(school[n]);
            //cout << "Trace length:" << length << endl;
        }

        // Wait for a key press (30 milliseconds delay)
        int key = waitKey(30);

        //Frame step forward or backwards
        if ((key == 46) || (key == 44))
        {
            if (key == 44)
            {
                //go back one frame
                int tmp = cap.get(CAP_PROP_POS_FRAMES);
                cap.set(CAP_PROP_POS_FRAMES, tmp - 2);
            }

            cap >> src;

            // Check if the video has ended
            if (src.empty()) {
                std::cout << "End of video." << std::endl;
                cap.set(CAP_PROP_POS_FRAMES, 0);
                cap >> src;
                //break;
            }

            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);
            if (showAllPaths_Flag)
            {
                for (int i = 0; i < 10; i++)
                {
                    school[i].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
                }
            }
            else
            {
                school[n].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
            }
            imshow(filename, out_frame);
        }

        // Break the loop if 'Esc' key is pressed
        if (key == 27) {
            cout << "Video playback terminated by user." << std::endl;
            break;
        }

        //Switch between animals
        if ((key >= 48) && (key < 58)) {
            n = key - 48;
            cout << "Tracking switched to Fish:" << n << endl;

            int tmp = cap.get(CAP_PROP_POS_FRAMES);
            cap.set(CAP_PROP_POS_FRAMES, tmp - 1);
            cap >> src;

            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);
            school[n].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
            imshow(filename, out_frame);
        }

        // Delete path for current animal
        if (key == 100) //d
        {
            cout << "Path for Fish " << n << " deleted" << endl;
            school[n].points.clear();

            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);
            imshow(filename, out_frame);
        }

        if (key == 115) //s
        {
            showAllPaths_Flag = !showAllPaths_Flag;

            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);

            if (showAllPaths_Flag)
            {
                for (int i = 0; i < 10; i++)
                {
                    school[i].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
                }
                cout << "Showing all paths" << endl;
            }
            else
            {
                school[n].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
                cout << "Showing one path" << endl;
            }
            imshow(filename, out_frame);
        }

        if (key == 119) //w
        {
            //Write to file
            outFile.open(out_filename);

            // Check if the file is opened successfully
            if (!outFile) {
                cout << "Error: Couldn't open the file!" << endl;
            }
            outFile << "Body Length (px): " << defaultbodylength << endl;
            writeSchoolData(school, outFile, defaultbodylength);

            outFile.close();
        }

        if (key == 98) //b
        {
            // Adjust body length for current fish
            cout << "Draw line to measure BL for Fish " << n << endl;
            cout << "Press enter to confirm" << endl;

            bool bl_selected = false;
            while (!bl_selected)
            {
                if (drawing) {
                    src = out_frame.clone();
                    line(src, startpoint, endpoint, GREEN, 2);
                    cv::imshow(filename, src);
                }

                if (mouseClickRelease_Flag)
                {
                    school[n].bodylength = calculateBL(startpoint, endpoint);
                    cout << "Body Length (px) for Fish " << n << ": " << school[n].bodylength << endl;
                    mouseClickRelease_Flag = false;
                }

                char bl_key = waitKey(10);
                if (bl_key == 13) {
                    cout << "BL selected for Fish " << n << "!" << endl;
                    mouseClick_Flag = false;
                    updateVideoData(cap, out_frame, n, school[n].path_colour);
                    imshow(filename, out_frame);
                    bl_selected = true;
                }
                if (bl_key == 27) {
                    cout << "BL selection cancelled for Fish " << n << "." << endl;
                    bl_selected = true;
                }
            }
        }

        if (key == 8) //Backspace
        {
            cout << "Returning to start of clip." << endl;
            cap.set(CAP_PROP_POS_FRAMES, 0);
            cap >> src;
            src.copyTo(out_frame);
            updateVideoData(cap, out_frame, n, school[n].path_colour);
            if (showAllPaths_Flag)
            {
                for (int i = 0; i < 10; i++)
                {
                    school[i].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
                }
            }
            else
            {
                school[n].drawPath(out_frame, cap.get(CAP_PROP_POS_FRAMES));
            }
            imshow(filename, out_frame);
        }

        if (key == 104) //h help commands
        {
            cout << "----------- Commands ------------" << endl;
            cout << "'>' - move forward one frame" << endl;
            cout << "'<' - move backward one frame" << endl;
            cout << "'0-9' - switch between fish to track" << endl;
            cout << "'s' - toggle showing all fish paths" << endl;
            cout << "'d' - delete all points on current path" << endl;
            cout << "'w' - write data to file" << endl;
            cout << "'b' - adjust body length for current fish" << endl;
            cout << "'Backspace' - return to first frame" << endl;
            cout << "---------------------------------" << endl;
        }
    }

    // Release the video capture object and close the window
    cap.release();
    destroyAllWindows();

    return 0;
}


