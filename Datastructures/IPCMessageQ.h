#ifndef IPC_MESSAGE_Q_H
#define IPC_MESSAGE_Q_H
#include <Datastructures\BaseIPCBufferQ.h>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <Datastructures\SerializeDeSerialize.h>
#include <Datastructures\BlockingQueue.h>

class BaseIPCMessageQ :public BaseIPCBufferQ{

protected:

	boost::interprocess::message_queue *m_mq;

private:
	BaseIPCMessageQ();
	BaseIPCMessageQ(const BaseIPCMessageQ &_);
	BaseIPCMessageQ(BaseIPCMessageQ &&_);
	BaseIPCMessageQ * operator = (const BaseIPCMessageQ &_);
	BaseIPCMessageQ * operator = (BaseIPCMessageQ &&_);

public:

	BaseIPCMessageQ(const std::string &_id,
		bool _sender) :BaseIPCBufferQ(_id, _sender){
		//Erase previous message queue
		if (m_is_sender){
			boost::interprocess::message_queue::remove(m_id.c_str());
		}
		//Create a message_queue.
		m_mq = new boost::interprocess::message_queue
			(boost::interprocess::open_or_create //only create
			, _id.c_str() //name
			, 100 //max message number
			, m_data_chunk //max message size
			);
	}

	BaseIPCMessageQ(const std::string &_id,
		bool _open_only,
		bool _sender) :BaseIPCBufferQ(_id, _sender){
		//Erase previous message queue
		if (m_is_sender){
			boost::interprocess::message_queue::remove(m_id.c_str());
		}
		//Create a message_queue.
		if (_open_only){
			m_mq = nullptr;
			if (is_open()){
				m_mq = new boost::interprocess::message_queue
					(boost::interprocess::open_only
					, _id.c_str());
			}

		}
		else{
			m_mq = new boost::interprocess::message_queue
				(boost::interprocess::open_or_create // create or open
				, _id.c_str() //name
				, 100 //max message number
				, m_data_chunk //max message size
				);
		}
	}

	virtual ~BaseIPCMessageQ(){
		delete m_mq;
	};

	std::size_t size()const {
		return m_mq->get_num_msg();
	};

	// checks if the message queue is open
	// and checks if the buffer q is connected to it
	// this might be an expesnive check, be careful usingthis
	bool connected() const{
		if (is_open()){
			return m_mq != nullptr;
		}
		return false;
	}

	//  deletes the older queue,(doesnt remove it from the 
	// system resource. Opens a new connection
	bool open(){
		if (m_mq){
			delete m_mq;
		}
		m_mq = nullptr;
		if (is_open()){
			m_mq = new boost::interprocess::message_queue
				(boost::interprocess::open_only
				, m_id.c_str());
			return true;
		}
		return false;
	}

	//checks if the message queue is open
	// this might be an expensive check, use carefully
	bool is_open()const{
		try{
			boost::interprocess::message_queue m(boost::interprocess::open_only, m_id.c_str());
			return true;
		}
		catch (boost::interprocess::interprocess_exception &ex){
			std::cout << ex.what() << std::endl;
			return false;
		}

	}

	std::string get_protocol_name() const{
		return std::string("Message Queue");
	}
};

template<typename QueueData>
class IPCMessageQ :public BaseIPCMessageQ{

	BlockingQueue<QueueData> m_queue;

	void RunSender(){
		QueueData *data;
		auto this_ptr = this;
		auto sendProcedure = [&this_ptr](IO::chunky_data &_chunk){
			this_ptr->m_mq->send(_chunk.p_data, _chunk.p_size, 0);
		};

		while (m_queue.Remove(&data)){
			if (data){
				std::vector<IO::chunky_data > chunks;
				IO::break_to_chunks(m_data_chunk, *data, chunks);
				unsigned int total_chunks = chunks.size();
				m_mq->send(&total_chunks, sizeof(total_chunks), 0);

				BaseIPCBufferQ::Send(chunks, sendProcedure);
				delete data;
			}
		}
	}

	void RunReceiver(){

		unsigned int priority;
		std::size_t recvd_size;
		auto this_ptr = this;

		auto receive_procedure = [&this_ptr, &priority](char *&_d, size_t &_received_size)->bool{
			while (!this_ptr->m_mq->timed_receive(_d, this_ptr->m_data_chunk, _received_size, priority, boost::posix_time::ptime(boost::posix_time::second_clock::universal_time() + this_ptr->m_time_out))){
				if ((!this_ptr->is_open()) || this_ptr->m_queue.IsShutDown()){
					return false;
				}
			}
			return true;
		};

		while (!m_queue.IsShutDown()){

			unsigned int total_chunks = 0;
			//m_mq->receive(&total_chunks, sizeof(total_chunks), recvd_size, priority);
			while (!m_mq->timed_receive(&total_chunks, sizeof(total_chunks), recvd_size, priority, boost::posix_time::ptime(boost::posix_time::second_clock::universal_time() + m_time_out))){
				if ((!is_open()) || m_queue.IsShutDown())
					return;
			}
			if (recvd_size == sizeof(total_chunks)){
				std::vector<IO::chunky_data >chunks;
				
				BaseIPCBufferQ::Receive(chunks, receive_procedure, total_chunks);
				if (!chunks.empty())
					m_queue.Insert(IO::reconstruct_from_chunks<QueueData>(chunks));
			}
		}
	}

	IPCMessageQ();
	IPCMessageQ(const IPCMessageQ<QueueData> &_);
	IPCMessageQ(IPCMessageQ<QueueData> &&_);
	IPCMessageQ * operator = (const IPCMessageQ<QueueData> &_);
	IPCMessageQ * operator = (IPCMessageQ<QueueData> &&_);

public:

	IPCMessageQ(const std::string &_id,
		bool _sender) :BaseIPCMessageQ(_id, _sender){

		if (m_is_sender)
			m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCMessageQ<QueueData>::RunSender), this));
		else
			m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCMessageQ<QueueData>::RunReceiver), this));
	}

	~IPCMessageQ(){
		ShutDown();
		CleanUp();
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

		if (m_is_sender){
			boost::interprocess::message_queue::remove(m_id.c_str());
		}
	}

};

#endif