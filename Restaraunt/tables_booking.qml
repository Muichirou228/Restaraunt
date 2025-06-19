import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Window {
    id: bookWindow
    width: 1000
    height: 1000
    minimumWidth: 1000
    minimumHeight: 800
    visible: true
    color: "black"
    property int tableWarning: 0
    Component.onCompleted: {
        tableWarning = 0
    }
    StackView {
        id: stackViewForBookings
        anchors.fill: parent
        initialItem: bookPage
    }
    Component {
        id: bookPage
        Item {
            Text {
                id: bookText
                width: parent.width
                text: "Бронирование столов (не больше 2 столов на 1 день)"
                wrapMode: Text.WrapAnywhere
                font.family: "Verdana"
                font.bold: true
                font.pixelSize: 40
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 40
            }

            CustomCalendar {
                id: calendar
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: bookText.bottom
                anchors.topMargin: 50
                onDateChanged: {
                    var newDate = selectedDate.toISOString().split('T')[0]
                    console.log("New date = ", newDate)
                    tables.resetTables();
                    dbHandler.getBookingsOnDate(newDate);
                    tableWarning = 0;
                    confirmrect.visible = false;
                }
            }

            TablesScheme {
                id: tables
                onTableClicked: function (tableWarning) {
                    bookWindow.tableWarning = tableWarning
                    if (check.checked && bookWindow.tableWarning > 0) confirmrect.visible = true; else confirmrect.visible = false;
                }
            }

            CheckBox {
                id: check
                text: "Я прочитал(а) политику ресторана"
                font.family: "Verdana"
                font.bold: true
                spacing: 20
                font.pixelSize: 20
                anchors.bottom: confirm.top
                anchors.bottomMargin: 35
                anchors.horizontalCenter: parent.horizontalCenter
                contentItem: Text{
                    text: parent.text
                    font: parent.font
                    color: "white"  // Белый цвет текста
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: parent.indicator.width + parent.spacing  // Учитываем увеличенный spacing
                }
                onCheckedChanged: {
                    if (check.checked && bookWindow.tableWarning > 0)
                        confirmrect.visible = true; else confirmrect.visible = false;
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
                        visible: false
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
                                stackViewForBookings.push("../bookForm.qml", {bookingDate : calendar.selectedDate.toISOString().split('T')[0] ,tablesBook : tables.getBookedTables()});
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
                onBookingsReady:{
                    for (var i = 0; i < bookedTables.length; i++) {
                        console.log("Занятые столы: ", bookedTables[i]);
                    }
                }
            }
        }
    }
}
