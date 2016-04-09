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
    id: root

    property int margin: 5
    //fully working example video arg string for combing mov files in expected format:
    //-f concat -i <source> -pix_fmt yuv420p -vcodec libx264 -acodec aac -strict -2 -y -filter_complex "pan=stereo|c0=FL|c1=FR, aformat=channel_layouts=stereo" <output>.mp4
    //to lower file size adjust -crf (use 18-24, the lower the number the bigger the file size)
    //to make conversion faster, you could add -preset fast (or -preset ultrafast)
    //TODO check if -filter_complex stuff is actually needed on real files
    readonly property string defaultVideoArg: "-f concat -i <source> -pix_fmt yuv420p -vcodec libx264 -acodec aac -strict -2 -y -filter_complex \"pan=stereo|c0=FL|c1=FR, aformat=channel_layouts=stereo\" <output>.mp4"
    readonly property string defaultPodcastArg: "-i <input>.mp4 -b:a 192K -vn -y <output>.mp3"
    readonly property string defaultSourceName: "Capture"
    readonly property string defaultOutputName: "Service"

    property string videoArgumentString;
    property string podcastArgumentString;

    property bool isConverting: false;

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
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
                enabled: !isConverting
                Rectangle {
                    anchors.fill: parent
                    border.color: "red"
                    color: "transparent"
                    visible: sourcePath.text==""
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
                enabled: !isConverting
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
                enabled: !isConverting
                Rectangle {
                    anchors.fill: parent
                    border.color: "red"
                    color: "transparent"
                    visible: location1Path.text==""
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
                enabled: !isConverting
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
                enabled: !isConverting
            }
            Button {
                text: "Browse"
                onClicked: {
                    if(FileHelper.fileExists(location2Path.text))
                        fileDialog.folder = "file:///" + location2Path.text
                    fileDialog.textFieldToEdit = location2Path
                    fileDialog.open()
                }
                enabled: !isConverting
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
            text: "Please select the locations above, then click Convert below!"
            onLineCountChanged: {
                if(lineCount>5000)
                    remove(0, length/lineCount * 2);
            }
        }

        RowLayout {
            id: buttonBar
            spacing: margin
            anchors.right: parent.right
            layoutDirection: Qt.RightToLeft
            Button {
                id: closeButton
                text: "Close"
                enabled: !isConverting
                activeFocusOnPress: true;
                onClicked: Qt.quit();
            }
            Button {
                id: convertButton
                text: isConverting ? "Cancel" : "Convert"
                enabled: sourcePath.text!="" && location1Path.text!=""
                activeFocusOnPress: true;
                onClicked: {
                    if(isConverting)
                    {
                        isConverting=false;
                        FileHelper.cancel();
                    }
                    else
                    {
                        isConverting = true;
                        ffmpegOutput.text = "Starting conversion..."
                        FileHelper.start();
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        FileHelper.readyRead.connect(readyRead);
        FileHelper.copyProgress.connect(copyProgress);
        FileHelper.copyError.connect(copyError);
        FileHelper.encodingFinished.connect(finished);
    }

    Component.onDestruction: {
        FileHelper.readyRead.disconnect(readyRead);
        FileHelper.copyProgress.disconnect(copyProgress);
        FileHelper.copyError.disconnect(copyError);
        FileHelper.encodingFinished.disconnect(finished);
    }

    function readyRead() {
        ffmpegOutput.append(FileHelper.getOutput());
    }

    function copyProgress(progress) {
        ffmpegOutput.append("Copying file at " + progress + "%");
    }

    function copyError(error) {
        console.log("Copy Error: " + error);
        errorDialog.text = error;
        errorDialog.open();
    }

    function finished() {
        isConverting = false;
        ffmpegOutput.append("---------------------------Done---------------------------")
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
//        standardButtons: StandardButton.Cancel | StandardButton.Save | StandardButton.RestoreDefaults

        contentItem: Rectangle {
            id: contentHolder
            SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }
//            color: "lightgray"
            color: myPalette.window
            property bool resetTextFields
            implicitHeight: settingsDialog.height
            implicitWidth: settingsDialog.width
            anchors {
                centerIn: parent
            }
            ColumnLayout {
                spacing: margin
                anchors.centerIn: parent
                width: parent.width-margin*2
                height: parent.height-margin*2

                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    Text {
                        text: "Video command: "
                    }

                    TextField {
                        id: videoArg
                        Layout.fillWidth: true
                        Rectangle {
                            anchors.fill: parent
                            border.color: "red"
                            color: "transparent"
                            visible: videoArg.text==""
                        }
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
                        Rectangle {
                            anchors.fill: parent
                            border.color: "red"
                            color: "transparent"
                            visible: podcastArg.text==""
                        }
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
                        Rectangle {
                            anchors.fill: parent
                            border.color: "red"
                            color: "transparent"
                            visible: sourceName.text==""
                        }
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
                        Rectangle {
                            anchors.fill: parent
                            border.color: "red"
                            color: "transparent"
                            visible: outputName.text==""
                        }
                    }
                }
                Rectangle {
                    id: filler
                    Layout.fillHeight: true
                    Layout.preferredWidth: 0
                }
                //replaced standard buttons with custom buttons so that Save can be disabled if invalid settings
                RowLayout {
                    spacing: margin
                    Layout.fillWidth: true
                    layoutDirection: Qt.RightToLeft
                    Button {
                        text: "Save"
                        enabled: videoArg.text!="" && podcastArg.text!="" && sourceName.text!="" && outputName.text!=""
                        onClicked: {
                            settingsDialog.accepted()
                            settingsDialog.close()
                        }
                    }
                    Button {
                        text: "Cancel"
                        onClicked: settingsDialog.rejected()
                    }
                    Rectangle {
                        id: spacer
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Restore Defaults"
                        onClicked: settingsDialog.reset()
                    }
                }
            }
        }

        Component.onCompleted: {
//            settingsDialog.saveButton.enabled = false
//            console.log(JSON.stringify(settingsDialog))
        }

        function showValues() {
            videoArg.text = SettingsHelper.value("videoArg", defaultVideoArg);
            podcastArg.text = SettingsHelper.value("podcastArg", defaultPodcastArg);
            sourceName.text = SettingsHelper.value("sourceName", defaultSourceName);
            outputName.text = SettingsHelper.value("outputName", defaultOutputName);
        }

        function saveValues()
        {
            if(videoArg.text==="" || podcastArg.text==="" || sourceName.text==="" || outputName.text==="")
                return false;
            SettingsHelper.setValue("videoArg", videoArg.text)
            SettingsHelper.setValue("podcastArg", podcastArg.text)
            SettingsHelper.setValue("sourceName", sourceName.text)
            SettingsHelper.setValue("outputName", outputName.text)
            return true;
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
