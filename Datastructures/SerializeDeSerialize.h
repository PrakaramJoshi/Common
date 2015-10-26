#pragma once
#define SSEDES
#ifdef SSEDES
#define CODESSEDES(X) X
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/level_enum.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <boost/asio.hpp>
#define SERIALIZE(X) friend class boost::serialization::access;\
	template<class Archive> void save(Archive& ar, const unsigned version) const{X}
#define DESERIALIZE(X) template<class Archive> void load(Archive& ar, const unsigned version) {X}\
	BOOST_SERIALIZATION_SPLIT_MEMBER();
#define SERIALIZEBASE {ar & boost::serialization::base_object<_Super>(*this);}
#define REGISTERCLASS(X) { ar.template register_type<X>();}
#define ABSTRACTCLASS(X) {BOOST_SERIALIZATION_ASSUME_ABSTRACT(X);}
#define DESERIALIZEBASE {ar & boost::serialization::base_object<_Super>(*this);}
#define SERIALIZEPRIMITIVEPOINTERPRESAVE(MYTYPE,TEMPVARP,SAVEVARP)	MYTYPE TEMPVARP;\
	if(SAVEVARP!=NULL)\
															{\
															TEMPVARP.isNull=false;\
															TEMPVARP.val =*SAVEVARP;\
															}\
															else\
															{\
															TEMPVARP.isNull=true;\
															}z

#define SERIALIZEPRIMITIVEPOINTERPOSTSAVE(TEMPVARP){}
#define DESERIALIZEPRIMITIVEPOINTERPRELOAD(MYTYPE,TEMPVARP) MYTYPE TEMPVARP;
#define DESERIALIZEPRIMITIVEPOINTERPOSTLOAD(PRIMITIVETYPE,TEMPVARP,LOADVARP) if(TEMPVARP.isNull==false)\
	LOADVARP=NULL;\
																			 else \
																			 { \
																			 LOADVARP= new PRIMITIVETYPE(TEMPVARP.val); \

#ifdef DEBUG_CG
#define DEBUG_STR \
	virtual std::string GetDebugStr(){ \
		std::stringstream str; \
		boost::archive::text_oarchive oa(str); \
		oa << this; \
		return str.str(); \
	} \
	virtual void *ReconstructMe(const std::string &_str){ \
		decltype(this) t=NULL; \
		std::stringstream str; \
		str<<_str; \
		boost::archive::text_iarchive iss(str); \
		iss >> t; \
		return (void*)t ; \
	} 

#endif
#endif
#ifndef SSEDES
#define CODESSEDES(X)
#define SERIALIZE(X)
#define DESERIALIZE(X)
#define SERIALIZEBASE
#define REGISTERCLASS(X)
#define ABSTRACTCLASS(X)
#define DESERIALIZEBASE
#define DEBUG_STR
#endif

namespace IO{
	struct chunky_data{
		char *p_data;
		unsigned int p_size;
		chunky_data(char *_data, unsigned int _size) :p_data(_data), p_size(_size){ ; };
	}typedef chunky_data;

	template<typename Data>
	void break_to_chunks(unsigned int _size,
		Data &_data,
		std::vector<chunky_data> &_chunks){
		std::stringstream sstr;
		boost::archive::text_oarchive oa(sstr);
		oa << _data;
		while (sstr){
			char *d = new char[_size];
			sstr.read(d, _size);
			_chunks.push_back(chunky_data(d, sstr.gcount()));
		}
	};

	template<typename Data>
	Data* reconstruct_from_chunks(std::vector<chunky_data> &_chunks){
		std::stringstream sstr;
		for (auto c : _chunks){
			sstr.write(c.p_data, c.p_size);
			delete[] c.p_data;
		}
		boost::archive::text_iarchive oa(sstr);
		Data data;
		oa >> data;
		return new Data(data);
	};
	template<typename Data>
	void get_char_array_from_data(Data &_data,
		std::vector<char> &_chunks){
		std::stringstream sstr;
		boost::archive::text_oarchive oa(sstr);
		oa << _data;
		std::string str = sstr.str();
		sstr.clear();

		std::copy(str.begin(), str.end(), std::back_inserter(_chunks));
	};

	template<typename Data>
	Data* get_data_from_char_array(std::vector<char> &_chunks){
		std::stringstream sstr;
		for (auto c : _chunks)
			sstr << c;
		boost::archive::text_iarchive oa(sstr);
		Data data;
		oa >> data;
		return new Data(data);
	}


	template<typename Data>
	void get_buffer_from_data(boost::asio::streambuf &_buf,
		Data &_data){
		std::ostream os(&_buf);
		boost::archive::text_oarchive oa(os);
		oa << _data;
	}

	template<typename Data>
	Data * get_data_from_buffer(boost::asio::streambuf &_buf){
		std::istream is(&_buf);
		boost::archive::text_iarchive ar(is);
		Data data;
		ar >> data;
		return new Data(data);
	}
}
