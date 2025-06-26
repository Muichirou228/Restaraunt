import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: ordersTable
    anchors.fill: parent
    anchors.margins: 15
    color: "transparent"

    property var ordersModel: []
    property var tempItems: [] // Для хранения временных элементов

    // Стиль текста для всей таблицы
    property alias fontFamily: defaultText.font.family
    property alias fontSize: defaultText.font.pixelSize

    Connections {
        target: dbHandler
        onProductsLoaded: {
            dishInput.model = dbHandler.productList();
        }
        // onErrorOccurred: {
        //     console.error("Error:", error)
        // }
    }

    function getTempItemsData() {
        var items = [];
        for (var i = 0; i < tempItems.length; i++) {
            items.push({
                name: tempItems[i].name,
                quantity: tempItems[i].quantity
            });
            console.log("Temp items name and count = ", tempItems[i].name, tempItems[i].quantity)
        }
        return items;
    }

    Component.onCompleted: {
        dbHandler.loadProducts();
    }

    Text {
        id: defaultText
        visible: false
        font.family: "Roboto"
        font.pixelSize: 16
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Заголовок таблицы
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#333333"
            radius: 5

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 0

                Text {
                    text: "Блюдо"
                    color: "white"
                    font.bold: true
                    font.pixelSize: defaultText.font.pixelSize + 2
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }

                Text {
                    text: "Кол-во"
                    color: "white"
                    font.bold: true
                    font.pixelSize: defaultText.font.pixelSize + 2
                    Layout.preferredWidth: parent.width * 0.2
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: "Цена"
                    color: "white"
                    font.bold: true
                    font.pixelSize: defaultText.font.pixelSize + 2
                    Layout.preferredWidth: parent.width * 0.3
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        // Тело таблицы
        ListView {
            id: ordersListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ordersModel.concat(tempItems) // Объединяем основные и временные элементы
            spacing: 1

            delegate: Rectangle {
                id: rowDelegate
                width: ordersListView.width
                height: 50
                color: index % 2 === 0 ? "#1e1e1e" : "#252525"
                radius: 3

                property int itemIndex: index
                property bool isTempItem: modelData.isTemp || false

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    spacing: 0

                    Text {
                        text: modelData.name
                        color: "white"
                        font.pixelSize: defaultText.font.pixelSize + 2
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: modelData.quantity || 1
                        color: "white"
                        font.pixelSize: defaultText.font.pixelSize + 2
                        Layout.preferredWidth: parent.width * 0.2
                        horizontalAlignment: Text.AlignHCenter
                    }

                    // Для временных элементов показываем кнопку удаления
                    Item {
                        visible: rowDelegate.isTempItem
                        Layout.preferredWidth: parent.width * 0.3
                        Layout.fillHeight: true

                        Button {
                            id: deleteButton
                            anchors.right: parent.right
                            width: 30
                            height: 30
                            anchors.verticalCenter: parent.verticalCenter
                            text: "×"
                            font.bold: true
                            font.pixelSize: defaultText.font.pixelSize + 4

                            background: Rectangle {
                                color: deleteButton.hovered ? "#444444" : "transparent"
                                radius: 3
                            }

                            contentItem: Text {
                                text: parent.text
                                color: "#ff5555"
                                font: parent.font
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            onClicked: {
                                // Находим индекс элемента в tempItems
                                var tempIndex = rowDelegate.itemIndex - ordersModel.length;
                                if (tempIndex >= 0 && tempIndex < tempItems.length) {
                                    // Создаем новый массив без удаляемого элемента
                                    var newTempItems = tempItems.slice();
                                    newTempItems.splice(tempIndex, 1);
                                    tempItems = newTempItems;
                                }
                            }
                        }
                    }

                    // Для обычных элементов показываем цену
                    Text {
                        visible: !rowDelegate.isTempItem
                        text: modelData.price + " ₽"
                        color: "white"
                        font.pixelSize: defaultText.font.pixelSize + 2
                        Layout.preferredWidth: parent.width * 0.3
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        }

        // Поле ввода и кнопка добавления
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                spacing: 10

                ComboBox {
                    id: dishInput
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    font.pixelSize: defaultText.font.pixelSize
                    model: []
                    contentItem: Text {
                        text: dishInput.displayText
                        font: dishInput.font
                        color: "white"
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: "#333333"
                        radius: 5
                        border.color: "#555555"
                        border.width: 1
                    }
                }

                Button {
                    id: addButton
                    Layout.preferredWidth: height
                    Layout.fillHeight: true
                    text: "+"
                    font.bold: true
                    font.pixelSize: defaultText.font.pixelSize + 4
                    background: Rectangle {
                        color: "#4CAF50"
                        radius: 5
                    }

                    onClicked: {
                        if (dishInput.currentText) {
                            var dishName = dishInput.currentText;
                            var existingItemIndex = -1;

                            // Ищем такой же элемент в tempItems
                            for (var i = 0; i < tempItems.length; i++) {
                                if (tempItems[i].name === dishName) {
                                    existingItemIndex = i;
                                    break;
                                }
                            }

                            if (existingItemIndex >= 0) {
                                // Если нашли - увеличиваем количество
                                var updatedItems = tempItems.slice();
                                updatedItems[existingItemIndex].quantity += 1;
                                tempItems = updatedItems;
                            } else {
                                // Если не нашли - добавляем новый
                                var newItem = {
                                    name: dishName,
                                    quantity: 1,
                                    isTemp: true,
                                    id: Date.now() + Math.random()
                                };
                                tempItems = tempItems.concat([newItem]);
                            }
                        }
                    }
                }
            }
        }

        // Итоговая сумма (только для основных элементов)
        Rectangle {
            id: tot
            Layout.fillWidth: true
            height: 60
            color: "#333333"
            radius: 5

            property real totalSum: {
                var sum = 0;
                for (var i = 0; i < ordersModel.length; i++) {
                    sum += ordersModel[i].price * ordersModel[i].quantity;
                }
                return sum;
            }

            Text {
                text: "Итого: " + parent.totalSum + " ₽"
                color: "white"
                font.bold: true
                font.pixelSize: defaultText.font.pixelSize + 4
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    function updateOrders(newOrders) {
        ordersModel = newOrders;
    }
}
