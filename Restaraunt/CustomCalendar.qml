import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 600
    height: 150

    // Текущая выбранная дата (доступна извне)
    property date selectedDate: new Date()

    // Сигнал при изменении даты
    signal dateChanged(date newDate)

    // Функция определения дней в месяце (с учетом високосных годов)
    function daysInMonth(month, year) {
        const febDays = ((year % 4 === 0 && year % 100 !== 0) || (year % 400 === 0)) ? 29 : 28;
        return [31, febDays, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][month];
    }

    // Обновление списка дней при изменении месяца/года
    function updateDays() {
        const days = daysInMonth(monthCombo.currentIndex, yearCombo.currentValue);
        dayCombo.model = Array.from({length: days}, (_, i) => i + 1);
        if (dayCombo.currentIndex >= days) {
            dayCombo.currentIndex = days - 1;
        }
        updateDate();
    }

    // Обновление выбранной даты
    function updateDate() {
        root.selectedDate = new Date(yearCombo.currentValue, monthCombo.currentIndex, dayCombo.currentValue);
        dateChanged(root.selectedDate);
    }

    // Основной layout
    RowLayout {
        anchors.fill: parent
        spacing: 15

        // Блок "День"
        Label {
            text: "День"
            font.bold: true
            font.pixelSize: 30
            color: "white"
            Layout.alignment: Qt.AlignVCenter
        }

        ComboBox {
            id: dayCombo
            model: 31
            currentIndex: new Date().getDate() - 1
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            font.pixelSize: 30
            Layout.alignment: Qt.AlignVCenter
            onActivated: updateDate()
        }

        // Большой отступ перед блоком "Месяц"
        Item { Layout.preferredWidth: 30 }

        // Блок "Месяц"
        Label {
            text: "Месяц"
            font.bold: true
            font.pixelSize: 30
            color: "white"
            Layout.alignment: Qt.AlignVCenter
        }

        ComboBox {
            id: monthCombo
            model: ["Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                   "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"]
            currentIndex: new Date().getMonth()
            Layout.preferredWidth: 160
            Layout.preferredHeight: 40
            font.pixelSize: 30
            Layout.alignment: Qt.AlignVCenter
            onActivated: updateDays()
        }
        Item { Layout.preferredWidth: 30 }
        // Блок "Год"
        Label {
            text: "Год"
            font.bold: true
            color: "white"
            font.pixelSize: 30
            Layout.alignment: Qt.AlignVCenter
        }

        ComboBox {
            id: yearCombo
            model: [2024, 2025, 2026, 2027]
            currentIndex: 0
            Layout.preferredWidth: 90
            Layout.preferredHeight: 40
            font.pixelSize: 30
            Layout.alignment: Qt.AlignVCenter
            onActivated: updateDays()
        }
    }

    // Инициализация при создании
    Component.onCompleted: updateDays()
}
