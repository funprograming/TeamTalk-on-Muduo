/*
 * HttpConn.cpp
 *
 *  Created on: 2013-9-29
 *      Author: ziteng@mogujie.com
 */

#include "HttpConn.h"
#include "LoginConn.h"
#include "json/json.h"
#include <string>

using namespace std;

LoginHttpConn::LoginHttpConn(EventLoop *loop,
    const InetAddress& listenAddr)
: server_(loop, listenAddr, "LoginHttpConn")
{
    server.setHttpCallback(
        bind(&HttpConn::onRequest, this));
}

void LoginHttpConn::onRequest(const HttpRequest& req, HttpResponse* resp)
{
    if (strncmp(req.path(), "/msg_server", 11) == 0) {
        // string content = m_cHttpParser.GetBodyContent();
        _HandleMsgServRequest(resp);
    } else {
        printf("url unknown, url=%s ", req.path());
        // Close();
    }
}
void LoginHttpConn::_HandleMsgServRequest(HttpResponse* resp)
{
    msg_serv_info_t* pMsgServInfo;
    uint32_t min_user_cnt = (uint32_t)-1;
    unordered_map<const TcpConnectionPtr&, msg_serv_info_t*>::iterator it_min_conn = g_msg_serv_info_.end();
    unordered_map<const TcpConnectionPtr&, msg_serv_info_t*>::iterator it;

    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/html");
    // resp->addHeader("Server", "Muduo");

    if(g_msg_serv_info_.size() <= 0)
    {
        Json::Value value;
        value["code"] = 1;
        value["msg"] = "没有msg_server";
        string strContent = value.toStyledString();
        // char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        // snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        // Send((void*)szContent, strlen(szContent));
        // delete [] szContent;
        resp->setBody(strContent);
        return ;
    }
    
    for (it = g_msg_serv_info_.begin() ; it != g_msg_serv_info_.end(); it++) {
        pMsgServInfo = it->second;
        if ( (pMsgServInfo->cur_conn_cnt < pMsgServInfo->max_conn_cnt) &&
            (pMsgServInfo->cur_conn_cnt < min_user_cnt)) {
            it_min_conn = it;
            min_user_cnt = pMsgServInfo->cur_conn_cnt;
        }
    }
    
    if (it_min_conn == g_msg_serv_info_.end()) {
        log("All TCP MsgServer are full ");
        Json::Value value;
        value["code"] = 2;
        value["msg"] = "负载过高";
        string strContent = value.toStyledString();
        // char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        // snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        // Send((void*)szContent, strlen(szContent));
        // conn->shutdown();
        // delete [] szContent;
        resp->setBody(strContent);
        return;
    } else {
        Json::Value value;
        value["code"] = 0;
        value["msg"] = "";
        value["priorIP"] = string(it_min_conn->second->ip_addr1);
        value["backupIP"] = string(it_min_conn->second->ip_addr2);
        value["port"] = int2string(it_min_conn->second->port);
        string strContent = value.toStyledString();
        // char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        // uint32_t nLen = strContent.length();
        // snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());
        // Send((void*)szContent, strlen(szContent));
        // conn->shutdown();
        // delete [] szContent;
        resp->setBody(strContent);
        return;
    }
}
