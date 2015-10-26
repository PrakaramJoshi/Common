#ifndef IPC_BUFFER_SOCKET_UDP_H
#define IPC_BUFFER_SOCKET_UDP_H

#include <Datastructures\SerializeDeSerialize.h>
#include <Datastructures\BaseIPCBufferSocket.h>
#include <StringUtils.h>
#include <iostream>
template<typename QueueData>
class IPCBufferSocketUDP :public BaseIPCBufferSocket<QueueData>{

	void RunSender(){
		try{
			boost::asio::ip::udp::socket socket(get_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(m_id), get_port()));

			boost::asio::ip::udp::endpoint sender_endpoint;

			QueueData *data;

			auto sendProcedure = [&socket, &sender_endpoint](IO::chunky_data &_chunk){
				socket.send_to(boost::asio::buffer(_chunk.p_data, _chunk.p_size), sender_endpoint);
			};

			while (m_queue.Remove(&data)){
				if (data){

					std::vector<IO::chunky_data > chunks;
					IO::break_to_chunks(m_data_chunk, *data, chunks);
					unsigned int total_chunks = chunks.size();

					while (true){
						//receive hello from the receiver.
						char hello;
						size_t length = socket.receive_from(
							boost::asio::buffer(&hello, sizeof(hello)), sender_endpoint);
						if (hello == 'H')
							break;
					}
					
					socket.send_to(boost::asio::buffer(&total_chunks, sizeof(total_chunks)), sender_endpoint);

					BaseIPCBufferQ::Send(chunks, sendProcedure);
					delete data;

				}
			}
		}
		catch (boost::system::system_error &e){
			std::cout << e.what() << std::endl;

		}
		
	}

	void RunReceiver(){

		boost::asio::ip::udp::resolver resolver(get_service());
		boost::asio::ip::udp::socket socket(get_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
		std::string port = StringUtils::get_string(get_port());
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_id, port.c_str());

		boost::asio::ip::udp::resolver::iterator iterator = resolver.resolve(query);
		boost::asio::ip::udp::resolver::iterator end;
		while (iterator == end){
			iterator = resolver.resolve(query);
		}
		boost::asio::ip::udp::endpoint sender_endpoint;

		auto data_chunk = m_data_chunk;

		// lambda to define how to receive the raw data from the channel
		auto receive_procedure = [&socket, &sender_endpoint, &data_chunk](char *&_d, size_t &_received_size)->bool{
			_received_size = socket.receive_from(boost::asio::buffer(_d, data_chunk), sender_endpoint);
			return true;
		};

		while (!m_queue.IsShutDown()){
			try{
				//ask for the message from the sender
				char hello = 'H';
				socket.send_to(boost::asio::buffer(&hello, sizeof(hello)), *iterator);
				unsigned int total_chunks = 0;
				//receive the size of data to be received
				size_t reply_size = socket.receive_from(boost::asio::buffer(&total_chunks, sizeof(total_chunks)), sender_endpoint);

				std::vector<IO::chunky_data >chunks;
				// get the data
				BaseIPCBufferQ::Receive(chunks, receive_procedure, total_chunks);

				//if the received data is not empty, recontruct QueuData object from it
				if (!chunks.empty())
					m_queue.Insert(IO::reconstruct_from_chunks<QueueData>(chunks));
			}
			catch (boost::system::system_error &e){
				std::cout << e.what() << std::endl;

			}
			
		}

	}

	IPCBufferSocketUDP();
	IPCBufferSocketUDP(const IPCBufferSocketUDP<QueueData> &_);
	IPCBufferSocketUDP(IPCBufferSocketUDP<QueueData> &&_);
	IPCBufferSocketUDP * operator = (const IPCBufferSocketUDP<QueueData> &_);
	IPCBufferSocketUDP * operator = (IPCBufferSocketUDP<QueueData> &&_);

public:

	IPCBufferSocketUDP(const std::string &_id,
		unsigned short _port,
		bool _sender) :BaseIPCBufferSocket<QueueData>(_id, _port, _sender){
			if (is_sender())
				m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCBufferSocketUDP<QueueData>::RunSender), this));
			else
				m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCBufferSocketUDP<QueueData>::RunReceiver), this));
		}

	~IPCBufferSocketUDP(){

	}

	std::string get_protocol_name() const{
		return BaseIPCBufferSocket<QueueData>::get_protocol_name()+ std::string(":UDP");
	}

};
#endif