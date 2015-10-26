#ifndef BASE_IPC_BUFFER_SOCKET_H
#define BASE_IPC_BUFFER_SOCKET_H
#include <Datastructures\BaseIPCBufferQ.h>
#include <Datastructures\BlockingQueue.h>
#include <Datastructures\SerializeDeSerialize.h>
#include <boost/asio.hpp>

template<typename QueueData>
class BaseIPCBufferSocket :public BaseIPCBufferQ{

protected:
	BlockingQueue<QueueData> m_queue;
	boost::asio::io_service m_io_service;
	unsigned short m_port;
private:
	BaseIPCBufferSocket();
	BaseIPCBufferSocket(const BaseIPCBufferSocket &_);
	BaseIPCBufferSocket(BaseIPCBufferSocket &&_);
	BaseIPCBufferSocket * operator = (const BaseIPCBufferSocket &_);
	BaseIPCBufferSocket * operator = (BaseIPCBufferSocket &&_);

public:

	BaseIPCBufferSocket(const std::string &_id,
		unsigned short _port,
		bool _sender) :BaseIPCBufferQ(_id, _sender), m_port(_port){
	}

	BaseIPCBufferSocket(const std::string &_id,
		bool _open_only,
		unsigned short _port,
		bool _sender) :BaseIPCBufferQ(_id, _sender), m_port(_port){
	}

	virtual ~BaseIPCBufferSocket(){
		ShutDown();
		CleanUp();
	};

	virtual void RunSender() = 0;

	virtual void RunReceiver() = 0;

	std::size_t size()const {
		return 0;
	};

	// checks if the message queue is open
	// and checks if the buffer q is connected to it
	// this might be an expesnive check, be careful usingthis
	bool connected() const{
		return true;
	}

	//  deletes the older queue,(doesnt remove it from the 
	// system resource. Opens a new connection
	bool open(){
		return true;
	}

	//checks if the message queue is open
	// this might be an expensive check, use carefully
	bool is_open()const{

		return true;

	}

	void set_max_size(int _maxQSize){
		m_queue.set_max_size(_maxQSize);
	};

	// Add data to the queue
	void Send(QueueData *_data)
	{
		if (m_is_sender)
			m_queue.Insert(_data);
	};

	int Receive(QueueData**_data){
		if (!m_is_sender)
			return m_queue.Remove(_data);
		return 0;
	}

	int Receive_try(QueueData**_data){
		if (!m_is_sender)
			return m_queue.Remove_try(_data);
		return 0;
	}

	int CanSend()
	{
		return m_is_sender && m_queue.CanInsert();
	};

	void ShutDown()
	{
		m_queue.ShutDown();
		m_run_thread.join_all();
	}

	void CleanUp()
	{
		m_queue.CleanUp();
	}

	inline unsigned short get_port(){
		return m_port;
	}

	inline boost::asio::io_service& get_service(){
		return m_io_service;
	}

	virtual std::string get_protocol_name() const{
		return std::string("Buffer Socket");
	}

};
#endif