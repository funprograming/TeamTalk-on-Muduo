/*
 * HttpConn.h
 *
 *  Created on: 2013-9-29
 *      Author: ziteng
 */

#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include "muduo/net/http/HttpServer.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"

class LoginHttpConn
{
public:
    LoginHttpConn(EventLoop *loop, const InetAddress& listenAddr) = delete;
    void onRequest(const HttpRequest& req, HttpResponse* resp);

private:
    void _HandleMsgServRequest(HttpResponse* resp);
    HttpServer server_;

};
#endif /* IMCONN_H_ */
