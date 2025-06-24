import QtQuick
import QtQuick.Layouts

Item {
    id: root
    width: parent.width
    height: parent.height - parent.height * 0.1
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    signal tableClicked(int tableNum)
    property var tableStatus: ({})
    function updateTableStatus(table_num, status) {
        tableStatus[table_num] = status;
        var item = repeater.itemAt(parseInt(table_num) - 1);
            if (item) {
                item.color = getTableColor(status);
            }
    }
    function getTableColor(status) {
            switch(status) {
                case "occupied": return "yellow";
                //case "reserved": return "orange";
                //case "dirty": return "brown";
                default: return "white";
            }
        }
    Connections {
            target: dbHandler
            onTableStatusChecked: function(tableNum, status) {
                updateTableStatus(tableNum, status);
            }
        }

    Component.onCompleted: {
        for (var i = 1; i <= 20; i++) {
            dbHandler.checkTableStatus(i.toString());
        }
    }

    GridLayout {
        id: grid
        anchors.centerIn: parent
        rowSpacing: 30
        columnSpacing: 30
        rows: 4
        columns: 5
        Repeater {
            id: repeater
            model: 20
            delegate: Rectangle {
                width: 70
                property bool selected: false
                radius: 10
                height: 70
                color: "white"
                enabled: true
                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        tableClicked(index + 1);
                    }
                }
                Text {
                    id: tableText
                    anchors.centerIn: parent
                    color: "black"
                    font.family: "Verdana"
                    font.bold: true
                    font.pixelSize: 30
                    text: index + 1
                }
            }
        }
    }
}
