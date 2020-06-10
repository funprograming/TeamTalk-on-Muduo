/*
 * LoginConn.h
 *
 *  Created on: 2013-6-21
 *      Author: jianqingdu
 */

#ifndef LOGINCONN_H_
#define LOGINCONN_H_

#include <memory>
#include "muduo/net/TcpServer.h"
#include <boost/circular_buffer.hpp>
#include <unordered_set>

typedef struct  {
    string		ip_addr1;	// 电信IP
    string		ip_addr2;	// 网通IP
    uint16_t	port;
    uint32_t	max_conn_cnt;
    uint32_t	cur_conn_cnt;
    string 		hostname;	// 消息服务器的主机名
} msg_serv_info_t;

std::unordered_map<const TcpConnectionPtr&, msg_serv_info_t*> g_msg_serv_info_;
typedef std::shared_ptr<IM::Server::IMMsgServInfo> MsgServInfoPtr;
typedef std::shared_ptr<IM::Server::IMUserCntUpdate> UserCntUpdatePtr;
typedef std::shared_ptr<IM::Login::IMMsgServReq> MsgServReqPtr;

class LoginConn
{
public:
	LoginConn(EventLoop* loop, const InetAddress& listenAddr) = delete;

	void start()
	{
		server_.start();
	}


private:
	void onConnection(const TcpConnectionPtr& conn);
	void onMsgServInfo(const TcpConnectionPtr& conn,const MsgServInfoPtr& msg,Timestamp time);
	void onMsgServReq(const TcpConnectionPtr& conn,const MsgServInfoPtr& msg,Timestamp);
	void onUserCntUpdat(const TcpConnectionPtr& conn,const UserCntUpdatePtr& message,Timestamp);
	void LoginConn::onTimer(void);
	void LoginConn::HeartBeart(const TcpConnectionPtr& conn);
	void LoginConn::onConnection(const TcpConnectionPtr& conn)
	void LoginConn::onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time);

	TcpServer server_;
  	ProtobufDispatcher dispatcher_;
    ProtobufCodec codec_;
	
	total_online_user_cnt_;


	void dumpConnectionBuckets() const;

	typedef boost::weak_ptr<muduo::net::TcpConnection> WeakTcpConnectionPtr;

	struct Entry : public muduo::copyable
	{
		explicit Entry(const WeakTcpConnectionPtr& weakConn)
		: weakConn_(weakConn)
		{
		}

		~Entry()
		{
			muduo::net::TcpConnectionPtr conn = weakConn_.lock();
			if (conn)
			{
				conn->shutdown();
			}
		}

		WeakTcpConnectionPtr weakConn_;
	};
	typedef shared_ptr<Entry> EntryPtr;
	typedef weak_ptr<Entry> WeakEntryPtr;
	typedef unordered_set<EntryPtr> Bucket;
	typedef boost::circular_buffer<Bucket> WeakConnectionList;

	WeakConnectionList connectionBuckets_;

};

#endif /* LOGINCONN_H_ */
