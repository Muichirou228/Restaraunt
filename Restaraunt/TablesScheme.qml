import QtQuick
import QtQuick.Layouts

Item {
    id: root
    width: parent.width
    height: parent.height - parent.height * 0.1
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    property int tableWarning: 0
    signal tableClicked(int tableWarning)
    GridLayout {
        id: grid
        anchors.centerIn: parent
        rowSpacing: 30
        columnSpacing: 30
        rows: 4
        columns: 5
        Repeater {
            model: 20
            delegate: Rectangle {
                width: 70
                property bool selected: false
                radius: 10
                height: 70
                color: "white"
                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onEntered: {
                        if (!selected) {
                            parent.color = "gray"
                            tableText.color = "white"
                        }
                    }
                    onExited: {
                        if (!selected) {
                            parent.color = "white"
                            tableText.color = "black"
                        }
                    }
                    onClicked: {
                        if (!selected) {
                            if (tableWarning >= 0 && tableWarning < 2) {
                                parent.color = "#006400"
                                tableText.color = "white"
                                selected = true
                                tableWarning++
                                root.tableClicked(tableWarning)
                            }
                        } else if (tableWarning <= 2 && tableWarning > 0) {
                            parent.color = "gray"
                            tableText.color = "white"
                            selected = false
                            tableWarning--
                            root.tableClicked(tableWarning)
                        }
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
