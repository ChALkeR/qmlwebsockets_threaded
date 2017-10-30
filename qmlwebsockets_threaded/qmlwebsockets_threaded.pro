QT = core websockets qml qml-private core-private

TARGETPATH = QtWebSockets

HEADERS +=  qmlwebsocketsthreaded_plugin.h \
            qqmlwebsocketthreaded.h

SOURCES +=  qmlwebsocketsthreaded_plugin.cpp \
            qqmlwebsocketthreaded.cpp

OTHER_FILES += qmldir

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

IMPORT_VERSION = 1.0

load(qml_plugin)
