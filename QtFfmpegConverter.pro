TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    settingshelper.cpp \
    filehelper.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    settingshelper.h \
    filehelper.h

win32:{

    file_pathes += "\"$$PWD/libs/ffmpeg-win32/bin/ffmpeg.exe\""

    CONFIG(release, debug|release):{
        destination_pathes += $$OUT_PWD/release/
#        destination_pathes += Path/To/Deploy/Directory
    }
    else:CONFIG(debug, debug|release):{
        destination_pathes += $$OUT_PWD/debug/
    }

    for(file_path,file_pathes){
        file_path ~= s,/,\\,g
        for(dest_path,destination_pathes){
            dest_path ~= s,/,\\,g
            QMAKE_POST_LINK += $$quote(xcopy $${file_path} $${dest_path} /I /Y $$escape_expand(\n\t))
         }
    }
}
