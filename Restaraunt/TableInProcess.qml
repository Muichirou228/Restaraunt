import QtQuick 2.15

Rectangle {
    color: "black"
    Component.onCompleted: {
        dbHandler.getTableOrders(table_num);
        //console.log("table num = ", table_num)
    }

    property int table_num : 0
    Text {
        id: tableText
        font.family: "Verdana"
        color: "white"
        font.pixelSize: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40
        text: "Стол " + table_num.toString()
        font.bold: true
    }

    RecipeButton {
        onButtonClicked: {
        }
    }

    SaveButton {
        onButtonClicked: {

        }
    }

    BackButton {
        onButtonClicked: {
            stackViewForWaiters.pop();
        }
    }

    OrderInTable {
        id: tableInfo
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 20
            topMargin: 150
        }
    }

    Connections {
        target: dbHandler
        onTableOrdersReady: (orderItems) => {
            tableInfo.ordersModel = orderItems;
        }
    }
}
