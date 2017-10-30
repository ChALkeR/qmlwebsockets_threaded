/****************************************************************************
**
** Copyright (C) 2017 Nikita Skovoroda <chalkerx@gmail.com>.
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

#include "qwebsocketthreaded.h"
#include <QtWebSockets/QWebSocket>
//#include <QDebug>

QWebSocketThreaded::QWebSocketThreaded(const QString &origin,
                                       QWebSocketProtocol::Version version,
                                       QObject *parent)
    : QObject(parent)
{
    QWebSocket *websocket = new QWebSocket(origin, version);
    //QWebSocketWrap *websocket = new QWebSocketWrap(origin, version);
    websocket->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, websocket, &QObject::deleteLater);

    connect(this, &QWebSocketThreaded::closeCommand, websocket, &QWebSocket::close);
    connect(this, &QWebSocketThreaded::openCommand, websocket, static_cast<void (QWebSocket::*)(const QUrl &)>(&QWebSocket::open));
    connect(this, &QWebSocketThreaded::sendTextMessageCommand, websocket, &QWebSocket::sendTextMessage);
    connect(this, &QWebSocketThreaded::sendBinaryMessageCommand, websocket, &QWebSocket::sendBinaryMessage);

    connect(websocket, &QWebSocket::connected, this, &QWebSocketThreaded::connectedHandler);
    connect(websocket, &QWebSocket::disconnected, this, &QWebSocketThreaded::disconnectedHandler);
    connect(websocket, &QWebSocket::stateChanged, this, &QWebSocketThreaded::stateChangedHandler);
    connect(websocket, &QWebSocket::textMessageReceived, this, &QWebSocketThreaded::textMessageReceivedHandler);
    connect(websocket, &QWebSocket::binaryMessageReceived, this, &QWebSocketThreaded::binaryMessageReceivedHandler);
    connect(websocket,
            static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
            this,
            &QWebSocketThreaded::errorHandler);
    
    m_thread.start();
}
QWebSocketThreaded::~QWebSocketThreaded() {
    m_thread.quit();
    m_thread.wait();
}

void QWebSocketThreaded::close(QWebSocketProtocol::CloseCode closeCode, const QString &reason) {
    closeCommand(closeCode, reason);
}
void QWebSocketThreaded::open(const QUrl &url) {
    m_url = url;
    openCommand(url);
}

void QWebSocketThreaded::connectedHandler() {
    //qDebug() << "connectedHandler";
    connected();
}
void QWebSocketThreaded::disconnectedHandler() {
    //qDebug() << "disconnectedHandler";
    disconnected();
}
void QWebSocketThreaded::stateChangedHandler(QAbstractSocket::SocketState state) {
    //qDebug() << "stateChangedHandler";
    m_state = state;
    stateChanged(state);
}
void QWebSocketThreaded::textMessageReceivedHandler(const QString &message) {
    //qDebug() << "textMessageReceivedHandler";
    textMessageReceived(message);
}
void QWebSocketThreaded::binaryMessageReceivedHandler(const QByteArray &message) {
    //qDebug() << "binaryMessageReceivedHandler";
    binaryMessageReceived(message);
}
void QWebSocketThreaded::errorHandler(QAbstractSocket::SocketError err) {
    //qDebug() << "errorHandler";
    error(err);
}

QString QWebSocketThreaded::errorString() const {
    // TODO
    return m_errorString;
}
QAbstractSocket::SocketState QWebSocketThreaded::state() const {
    return m_state;
}
QUrl QWebSocketThreaded::requestUrl() const {
    return m_url;
}
qint64 QWebSocketThreaded::sendTextMessage(const QString &message) {
    sendTextMessageCommand(message);
    // TODO: get length? That needs sync blocking
    return -1;
}
qint64 QWebSocketThreaded::sendBinaryMessage(const QByteArray &data) {
    sendBinaryMessageCommand(data);
    // TODO: get length? That needs sync blocking
    return -1;
}
