import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

//import org.crossway.converter 1.0

ApplicationWindow {
    title: qsTr("SimpleConverter")
    width: 640
    height: 480
    visible: true

    property int margin: 5
    readonly property string defaultVideoArg: "-i <source>.mov -pix_fmt yuv420p -vcodec libx264 -acodec libfaac <output>.mp4"
    readonly property string defaultPodcastArg: "-i <input>.mp4 -b:a 192K -vn <output>.mp3"
    readonly property string defaultSourceName: "blah"
    readonly property string defaultOutputName: "crossway"

    property string videoArgumentString;
    property string podcastArgumentString;

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
        Menu {
            title: qsTr("&Tools")
            MenuItem {
                text: qsTr("Options")
                onTriggered: settingsDialog.open();
            }
        }
    }

    ColumnLayout {
        id: mainColumn
        anchors {
            fill: parent
            margins: margin
        }
        spacing: margin

        RowLayout {
            id: source
            spacing: margin
            Layout.fillWidth: true
            Text {
                text: "Source Location: "
            }
            TextField {
                id: sourcePath
                Layout.fillWidth: true
                text: SettingsHelper.value("sourcePath", "");
                onEditingFinished: {
                    SettingsHelper.setValue("sourcePath", text);
                }
            }
            Button {
                text: "Browse"
                onClicked: {
                    if(FileHelper.fileExists(sourcePath.text))
                        fileDialog.folder = "file:///" + sourcePath.text
                    fileDialog.textFieldToEdit = sourcePath
                    fileDialog.open()
                }
            }
        }

        Rectangle {
            id: dividerLine
            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: "black"
        }

        RowLayout {
            id: location1
            spacing: margin
            Layout.fillWidth: true
            Text {
                text: "Google Drive Location: "
            }
            TextField {
                id: location1Path
                Layout.fillWidth: true
                text: SettingsHelper.value("location1Path", "");
                onEditingFinished: {
                    SettingsHelper.setValue("location1Path", text);
                }
            }
            Button {
                text: "Browse"
                onClicked: {
                    if(FileHelper.fileExists(location1Path.text))
                        fileDialog.folder = "file:///" + location1Path.text
                    fileDialog.textFieldToEdit = location1Path
                    fileDialog.open()
                }
            }
        }

        RowLayout {
            id: location2
            spacing: margin
            Layout.fillWidth: true
            Text {
                text: "Shared Folder Location: "
            }
            TextField {
                id: location2Path
                Layout.fillWidth: true
                text: SettingsHelper.value("location2Path", "");
                onEditingFinished: {
                    SettingsHelper.setValue("location2Path", text);
                }
            }
            Button {
                text: "Browse"
                onClicked: {
                    if(FileHelper.fileExists(location2Path.text))
                        fileDialog.folder = "file:///" + location2Path.text
                    fileDialog.textFieldToEdit = location2Path
                    fileDialog.open()
                }
            }
        }

        Rectangle {
            id: dividerLine2
            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: "black"
        }

        TextArea {
            id: ffmpegOutput
            Layout.fillHeight: true
            Layout.fillWidth: true
            readOnly: true
            text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, "
        }

        RowLayout {
            id: buttonBar
            spacing: margin
            anchors.right: parent.right
            layoutDirection: Qt.RightToLeft
            Button {
                id: closeButton
                text: "Close"
                onClicked: Qt.quit();
            }
            Button {
                id: convertButton
                text: "Convert"
                onClicked: {
                    if(SettingsHelper.value("sourcePath", "")==="")
                    {
                        errorDialog.text = "Source Path not set";
                        errorDialog.open();
                    } else if(SettingsHelper.value("location1Path", "")==="")
                    {
                        errorDialog.text = "Google Drive Location not set";
                        errorDialog.open();
                    }
//                    videoArgumentString =
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please select a location.")
        selectFolder: true

        property var textFieldToEdit

        onAccepted: {
            var path = fileUrl.toString();
            // remove prefixed "file:///"
            path= path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            console.log("You chose: " + cleanPath)
            if(textFieldToEdit)
            {
                textFieldToEdit.text = cleanPath
                textFieldToEdit.editingFinished();
            }
        }
        onRejected: {
            fileDialog.close();
        }
    }

    MessageDialog {
        id: errorDialog
        title: "Error"
        icon: StandardIcon.Critical
        standardButtons: StandardButton.Ok
        onAccepted: {
            errorDialog.close()
        }
    }

    MessageDialog {
        id: confirmationDialog
        title: "Reset"
        icon: StandardIcon.Question
        text: "Are you sure you want to reset all settings to default?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            settingsDialog.setDefaultValues()
            settingsDialog.close()
        }
        onNo: {
            confirmationDialog.close();
        }
    }

    Dialog {
        id: settingsDialog
        title: "Settings"
        width: 600
        height: 200
        standardButtons: StandardButton.Cancel | StandardButton.Save | StandardButton.RestoreDefaults

        Rectangle {
            id: contentHolder
            color: "lightgray"
            property bool resetTextFields
            anchors {
                margins: margin
                fill: parent
            }
            ColumnLayout {
                spacing: margin
                width: parent.width
                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    Text {
                        text: "Video command: "
                    }

                    TextField {
                        id: videoArg
                        Layout.fillWidth: true
                    }
                }
                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    Text {
                        text: "Podcast command: "
                    }

                    TextField {
                        id: podcastArg
                        Layout.fillWidth: true
                    }
                }
                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    Text {
                        text: "Source File Name: "
                    }

                    TextField {
                        id: sourceName
                        Layout.fillWidth: true
                    }
                }
                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    Text {
                        text: "Output File Name: "
                    }

                    TextField {
                        id: outputName
                        Layout.fillWidth: true
                    }
                }
            }
        }
        function showValues() {
            videoArg.text = SettingsHelper.value("videoArg", defaultVideoArg);
            podcastArg.text = SettingsHelper.value("videoArg", defaultVideoArg);
            sourceName.text = SettingsHelper.value("sourceName", defaultSourceName);
            outputName.text = SettingsHelper.value("outputName", defaultOutputName);
        }

        function saveValues()
        {
            SettingsHelper.setValue("videoArg", videoArg.text)
            SettingsHelper.setValue("podcastArg", podcastArg.text)
            SettingsHelper.setValue("sourceName", sourceName.text)
            SettingsHelper.setValue("outputName", outputName.text)
        }
        function setDefaultValues() {
            videoArg.text = defaultVideoArg;
            podcastArg.text = defaultPodcastArg;
            sourceName.text = defaultSourceName;
            outputName.text = defaultOutputName;
            SettingsHelper.setValue("videoArg", defaultVideoArg)
            SettingsHelper.setValue("podcastArg", defaultPodcastArg)
            SettingsHelper.setValue("sourceName", defaultSourceName)
            SettingsHelper.setValue("outputName", defaultOutputName)
        }

        onVisibleChanged: {
            if(visible)
                showValues()
        }

        onAccepted: {
            saveValues();
        }

        onReset: {
            confirmationDialog.open()
        }

        onRejected: {
            contentHolder.resetTextFields = !contentHolder.resetTextFields
            this.close()
        }
    }
}
