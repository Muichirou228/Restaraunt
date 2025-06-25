import QtQuick 2.15

Item {
    signal buttonClicked;
        id: backButton
        width: 80
        height: 80
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
            rightMargin: 120
        }
        Rectangle {
            color: "gray"
            id: backrect
            anchors.fill: parent
            radius: 5
            Image {
                source: "../save.png"
                width: parent.width * 0.5
                height: parent.height * 0.5
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    buttonClicked();
                }
                onEntered: {
                    backrect.color = "white";
                }
                onExited: {
                    backrect.color = "gray";
                }
            }
        }
    }
