@echo off
echo ------------------------------------------------  preprare for face_recognition ----------------------------------------------------
cd/d %~dp0 
if not exist ..\third\boost_1_58_0\boost\ "..\tools\HaoZip\HaoZipC.exe" x ..\third\boost_1_58_0\boost_1_58_0.7z -o..\third\boost_1_58_0\ -y
if not exist ..\third\opencv-3.1.0\build\win\ "..\tools\HaoZip\HaoZipC.exe" x ..\third\opencv-3.1.0\build\win.7z -o..\third\opencv-3.1.0\build\ -y

echo ------------------------------------------------  preprare for face_recognition completed ------------------------------------------