#include "qwebsocketserver.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkProxy>
#include "qwebsocketprotocol.h"
#include "handshakerequest_p.h"
#include "handshakeresponse_p.h"
#include "qwebsocket.h"

/*!
	\class WebSocketServer

	The WebSocketServer class provides a websocket-based server.
	It is modeled after QTcpServer, and behaves the same. So, if you know how to use QTcpServer, you know how to use WebSocketServer.\n
	This class makes it possible to accept incoming websocket connections.\n
	You can specify the port or have WebSocketServer pick one automatically.\n
	You can listen on a specific address or on all the machine's addresses.\n
	Call listen() to have the server listen for incoming connections.\n

	The newConnection() signal is then emitted each time a client connects to the server.\n
	Call nextPendingConnection() to accept the pending connection as a connected WebSocket.
	The function returns a pointer to a WebSocket in QAbstractSocket::ConnectedState that you can use for communicating with the client.\n
	If an error occurs, serverError() returns the type of error, and errorString() can be called to get a human readable description of what happened.\n
	When listening for connections, the address and port on which the server is listening are available as serverAddress() and serverPort().\n
	Calling close() makes WebSocketServer stop listening for incoming connections.\n
	Although WebSocketServer is mostly designed for use with an event loop, it's possible to use it without one. In that case, you must use waitForNewConnection(), which blocks until either a connection is available or a timeout expires.

	\ref echoserver

	\author Kurt Pattyn (pattyn.kurt@gmail.com)

	\sa WebSocket
*/

/*!
  \page echoserver WebSocket server example
  \brief A sample websocket server echoing back messages sent to it.

  \section Description
  The echoserver example implements a web socket server that echoes back everything that is sent to it.
  \section Code
  We start by creating a WebSocketServer (`new WebSocketServer()`). After the creation, we listen on all local network interfaces (`QHostAddress::Any`) on the specified \a port.
  \snippet echoserver.cpp constructor
  If listening is successful, we connect the `newConnection()` signal to the slot `onNewConnection()`.
  The `newConnection()` signal will be thrown whenever a new web socket client is connected to our server.

  \snippet echoserver.cpp onNewConnection
  When a new connection is received, the client WebSocket is retrieved (`nextPendingConnection()`), and the signals we are interested in
  are connected to our slots (`textMessageReceived()`, `binaryMessageReceived()` and `disconnected()`).
  The client socket is remembered in a list, in case we would like to use it later (in this example, nothing is done with it).

  \snippet echoserver.cpp processMessage
  Whenever `processMessage()` is triggered, we retrieve the sender, and if valid, send back the original message (`send()`).
  The same is done with binary messages.
  \snippet echoserver.cpp processBinaryMessage
  The only difference is that the message now is a QByteArray instead of a QString.

  \snippet echoserver.cpp socketDisconnected
  Whenever a socket is disconnected, we remove it from the clients list and delete the socket.
  Note: it is best to use `deleteLater()` to delete the socket.
*/

/*!
	\fn void WebSocketServer::newConnection()
	This signal is emitted every time a new connection is available.

	\sa hasPendingConnections() and nextPendingConnection().
*/

/*!
	Constructs a new WebSocketServer.

	\a parent is passed to the QObject constructor.
 */
QWebSocketServer::QWebSocketServer(const QString &serverName, QObject *parent) :
	QObject(parent),
	m_pTcpServer(0),
	m_serverName(serverName),
	m_pendingConnections()
{
	m_pTcpServer = new QTcpServer(this);
	connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

/*!
	Destroys the WebSocketServer object. If the server is listening for connections, the socket is automatically closed.
	Any client WebSockets that are still connected are closed and deleted.

	\sa close()
 */
QWebSocketServer::~QWebSocketServer()
{
	while (!m_pendingConnections.isEmpty())
	{
		QWebSocket *pWebSocket = m_pendingConnections.dequeue();
		pWebSocket->close(QWebSocketProtocol::CC_GOING_AWAY, "Server closed.");
		pWebSocket->deleteLater();
	}
	m_pTcpServer->deleteLater();
}

/*!
  Closes the server. The server will no longer listen for incoming connections.
 */
void QWebSocketServer::close()
{
	m_pTcpServer->close();
}

/*!
	Returns a human readable description of the last error that occurred.

	\sa serverError().
*/
QString QWebSocketServer::errorString() const
{
	return m_pTcpServer->errorString();
}

/*!
	Returns true if the server has pending connections; otherwise returns false.

	\sa nextPendingConnection() and setMaxPendingConnections().
 */
bool QWebSocketServer::hasPendingConnections() const
{
	return !m_pendingConnections.isEmpty();
}

/*!
	Returns true if the server is currently listening for incoming connections; otherwise returns false.

	\sa listen().
 */
bool QWebSocketServer::isListening() const
{
	return m_pTcpServer->isListening();
}

/*!
	Tells the server to listen for incoming connections on address \a address and port \a port.
	If \a port is 0, a port is chosen automatically.\n
	If \a address is QHostAddress::Any, the server will listen on all network interfaces.

	Returns true on success; otherwise returns false.

	\sa isListening().
 */
bool QWebSocketServer::listen(const QHostAddress &address, quint16 port)
{
	return m_pTcpServer->listen(address, port);
}

/*!
	Returns the maximum number of pending accepted connections. The default is 30.

	\sa setMaxPendingConnections() and hasPendingConnections().
 */
int QWebSocketServer::maxPendingConnections() const
{
	return m_pTcpServer->maxPendingConnections();
}

/*!
	This function is called to add the socket to the list of pending incoming websocket connections.

	\sa nextPendingConnection() and hasPendingConnections()
*/
void QWebSocketServer::addPendingConnection(QWebSocket *pWebSocket)
{
	if (m_pendingConnections.size() < maxPendingConnections())
	{
		m_pendingConnections.enqueue(pWebSocket);
	}
}

/*!
	Returns the next pending connection as a connected WebSocket object.
	The socket is created as a child of the server, which means that it is automatically deleted when the WebSocketServer object is destroyed. It is still a good idea to delete the object explicitly when you are done with it, to avoid wasting memory.
	0 is returned if this function is called when there are no pending connections.

	Note: The returned WebSocket object cannot be used from another thread..

	\sa hasPendingConnections().
*/
QWebSocket *QWebSocketServer::nextPendingConnection()
{
	QWebSocket *pWebSocket = 0;
	if (!m_pendingConnections.isEmpty())
	{
		pWebSocket = m_pendingConnections.dequeue();
	}
	return pWebSocket;
}

/*!
	Returns the network proxy for this socket. By default QNetworkProxy::DefaultProxy is used.

	\sa setProxy().
*/
QNetworkProxy QWebSocketServer::proxy() const
{
	return m_pTcpServer->proxy();
}

/*!
	Returns the server's address if the server is listening for connections; otherwise returns QHostAddress::Null.

	\sa serverPort() and listen().
 */
QHostAddress QWebSocketServer::serverAddress() const
{
	return m_pTcpServer->serverAddress();
}

/*!
	Returns an error code for the last error that occurred.
	\sa errorString().
 */
QAbstractSocket::SocketError QWebSocketServer::serverError() const
{
	return m_pTcpServer->serverError();
}

/*!
	Returns the server's port if the server is listening for connections; otherwise returns 0.
	\sa serverAddress() and listen().
 */
quint16 QWebSocketServer::serverPort() const
{
	return m_pTcpServer->serverPort();
}

/*!
	Sets the maximum number of pending accepted connections to \a numConnections.
	WebSocketServer will accept no more than \a numConnections incoming connections before nextPendingConnection() is called.\n
	By default, the limit is 30 pending connections.

	Clients may still able to connect after the server has reached its maximum number of pending connections (i.e., WebSocket can still emit the connected() signal). WebSocketServer will stop accepting the new connections, but the operating system may still keep them in queue.
	\sa maxPendingConnections() and hasPendingConnections().
 */
void QWebSocketServer::setMaxPendingConnections(int numConnections)
{
	m_pTcpServer->setMaxPendingConnections(numConnections);
}

/*!
	\brief Sets the explicit network proxy for this socket to \a networkProxy.

	To disable the use of a proxy for this socket, use the QNetworkProxy::NoProxy proxy type:

	\code
		server->setProxy(QNetworkProxy::NoProxy);
	\endcode

	\sa proxy().
*/
void QWebSocketServer::setProxy(const QNetworkProxy &networkProxy)
{
	m_pTcpServer->setProxy(networkProxy);
}

/*!
	Sets the socket descriptor this server should use when listening for incoming connections to \a socketDescriptor.

	Returns true if the socket is set successfully; otherwise returns false.\n
	The socket is assumed to be in listening state.

	\sa socketDescriptor() and isListening().
 */
bool QWebSocketServer::setSocketDescriptor(int socketDescriptor)
{
	return m_pTcpServer->setSocketDescriptor(socketDescriptor);
}

/*!
	Returns the native socket descriptor the server uses to listen for incoming instructions, or -1 if the server is not listening.
	If the server is using QNetworkProxy, the returned descriptor may not be usable with native socket functions.

	\sa setSocketDescriptor() and isListening().
 */
int QWebSocketServer::socketDescriptor() const
{
	return m_pTcpServer->socketDescriptor();
}

/*!
	Waits for at most \a msec milliseconds or until an incoming connection is available.
	Returns true if a connection is available; otherwise returns false.
	If the operation timed out and \a timedOut is not 0, \a *timedOut will be set to true.

	\note This is a blocking function call.
	\note Its use is disadvised in a single-threaded GUI application, since the whole application will stop responding until the function returns. waitForNewConnection() is mostly useful when there is no event loop available.
	\note The non-blocking alternative is to connect to the newConnection() signal.

	If \a msec is -1, this function will not time out.

	\sa hasPendingConnections() and nextPendingConnection().
*/
bool QWebSocketServer::waitForNewConnection(int msec, bool *timedOut)
{
	return m_pTcpServer->waitForNewConnection(msec, timedOut);
}

/*!
  Returns a list of websocket versions that this server is supporting.
 */
QList<QWebSocketProtocol::Version> QWebSocketServer::supportedVersions() const
{
	QList<QWebSocketProtocol::Version> supportedVersions;
	supportedVersions << QWebSocketProtocol::currentVersion();	//we only support V13
	return supportedVersions;
}

/*!
  Returns a list of websocket subprotocols that this server supports.
 */
QList<QString> QWebSocketServer::supportedProtocols() const
{
	QList<QString> supportedProtocols;
	return supportedProtocols;	//no protocols are currently supported
}

/*!
  Returns a list of websocket extensions that this server supports.
 */
QList<QString> QWebSocketServer::supportedExtensions() const
{
	QList<QString> supportedExtensions;
	return supportedExtensions;	//no extensions are currently supported
}

//Checking on the origin does not make much sense when the server is accessed
//via a non-browser client, as that client can set whatever origin header it likes
//In case of a browser client, the server SHOULD check the validity of the origin
//see http://tools.ietf.org/html/rfc6455#section-10
bool QWebSocketServer::isOriginAllowed(const QString &origin) const
{
	Q_UNUSED(origin)
	return true;
}

void QWebSocketServer::onNewConnection()
{
	QTcpSocket *pTcpSocket = m_pTcpServer->nextPendingConnection();
	connect(pTcpSocket, SIGNAL(readyRead()), this, SLOT(handshakeReceived()));
}

void QWebSocketServer::onCloseConnection()
{
	QTcpSocket *pTcpSocket = qobject_cast<QTcpSocket*>(sender());
	if (pTcpSocket != 0)
	{
		pTcpSocket->close();
	}
}

void QWebSocketServer::handshakeReceived()
{
	QTcpSocket *pTcpSocket = qobject_cast<QTcpSocket*>(sender());
	if (pTcpSocket != 0)
	{
		bool success = false;
		bool isSecure = false;
		HandshakeRequest request(pTcpSocket->peerPort(), isSecure);
		QTextStream textStream(pTcpSocket);
		textStream >> request;

		HandshakeResponse response(request,
								   m_serverName,
								   isOriginAllowed(request.getOrigin()),
								   supportedVersions(),
								   supportedProtocols(),
								   supportedExtensions());
		disconnect(pTcpSocket, SIGNAL(readyRead()), this, SLOT(handshakeReceived()));

		if (response.isValid())
		{
			QTextStream httpStream(pTcpSocket);
			httpStream << response;
			httpStream.flush();

			if (response.canUpgrade())
			{
				QWebSocket *pWebSocket = QWebSocket::upgradeFrom(pTcpSocket, request, response);
				if (pWebSocket)
				{
					pWebSocket->setParent(this);
					addPendingConnection(pWebSocket);
					Q_EMIT newConnection();
					success = true;
				}
				else
				{
					qDebug() << "WebSocketServer::handshakeReceived: Upgrading to WebSocket failed.";
				}
			}
			else
			{
				qDebug() << "WebSocketServer::handshakeReceived: Cannot upgrade to websocket.";
			}
		}
		else
		{
			qDebug() << "WebSocketServer::handshakeReceived: Invalid response. This should not happen!!!";
		}
		if (!success)
		{
			qDebug() << "WebSocketServer::handshakeReceived: Closing socket because of invalid or unsupported request";
			pTcpSocket->close();
		}
	}
	else
	{
		qDebug() << "WebSocketServerImp::handshakeReceived: Sender socket is NULL. This should not happen!!!";
	}
}