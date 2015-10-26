#ifndef REFLEX3_H
#define REFLEX3_H
#include <map>
#include <memory>
#include <functional>
#include <vector>
#include <regex>
#include <boost\preprocessor.hpp>
#include <boost\function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>

#define ACE_REFLEX3_FUNCTION_ARG_TYPE(J,I,D) \
		BOOST_PP_COMMA_IF(BOOST_PP_SUB(BOOST_PP_INC(I),1)) BOOST_PP_CAT(BOOST_PP_CAT(reflex3::ArgType<__func_type ,BOOST_PP_CAT(BOOST_PP_COMMA(), BOOST_PP_SUB(BOOST_PP_INC(I),1))),>)

#define ACE_REFLEX3_FUNCTION_ARG_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_REFLEX3_FUNCTION_ARG_TYPE,BOOST_PP_EMPTY)

#define ACE_CLASS_FUNCTION_ARG_TYPE(J,I,D) \
		, BOOST_PP_CAT(BOOST_PP_CAT(reflex3::ArgType<__func_type ,BOOST_PP_CAT(BOOST_PP_COMMA(), BOOST_PP_INC(I))),>)

#define ACE_CLASS_FUNCTION_ARG_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_CLASS_FUNCTION_ARG_TYPE,BOOST_PP_EMPTY)

/*-----------------------------------------------
-------------------------------------------------
-------------------------------------------------
The code to generate parameter placeholders : std::placeholders::_1 and so forth, starts with comma always*/
#define ACE_GET_PARAMS_IMPL(J,I,D) \
		,BOOST_PP_CAT(std::placeholders::,BOOST_PP_CAT(_,BOOST_PP_INC(I)))

#define ACE_GET_PARAMS(ARITY) BOOST_PP_REPEAT(ARITY,ACE_GET_PARAMS_IMPL,BOOST_PP_EMPTY)
/*end of placeholder parameter generation*/

#define REGISTER(FUNC, NAME, ARITY) \
	{\
		typedef decltype(FUNC) __func_type; \
		using func = reflex3::Cb_t<ACE_REFLEX3_FUNCTION_ARG_TYPES(ARITY)>;\
		std::unique_ptr<func> f(new func(FUNC));\
		__scoped_callback.callBackMap.insert(reflex3::CallbackMap::value_type(NAME,std::move(f)));\
	}

#define REGISTER_CLASS_FUNCTION(FUNC,CLASS_NAME, NAME,ARITY) \
	{\
		typedef decltype(FUNC) __func_type;\
		typedef reflex3::Cb_t<CLASS_NAME* ACE_CLASS_FUNCTION_ARG_TYPES(ARITY)> func;\
		auto bindpbj = std::bind(FUNC ACE_GET_PARAMS(BOOST_PP_ADD(ARITY,1)));\
		std::unique_ptr<func> f(new func(bindpbj));\
		__scoped_callback.callBackMap.insert(reflex3::CallbackMap::value_type(NAME, std::move(f)));\
	}\

#define SCOPED_CALLBACK_OBJ __scoped_callback

#define ENABLE_REFLEX\
		reflex3::CallBack SCOPED_CALLBACK_OBJ;

#define RUN(NAME,...)\
	reflex3::run(SCOPED_CALLBACK_OBJ,NAME,__VA_ARGS__);

#define RUN_CLASS_MEMBER(NAME,CLASS_OBJ,...)\
	reflex3::run(SCOPED_CALLBACK_OBJ,NAME,CLASS_OBJ,__VA_ARGS__);

#define ADD_PARAM_DESC(NAME,PARAM) \
	__scoped_callback.add_param_desc(NAME,PARAM);

#define ADD_FUNC_DESC(NAME,DESC) \
	__scoped_callback.add_desc(NAME,DESC);

#define IS_CALLBACK(NAME)\
		__scoped_callback.is_registered_callback(NAME);

#define GET_CALLBACK_NAMES(NameVector)\
		NameVector = __scoped_callback.get_all_registered();

#define GET_CALLBACK_DESC(NAME,DESC)\
		DESC = __scoped_callback.get_description(NAME);

#define GET_CALLBACK_PARAMS(NAME,PARAMS_VECTOR)\
		PARAMS_VECTOR = __scoped_callback.get_params(NAME);

#define GET_CALLBACK_NAMES_LIKE(REGEX,NameVector)\
		NameVector = __scoped_callback.get_all_registered_like(REGEX);

namespace reflex3 {

	template <typename FuncType>
	using ResultType = typename boost::function_types::result_type<FuncType>::type;

	template <typename FuncType, size_t ArgIndex>
	using ArgType = typename boost::mpl::at_c<boost::function_types::parameter_types<FuncType>, ArgIndex>::type;

	// The base type that is stored in the collection.
	struct Func_t {
		virtual ~Func_t() = default;
	};
	// The map that stores the callbacks.
	using CallbackMap = std::map<std::string, std::unique_ptr<Func_t>>;
	using CallbackMapParam = std::map<std::string, std::vector<std::string>>;
	using CallbackMapDesc = std::map<std::string, std::string>;

	typedef struct CallBack {
		// The map that stores the callbacks.

		CallbackMap callBackMap;

		CallbackMapParam callback_map_params;

		CallbackMapDesc callback_map_desc;

		CallBack(CallBack&&_callback) {
			callBackMap = std::move(_callback.callBackMap);
		}

		CallBack& operator=(CallBack&&_callback) {
			callBackMap = std::move(_callback.callBackMap);
			return *this;
		}

		CallBack(const CallBack&) = delete;

		CallBack& operator=(const CallBack&) = delete;

		CallBack() {

		};

		void add_desc(const std::string &_name,
			const std::string &_desc) {
			auto iter = callBackMap.find(_name);
			if (iter == callBackMap.end())
				throw std::runtime_error("unable to add description to an unregistered function callback: " + _name);
			callback_map_desc.insert(std::make_pair(_name, _desc));
		};

		void add_param_desc(const std::string &_name,
			const std::string &_params) {
			auto iter = callBackMap.find(_name);
			if (iter == callBackMap.end())
				throw std::runtime_error("unable to add parameter description to an unregistered function callback: " + _name);
			callback_map_params[_name].push_back(_params);
		};

		bool is_registered_callback(const std::string &_name)const {
			auto iter = callBackMap.find(_name);
			return iter != callBackMap.end();
		}

		std::string get_description(const std::string &_name)const {
			auto iter = callBackMap.find(_name);
			if (iter == callBackMap.end())
				throw std::runtime_error("unable to get description for an unregistered function callback: " + _name);
			auto desc_iter = callback_map_desc.find(_name);
			if (desc_iter == callback_map_desc.end())
				return "no description available";
			return desc_iter->second;
		}

		std::vector<std::string> get_params(const std::string &_name)const {
			auto iter = callBackMap.find(_name);
			if (iter == callBackMap.end())
				throw std::runtime_error("unable to get parameters for an unregistered function callback: " + _name);
			auto desc_iter = callback_map_params.find(_name);
			if (desc_iter == callback_map_params.end())
				return std::vector<std::string>();
			return desc_iter->second;
		}

		std::vector<std::string> get_all_registered()const {
			std::vector<std::string> all_registered_callbacks;
			all_registered_callbacks.reserve(callBackMap.size());
			for (const auto & c : callBackMap) {
				all_registered_callbacks.push_back(c.first);
			}
			return all_registered_callbacks;
		}

		std::vector<std::string> get_all_registered_like(const std::string &_regex)const {
			std::vector<std::string> all_registered_callbacks;
			all_registered_callbacks.reserve(callBackMap.size());
			std::regex regex(_regex);
			for (const auto & c : callBackMap) {
				if (std::regex_match(c.first, regex))
					all_registered_callbacks.push_back(c.first);
			}
			return all_registered_callbacks;
		}

	}CallBack;
	// The derived type that represents a callback.
	template<typename ...A>
	struct Cb_t : public Func_t {
		using cb = std::function<void(A...)>;
		cb callback;
		Cb_t(cb p_callback) : callback(p_callback) { }
		~Cb_t() {

		}
	};

	template<typename ...Args>
	void run(CallBack &_scoped_callback, const std::string &_name, Args&&... args) {
		auto indexiter = _scoped_callback.callBackMap.find(_name);
		if (indexiter != _scoped_callback.callBackMap.end()) {
			reflex3::call(indexiter->second.get(), std::forward<Args>(args)...);
		}
		else
			throw std::runtime_error(" not found as a registered function name in reflex instance" + _name);
	}

	// Wrapper function to call the callback stored at the given index with the
	// passed argument.
	template<typename ...A>
	void call(const Func_t* _base, A&& ... args) {
		using func_t = Cb_t<A...>;
		using cb_t = std::function<void(A...)>;
		const Func_t& base = *_base;
		const cb_t& fun = static_cast<const func_t&>(base).callback;
		fun(std::forward<A>(args)...);
	}

} // end anonymous namespace
#endif