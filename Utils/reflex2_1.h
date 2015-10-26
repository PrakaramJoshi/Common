#pragma once
#include <set>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <functional>
#include <boost\bind.hpp>
#include <boost\preprocessor.hpp>
#include <boost\function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>

/* some useful macros used by the reflex2*/
#define ACE_REFLEX_SUPPORT_PARAMS 10
/*-----------------------------------------------
The code to generate parameter placeholders : std::placeholders::_1 and so forth, starts with comma always*/
#define ACE_GET_PARAMS_IMPL(J,I,D) \
		,BOOST_PP_CAT(std::placeholders::,BOOST_PP_CAT(_,BOOST_PP_INC(I)))

#define ACE_GET_PARAMS(ARITY) BOOST_PP_REPEAT(ARITY,ACE_GET_PARAMS_IMPL,BOOST_PP_EMPTY)
/*end of placeholder parameter generation*/

/*-------------------------------------------------
code to generate typename T1 and so forth, comma is added only between tokens.*/
#define ACE_GET_TYPES_IMPL(J,I,D) \
		BOOST_PP_COMMA_IF(BOOST_PP_SUB(BOOST_PP_INC(I),1)) BOOST_PP_CAT(typename T,BOOST_PP_INC(I))

#define ACE_GET_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_GET_TYPES_IMPL,BOOST_PP_EMPTY)
/*end of placeholder typname generation*/

/*---------------------------------------------------
code to generate function signatures for type manipulation, e.g.
template<typename T1>
auto FunctoType()->decltype(std::bind(_fn<T1>,std::placeholders::_1);*/
#define ACE_FUNCTION_TO_TYPE(J,I,D) \
		template<ACE_GET_TYPES(BOOST_PP_INC(I))> auto FunctoType()->decltype(std::bind(reflex2::_fn<ACE_GET_TYPES(BOOST_PP_INC(I))> ACE_GET_PARAMS(BOOST_PP_INC(I)))); \
		BOOST_PP_EMPTY()

#define ACE_FUNCTIONS_TO_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_FUNCTION_TO_TYPE,BOOST_PP_EMPTY)
/*end of function signature code generations*/

/*---------------------------------------------------
code to generate function signatures for type manipulation, e.g.
template<typename T1>
auto FunctoType()->decltype(std::bind(_fn<T1>,std::placeholders::_1);*/
#define ACE_CLASS_FUNCTION_TO_TYPE(J,I,D) \
		template<ACE_GET_TYPES(BOOST_PP_INC(I))> auto FunctoType()->decltype(std::bind(&D::_fn<ACE_GET_TYPES(BOOST_PP_INC(I))>,(&D()) ACE_GET_PARAMS(BOOST_PP_INC(I)))); \
		BOOST_PP_EMPTY()

#define ACE_CLASS_FUNCTIONS_TO_TYPES(ARITY,CLASS_NAME) BOOST_PP_REPEAT(ARITY,ACE_CLASS_FUNCTION_TO_TYPE,CLASS_NAME)
/*end of function signature code generations*/

/*-------------------------------------------------
code to generate Register class functions */
#define ACE_GENERATE_REGISTER_CLASS_FUNCTION(J,I,D) \
template<typename Function, typename CLASS_PTR, ACE_GET_TYPES(BOOST_PP_INC(I))> \
	void RegisterClassFunction(Function _func, CLASS_PTR _class_ptr, const std::string &_name, reflex2::reflex &_reflex_obj) { \
	auto functor = std::bind(_func, _class_ptr  ACE_GET_PARAMS(BOOST_PP_INC(I))); \
	std::cout<<typeid(CLASS_PTR).name()<<std::endl;\
	_reflex_obj.Add<decltype(functor), ACE_GET_TYPES(BOOST_PP_INC(I))>(functor, _name); \
};

#define ACE_GENERATE_REGISTER_CLASS_FUNCTIONS(ARITY) BOOST_PP_REPEAT(ARITY,ACE_GENERATE_REGISTER_CLASS_FUNCTION,BOOST_PP_EMPTY) \

/*end of placeholder typname generation*/

/*-------------------------------------------------
code to generate Register functions and then generate the function to type for each of the registe function*/
#define ACE_GENERATE_REGISTER_FUNCTION(J,I,D)\
template<typename Function, ACE_GET_TYPES(BOOST_PP_INC(I))> \
	void Register(Function _func, const std::string &_name, reflex2::reflex &_reflex_obj) { \
	auto functor = std::bind(_func  ACE_GET_PARAMS(BOOST_PP_INC(I)));\
	_reflex_obj.Add<decltype(functor), ACE_GET_TYPES(BOOST_PP_INC(I))>(functor, _name);\
};

#define ACE_GENERATE_REGISTER_FUNCTIONS(ARITY) BOOST_PP_REPEAT(ARITY,ACE_GENERATE_REGISTER_FUNCTION,BOOST_PP_EMPTY) \
BOOST_PP_EMPTY() \
ACE_GENERATE_REGISTER_CLASS_FUNCTIONS(ARITY) \
BOOST_PP_EMPTY() \
ACE_FUNCTIONS_TO_TYPES(ARITY)
/*end of placeholder typname generation*/

/* code enable registertion*/

#define ACE_FUNCTION_ARG_TYPE(J,I,D) \
		BOOST_PP_CAT(BOOST_PP_COMMA(),BOOST_PP_CAT(BOOST_PP_CAT(reflex2::ArgType<__func_type ,BOOST_PP_CAT(BOOST_PP_COMMA(), BOOST_PP_SUB(BOOST_PP_INC(I),1))),>))

#define ACE_FUNCTION_ARG_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_FUNCTION_ARG_TYPE,BOOST_PP_EMPTY)


#define ACE_CLASS_FUNCTION_ARG_TYPE(J,I,D) \
		BOOST_PP_CAT(BOOST_PP_COMMA(),BOOST_PP_CAT(BOOST_PP_CAT(reflex2::ArgType<__func_type ,BOOST_PP_CAT(BOOST_PP_COMMA(), BOOST_PP_INC(I))),>))

#define ACE_CLASS_FUNCTION_ARG_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_CLASS_FUNCTION_ARG_TYPE,BOOST_PP_EMPTY)

#define REGISTER(FUNCTION,NAME,ARITY) \
	{ \
	typedef decltype(FUNCTION) __func_type; \
	reflex2::Register<__func_type ACE_FUNCTION_ARG_TYPES(ARITY) >(FUNCTION,NAME,reflex_obj); \
	};

#define ACE_FUNCTION_ARG_TYPE(J,I,D) \
		BOOST_PP_CAT(BOOST_PP_COMMA(),BOOST_PP_CAT(BOOST_PP_CAT(reflex2::ArgType<__func_type ,BOOST_PP_CAT(BOOST_PP_COMMA(), BOOST_PP_SUB(BOOST_PP_INC(I),1))),>))

#define ACE_FUNCTION_ARG_TYPES(ARITY) BOOST_PP_REPEAT(ARITY,ACE_FUNCTION_ARG_TYPE,BOOST_PP_EMPTY)

#define REGISTER_CLASS_FUNCTION(FUNCTION,OBJ_PTR,NAME,ARITY) \
	{ \
	typedef decltype(FUNCTION) __class_func_type; \
	typedef decltype(OBJ_PTR) __CLASS_PTR; \
	typedef reflex2::get_Types<__class_func_type>::type::type __func_type; \
	reflex2::RegisterClassFunction<__class_func_type,__CLASS_PTR ACE_CLASS_FUNCTION_ARG_TYPES(ARITY) >(FUNCTION,OBJ_PTR,NAME,reflex_obj); \
	};
/*end of code enabling registration*/

#define REGISTER_CLASS(CLASS_NAME) \
	template<typename ...Args> \
	void _fn(Args ...args) { \
	} \
	ACE_CLASS_FUNCTIONS_TO_TYPES(ACE_REFLEX_SUPPORT_PARAMS,CLASS_NAME)

#define RUN(...)\
	reflex_obj.Run(__VA_ARGS__);

#define RUN_CLASS_FUNCTION(...)\
	reflex_obj.RunMemberFunction(__VA_ARGS__);

#define ENABLE_REFLEX \
reflex2::reflex reflex_obj;

namespace reflex2 {
	template<typename CallBack, typename... Args>
	struct CallBackMap;
	struct reflex;
	typedef void*(reflex::*GetMapFunc)();
	/* --------------------------------------------------------
	code to determine the argument list type names*/
	template <typename... Args>
	struct Impl;

	template <>
	struct Impl<>
	{
		static std::string name() {
			return "";
		}
	};

	template <typename First, typename... Args>
	struct Impl<First, Args...>
	{
		static std::string name() {
			return std::string(typeid(First).name()) + " " + Impl<Args...>::name();
		}
	};

	template <typename... Args>
	std::string type_name() {
		return Impl<Args...>::name();
	}
	/*end of code to determine the type names for argument list*/

	/* code to determine the type name for argument list for member function*/
	template <typename... Args>
	struct ImplTypes;

	template <typename First, typename... Args>
	struct ImplTypes<First, Args...>
	{
		typedef First type;
	};

	template <>
	struct ImplTypes<>
	{
		typedef void type;
	};

	template <typename... Args>
	struct get_Types {
		typedef decltype(ImplTypes<Args...>()) type;
	};

	/* end of code to determine the type name for argument list for member function*/

	template <typename FuncType>
	using ResultType = typename boost::function_types::result_type<FuncType>::type;

	template <typename FuncType, size_t ArgIndex>
	using ArgType = typename boost::mpl::at_c<boost::function_types::parameter_types<FuncType>, ArgIndex>::type;

	/* section of file used to store function signatures....*/
	template<typename ...Args>
	void _fn(Args ...args) {
	}

	auto FunctoType()->decltype(std::bind(_fn<>));

	template<typename Function>
	void Register(Function _func, const std::string &_name, reflex2::reflex &_reflex_obj) {
		auto functor = std::bind(_func);
		_reflex_obj.Add<decltype(functor)>(functor, _name);
	}

	template<typename Function, typename CLASS_PTR>
	void RegisterClassFunction(Function _func, CLASS_PTR _class_ptr, const std::string &_name, reflex2::reflex &_reflex_obj) {
		auto functor = std::bind(_func, _class_ptr);
		_reflex_obj.Add<decltype(functor)>(functor, _name);
	}


	ACE_GENERATE_REGISTER_FUNCTIONS(ACE_REFLEX_SUPPORT_PARAMS)

	/* end of section of file to store function signatures*/

	template<typename CallBack, typename... Args>
	struct CallBackMap {
		std::map<std::string, CallBack> p_map;
		const std::string typestr;
		void insert(const std::string &_name, CallBack _callback) {
			p_map.insert(std::make_pair(_name, _callback));
		};

		CallBackMap(GetMapFunc _func, reflex&_reflex) :typestr(reflex2::type_name<Args...>()) {
			_reflex.allGetMaps.push_back(_func);
			//std::cout << "created new CallBackMap type: " << " " << typeid(CallBack).name() << std::endl;
		}
		~CallBackMap() {
		}
	};

	struct reflex {
		std::vector<GetMapFunc> allGetMaps;
		std::set<std::string> funcNames;
		template<typename CallBack, typename... Args>
		void Add(CallBack _callBack, const std::string &_name) {
			if (funcNames.find(_name) == funcNames.end()) {
				funcNames.insert(_name);
				static_cast<CallBackMap<CallBack, Args...>*>(reflex2::reflex::GetMap<CallBack, Args...>())->insert(_name, _callBack);
			}
			else
				throw std::runtime_error("Cannot Register two functions with the same name within the same scope, Duplicate name : " + _name);
		}

		template<typename CallBack, typename... Args>
		void* GetMap() {
			static CallBackMap<CallBack, Args...> callbacks(&reflex2::reflex::GetMap<CallBack, Args...>, *this);
			return &callbacks;
		}

		template<typename CallBackType>
		void Run0ParamFunction(const std::string &_name) {
			typedef CallBackMap<CallBackType> CallBackMap;
			auto param_type = reflex2::type_name();
			for (auto i = 0; i < allGetMaps.size(); ++i) {
				auto c = allGetMaps[i];
				auto map = static_cast<CallBackMap*>((this->*c)());
				if (map->typestr != param_type)
					continue;
				auto &nameCallBackMap = map->p_map;
				auto iter = nameCallBackMap.find(_name);
				if (iter != nameCallBackMap.end()) {
					std::cout << "Found :\t" << _name << std::endl;
					(*iter).second();
					return;
				}
			}
			std::cout << "Not found :\t" << _name << std::endl;
		}

		template<typename CallBackType,typename...Args>
		void RunNParamFunction(const std::string &_name, Args ...args) {
			typedef CallBackMap<CallBackType, Args...> CallBackMap;
			auto param_type = reflex2::type_name<Args...>();
			for (auto i = 0; i < allGetMaps.size(); ++i) {
				auto c = allGetMaps[i];
				auto map = static_cast<CallBackMap*>((this->*c)());
				if (map->typestr != param_type)
					continue;
				auto &nameCallBackMap = map->p_map;
				auto iter = nameCallBackMap.find(_name);
				if (iter != nameCallBackMap.end()) {
					std::cout << "Found :\t" << _name << std::endl;
					(*iter).second(args...);
					return;
				}
			}
			std::cout << "Not found :\t" << _name << std::endl;
		}

		template<typename... Args>
		void Run(const std::string &_name, Args ...args) {
			typedef decltype(reflex2::FunctoType<Args...>()) CallBackType;
			RunNParamFunction < CallBackType, Args...>(_name, args...);
		}

		template<typename CallBackType,typename ...Args>
		void RunClassFunction(const std::string &_name, Args ... args) {
			RunNParamFunction < CallBackType, Args...>(_name, args...);
		}

		template<typename Class,typename T1>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 args) {
			typedef decltype(_obj.FunctoType<T1>()) CallBackType;
			RunClassFunction<CallBackType, T1>(_name, args);
		}

		template<typename Class, typename T1,typename T2>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2) {
			typedef decltype(_obj.FunctoType<T1,T2>()) CallBackType;
			RunClassFunction<CallBackType, T1,T2>(_name, _v1,_v2);
		}

		template<typename Class, typename T1, typename T2,typename T3>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3) {
			typedef decltype(_obj.FunctoType<T1, T2,T3>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2,T3>(_name, _v1, _v2, _v3);
		}

		template<typename Class, typename T1, typename T2, typename T3,typename T4>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3,T4 _v4) {
			typedef decltype(_obj.FunctoType<T1, T2, T3,T4>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3,T4>(_name, _v1, _v2, _v3,_v4);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4,typename T5>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4,T5 _v5) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4,T5>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3,T4,T5>(_name, _v1, _v2, _v3, _v4,_v5);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4, typename T5,typename T6>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4, T5 _v5,T6 _v6) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4,T5,T6>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3, T4, T5,T6>(_name, _v1, _v2, _v3, _v4, _v5,_v6);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4, T5 _v5, T6 _v6, T7 _v7) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4, T5, T6,T7>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3, T4, T5, T6,T7>(_name, _v1, _v2, _v3, _v4, _v5, _v6,_v7);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,typename T8>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4, T5 _v5, T6 _v6, T7 _v7,T8 _v8) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4, T5, T6, T7,T8>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3, T4, T5, T6, T7,T8>(_name, _v1, _v2, _v3, _v4, _v5, _v6, _v7,_v8);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8,typename T9>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4, T5 _v5, T6 _v6, T7 _v7, T8 _v8,T9 _v9) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4, T5, T6, T7, T8,T9>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3, T4, T5, T6, T7, T8,T9>(_name, _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8,_v9);
		}

		template<typename Class, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9,typename T10>
		void RunMemberFunction(Class &_obj, const std::string &_name, T1 _v1, T2 _v2, T3 _v3, T4 _v4, T5 _v5, T6 _v6, T7 _v7, T8 _v8, T9 _v9,T10 _v10) {
			typedef decltype(_obj.FunctoType<T1, T2, T3, T4, T5, T6, T7, T8, T9,T10>()) CallBackType;
			RunClassFunction<CallBackType, T1, T2, T3, T4, T5, T6, T7, T8, T9,T10>(_name, _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9,_v10);
		}

		template<typename Class>
		void RunMemberFunction(Class &_obj, const std::string &_name) {
			typedef decltype(_obj.FunctoType()) CallBackType;
			Run0ParamFunction<CallBackType>(_name);
		}

		void Run(const std::string &_name) {
			typedef decltype(reflex2::FunctoType()) CallBackType;
			Run0ParamFunction<CallBackType>(_name);
		}
	};
}
