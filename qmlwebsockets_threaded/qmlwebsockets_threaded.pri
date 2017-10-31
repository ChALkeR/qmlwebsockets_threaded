QT += websockets qml

HEADERS +=  $$PWD/qmlwebsocketsthreaded_plugin.h \
            $$PWD/qwebsocketthreaded.h \
            $$PWD/qqmlwebsocketthreaded.h

SOURCES +=  $$PWD/qmlwebsocketsthreaded_plugin.cpp \
            $$PWD/qwebsocketthreaded.cpp \
            $$PWD/qqmlwebsocketthreaded.cpp

OTHER_FILES += $$PWD/qmldir
