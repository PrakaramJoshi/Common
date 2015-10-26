#ifndef BASE_IPC_BUFFER_Q_H
#define BASE_IPC_BUFFER_Q_H
#include <string>
#include <boost/thread.hpp>
#include <Datastructures\SerializeDeSerialize.h>
class BaseIPCBufferQ{

	std::size_t m_total_received;
	std::size_t m_total_sent;
protected:
	
	unsigned int m_data_chunk;
	boost::thread_group m_run_thread;
	boost::posix_time::time_duration m_time_out;
	std::string m_id;
	bool m_is_sender;

private:
	BaseIPCBufferQ();
	BaseIPCBufferQ(const BaseIPCBufferQ &_);
	BaseIPCBufferQ(BaseIPCBufferQ &&_);
	BaseIPCBufferQ * operator = (const BaseIPCBufferQ &_);
	BaseIPCBufferQ * operator = (BaseIPCBufferQ &&_);

public:

	BaseIPCBufferQ(const std::string &_id,
		bool _sender){
		//Erase previous message queue
		m_is_sender = _sender;
		m_id = _id;
		m_data_chunk = sizeof(unsigned int);
		m_time_out = boost::posix_time::seconds(10);
	}

	virtual ~BaseIPCBufferQ(){
	};

	std::size_t total_sent()const{
		return m_total_sent;
	}

	// gets the time out period in seconds 
	// used by the receiver.
	long get_current_time_out()const{
		return m_time_out.total_seconds();
	}

	// set the time out period for the receiver 
	// the time out will be used to check if the 
	// message queue is open or not.
	// the time out period is in seconds
	void set_current_time_out(long _time_out_seconds){
		m_time_out = boost::posix_time::seconds(_time_out_seconds);
	}

	std::size_t total_received()const{
		m_total_received;
	}

	std::string id()const{
		return m_id;
	}

	bool is_sender()const{
		return m_is_sender;
	}

	template<typename ReceiveProcedure>
	void Receive(std::vector<IO::chunky_data> &_chunks,
		ReceiveProcedure &_p,
		const unsigned int _total_chunks){
		for (unsigned int i = 0; i < _total_chunks; i++){
			char *d = new char[m_data_chunk];
			size_t received_size;
			if (!_p(d, received_size)){
				delete[] d;
				for (auto c : _chunks)
					delete[] c.p_data;
				_chunks.clear();
				return;
			}
			_chunks.push_back(IO::chunky_data(d, received_size));
		}
		m_total_received++;
	}

	template<typename SendProcedure>
	void Send(std::vector<IO::chunky_data> &_chunks,
		SendProcedure &_p){
		m_total_sent++;
		for (auto c : _chunks){
			_p(c);
			delete[] c.p_data;
		}
	}

	virtual bool connected() const = 0;

	virtual bool open() = 0;

	virtual bool is_open()const = 0;

	virtual std::string get_protocol_name() const = 0;
};

#endif