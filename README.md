# ras_speedo

## Video conversion
1. Edit trim.bat to change input and output folder (right click -> edit). e.g:
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
2. Run trim.bat. This will go through the input folder and reduce the length, resolution and frame rate of the videos.
3. There will now be a folder of smaller videos ready for analysis.

 ## Video Analysis
