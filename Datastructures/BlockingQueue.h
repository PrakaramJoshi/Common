#ifndef BUFFER_H
#define BUFFER_H
#include <boost/thread.hpp>
#include <queue>

template<class QueueData>class BlockingQueue
{
private:
	std::queue<QueueData*> m_queue;						// Use STL queue to store data
	boost::mutex m_mutex;						// The mutex to synchronise on
	boost::condition_variable m_cond;		// The condition to wait for
	bool m_done;
	int m_maxQSize;

	BlockingQueue(const BlockingQueue &);

	BlockingQueue* operator=(const BlockingQueue &);

	BlockingQueue(BlockingQueue &&);

	BlockingQueue* operator=(BlockingQueue &&);

public:

	BlockingQueue(int _maxQSize = -1)
	{
		m_done = false;
		m_maxQSize = _maxQSize;//_maxQSize;
	}

	~BlockingQueue(){
		ShutDown();
		CleanUp();
	};

	void set_max_size(int _maxQSize){
		// Acquire lock on the queue
		boost::unique_lock<boost::mutex> lock(m_mutex);
		m_maxQSize = _maxQSize;
	};

	// Add data to the queue and notify others
	auto Insert(QueueData *_data)->decltype(m_queue.size())
	{
		// Acquire lock on the queue
		boost::unique_lock<boost::mutex> lock(m_mutex);
		if (!m_done)
		{
			while (m_maxQSize != -1 && m_queue.size() >= m_maxQSize)
				m_cond.wait(lock);
			m_queue.push(_data);
		}

		// Notify others that data is ready
		auto returnval = m_queue.size();
		lock.unlock();
		m_cond.notify_all();
		return returnval;
	} // Lock is automatically released here

	// Get data from the queue. Wait for data if not available
	int Remove(QueueData **_data)
	{
		// Acquire lock on the queue
		boost::unique_lock<boost::mutex> lock(m_mutex);

		// When there is no data, wait till someone fills it.
		// Lock is automatically released in the wait and obtained
		// again after the wait
		while (m_queue.size() == 0 && !m_done) m_cond.wait(lock);

		if (m_queue.size() == 0 && m_done)
		{
			lock.unlock();
			m_cond.notify_all();
			return 0;
		}
		// Retrieve the data from the queue
		*_data = &(*m_queue.front()); m_queue.pop();
		lock.unlock();
		m_cond.notify_all();
		return 1;
	}; // Lock is automatically released here;

	// Get data from the queue. Wait for data if not available
	int Remove_try(QueueData **_data)
	{
		// Acquire lock on the queue
		;
		boost::unique_lock<boost::mutex> lock(m_mutex);


		if (m_queue.size() == 0 && m_done)
		{
			lock.unlock();
			m_cond.notify_all();
			return 0;
		}
		if (m_queue.size() == 0){
			*_data = nullptr;
			lock.unlock();
			m_cond.notify_all();
			return 1;
		}
		// Retrieve the data from the queue
		*_data = &(*m_queue.front()); m_queue.pop();
		lock.unlock();
		m_cond.notify_all();
		return 1;
	}; // Lock is automatically released here;

	// Get data from the queue. Wait for data if not available
	int CanInsert()
	{
		// Acquire lock on the queue
		boost::unique_lock<boost::mutex> lock(m_mutex);
		if (m_done)
			return 0;
		return 1;
	}; // Lock is automatically released here;

	auto size()->decltype(m_queue.size()){
		boost::unique_lock<boost::mutex> lock(m_mutex);
		return m_queue.size();
	}

	void ShutDown()
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
		m_done = true;
		lock.unlock();
		m_cond.notify_all();
	}

	void Restart() {
		CleanUp();
		boost::unique_lock<boost::mutex> lock(m_mutex);
		m_done = false;
		lock.unlock();
		m_cond.notify_all();
	}

	bool IsShutDown(){
		boost::unique_lock<boost::mutex> lock(m_mutex);
		return m_done;
	}

	void CleanUp()
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
		while (!m_queue.empty())
		{
			m_queue.pop();
		}
	}
};

#endif