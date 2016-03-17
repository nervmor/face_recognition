#what's the project
A project for face recogniton writen by c++ and base on OpenCV,boost,flandmark..etc.

It can detects face in picture and recogniton the it in the picture.as a result, return the label of the face in the trained model.

#what is the platform of it
It's desgined running on both windows and linux.

It can only runs on windows at present.

We will support it on linux when it's stable on windows.

#how to build
Before build, you will unzip code/third/opencv-3.1.0/build/win/debug/lib/opencv_world310d.7z to opencv_world310d.lib(github can't upload filesize>100M)

Build:

for windows:Just build code/platform/win/face_recognition.sln by msvc2013
