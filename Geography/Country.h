#pragma once
#include <string>
#include <unordered_set>

//				a class to represent a country
//				each country has a name, id and a small name
//				assumption: the application developer has the responsibility
//				to make sure the name is in lower case and has the 
//				leading the trailing spaces trimmed.
class Country
{
	std::string m_name;
	unsigned int m_id;
	std::string m_small_name;
public:
	Country();
	inline std::string name()const { return m_name; };

	inline std::string small_name()const { return m_small_name; };

	inline unsigned int id()const { return m_id; };

	inline void set_name(const std::string&_name){ m_name = _name; };

	inline void set_small_name(const std::string &_sm_name){ m_small_name = _sm_name; };

	inline void set_id(const unsigned int &_id){ m_id = _id; };

	inline bool operator<(const Country &_country)const { return name() < _country.name(); };

	inline bool operator==(const Country &_country)const { return  name() == _country.name(); };


	~Country();
};

struct CountryHashName{
	std::size_t operator()(const Country &_country)const{
		return std::hash<std::string>()(_country.name());
	};
};

struct CountryNameEqual{
	bool operator()(const Country &_a, const Country &_b)const{
		return _a.name() == _b.name();
	};
};

struct CountryHashId{
	std::size_t operator()(const Country &_country)const{
		return std::hash<unsigned int>()(_country.id());
	};
};

struct CountryIdEqual{
	bool operator()(const Country &_a, const Country &_b)const{
		return _a.id() == _b.id();
	};
};

struct CountryHashSmallName{
	std::size_t operator()(const Country &_country)const{
		return std::hash<std::string>()(_country.small_name());
	};
};

struct CountrySmallNameEqual{
	bool operator()(const Country &_a, const Country &_b)const{
		return _a.small_name() == _b.small_name();
	};
};



//			Collection of countries maintained by name , small name and id
//			assumption: number of countries ~200, hence keeping multiple copies
//			is ok.
class Countries{
	std::unordered_set<Country, CountryHashName, CountryNameEqual> m_countries_name;
	std::unordered_set<Country, CountryHashId,CountryIdEqual> m_countries_id;
	std::unordered_set<Country, CountryHashSmallName, CountrySmallNameEqual> m_countries_sm_name;

	unsigned int m_last_id;


	unsigned int add_country_internal(Country &_c);

	template<typename container,
		typename SetFunc,
		typename Key>
		Country find(const Key &_k,
					SetFunc &_func,
					const container&_container)const;

public:

	Countries();
	~Countries(){};
	unsigned int add_country(const std::string &_name,
					const std::string &_sm_name);

	Country find_by_id(const unsigned int &_id)const;

	Country find_by_name(const std::string &_name)const;

	Country find_by_small_name(const std::string &_sm_name)const;

	bool is_country(const std::string &_name)const;

	void print_all()const;


};