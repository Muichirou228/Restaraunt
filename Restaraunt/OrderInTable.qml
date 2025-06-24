import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: ordersTable
    anchors.fill: parent
    anchors.margins: 15
    color: "transparent"

    property var ordersModel: []

    // Стиль текста для всей таблицы
    property alias fontFamily: defaultText.font.family
    property alias fontSize: defaultText.font.pixelSize

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
            model: ordersModel
            spacing: 1

            delegate: Rectangle {
                width: ordersListView.width
                height: 50
                color: index % 2 === 0 ? "#1e1e1e" : "#252525"
                radius: 3

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
                        text: modelData.quantity
                        color: "white"
                        font.pixelSize: defaultText.font.pixelSize + 2
                        Layout.preferredWidth: parent.width * 0.2
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Text {
                        text: modelData.price + " ₽"
                        color: "white"
                        font.pixelSize: defaultText.font.pixelSize + 2
                        Layout.preferredWidth: parent.width * 0.3
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        }

        // Итоговая сумма
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
