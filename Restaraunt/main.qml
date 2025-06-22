import QtQuick
import QtQuick.Controls
Window {
    id: mainWindow
    minimumWidth: 640
    minimumHeight: 440
    maximumWidth: 640
    maximumHeight: 440
    visible: true
    Connections {
        target: dbHandler
        onCodeChecked: (name) => {
            if (name !== "") {
                mainWindow.hide();
                var component = Qt.createComponent("../waiter_main.qml")
                var waiterWindow = component.createObject(null, {waiterName : name})
                waiterWindow.closing.connect(function() {
                mainWindow.show();
                })
                waiterWindow.show();
            }
        }
    }

    Image {
        id: birdie
        source: "../birdie.png"
        width: 100
        height: 100
        anchors {
            top: parent.top
            left: parent.left
        }
    }


    Text {
        id: signinText
        text: "Авторизация"
        font.family: "Verdana"
        font.bold: true
        font.pixelSize: 40
        color: "blue"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 80
    }

    Text {
        id: codeText
        text: "Код персонала"
        font.family: "Verdana"
        font.pixelSize: 30
        font.bold: true
        color: "black"
        anchors.left: parent.left
        anchors.top: signinText.bottom
        anchors.topMargin: 60
        anchors.leftMargin: 60
    }

    TextField {
        id: codeInput
        anchors.left: codeText.right
        anchors.top: codeText.top
        anchors.leftMargin: 30
        color: "black"
        echoMode: TextInput.Password
        font.family: "Verdana"
        font.pixelSize: 30
        width: 200
        maximumLength: 6
        placeholderText: "Введите код"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Item {
        width: 50
        height: 50
        anchors {
            left: codeInput.right
            top: codeInput.top
            leftMargin: 15
            bottomMargin: 10
        }
        Image {
            anchors.fill: parent
            source: "../enter.png"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (codeInput.text === "" || codeInput.text.length < 6) {
                    codeInput.text = "";
                    codeInput.focus = false;
                    codeInput.placeholderText = "6 цифр!!!"
                } else {
                    dbHandler.checkIfUserCodeExists(codeInput.text);
                }
            }
        }
    }

    Rectangle {
        id: line
        color : "black"
        width: parent.width
        height: 3
        anchors {
            top: codeInput.bottom
            left: parent.left
            topMargin: 50
        }
    }

    Text {
        id: bookText
        text: "Бронь столов    -> "
        font.family: "Verdana"
        font.pixelSize: 40
        font.bold: true
        color: "black"
        anchors.left: parent.left
        anchors.top: line.bottom
        anchors.topMargin: 50
        anchors.leftMargin: 30
    }

    Image {
        source: "../booking.png"
        anchors {
            top: line.bottom
            left: bookText.right
            leftMargin: 30
            topMargin: 20
        }
        width: 120
        height: 120
        MouseArea{
            cursorShape: Qt.PointingHandCursor
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                parent.source = "../booking_changed.png"
            }
            onExited: {
                parent.source = "../booking.png"
            }
            onClicked: {
                mainWindow.hide();
                var component = Qt.createComponent("../tables_booking.qml")
                var tablesWindow = component.createObject(null)
                tablesWindow.closing.connect(function() {
                mainWindow.show();
                })
                tablesWindow.show();
            }
        }
    }
}
