#include "Country.h"
#include <functional>
#include <Logger\Logger.h>
#include <StringUtils.h>
#include <boost\algorithm\string.hpp>


using namespace AceLogger;
Country::Country()
{
	m_name = "-NA-";
	m_id = 0;
	m_small_name = "-NA-";
}


Country::~Country()
{
}


Countries::Countries(){ 
	m_last_id = 0; 
	Country c; 
	c.set_id(m_last_id);
	add_country_internal(c);
};

unsigned int Countries::add_country_internal(Country &_c){
	auto iter = m_countries_name.find(_c);
	if (m_countries_name.find(_c) != m_countries_name.end())
		return (*iter).id();
	m_countries_id.insert(_c);
	m_countries_name.insert(_c);
	m_countries_sm_name.insert(_c);
	++m_last_id;
	return _c.id();
}

unsigned int Countries::add_country(const std::string &_name,
							const std::string &_sm_name){
	Country c;
	std::string name = _name;
	std::string sm_name = _sm_name;

	boost::to_lower(name);
	boost::trim(name);

	boost::to_lower(sm_name);
	boost::trim(sm_name);

	c.set_name(name);
	c.set_small_name(sm_name);
	c.set_id(m_last_id);
	return add_country_internal(c);


}

template<typename container,
	typename SetFunc,
	typename Key>
Country Countries::find(const Key &_k,
						SetFunc &_func,
						const container&_container)const{

	Country c;
	_func(c, _k);
	auto iter = _container.find(c);
	if (iter != _container.end())
		return *iter;
	return Country();
}

Country Countries::find_by_id(const unsigned int &_id)const{

	return find(_id, 
				[](Country &_c, const unsigned int &__id)->void{_c.set_id(__id); },
				m_countries_id);
}

Country Countries::find_by_name(const std::string &_name)const{
	return find(_name,
		[](Country &_c, const std::string &__name)->void{_c.set_name(__name); },
		m_countries_name);
}

Country Countries::find_by_small_name(const std::string &_sm_name)const{
	return find(_sm_name,
		[](Country &_c, const std::string &__sm_name)->void{_c.set_small_name(__sm_name); },
		m_countries_sm_name);
}

bool Countries::is_country(const std::string &_name)const{
	Country c;
	return !(c == find_by_name(_name));
}

void Countries::print_all()const{
	for (const auto &c : m_countries_name){
		Log(c.name() + "\t\t" + c.small_name() + "\t" + StringUtils::get_string(c.id()));
	}
}