# ras_speedo

## Video conversion
1. Edit trim.bat to change the input and output folder (right click -> edit). e.g:
```
@echo off
setlocal enabledelayedexpansion

:: Path to the folder containing videos to trim
set input_folder="C:\Users\jrap017\Videos\fish_vids"

:: Path to the folder where trimmed videos will be saved
set output_folder="C:\Users\jrap017\Videos\fish_vids_trimmed"

:: Ensure output folder exists
mkdir "%output_folder%" 2>nul

REM Loop through all video files in the input folder
for %%F in ("%input_folder%\*.mp4") do (
    REM Get the filename without extension
    set "filename=%%~nF"
    
    REM Trim the video (e.g., start from 0 seconds, keep 30 seconds)
    ffmpeg -i "%%F" -ss 00:00:00 -t 00:00:30 -vf scale=1280:720 -r 10 "%output_folder%\!filename!_trimmed.mp4"
)
```
2. Run trim.bat. This will go through the input folder and reduce the length, resolution and frame rate of the videos. It will then copy to the ouput folder.
3. There will now be a folder of smaller videos ready for analysis.

 ## Video Analysis
 1. Run ras-speedo.exe.
 2. Enter file path of video to analyse. You can copy filepath by holding the shift key, right clicking the file and selecting copy as path. (Make sure to delete the quotation marks).
 3. Draw a line on the frame to measure nominal fish body length and press enter. This will be used for speed calculation.
 4. Select 10 fish at random and track their swimming path through the video. Use a left mouse click to mark a point and the '>' '<' keys to cycle through the frames. Other commmands can be found below or by pressing the 'h' key.
```
----------- Commands ------------
'>' - move forward one frame
'<' - move backward one frame
'0-9' - switch between fish to track
's' - toggle showing all fish paths
'd' - delete all points on current path
'w' - write data to file
'Backspace' - return to first frame
---------------------------------
```
4. Once 10 fish have been tracked press the 'w' key to write data to file and display on screen.
