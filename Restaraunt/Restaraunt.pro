QT += quick network

SOURCES += \
        databasehandler.cpp \
        main.cpp

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += \
        TablesScheme.qml \
        resources \
        birdie.png \
        booking.png \
        booking_changed.png \
        tables_booking.qml \
        tick.png \
        CustomCalendar.qml \
        back.png \
        BackButton.qml \
        bookForm.qml \
        endBooking.qml \
        enter.png \
        waiter_main.qml \
        TablesSchemeForWaiters.qml \
        OrderInTable.qml \
        TableInProcess.qml \
        recipe.png \
        RecipeButton.qml \
        save.png \
        SaveButton.qml

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = image.ico

HEADERS += \
    databasehandler.h

DISTFILES += \
    BackButton.qml \
    CustomCalendar.qml \
    OrderInTable.qml \
    RecipeButton.qml \
    SaveButton.qml \
    TableInProcess.qml \
    TablesScheme.qml \
    TablesSchemeForWaiters.qml \
    back.png \
    birdie.png \
    bookForm.qml \
    booking.png \
    booking_changed.png \
    endBooking.qml \
    enter.png \
    image.ico \
    recipe.png \
    save.png \
    tables_booking.qml \
    tick.png \
    waiter_main.qml
