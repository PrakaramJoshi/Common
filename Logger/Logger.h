#pragma once
#ifdef emit
#define EMIT_BACK_UP emit
#undef emit
#endif
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <boost/thread.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <Datastructures/DataStructure.h>
#include <Utils/MemoryCache.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <regex>
#include <atomic>
#include <functional>
#include <boost\algorithm\string.hpp>
#ifdef ACE_LOGGER_ENABLE_WEB_SOCKETS
#include "../socketio/include/sio_client.h"
#include "../socketio/include/sio_message.h"
#endif
#pragma warning(disable : 4482)
#pragma warning(disable : 4996)
namespace AceLogger
{
#ifdef ACE_LOGGER_ENABLE_WEB_SOCKETS
	typedef sio::string_message			str_message;
	typedef sio::message::list			message_list;
	typedef sio::event					web_event;
	typedef sio::socket::event_listener	web_event_listner;
	typedef sio::client					web_client;
#else
	// stubs for projects which are not using socket io.
	struct web_event {
		struct msg_container {
			std::string get_string() {
				return "";
			}
		};

		msg_container p_msg_container;

		msg_container* get_message() {
			return &p_msg_container;
		}
	};

	struct message_list {
		message_list(const std::string &_str) {
			(void)_str;
		}
		template<typename T>
		void push(const T&_val) {
			(void)_val;
		};
	};

	struct stub_socket {
		void emit(const std::string &_event, const message_list &_data) {
			(void)_event;
			(void)_data;
		}
		template<typename T>
		void on(const std::string, T&&) {
		}
	};
	struct web_client {
		stub_socket p_socket;
		void connect(const std::string &_str) {
			(void)_str;
		};
		stub_socket* socket(const std::string &_nbsp = "") {
			return &p_socket;
		};
		void sync_close() {

		}
	};


	struct str_message {
		static std::string create(const std::string &_str) {
			return _str;
		}
	};

	typedef std::function<void(web_event&)> web_event_listner;
#endif

	enum MessageType{LOG_STATUS,LOG_ERROR,LOG_WARNING};
	enum LOG_TYPE{	DISPLAY	=	0x001, 
					FILE	=	0x002,
					WEB		=	0x004,
					ALL		=	0x007 };
	struct Message{
		std::string p_msg;
		MessageType p_messageType;
		LOG_TYPE p_log_type;
		Message() {
			p_msg = "";
			p_messageType = MessageType::LOG_STATUS;
			p_log_type = LOG_TYPE::ALL;
		};
		Message(const std::string &_msg, 
				const MessageType &_messageType, 
					const LOG_TYPE &_log_type) :
			p_msg(_msg), p_messageType(_messageType), p_log_type(_log_type){
		};
	};
	inline std::tm localtime(std::time_t const & time) {
		std::tm tm_snapshot;
#if (defined(__MINGW32__) || defined(__MINGW64__))
		memcpy(&tm_snapshot, ::localtime(&time), sizeof(std::tm));
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
		localtime_s(&tm_snapshot, &time);
#else
		localtime_r(&time, &tm_snapshot); // POSIX  
#endif
		return tm_snapshot;
	}
	std::string static time_stamp_file_name() {

		std::ostringstream s;

		std::time_t t = std::time(NULL);
		std::tm timeinfo = AceLogger::localtime(t);

		s << timeinfo.tm_mon + 1 
			<< "." 
			<< timeinfo.tm_mday 
			<< "." 
			<< timeinfo.tm_year + 1900 
			<< " " 
			<< timeinfo.tm_hour 
			<< "." 
			<< timeinfo.tm_min 
			<< "." 
			<< timeinfo.tm_sec;
		return s.str();
	}
	// fwd declaration required for LogView class
	void ResetErrorCount();
	void ResetWarningCount();
	void ResetStatusCount();
	void static Log(const std::string &_msg, MessageType _type, LOG_TYPE _log_type);
	/* LogView can be derived to push the log messages to custom viewer...*/
	class LogView {
		std::string m_username;
		std::string m_toolname;
		std::string m_starttime;
		std::string m_version;
		std::string m_platform;
		std::string m_path;
	public:
		LogView() {
			m_username = "";
			m_toolname = "";
			m_starttime = "";
			m_version = "";
			m_platform = "";
			m_path = "";
		}
		LogView(const std::string &_username,
			const std::string &_toolname,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) : 
			m_username(_username),m_toolname(_toolname), 
			m_starttime(_starttime),m_version(_version), 
			m_platform(_platform),m_path(_path) {

		}

		virtual void init(const std::string &_username,
			const std::string &_toolname,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) {
			m_username=_username;
			m_toolname=_toolname;
			m_starttime=_starttime;
			m_version=_version;
			m_platform = _platform;
			m_path = _path;
		}
		virtual void show_err(const std::string &_text, int _count) {
			std::cout << _text << std::endl;
		};
		virtual void show_warn(const std::string &_text, int _count) {
			std::cout << _text << std::endl;
		};
		virtual void show_status(const std::string &_text, int _count) {
			std::cout << _text << std::endl;
		};

		virtual void reset_error_count() {
			AceLogger::ResetErrorCount();
		}

		virtual void reset_warning_count() {
			AceLogger::ResetWarningCount();
		}

		virtual void reset_status_count() {
			AceLogger::ResetStatusCount();
		}

		void add_log_mark(const std::string &_marker) {
			std::string marker_msg = "\n----------------------------------------------------------------------------------------------------\n" + _marker;
			AceLogger::Log(marker_msg, MessageType::LOG_STATUS,LOG_TYPE::ALL);
		}

		void add_log_mark() {
			std::string marker_msg = "\n----------------------------------------------------------------------------------------------------";
			AceLogger::Log(marker_msg, MessageType::LOG_STATUS, LOG_TYPE::ALL);
		}

		std::string get_username() {
			return m_username;
		}

		std::string get_platform() { 
			return m_platform;
		}

		std::string get_version() {
			return m_version;
		}

		std::string get_toolname() {
			return m_toolname;
		}

		std::string get_starttime() {
			return m_starttime;
		}

		std::string get_path() {
			return m_path;
		
		}

		virtual void finish_log() {

		}

		virtual void flush() {
			std::flush(std::cout);
		};
		virtual ~LogView() {
		};
	};
	class LogViewFile:public LogView {
		std::ofstream ofsLog;
	public:
		LogViewFile() {
		}

		virtual void init(const std::string &_user,
			const std::string &_toolName,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) {

			LogView::init(
				_user,
				_toolName,
				_starttime,
				_version,
				_platform,
				_path);
			std::string startTime = time_stamp_file_name();
			std::string dir = _path + "\\";
			dir += _toolName;

			if (!boost::filesystem::exists(dir)) {
				if (!boost::filesystem::create_directory(dir)) {
					std::runtime_error("unable to create logging dir " + dir);
				}
			}

			dir = dir + "\\";
			dir += _user;
			boost::filesystem::path somepath(dir);
			if (!boost::filesystem::exists(somepath)) {
				if (!boost::filesystem::create_directory(dir)) {
					std::runtime_error("unable to create logging dir " + dir);
				}
			}

			dir += "\\";
			dir += startTime;
			dir += "_Log.log";
			ofsLog.open(dir.c_str());
			ofsLog << "Header size = 5\n" ;
			ofsLog << "Log version = 2.0\n" ;
			ofsLog << _version <<"\n";
			ofsLog << "start time " << _starttime << "\n";
		}
		virtual void show_err(const std::string &_text, int _count) {
			ofsLog << _text << std::endl;
		};

		virtual void show_warn(const std::string &_text, int _count) {
			ofsLog << _text << std::endl;
		};

		virtual void show_status(const std::string &_text, int _count) {
			ofsLog << _text << std::endl;
		};

		virtual void flush() {
			ofsLog.flush();
		};

		virtual void finish_log() {
			flush();
			ofsLog.close();
		}

		virtual ~LogViewFile() {
			if(ofsLog.is_open())
				ofsLog.close();
		};
	};
	
	class LogViewWeb :public LogView {

		std::atomic<bool> exist_response_reached;
		std::string m_web_session_id;
		web_client m_web_client;

		void emit(const std::string &_event,
			const message_list &_message) {
			get_socket()->emit(_event,
							_message);
		}

		void emit_session_details() {
			message_list session_info(get_username());
			session_info.push(str_message::create(get_toolname()));
			session_info.push(str_message::create(get_version()));
			session_info.push(str_message::create(get_platform()));
			session_info.push(str_message::create(get_starttime()));
			emit("session_info_details",
				session_info);
		};

		void on_session_info(web_event &_event) {
			emit_session_details();
			m_web_session_id = _event.get_message()->get_string();
		};

		auto get_socket()->decltype(m_web_client.socket()) {
			return m_web_client.socket("logs_channel");
		}

	public:
		LogViewWeb() {
			exist_response_reached = false;
			m_web_session_id = "";
		}

		virtual void init(const std::string &_user,
			const std::string &_toolName,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) {
			
			LogView::init(
				_user,
				_toolName,
				_starttime,
				_version,
				_platform,
				_path);

			m_web_client.connect("http://localhost:8080");
			emit_session_details();
			auto callback = std::bind(&AceLogger::LogViewWeb::on_session_info, this, std::placeholders::_1);
			web_event_listner func(callback);
			get_socket()->on("send_session_info", func);
		}

		virtual void show_err(const std::string &_text, int _count) {
			emit("log", message_list(_text));
		};

		virtual void show_warn(const std::string &_text, int _count) {
			emit("log", message_list(_text));
		};

		virtual void show_status(const std::string &_text, int _count) {
			emit("log", message_list(_text));
		};

		virtual void flush() {

		};

		std::string get_session_id() const{
			return m_web_session_id;
		}

		void on_exit_response_reached(web_event &_event) {
			exist_response_reached = true;
		}

		web_client& get_client() {
			return m_web_client;
		}
		
		auto get_socket_commands_channel()->decltype(m_web_client.socket()) {
			return m_web_client.socket("logs_channel");
		}

		virtual void finish_log() {
#ifdef ACE_LOGGER_ENABLE_WEB_SOCKETS
			flush();
			exist_response_reached = false;
			auto callback = std::bind(&AceLogger::LogViewWeb::on_exit_response_reached, this, std::placeholders::_1);
			web_event_listner func(callback);
			get_socket()->on("exit_reponse_reached", func);
			emit("closing", message_list(""));
			auto tick = boost::posix_time::microsec_clock::local_time();
			while (!exist_response_reached.load()) {
				auto tock = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration diff = tock - tick;
				if (diff.total_seconds()>2) {
					break;
				}
			}
			m_web_client.sync_close();
#endif
		}

		virtual ~LogViewWeb() {
			
		};
	};
	class Logger
	{
		LogView *m_log_view;
		LogView m_default_view;
		LogViewFile m_default_file_view;
		LogViewWeb  m_log_web;
		

		std::string m_toolname;
		std::string m_username;
		std::string m_version;
		std::string m_starttime;
		std::string m_log_dir;

		boost::mutex m_mutex;
		std::atomic<bool> m_log_closed;

		std::atomic<int> m_errorCount;
		std::atomic<int> m_warningCount;
		std::atomic<int> m_statusCount;

		std::atomic<size_t> m_pending_logs;

		MemoryCache<Message> m_message_cache;

		BlockingQueue<Message> m_logMsgBuffer;
	public:
		
		
		boost::thread_group m_loggingThread;
		static Logger*& GetInstance(){
			static Logger *instance = new Logger();
			return instance;
		};

		static void DeInit() {
			GetInstance()->finish_log();
			GetInstance()->m_loggingThread.join_all();
			if (GetInstance()->m_log_view != &GetInstance()->m_default_view)
				delete GetInstance()->m_log_view;
			GetInstance()->m_log_view = nullptr;
			delete GetInstance();
			GetInstance() = nullptr;
		}

		int get_error_count()const {
			return m_errorCount.load();
		}

		int get_warn_count()const {
			return m_warningCount.load();
		}

		int get_status_count()const {
			return m_statusCount.load();
		}

		void reset_error_count() {
			m_errorCount.store(0);
		}

		void reset_warn_count() {
			m_warningCount.store(0);
		}

		void reset_status_count() {
			m_statusCount.store(0);
		}

		void set_tool_name(const std::string &_toolname) {
			m_toolname = _toolname;
		}
		
		void set_version(const std::string &_version) {
			m_version = _version;
		}

		void set_log_dir(const std::string &_dir) {
			m_log_dir = _dir;
		}

		std::string get_toolname()const {
			return m_toolname;
		}

		std::string get_version()const {
			return m_version;
		}

		std::string get_username()const {
			return m_username;
		}

		std::string get_starttime()const {
			return m_starttime;
		}

		std::string get_log_dir()const {
			return m_log_dir;
		}

		std::string get_web_session_id()const {
			return m_log_web.get_session_id();
		}

		web_client& get_web_client() {
			return m_log_web.get_client();
		}

		auto get_web_commands_channel()->decltype(m_log_web.get_socket_commands_channel()) {
			return m_log_web.get_socket_commands_channel();
		}

		size_t get_pending_logs()const {
			return m_pending_logs;
		}

		void add_log(Message *_msg) {
			m_pending_logs.fetch_add(1);
			m_logMsgBuffer.Insert(_msg);
		}

		void init() {
			m_message_cache.fill_cache();
			m_loggingThread.create_thread(boost::bind(
							boost::mem_fn(&AceLogger::Logger::LogMessage_Internal), 
								this));

			std::string platform = "windows-x86";
#ifdef _WIN64
			platform = "windows-x64";
#endif 
#ifdef linux
			platform = "linux";
#endif
			std::cout << "initializing default log view " << std::endl;
			m_default_view.init(get_username(),
				get_toolname(),
				get_starttime(),
				get_version(),
				platform,
				get_log_dir());

			std::cout << "initializing file log view " << std::endl;
			m_default_file_view.init(get_username(),
				get_toolname(),
				get_starttime(),
				get_version(),
				platform,
				get_log_dir());

			std::cout << "initializing web log view " << std::endl;
			m_log_web.init(get_username(),
				get_toolname(),
				get_starttime(),
				get_version(),
				platform,
				get_log_dir());

			m_pending_logs.store(0);
		}
		
		void inline log_flush_internal(){
			if (m_log_closed)
				return;
			while (m_logMsgBuffer.size() > 0|| get_pending_logs()>0){
			}
			try{
				m_log_view->flush();
				m_default_file_view.flush();
				m_log_web.flush();
			}
			catch (...){
			}
		}

		void set_default_log_viewer() {
			set_log_viewer(nullptr);
		}

		void set_log_viewer(LogView *_viewer) {
			boost::unique_lock<boost::mutex> lock(m_mutex);
			if (_viewer != m_log_view) {
				if(m_log_view!=&m_default_view)
					delete m_log_view;
				m_log_view = _viewer;
			}
			if (!m_log_view)
				m_log_view = &m_default_view;
		}

		void finish_log() {

			boost::unique_lock<boost::mutex> lock(m_mutex);
			if (m_log_closed)
				return;
			m_logMsgBuffer.ShutDown();

			log_flush_internal();

			if (m_log_view && m_log_view != &m_default_view)
				m_log_view->finish_log();

			m_default_view.finish_log();

			m_default_file_view.finish_log();

			m_log_web.finish_log();
			m_log_closed = true;
		}

		Message * get_message_ptr() {
			return m_message_cache.get_mem();
		}
	private:
		~Logger(){
		};
		Logger(){
			m_log_closed = false;
			m_errorCount = 0;
			m_warningCount = 0;
			m_statusCount = 0;
			set_user_name();
			set_starttime();
			// default log view created
			m_log_view = &m_default_view;
		}
		Logger(const Logger&);
		Logger& operator=(Logger const &);
		std::string  GetTimeString()
		{
			std::time_t t = std::time(NULL);
			std::tm timeinfo = AceLogger::localtime(t);
			char current_time[27];
			current_time[26] = '\0';
			sprintf_s(current_time, 27, "[%02d-%02d-%04d %02d:%02d:%02d] ", 
				timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900,
				timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
			return std::string(current_time);
		}

		void set_starttime() {
			m_starttime = GetTimeString();
		}
		void set_user_name() {
			m_username = "User";
			std::string env_variable = "USERNAME";
#ifdef linux
			env_variable = "USER";
#endif
			auto username = std::getenv(env_variable.c_str());
			if (username) {
				m_username = std::string(username);
			}
		}
		void inline LogMessage_Internal() {
			Message *message = nullptr;
			while (m_logMsgBuffer.Remove(&message)) {
				if (!message) {
					m_pending_logs.fetch_sub(1);
					continue;
				}
				std::string timeStamp = GetTimeString();
				std::string msgType = "UNKNOWN";
				if ((message)->p_messageType == MessageType::LOG_STATUS) {
					msgType = "[STATUS]:\t";
					m_statusCount++;
				}
				else if ((message)->p_messageType == MessageType::LOG_ERROR) {
					msgType = "[ERROR]:\t";
					m_errorCount++;

				}
				else if ((message)->p_messageType == MessageType::LOG_WARNING) {
					msgType = "[WARNING]:\t";
					m_warningCount++;
				}
				try {
					timeStamp = timeStamp + " " + msgType + (message)->p_msg;
					if (message->p_log_type&LOG_TYPE::DISPLAY) {
						if ((message)->p_messageType == MessageType::LOG_STATUS) {
							m_log_view->show_status(timeStamp, m_statusCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_ERROR) {
							m_log_view->show_err(timeStamp, m_errorCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_WARNING) {
							m_log_view->show_warn(timeStamp, m_warningCount);
						}
					}
					if (message->p_log_type&LOG_TYPE::FILE) {

						if ((message)->p_messageType == MessageType::LOG_STATUS) {
							m_default_file_view.show_status(timeStamp, m_statusCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_ERROR) {
							m_default_file_view.show_err(timeStamp, m_errorCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_WARNING) {
							m_default_file_view.show_warn(timeStamp, m_warningCount);
						}
					}

					if (message->p_log_type&LOG_TYPE::WEB) {

						if ((message)->p_messageType == MessageType::LOG_STATUS) {
							m_log_web.show_status(timeStamp, m_statusCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_ERROR) {
							m_log_web.show_err(timeStamp, m_errorCount);
						}
						else if ((message)->p_messageType == MessageType::LOG_WARNING) {
							m_log_web.show_warn(timeStamp, m_warningCount);
						}
					}
				}
				catch (...) {
					throw std::runtime_error("logging system encountered runtime error");
				}
				m_message_cache.add_to_cache(message);
				m_pending_logs.fetch_sub(1);
			}
		};
		
	};
	
	void static Log(const std::string &_msg,
					MessageType _type=MessageType::LOG_STATUS,
					LOG_TYPE _log_type=LOG_TYPE::ALL){
		auto msg = Logger::GetInstance()->get_message_ptr();
		msg->p_msg = _msg;
		msg->p_log_type = _log_type;
		msg->p_messageType = _type;
		Logger::GetInstance()->add_log(msg);
	};

	void static LogErr(const std::string &_msg, 
					 LOG_TYPE _log_type = LOG_TYPE::ALL) {

		Log(_msg, MessageType::LOG_ERROR, _log_type);
	};

	void static LogWarn(const std::string &_msg, 
						LOG_TYPE _log_type = LOG_TYPE::ALL) {

		Log(_msg, MessageType::LOG_WARNING, _log_type);
	};

	void static log_flush(){
		Logger::GetInstance()->log_flush_internal();
	};

	void static Log_now(const std::string &_msg, 
						MessageType _type = MessageType::LOG_STATUS, 
						LOG_TYPE _log_type = LOG_TYPE::ALL) {
		Log(_msg, _type, _log_type);
		log_flush();
	}

	static web_client& get_web_client() {
		return Logger::GetInstance()->get_web_client();
	}

	static auto get_web_commands_channel()->decltype(Logger::GetInstance()->get_web_commands_channel()) {
		return Logger::GetInstance()->get_web_commands_channel();
	}
	
	int static GetErrorCount(){
		return Logger::GetInstance()->get_error_count();
	}

	int static GetWarningCount(){
		return Logger::GetInstance()->get_warn_count();
	}

	int static GetStatusCount(){
		return Logger::GetInstance()->get_status_count();
	}

	static std::string GetToolName() {
		return Logger::GetInstance()->get_toolname();
	}

	void static SetLogViewer(LogView *_viewer) {
		if (!_viewer)
			return;
		log_flush(); 
		Logger::GetInstance()->set_log_viewer(_viewer);
	}

	void static SetDefaultViewer() {
		
		Logger::GetInstance()->set_default_log_viewer();
	}

	void static ResetErrorCount() {
		Logger::GetInstance()->reset_error_count();
	}

	void static ResetStatusCount() {
		Logger::GetInstance()->reset_status_count();
	}

	void static ResetWarningCount() {
		Logger::GetInstance()->reset_warn_count();
	}

	void static ResetAllCounters(){
		Logger::GetInstance()->reset_error_count();
		Logger::GetInstance()->reset_status_count();
		Logger::GetInstance()->reset_warn_count();
	}

	void static FinishLog()
	{
		Logger::GetInstance()->finish_log();	
	}

	bool static is_log_file(const boost::filesystem::path &_path){
		if (!boost::filesystem::is_regular_file(_path))
			return false; // log file has to be a file
		if (!boost::iequals(_path.extension().string(), "log"))
			return false; // extension has to be log
		return true;
	}

	void static all_log_files(const std::string &_dir,
							const std::string &_toolName,
							std::vector<std::string> &_allLogFiles,
							bool _recursive){

		boost::filesystem::directory_iterator end_iter;
		std::regex e(_toolName);
		std::queue<boost::filesystem::path> dirs;
		dirs.push(boost::filesystem::path(_dir));
		while (!dirs.empty()){
			boost::filesystem::path p = dirs.front();
			dirs.pop();
			for (boost::filesystem::directory_iterator dir_iter(p); dir_iter != end_iter; ++dir_iter){
				if (std::regex_match(dir_iter->path().filename().string(), e)){
					if (is_log_file(*dir_iter)){
						_allLogFiles.push_back(dir_iter->path().string());
					}
					else if (_recursive){
						if (boost::filesystem::is_directory(*dir_iter)){
							dirs.push(*dir_iter);
						}
					}
				}
			}
		}	
	}

	bool static create_dir(const std::string &_path){
		if (!boost::filesystem::exists(_path))
		{
			return boost::filesystem::create_directory(_path);
		}
		return true;
	}
	
	void static RegisterLogger(const std::string _path,
								const std::string _toolName,
								const std::string _toolVersion)
	{
		static std::atomic<bool> log_started = false;
		if (log_started.load()) {
			std::runtime_error("logging system already registered");
		}
		log_started = true;
		Logger::GetInstance()->set_tool_name(_toolName);
		Logger::GetInstance()->set_version(_toolVersion);
		Logger::GetInstance()->set_log_dir(_path);
		Logger::GetInstance()->init();
	};

	class register_logger_helper{
		
	public:
		~register_logger_helper() {
			AceLogger::Logger::GetInstance()->DeInit();
		};
		register_logger_helper(const std::string &_dir_path,
			const std::string &_proj_name,
			const std::string &_version){

			AceLogger::RegisterLogger(_dir_path, _proj_name, _version);
			AceLogger::Log("Starting Application...");
		};
	};
	
};

#define REGISTER_LOGGER(log_dir,tool_name,version_info) \
		AceLogger::register_logger_helper logger_helper(log_dir, tool_name, version_info);

#ifdef EMIT_BACK_UP
#define emit 
#undef EMIT_BACK_UP
#endif