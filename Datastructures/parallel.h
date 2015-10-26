#pragma once
#include <map>
#include <DataStructure.h>
#include <boost/bind.hpp>
#include <type_traits>
#include<tuple>

// a namespace to create parallel code
namespace Parallel{

	// base returnable object 
	class BaseReturnable{
	BaseReturnable(const BaseReturnable&);
	BaseReturnable*operator = (const BaseReturnable&);
public:
	BaseReturnable(){;};
	virtual BaseReturnable *Copy() =0;
	virtual ~BaseReturnable(){;};
};

template<typename Data>
class Returnable:public BaseReturnable{
	Data *m_data;

public :
	Returnable(){
		m_data = nullptr;
	};

	Returnable(Data *_data){
		m_data=_data;
	};

	Data * GetData(){
		return m_data;
	};

	static BaseReturnable *create_returnable(Data *_data){
		return new Returnable(_data);
	};

	void SetData(Data*_d){
		m_data = _d;
	}

	BaseReturnable *Copy(){
		BaseReturnable *c= new Returnable<Data>(m_data);
		m_data =nullptr;
		return c;
	}

	~Returnable(){;};
};

class BaseRunable{
public:
	BaseRunable(){;};

	virtual BaseReturnable* run()=0;

	virtual ~BaseRunable(){;};
};

template<typename Op>
class Runable:public BaseRunable{
	Op m_op;

	Runable();

	Runable(const Runable &);

	Runable*operator =(const Runable&);

public:
	Runable(Op _o):m_op(_o){;};

	static BaseRunable * create_runable(Op _o){
		return new Runable<Op>(_o);
	}

	BaseReturnable* run(){
		typedef typename decltype(m_op()) DataOut;
		static_assert(std::is_pointer<DataOut>::value, "return value of the stage has to be pointer type");
		return Returnable<std::remove_pointer<DataOut>::type>::create_returnable(m_op());
	};

	~Runable(){;};
};
struct EngineOut{
	BaseRunable *p_runnable;

	BaseReturnable *p_returnable;

	EngineOut(BaseRunable *_runnable,
		BaseReturnable *_returnable):p_runnable(_runnable),p_returnable(_returnable){;};
};

class Engine
{
	BlockingQueue<BaseRunable> m_queues;
	BlockingQueue<EngineOut> m_Dataout;
	boost::thread_group m_engineThreads;
	unsigned int m_thread_count_max;
	unsigned int m_thread_count_min;
	bool m_allFinished;
	bool m_dataout;

	void run_all(BlockingQueue<BaseRunable>*_queues,
		BlockingQueue<EngineOut> *_dataOut,
		bool *_data_out){
		BaseRunable *runable;
		while(_queues->Remove(&runable)){
			if (*_data_out){
				_dataOut->Insert(new EngineOut(runable, runable->run()));
			}
			else{
				delete runable->run();
			}
			delete runable;
		}
	}

	Engine();

	Engine(const Engine &);

	Engine*operator=(const Engine &);

public:

	void Add(BaseRunable*_runable){
		m_queues.Insert(_runable);
	}

	void start(){
		m_allFinished = false;
		unsigned int created_threads=0;
		while(created_threads<m_thread_count_min){
			for(unsigned int i =0;i<m_thread_count_max && created_threads<m_thread_count_max;i++){
				try{
					m_engineThreads.create_thread(boost::bind(boost::mem_fn(&Engine::run_all),this,&m_queues,&m_Dataout,&m_dataout));
					created_threads++;
				}
				catch(boost::exception &e){
				}
			}
		}
	}

	void join(){
		m_queues.ShutDown();
		m_Dataout.ShutDown();
		m_engineThreads.join_all();
		m_allFinished= true;
	}

	BlockingQueue<EngineOut> *out_queue(){
		return &m_Dataout;
	}

	bool all_finished(){return m_allFinished;};

	void run(){
		start();
		join();
	};

	Engine(unsigned int _thread_count_max,
		unsigned int _thread_count_min,
		bool _data_out){
		m_thread_count_max=_thread_count_max;
		m_thread_count_min=_thread_count_min;
		m_allFinished= false;
		m_dataout=_data_out;
	}

	~Engine(void){
		join();
	}
};

class base_process_stage{
	int m_id;

protected:
	BlockingQueue<BaseReturnable> *m_data_in;
	BlockingQueue<BaseReturnable> *m_data_out;
	bool m_is_complete;

public:
	virtual ~base_process_stage(){;
	};

	virtual Returnable<void>*run()= 0;

	BlockingQueue<BaseReturnable> *get_in_q(){
		return m_data_in;
	};

	BlockingQueue<BaseReturnable> *get_out_q(){
		return m_data_out;
	};

	void set_in_q(BlockingQueue<BaseReturnable> *_in_q){
		m_data_in = _in_q;
	};

	void set_out_q(BlockingQueue<BaseReturnable> *_out_q){
		m_data_out = _out_q;
	}

	int get_id(){return m_id;};

	void set_id(int _id){m_id=_id;};

};

template<typename Operation,
		 typename DataIn>
class process_stage:public base_process_stage{
	
	Operation m_op;

	process_stage();
	process_stage(const process_stage &);
	process_stage*operator =(const process_stage&);
	
public:
	process_stage(Operation _op):m_op(_op){
		m_is_complete = false;
	};

	static base_process_stage * create_stage(Operation _o){
		return new process_stage<Operation,DataIn>(_o);
	};

	Returnable<void>* run(){
		BaseReturnable *d;
		DataIn *dataIn;
		typedef typename decltype(m_op(dataIn)) DataOut;
		static_assert(std::is_pointer<DataOut>::value, "return value of the stage has to be pointer type");
		DataOut data_out;
		if (std::is_same<DataIn, std::remove_pointer<DataOut>::type>::value){
			//resuse the returnable object
			while (m_data_in->Remove(&d)){
				dataIn = static_cast<Returnable<DataIn> *>(d)->GetData();
				data_out = m_op(dataIn);
				static_cast<Returnable<std::remove_pointer<DataOut>::type> *>(d)->SetData((data_out));
				(*m_data_out).Insert(d);
			}
		}
		else{
			// new returnable object is created
			while (m_data_in->Remove(&d)){
				dataIn = static_cast<Returnable<DataIn> *>(d)->GetData();
				delete d;
				data_out = m_op(dataIn);
				(*m_data_out).Insert(new Returnable<std::remove_pointer<DataOut>::type >(data_out));
			}
		}
		m_data_out->ShutDown();
		return nullptr;
	};
};

template<typename Operation>
class producer_stage :public base_process_stage{

	Operation m_op;

	producer_stage();
	producer_stage(const producer_stage &);
	producer_stage*operator =(const producer_stage&);

public:
	producer_stage(Operation _op) :m_op(_op){
		m_is_complete = false;
	};

	static base_process_stage * create_stage(Operation _o){
		return new producer_stage<Operation>(_o);
	};

	Returnable<void>* run(){
		
		m_op(m_data_out);
		m_data_out->ShutDown();
		return nullptr;
	};
};

template<typename Operation,typename DataIn>
class consumer_stage :public base_process_stage{

	Operation m_op;

	consumer_stage();
	consumer_stage(const consumer_stage &);
	consumer_stage*operator =(const consumer_stage&);

public:
	consumer_stage(Operation _op) :m_op(_op){
		m_is_complete = false;
	};

	static base_process_stage * create_stage(Operation _o){
		return new consumer_stage<Operation, DataIn>(_o);
	};

	Returnable<void>* run(){
		DataIn *d;
		BaseReturnable* data;
		while (m_data_in->Remove(&data)){
			d = static_cast<Returnable<DataIn> *>(data)->GetData();
			delete data;
			m_op(d);
		}
		
		return nullptr;
	};
};

class process_line{
	std::vector<base_process_stage* > m_stages;
	std::vector<BlockingQueue<BaseReturnable> *> m_created_q;
	boost::thread_group m_process_q;
	Engine *m_engine;
	unsigned int m_max_thread_count;
	unsigned int m_min_thread_count;

	process_line(const process_line &);
	process_line *operator=(const process_line&);

	void clear(){
		delete m_engine;
		m_engine =nullptr;

		for(auto s:m_stages){
			delete s;
		}

		for (auto q : m_created_q)
			delete q;

		m_created_q.clear();
		m_stages.clear();
	}

public:
	process_line(){
		m_engine =nullptr;
		m_max_thread_count = 0;
		m_min_thread_count = 0;
	};

	process_line(const unsigned int _max_thread_count,
				const unsigned int _min_thread_count){
		m_engine =nullptr;
		m_max_thread_count = _max_thread_count;
		m_min_thread_count = _min_thread_count;
	}

	inline void add_next_stage(base_process_stage *_stage){
		BlockingQueue<BaseReturnable> * in ;
		BlockingQueue<BaseReturnable> * out=new BlockingQueue<BaseReturnable>() ;
		m_created_q.push_back(out);

		if(m_stages.empty()){
			in = new BlockingQueue<BaseReturnable>();
			m_created_q.push_back(in);
		}
		else{
			in=m_stages.back()->get_out_q();
		}
		_stage->set_in_q(in);
		_stage->set_out_q(out);
		_stage->set_id(static_cast<int>(m_stages.size()+1));
		m_stages.push_back(_stage);
	};

	inline void add_stage_at_current_level(base_process_stage *_stage){
		{
			BlockingQueue<BaseReturnable> * in;
			BlockingQueue<BaseReturnable> * out;
			if (m_stages.empty()){
				in = new BlockingQueue<BaseReturnable>();
				out = new BlockingQueue<BaseReturnable>();
				m_created_q.push_back(in);
				m_created_q.push_back(out);
			}
			else{
				in = m_stages.back()->get_in_q();
				out = m_stages.back()->get_out_q();
			}
			_stage->set_in_q(in);
			_stage->set_out_q(out);
			_stage->set_id(static_cast<int>(m_stages.size() + 1));
			m_stages.push_back(_stage);
		};
	}

	void send(BaseReturnable *_d){
		m_stages[0]->get_in_q()->Insert(_d);
	}

	void ShutDown(){
		m_stages[0]->get_in_q()->ShutDown();
	}

	void start(){
		if(m_max_thread_count>m_stages.size()||m_max_thread_count==0)
			m_max_thread_count = static_cast<int>(m_stages.size());
		if(m_min_thread_count>m_stages.size()||m_min_thread_count==0)
			m_min_thread_count = static_cast<int>(m_stages.size());
		if(m_min_thread_count>m_max_thread_count)
			m_max_thread_count=m_min_thread_count;

		m_engine= new Engine(m_max_thread_count,m_min_thread_count,false);
		m_engine->start();
		Engine *engine = m_engine;
			std::for_each(m_stages.begin(),m_stages.end(),[&engine](base_process_stage* _stage){
				boost::function<Parallel::Returnable<void>* () >f(boost::bind(&Parallel::base_process_stage::run,_stage));
				auto runable=Parallel::Runable<decltype(f)>::create_runable(f);
				engine->Add(runable);
		});
	}

	bool receive(BaseReturnable*&_d){
		return m_stages.back()->get_out_q()->Remove(&_d)!=0;
	}

	void join(){
		m_engine->join();
	}

	~process_line(){
		clear();
	}
};

}
namespace example{
	void parallel_example(){
		Parallel::process_line process_line;

		auto append_a = [](std::string *_str)->std::string*{
			(*_str) += " a";
			return _str;
		};

		auto append_b=[](std::string *_str)->std::string*{
			(*_str) += " b";
			return _str;
		};

		auto append_c = [](std::string *_str)->std::pair<std::string,int>*{
			(*_str) += " c";
			std::string str = *_str;
			std::pair<std::string, int> *p = new std::pair<std::string, int>();
			p->first = str;
			p->second = 1;
			return p;
		};
		auto append_c_2 = [](std::string *_str)->std::string*{
			(*_str) += " c";
			return _str;
		};


		auto process_stage1 = Parallel::process_stage<decltype(append_a), std::string>::create_stage(append_a);
		auto process_stage2 = Parallel::process_stage<decltype(append_b), std::string>::create_stage(append_b);
		auto process_stage3 = Parallel::process_stage<decltype(append_c), std::string>::create_stage(append_c);
		auto process_stage3_2 = Parallel::process_stage<decltype(append_c_2), std::string>::create_stage(append_c_2);

		typedef std::pair<std::string, int> LastStageType;
		//typedef std::string LastStageType;

		process_line.add_next_stage(process_stage1);
		process_line.add_next_stage(process_stage2);
		process_line.add_next_stage(process_stage3);
		//process_line.add_stage(process_stage3_2);

		process_line.start();

		// method 1 to create producer consumder 
		// using pipeline

		auto consumer = [&process_line]()->void*{
			Parallel::BaseReturnable* received_data;
			while (process_line.receive(received_data)){
				Parallel::Returnable<LastStageType> *data = static_cast<Parallel::Returnable<LastStageType> *>(received_data);
				std::cout << (*data->GetData()).first << (*data->GetData()).second << std::endl;
				delete data->GetData();
				delete data;
			}
			return nullptr;
		};

		auto producer = [&process_line]()->void*{
			for (int i = 0; i<1000; i++){
				std::string *str1 = new std::string("str 1");
				std::string *str2 = new std::string("str 2");
				std::string *str3 = new std::string("str 3");
				process_line.send(Parallel::Returnable<std::string>::create_returnable(str1));
				process_line.send(Parallel::Returnable<std::string>::create_returnable(str2));
				process_line.send(Parallel::Returnable<std::string>::create_returnable(str3));
			}

			process_line.ShutDown();
			return nullptr;
		};

		// method 2 create producer consumer 
		// using engine directly
		Parallel::Engine engine(2, 2, false);
		auto runable_consumer = Parallel::Runable<decltype(consumer)>::create_runable(consumer);
		auto runable_producer = Parallel::Runable<decltype(producer)>::create_runable(producer);
		engine.start();
		engine.Add(runable_consumer);
		engine.Add(runable_producer);

		process_line.join();

		engine.join();
	}
}