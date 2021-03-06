TEMPLATE = app

VERSION = 1.0.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT += qml quick widgets

SOURCES += main.cpp \
    settingshelper.cpp \
    filehelper.cpp \
    asyncio.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    settingshelper.h \
    filehelper.h \
    asyncio.h

#icons
macx {
    ICON = $$PWD/mac/icon.icns
} else:win32:!winrt {
    RC_ICONS = $$PWD/windows/favicon.ico
}

#code to handle copying ffmpeg where needed
win32:{
#windows code for copying ffmpeg to the built directory
    file_pathes += "\"$$PWD/libs/ffmpeg-win32/bin/ffmpeg.exe\""

    CONFIG(release, debug|release):{
        destination_pathes += $$OUT_PWD/release/
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
macx:{
    APP_FILES.files = $$PWD/libs/ffmpeg-mac/bin/ffmpeg
    APP_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_FILES
}
