QT += core gui widgets network

CONFIG += c++17

win32-g++:QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
# QMAKE_LFLAGS += "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""

SOURCES += \
    $$files($$PWD/src/*.cpp)

HEADERS += \
    $$files($$PWD/src/*.h)

FORMS += $$PWD/src/server.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target