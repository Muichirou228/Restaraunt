import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Window {
    id: waiterWindow
    width: 1000
    height: 1000
    minimumWidth: 1000
    minimumHeight: 800
    visible: true
    color: "black"
    property string waiterName : ""
    Component.onCompleted: {
        //tables.updateTablesStatus();
    }

    Text {
        color: "white"
        anchors {
            left: parent.left
            top: parent.top
            margins: 30
        }
        text: waiterName
        font.bold: true
        font.pixelSize: 40
        font.family: "Verdana"
    }

    TablesSchemeForWaiters{
        id: tables
    }
}
