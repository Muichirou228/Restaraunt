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
        tick.png

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
    TablesScheme.qml \
    birdie.png \
    booking.png \
    booking_changed.png \
    image.ico \
    tables_booking.qml \
    tick.png
