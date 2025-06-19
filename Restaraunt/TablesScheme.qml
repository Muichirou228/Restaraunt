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
    property bool isLoading
    property var bookedTables : []
    function updateBookedTables(tables) {
            bookedTables = tables;
            isLoading = false;
            applyTableColors();
    }

    function getBookedTables() {
        var result = [];
        for (var i = 0; i < repeater.count; i++) {
            var item = repeater.itemAt(i);
            if (item.selected) {
                result.push(item.children[1].text);
            }
        }
        return result.join(" "); // Возвращаем строку с номерами через пробел
    }

    function applyTableColors() {
        for (var i = 0; i < repeater.count; i++) {
            var item = repeater.itemAt(i);
            if (item) {
                if (isLoading) {
                    item.color = "white";
                    item.children[0].hoverEnabled = false
                    item.children[1].color = "black";
                    item.enabled = false;
                }
                else if (bookedTables.includes(i+1)) {
                    item.color = "red";
                    item.enabled = false;
                    item.children[1].color = "white";
                    item.children[0].hoverEnabled = false;
                } else {
                    item.color = item.selected ? "#006400" : "white";
                    item.enabled = true;
                    item.children[1].color = item.selected ? "white" : "black";
                    item.children[0].hoverEnabled = true;
                }
                item.selected = false;
            }
        }
    }

    function resetTables() {
        tableWarning = 0;
        bookedTables = [];
        isLoading = true;
        applyTableColors();
    }

    signal tableClicked(int tableWarning)
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

    Connections {
            target: dbHandler
            onBookingsReady: {
                updateBookedTables(bookedTables);
            }
        }
}
