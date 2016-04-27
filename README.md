# SimpleVideoConverter
A program meant to do the video conversion we need without much user ability/action. Has settings to allow flexibility without reinstall.

##Other Info

####Qt
- This application was built using Qt, which means this code requires Qt to build. Once deployed as a standalone app though, the application can be run without installing Qt.
http://www.qt.io/download/

####FFMPEG
- This application makes use of FFMPEG for file conversion - https://www.ffmpeg.org/
- A prebuilt version is included in the repo that came from - https://ffmpeg.zeranoe.com/builds/

#Deployment
##Windows
- copy exe file from built directory (Must be release version) to c:\converterDeploy\{version_folder}
- then run below:
```Batchfile
//IMPORTANT - change 5.4 below to your current version Qt
SET PATH="%path%;C:\Qt\5.4\msvc2013\bin
qtenv2.bat
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
cd c:\converterDeploy\{version_folder}
windeployqt --qmldir C:\Users\{username}\Documents\QT\{qmldirHere} {EXEName}.exe
```
