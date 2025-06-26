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

    StackView {
        id: stackViewForWaiters
        anchors.fill: parent
        initialItem: waiterWindowComp
    }
    Component {
        id: waiterWindowComp
        Item {
            Text {
                id: waiterText
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
                onTableClicked: (tableNum) => {
                    stackViewForWaiters.push("../TableInProcess.qml", {table_num : tableNum , waiter_name : waiterName}
                    )
                }
            }
        }
    }




}
