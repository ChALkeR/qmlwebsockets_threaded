QT += websockets qml

TARGETPATH = QtWebSocketsThreaded

HEADERS +=  qmlwebsocketsthreaded_plugin.h \
            qwebsocketthreaded.h \
            qqmlwebsocketthreaded.h

SOURCES +=  qmlwebsocketsthreaded_plugin.cpp \
            qwebsocketthreaded.cpp \
            qqmlwebsocketthreaded.cpp

OTHER_FILES += qmldir

IMPORT_VERSION = 1.0
load(qml_plugin)
