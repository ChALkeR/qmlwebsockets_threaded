// Based on qwebsocket.h from QtWebSockets
/****************************************************************************
**
** Copyright (C) 2017 Nikita Skovoroda <chalkerx@gmail.com>.
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is based part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWEBSOCKETTHREADED_H
#define QWEBSOCKETTHREADED_H

#include <QObject>
#include <QThread>
#include <QtWebSockets/QWebSocket>

QT_BEGIN_NAMESPACE

class QWebSocketThreaded : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QWebSocketThreaded)

public:
    explicit QWebSocketThreaded(const QString &origin = QString(),
                        QWebSocketProtocol::Version version = QWebSocketProtocol::VersionLatest,
                        QObject *parent = Q_NULLPTR);
    virtual ~QWebSocketThreaded();

    //void abort();
    //QAbstractSocket::SocketError error() const;
    QString errorString() const;
    //bool flush();
    //bool isValid() const;
    //QHostAddress localAddress() const;
    //quint16 localPort() const;
    //QAbstractSocket::PauseModes pauseMode() const;
    //QHostAddress peerAddress() const;
    //QString peerName() const;
    //quint16 peerPort() const;
#ifndef QT_NO_NETWORKPROXY
    //QNetworkProxy proxy() const;
    //void setProxy(const QNetworkProxy &networkProxy);
#endif
    //void setMaskGenerator(const QMaskGenerator *maskGenerator);
    //const QMaskGenerator *maskGenerator() const;
    //qint64 readBufferSize() const;
    //void setReadBufferSize(qint64 size);

    //void resume();
    //void setPauseMode(QAbstractSocket::PauseModes pauseMode);

    QAbstractSocket::SocketState state() const;

    //QWebSocketProtocol::Version version() const;
    //QString resourceName() const;
    QUrl requestUrl() const;
    //QNetworkRequest request() const;
    //QString origin() const;
    //QWebSocketProtocol::CloseCode closeCode() const;
    //QString closeReason() const;

    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &data);

#ifndef QT_NO_SSL
    //void ignoreSslErrors(const QList<QSslError> &errors);
    //void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    //QSslConfiguration sslConfiguration() const;
#endif

public Q_SLOTS:
    void close(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
               const QString &reason = QString());
    void open(const QUrl &url);
    //void open(const QNetworkRequest &request);
    //void ping(const QByteArray &payload = QByteArray());
#ifndef QT_NO_SSL
    //void ignoreSslErrors();
#endif

Q_SIGNALS:
    //void aboutToClose();
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState state);
#ifndef QT_NO_NETWORKPROXY
    //void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *pAuthenticator);
#endif
    //void readChannelFinished();
    //void textFrameReceived(const QString &frame, bool isLastFrame);
    //void binaryFrameReceived(const QByteArray &frame, bool isLastFrame);
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
    void error(QAbstractSocket::SocketError error);
    //void pong(quint64 elapsedTime, const QByteArray &payload);
    //void bytesWritten(qint64 bytes);

#ifndef QT_NO_SSL
    //void sslErrors(const QList<QSslError> &errors);
    //void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator);
#endif

public Q_SLOTS:
    void connectedHandler();
    void disconnectedHandler();
    void stateChangedHandler(QAbstractSocket::SocketState state);
    void textMessageReceivedHandler(const QString &message);
    void binaryMessageReceivedHandler(const QByteArray &message);
    void errorHandler(QAbstractSocket::SocketError error);

Q_SIGNALS:
    void closeCommand(QWebSocketProtocol::CloseCode closeCode, const QString &reason);
    void openCommand(const QUrl &url);
    void sendTextMessageCommand(const QString &message);
    void sendBinaryMessageCommand(const QByteArray &data);

private:
    QThread m_thread;
    QString m_errorString;
    QUrl m_url;
    QAbstractSocket::SocketState m_state;
};

#endif // QWEBSOCKETTHREADED_H
