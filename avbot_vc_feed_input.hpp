
#pragma once

#include <algorithm>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/concept_check.hpp>
#include <boost/timedcall.hpp>

class avbot_vc_feed_input{

public:
	avbot_vc_feed_input(boost::asio::io_service & io_service)
	  : m_io_service(io_service)
	{
	}

	void async_input_read(boost::function<void (boost::system::error_code, std::string)> handler)
	{
		// 将 handler 进入 列队.
		m_input_wait_handlers.push_back(handler);
	}

	void async_input_read_timeout(int timed_out ,  boost::function<void (boost::system::error_code, std::string)> handler)
	{
		// 将 handler 进入 列队.
		m_input_wait_handlers.push_back(handler);
		boost::delayedcallsec(m_io_service, timed_out, boost::bind(&avbot_vc_feed_input::call_this_to_feed_timeout, this));
	}

	void call_this_to_feed_line(std::string line)
	{
		BOOST_FOREACH(boost::function<void (boost::system::error_code, std::string)> handler ,  m_input_wait_handlers)
		{
			handler(boost::system::error_code(), line);
		}
		m_input_wait_handlers.clear();
	}

	void call_this_to_feed_message(boost::property_tree::ptree message)
	{
		// format 后调用 call_this_to_feed_line
		try{
			std::string textmessage;

			textmessage = message.get<std::string>("message.text");

			call_this_to_feed_line(textmessage);

		}catch (...){}
	}

	void call_this_to_feed_timeout()
	{
		BOOST_FOREACH(boost::function<void (boost::system::error_code, std::string)> handler ,  m_input_wait_handlers)
		{
			handler(boost::system::errc::make_error_code(boost::system::errc::timed_out), std::string());
		}
		m_input_wait_handlers.clear();
	}

private:
	std::vector<boost::function<void (boost::system::error_code, std::string)> >
			m_input_wait_handlers;
	boost::asio::io_service & m_io_service;
};
