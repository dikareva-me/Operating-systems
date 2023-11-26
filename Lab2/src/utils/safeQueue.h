#ifndef __SAFE_QUEUE_H_
#define __SAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include "../conn/IConn.hpp"
#include <iostream>
#include <syslog.h>

class SafeQueue {
private:
    std::queue<IConn::Message> m_storage;
    mutable std::mutex m_mutex;
public:
	void push(const IConn::Message &val)
	{
		const std::lock_guard<std::mutex> lock(m_mutex);
		m_storage.push(val);
	}
	
	bool pop(IConn::Message *msg)
	{
		const std::lock_guard<std::mutex> lock(m_mutex);
		if (m_storage.empty()) {
			return false;
		}
		
		*msg = m_storage.front();
		m_storage.pop();
		return true;
	}
	
	bool getFromConn(IConn *con)
	{
		const std::lock_guard<std::mutex> lock(m_mutex);
		con->dropping();
		uint32_t countMessage = 0;
		con->readConn(&countMessage, sizeof(uint32_t));
		
		for (uint32_t i = 0; i < countMessage; i++) {
			IConn::Message msg;
			try
			{
				con->readConn(&msg, sizeof(IConn::Message));
			}
			catch (const char *err)
			{
				syslog(LOG_ERR, "%s", err);
				return false;
			}
			syslog(LOG_INFO, "Recieved data %s", msg.msg);
			m_storage.push(msg);
		}
		return true;
	}
	
	bool sendToConn(IConn *con)
	{
		const std::lock_guard<std::mutex> lock(m_mutex);
		con->dropping();
		uint32_t countMessage = m_storage.size();
		con->writeConn(&countMessage, sizeof(uint32_t));
		while (!m_storage.empty()) {
			try
			{
				con->writeConn(&m_storage.front(), sizeof(IConn::Message));
				syslog(LOG_INFO, "Send data");
			}
			catch (const char *err)
			{
				syslog(LOG_ERR, "%s", err);
				return false;
			}
			m_storage.pop();
		}
		return true;
	}
};

#endif //!__SAFE_QUEUE_H_