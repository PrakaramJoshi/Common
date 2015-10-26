#ifndef COUNTER_H
#define COUNTER_H
#include <atomic>
template<typename Obj>class Counter{
	std::atomic<Obj>m_count;
	std::atomic<bool> m_done;
public:
	Counter()
	{
		m_done = false;
		m_count = 0;
	}
	~Counter(){};
	// Add data to the queue and notify others
	Obj operator++()
	{
		if (!m_done)
		{
			m_count++;
		}
		return m_count;
	} // Lock is automatically released here

	Obj operator--()
	{

		if (!m_done)
		{
			m_count--;
		}
		return m_count;
	} // Lock is automatically released here
	Obj val(){
		return m_count;
	}
	void ShutDown()
	{
		m_done = true;
	}
};
#endif