import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls
    Rectangle {
        property string bookingDate: ""
        property string tablesBook: ""
        property string id_client_for_booking: ""
        property bool bookingStatus: true
        color: "black"
        BackButton {
            onButtonClicked: {
                stackViewForBookings.pop();
            }
        }
        Text {
            id: formText
            text: "Заполните форму"
            color: "white"
            font.family: "Verdana"
            font.pixelSize: 40
            font.bold: true
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 60
            }
        }
        RowLayout {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: formText.bottom
                topMargin: 150
            }
            ColumnLayout {
                anchors {
                    rightMargin: 100
                }
                spacing: 50
                Text {
                    text: "Фамилия"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
                Text {
                    text: "Имя"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
                Text {
                    text: "Отчество"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
                Text {
                    text: "Дата брони"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
                Text {
                    text: "Столы номер"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
                Text {
                    text: "Возраст"
                    color: "white"
                    font.family: "Verdana"
                    font.pixelSize: 30
                    font.bold: true
                }
            }

            ColumnLayout {
                id: layout2
                anchors {
                    leftMargin: 350
                }
                spacing: 47
                TextField {
                    id: secondNameInput
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    color: "black"
                    font.family: "Verdana"
                }
                TextField {
                    id: firstNameInput
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    color: "black"
                    font.family: "Verdana"
                }
                TextField {
                    id: thirdNameInput
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    color: "black"
                    font.family: "Verdana"
                }
                TextField {
                    id: dateInput
                    enabled: false
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    text: bookingDate
                    color: "black"
                    font.family: "Verdana"
                }
                TextField {
                    id: tablesInput
                    enabled: false
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    text: tablesBook
                    color: "black"
                    font.family: "Verdana"
                }
                TextField {
                    id: ageInput
                    Layout.preferredWidth: 300
                    font.pixelSize: 30
                    color: "black"
                    font.family: "Verdana"
                }
            }
        }

        Item {
                id: confirm
                width: 125
                height: 125
                anchors.horizontalCenter: parent.horizontalCenter
                anchors {
                    bottom: parent.bottom
                    bottomMargin: 30
                }
                Rectangle {
                    color: "white"
                    id: confirmrect
                    anchors.fill: parent
                    radius: 5
                    Image {
                        source: "../tick.png"
                        width: parent.width * 0.5
                        height: parent.height * 0.5
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (ageInput.text === "" || secondNameInput.text === "" || firstNameInput.text === "" || thirdNameInput.text === "") {
                                ageInput.placeholderText = "Пусто";
                                secondNameInput.placeholderText = "Пусто";
                                firstNameInput.placeholderText = "Пусто";
                                thirdNameInput.placeholderText = "Пусто";
                            } else {
                                dbHandler.checkIfClientExists(firstNameInput.text, secondNameInput.text, thirdNameInput.text, parseInt(ageInput.text));
                            }
                        }
                        onEntered: {
                            confirmrect.color = "gray";
                        }
                        onExited: {
                            confirmrect.color = "white";
                        }
                    }
                }
            }
        Connections {
            target: dbHandler
            onClientChecked: (clientId) => {
            id_client_for_booking = clientId;
            console.log("Client ID:", clientId);

            // Преобразуем строку столов в массив
            var tablesArray = tablesBook.split(' ').filter(item => item !== "").map(item => parseInt(item));

            if (id_client_for_booking === "") {
                dbHandler.addNewClient(firstNameInput.text, secondNameInput.text,
                thirdNameInput.text, parseInt(ageInput.text));
            } else {
                dbHandler.addNewBooking(bookingDate, tablesArray, id_client_for_booking);
            }

            }
            onClientAdded: (clientId) => {
                var tablesArray = tablesBook.split(' ').filter(item => item !== "").map(item => parseInt(item));
                dbHandler.addNewBooking(bookingDate, tablesArray, clientId);
            }
            onBookingAdded: (success, message) => {
                console.log (message);
                bookingStatus = success;
                if (bookingStatus) {
                    stackViewForBookings.push("../endBooking.qml");
                    //bookWindow.tables.bookedTables = {};
                }
            }
        }
    }



