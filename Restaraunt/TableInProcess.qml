import QtQuick 2.15

Rectangle {
    color: "black"
    Component.onCompleted: {
        dbHandler.getTableOrders(table_num);
    }
    property string waiter_name: ""
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
                console.log(tableInfo.tempItems.length.toString());
                if (tableInfo.tempItems.length !== 0) {
                    console.log ("Waiter name = ", waiter_name);
                    var ttt = tableInfo.getTempItemsData();
                    dbHandler.checkIfOrderExists(table_num, tableInfo.getTempItemsData());
                }
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
        onOrderItemsAdded: {
            stackViewForWaiters.pop();
        }
        onOrderExistingChecked: (result, items) => {
            if (result) {
                dbHandler.addOrderItemsForQML(table_num.toString(), items);
                console.log("table number = ", table_num.toString());
            } else {
                dbHandler.findWaiterIdByName(waiter_name, table_num, items);
            }
        }

        onErrorOccurred: function(message) {
            console.error("Database error:", message);
        }
    }
}
