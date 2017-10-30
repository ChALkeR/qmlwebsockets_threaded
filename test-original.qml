import QtQuick 2.0
import QtWebSockets 1.0
//import QtWebSocketsThreaded 1.0

Item {
  property int concurrency: 5
  property int size: 128 * 1024 // 128 KiB
  property real lastTick: Date.now()

  WebSocket {
    url: 'ws://echo.websocket.org'
    active: true
    onTextMessageReceived: {
      console.log('Received message: ' + message.length)
      var delta = Date.now() - lastTick
      if (delta > check.interval * 2)
        console.log('delta [missing]: ' + delta)
    }
    onStatusChanged: {
      console.log('Status: ' + status)
      if (status == WebSocket.Open)
        for (var i = 0; i < concurrency; i++) {
          console.log('Sending message: ' + size)
          sendTextMessage(new Array(size + 1).join('x'))
        }
    }
  }
  Timer {
    id: check
    running: true
    interval: 10
    repeat: true
    onTriggered: {
      var tick = Date.now()
      var delta = tick - lastTick
      if (delta > interval * 2)
        console.log('delta: ' + delta)
      lastTick = tick
    }
  }
}
