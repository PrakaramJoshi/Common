#ifndef IPC_BUFFER_SOCKET_TCP_H
#define IPC_BUFFER_SOCKET_TCP_H

#include <Datastructures\SerializeDeSerialize.h>
#include <Datastructures\BaseIPCBufferSocket.h>

template<typename QueueData>
class IPCBufferSocketTCP :public BaseIPCBufferSocket<QueueData>{

	void RunSender(){

		auto endp = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(m_id), get_port());
		boost::asio::ip::tcp::acceptor acceptor(get_service(), endp);
		boost::asio::ip::tcp::socket socket(get_service());

		QueueData *data;

		auto sendProcedure = [&socket](IO::chunky_data &_chunk){
			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(_chunk.p_data, _chunk.p_size), boost::asio::transfer_all(), ignored_error);
		};

		while (m_queue.Remove(&data)){
			if (data){

				acceptor.accept(socket);
				boost::system::error_code ignored_error;
				std::vector<IO::chunky_data > chunks;
			
				IO::break_to_chunks(m_data_chunk, *data, chunks);
				unsigned int total_chunks = chunks.size();
				boost::asio::write(socket, boost::asio::buffer(&total_chunks, sizeof(total_chunks)), boost::asio::transfer_all(), ignored_error);

				BaseIPCBufferQ::Send(chunks, sendProcedure);
				socket.close();
				delete data;

			}
		}
	}

	void RunReceiver(){

		boost::asio::ip::tcp::resolver resolver(get_service());
		boost::asio::ip::tcp::resolver::query query(m_id, "daytime");
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;

		boost::asio::ip::tcp::socket socket(get_service());
		boost::system::error_code error = boost::asio::error::host_not_found;
		auto connectedto = *endpoint_iterator;
		while (!m_queue.IsShutDown()){
			while (error && endpoint_iterator != end)
			{
				socket.close();
				connectedto = *endpoint_iterator;
				socket.connect(*endpoint_iterator++, error);
			}
			if (error){
				//std::cout << boost::system::system_error(error).what() << std::endl;
				endpoint_iterator = resolver.resolve(query);
				//throw boost::system::system_error(error);
			}
			else
				break;
		}
		std::cout << "Connected" << std::endl;

		auto data_chunk = m_data_chunk;

		// lambda to define how to receive the raw data from the channel
		auto receive_procedure = [&socket, &data_chunk](char *&_d, size_t &_received_size)->bool{
			boost::system::error_code error = boost::asio::error::host_not_found;
			_received_size = socket.read_some(boost::asio::buffer(_d, data_chunk), error);
			return true;
		};

		while (!m_queue.IsShutDown()){

			unsigned int total_chunks = 0;
			socket.read_some(boost::asio::buffer(&total_chunks, sizeof(total_chunks)), error);
			std::vector<IO::chunky_data >chunks;
			// get the data
			BaseIPCBufferQ::Receive(chunks, receive_procedure, total_chunks);

			//if the received data is not empty, recontruct QueuData object from it
			if (!chunks.empty())
				m_queue.Insert(IO::reconstruct_from_chunks<QueueData>(chunks));

			socket.close();
			socket.connect(connectedto, error);
			if (error){
				while (!m_queue.IsShutDown()){
					endpoint_iterator = resolver.resolve(query);
					while (error && endpoint_iterator != end)
					{
						socket.close();
						connectedto = *endpoint_iterator;
						socket.connect(*endpoint_iterator++, error);
					}
					if (!error){
						break;
					}
				}
			}
		}
	}

	IPCBufferSocketTCP();
	IPCBufferSocketTCP(const IPCBufferSocketTCP<QueueData> &_);
	IPCBufferSocketTCP(IPCBufferSocketTCP<QueueData> &&_);
	IPCBufferSocketTCP * operator = (const IPCBufferSocketTCP<QueueData> &_);
	IPCBufferSocketTCP * operator = (IPCBufferSocketTCP<QueueData> &&_);

public:

	IPCBufferSocketTCP(const std::string &_id,
		unsigned short _port,
		bool _sender) :BaseIPCBufferSocket<QueueData>(_id, _port,_sender){
		if ( is_sender())
			m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCBufferSocketTCP<QueueData>::RunSender), this));
		else
			m_run_thread.create_thread(boost::bind(boost::mem_fn(&IPCBufferSocketTCP<QueueData>::RunReceiver), this));
	}

	~IPCBufferSocketTCP(){

	}

	std::string get_protocol_name() const{
		return BaseIPCBufferSocket<QueueData>::get_protocol_name() + std::string(":TCP");
	}

};
#endif