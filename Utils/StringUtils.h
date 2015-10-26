#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <string>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\algorithm\string.hpp>
#include <regex>
namespace StringUtils{

	template<typename T>
	inline bool is_type(const std::string &_str,
				T &_val){

		try
		{
			_val=boost::lexical_cast<T>(_str);
			return true;
		}
		catch (boost::bad_lexical_cast &)
		{
			return false;
		}

	};

	template<>
	inline bool is_type<bool>(const std::string &_str,
		bool &_val){
		int val;
		if (is_type<int>(_str, val)){
			if (val)
				_val = true;
			else
				_val = false;
			return true;
		}
		return false;
	}

	template<typename T>
	inline bool is_type(const std::string &_str){
		
		T val;
		return is_type<T>(_str,val);

	};

	template<typename T>
	inline std::string get_string(const T &_val){

		return boost::lexical_cast<std::string>(_val);
	};

	template<typename T>
	inline std::string ToString(const T &_val){
		return get_string(_val);
	};

	template<>
	inline std::string get_string<bool>(const bool &_val){
		if (_val)
			return "1";
		return "0";
	}

	template<typename T>
	inline std::wstring get_wstring(const T&_val){
		return boost::lexical_cast<std::wstring>(_val);
	};

	template<>
	inline std::wstring get_wstring<std::string>(const std::string &_str){
		return std::wstring(_str.begin(), _str.end());
	};

	inline void split(const std::string &_str,
		std::vector<std::string> &_tokens,
		const std::string &_delim){

		boost::split(_tokens, _str, boost::is_any_of(_delim.c_str()));

	}

	inline void split(const std::string &s, std::vector<std::string> &_tokens,const char delimiter)
	{
		size_t start = 0;
		size_t end = s.find_first_of(delimiter);

		while (end <= std::string::npos)
		{
			_tokens.emplace_back(s.substr(start, end - start));

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = s.find_first_of(delimiter, start);
		}
	}

	inline void split_by_substr( std::string _str, std::vector<std::string> &_tokens, const std::string& _substr) {

		while (true) {
			auto start = _str.find(_substr);
			if (std::string::npos != start) {
				_tokens.push_back(_str.substr(0, start ));
				_str = _str.substr(start + _substr.length() + 1, _str.length() - start - _substr.length());
			}
			else {
				_tokens.push_back(_str);
				return;
			}
		}
	}

	inline bool IsDouble(const std::string &_str){
		return is_type<double>(_str);
	}

	inline double ToDouble(const std::string &_str){
		double val;
		is_type<double>(_str, val);
		return val;
	}

	inline int ToInt(const std::string &_str){
		int val;
		is_type<int>(_str, val);
		return val;
	}

	inline bool DoesPatternMatch(const std::string &_str, std::string &_pattern, const std::string &_t1, bool _t2=true, bool _t3=true){
		return std::regex_match(_str, std::regex(_pattern));
	};

	inline std::string TrimLeadingAndTrailing(const std::string &_str){
		std::string str = _str;
		boost::trim(str);
		return str;
	}

	inline int CompareNoCase(const std::string &_str1, const std::string &_str2){
		return boost::iequals(_str1, _str2) ? 0 : 1;
	};
#ifdef MFC_INCLUDED
	static inline std::string get_string(const CString &_cstr){
		CT2CA pszConvertedAnsiString(_cstr);
		return std::string(pszConvertedAnsiString);
	}

	static inline CString get_cstring(const std::string &_str){
		CString ss;
		ss.Format(_T("%S"), _str.c_str());
		return ss;
	}
#endif
}


#endif