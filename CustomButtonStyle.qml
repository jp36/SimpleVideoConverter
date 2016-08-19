import QtQuick 2.4
import QtQuick.Controls.Styles 1.4

ButtonStyle {
    label: StyledText{
        text: control.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    padding {
        right: 20
        left: 20
    }
}
