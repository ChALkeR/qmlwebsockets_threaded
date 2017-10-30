# A threaded wrapper for QML WebSocket interface

## The problem

This aims to be a work-around for
[QTBUG-63719](https://bugreports.qt.io/browse/QTBUG-63719),
without this QML WebSocket API blocks main event loop and user interface while
receiving messages over network — that is after the server started to send the
message and before the message was fully received, the UI is frozen.

That is visible on Wi-Fi networks, under heavy WebSocket usage, and especially
on 4G/mobile networks.

That issue could be observed in all current Qt versions at the date, including
5.9.2 and 5.10.0-beta.

See the `test-original.qml` file to see the problem, and `test-threaded` to
see this workaround in action.

## The solution

This repo provides a `WebSocketThreaded` import, which could be used as a
drop-in replacement for `WebSocket`, but it has some
[drawbacks](#the-drawbacks).

This work-around works by encapsulating `QWebSocket` inside a `QThread` and
passing data as events — see QWebSocketThreaded file for that.

That class could also potentially be used from C++ code, but it is not exported
and does not replicate full QWebSocket API — only the subset that was needed for
QML WebSocket.

## The drawbacks

* Starts a separate thread for each WebSocket instance
* QML WebSocketServer is not implemented
* `sendTextMessage` and `sendBinaryMessage` return -1 instead of the transferred
  size, but that shouldn't break anything as those are
  [documented](https://doc.qt.io/qt-5/qml-qtwebsockets-websocket.html#sendTextMessage-method)
  to return `void` (which is not actually true, btw).
* `errorString` is empty in most cases (FIXME)

## Copyright

This code is based on the original
[Qt WebSockets](https://github.com/qt/qtwebsockets) code and is licensed
accordingly (LGPL3/GPL2+).
