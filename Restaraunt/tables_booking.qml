import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Window {
    id: bookWindow
    width: 1000
    height: 1000
    minimumWidth: 1000
    minimumHeight: 800
    visible: true
    color: "black"
    property int tableWarning: 0
    Component.onCompleted: {
        tableWarning = 0
    }

    Text {
        id: bookText
        width: parent.width
        text: "Бронирование столов (не больше 2 столов на 1 день)"
        wrapMode: Text.WrapAnywhere
        font.family: "Verdana"
        font.bold: true
        font.pixelSize: 40
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40
    }

    TablesScheme {
        onTableClicked: function (tableWarning) {
            bookWindow.tableWarning = tableWarning
            if (bookWindow.tableWarning > 0) confirmrect.visible = true; else confirmrect.visible = false;
        }
    }

    Item {
            id: confirm
            width: 125
            height: 125
            anchors.horizontalCenter: parent.horizontalCenter
            anchors {
                bottom: parent.bottom
                bottomMargin: 30
            }
            Rectangle {
                color: "white"
                id: confirmrect
                anchors.fill: parent
                visible: false
                radius: 5
                Image {
                    source: "../tick.png"
                    width: parent.width * 0.5
                    height: parent.height * 0.5
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {

                    }
                    onEntered: {
                        confirmrect.color = "gray";
                    }
                    onExited: {
                        confirmrect.color = "white";
                    }
                }
            }
        }
}
