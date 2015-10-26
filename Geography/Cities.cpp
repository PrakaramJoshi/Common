#include "stdafx.h"
#include "Cities.h"
#include <boost\algorithm\string.hpp>
#include <Logger.h>
#include <StringUtils.h>
#include <fstream>
#include<chrono>
#include <thread>
#include <boost/filesystem.hpp>
using namespace AceLogger;
City::City(){
	m_name = "default_city";
	m_latitude = -1000;
	m_latitude = -1000;
}

 City::~City(){
	// TODO city ~ empty!
}

 City::City(const std::string &_name,
	 const float &_latitude,
	 const float &_longitude,
	 const unsigned int &_country_id){

	 set(_name,
		 _latitude,
		 _longitude,
		 _country_id);
 }

std::string City::name()const{
	return m_name;
}

void City::set(const std::string &_name,
	const float &_latitude,
	const float &_longitude,
	const unsigned int &_country_id){
	std::string name = _name;
	boost::trim(name);
	boost::to_lower(name);
	m_name = name;
	m_latitude = _latitude;
	m_longitude = _longitude;
	m_country_id = _country_id;

}
void City::set_name(const std::string &_name){
	std::string name = _name;
	boost::trim(name);
	boost::to_lower(name);
	m_name = name;
}

float City::longitude()const{
	return m_longitude;
}

float City::latitude()const{
	return m_latitude;
}

void City::set_country_id(const unsigned int &_id){
	m_country_id = _id;
}

unsigned int City::country_id()const{
	return m_country_id;
}

void City::set_latitude(const float &_latitude){
	m_latitude = _latitude;
}

void City::set_longitude(const float &_longitude){
	m_longitude = _longitude;
}

bool City::operator <(const City &_city)const{

	if(m_name < _city.name())
		return true;
	if (m_name>_city.name())
		return false;
	if (m_country_id < _city.country_id())
		return true;
	if (m_country_id>_city.country_id())
		return false;
	if (GeoGraphyUtils::less_than(longitude(), _city.longitude()))
		return true;
	if (GeoGraphyUtils::greater_than(longitude(), _city.longitude()))
		return false;
	if (GeoGraphyUtils::less_than(latitude(), _city.latitude()))
		return true;
	if (GeoGraphyUtils::greater_than(latitude(), _city.latitude()))
		return false;
	return false;

}

void City::print()const{
	Log(name() +",Country "+StringUtils::get_string(country_id())+ "," + StringUtils::get_string(m_latitude) + "," + StringUtils::get_string(m_longitude));
}

float City::distance(const float &_latitude,
					const float &_longitude)const{

	//haversine distance

	//-----------http://www.movable-type.co.uk/scripts/latlong.html
	float phi1 = GeoGraphyUtils::DTR(latitude());
	float phi2 = GeoGraphyUtils::DTR(_latitude);
	float delta_phi = phi2 - phi1;

	float lambda1 = GeoGraphyUtils::DTR(longitude());
	float lambda2 = GeoGraphyUtils::DTR(_longitude);
	float delta_lambda = lambda2 - lambda1;

	float a = (std::sin(delta_phi / 2.0)*std::sin(delta_phi / 2.0)) +
		std::cos(phi1)*std::cos(phi2)*std::sin(delta_lambda / 2.0)*std::sin(delta_lambda / 2.0);

	float c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

	float d = GeoGraphyUtils::earth_radius*c;

	return d;
}

float City::distance(const City &_city)const{

	return distance(_city.latitude(), _city.longitude());

}

Cities::Cities()
{
}


Cities::~Cities()
{
}

bool Cities::create_city(const std::string &_str,
						Countries &_countries,
						City &_city){

	auto country_sm_name = 0;
	auto city_name = 1;
	auto latitude = 5;
	auto longitude = 6;
	std::vector<std::string> tokens;
	StringUtils::split(_str, tokens, ',');
	if (tokens.size() >= (longitude + 1)){
		auto country_id = _countries.add_country(tokens[country_sm_name], tokens[country_sm_name]);
		_city.set(tokens[city_name],
			boost::lexical_cast<float>(tokens[latitude]),
			boost::lexical_cast<float>(tokens[longitude]),
			country_id);
		return true;
	}
	return false;
}
void Cities::CreateCities(std::set<City> &_cities,
	Countries &_countries,
	BlockingQueue<std::string> &_q,
	unsigned long &_read_cities,
	unsigned long &_collisions,
	const unsigned int &_read_count){

	std::string *data;
	_read_cities = 0;
	_collisions = 0;
	City city;
	while (_q.Remove(&data)){
		if (data){

			bool load_city = true;
			if (_read_count > 0){
				load_city = _read_count > _read_cities;
			}
			if (!(*data).empty() && load_city){
				if (create_city(*data, _countries, city)){
					_read_cities++;
					if (_read_cities % 50000 == 0){
						Log("read " + StringUtils::get_string(_read_cities) + "...");
					}
					_cities.insert(city);
					/*auto iter = _cities.find(city);
					if (iter != _cities.end())
					_collisions++;
					else*/
				}
			}
			delete data;
		}
	}
}
bool Cities::read_cities_dataset(const std::string &_data_file,
								const unsigned int &_read_count){
	std::ifstream ifs(_data_file);
	if (ifs.is_open()){
		Log("reading the cities data file...");
		std::string tmp;
		BlockingQueue<std::string> read_q;
		std::thread read_thread(&Cities::CreateCities, this, std::ref(m_cities),std::ref(m_countries), std::ref(read_q), std::ref(m_read_cities), std::ref(m_collisions),std::ref(_read_count));

		auto t_start = std::chrono::high_resolution_clock::now();
		std::getline(ifs, tmp);
		while (std::getline(ifs, tmp)){
			read_q.Insert(new std::string(tmp));
		}
		ifs.close();
		auto t_end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		auto size_read = boost::filesystem::file_size(boost::filesystem::path(_data_file)) / (1024.0*1024.0);
		auto speed = size_read*1000.0 / duration;
		
		read_q.ShutDown();
		read_thread.join();

		t_end = std::chrono::high_resolution_clock::now();
		auto duration2 = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		auto pageRate = (m_read_cities*1000.0) / duration2;

		Log("File size(MB) :" + StringUtils::get_string(size_read));
		Log("File IO speed(MB/s) : " + StringUtils::get_string(speed));

		std::string msg = "Read speed : ";
		char rate[256];
		rate[255] = '\0';
		sprintf_s(rate, 255, " [LPS: %3.2f]", pageRate);
		Log(msg + StringUtils::get_string(m_read_cities) + std::string(rate));
		Log("Total city names read : " + StringUtils::get_string(m_read_cities));
		Log("Total city name collisions : " + StringUtils::get_string(m_collisions));
		auto overhead = (duration2 - duration);
		Log("data creation overhead(ms) : " + StringUtils::get_string(overhead));
		return true;
	}
	else
		Log("unable to open the cities data file :" + _data_file, LOG_ERROR);
	return false;
}

bool Cities::save_to_file(const std::string &_file_path)const{
	std::ofstream ofs(_file_path);
	if (ofs.is_open()){
		Log("saving the city database file at : " + _file_path);
		ofs << ",CityName,,,,Latitude,Longitude" << std::endl;
		for (auto iter = m_cities.begin(); iter != m_cities.end(); ++iter){
			ofs << "," << (*iter).name() << ",,,," << (*iter).latitude() << "," << (*iter).longitude() << std::endl;
		}
		ofs.close();
		return true;
	}
	Log("unable to save the city database file at : "+_file_path, LOG_ERROR);
	return false;
}
unsigned long Cities::get_collisions()const{
	return m_collisions;
}

unsigned long Cities::get_read_cities()const{
	return m_read_cities;
}

const std::set<City> & Cities::cities()const{
	return m_cities;
}

void Cities::cities(std::vector<City> &_cities)const{

	_cities.reserve(_cities.size() + m_cities.size() + 1);
	for (auto iter = m_cities.begin(); iter != m_cities.end(); ++iter){
		_cities.push_back((*iter));
	}

}

std::map<City, unsigned long> Cities::get_random(float _count_percent,
	unsigned int &_max_hit,
	unsigned int &_min_hit,
	bool _silent)const{
	_count_percent = std::max(0.0, (double)_count_percent);
	_count_percent = std::min(1.0, (double)_count_percent);
	auto max_allowed = m_cities.size()*_count_percent;
	_max_hit = std::numeric_limits<unsigned int>::min();
	_min_hit = std::numeric_limits<unsigned int>::max();
	if (!_silent)
		Log("Generating random city list of size :" + StringUtils::get_string(max_allowed));

	std::map<unsigned int,unsigned int> index_selected;
	if (!_silent)
		Log("Generating random indicies...");
	while (index_selected.size() < max_allowed){
		auto val = rand() * 10000 % m_cities.size();

		auto iter = index_selected.find(val);
		
		if (iter == index_selected.end()){
			auto init_val = rand() % 50;
			index_selected[val] = init_val+1;
		}
		else
			(*iter).second = (*iter).second + 1;
	}
	unsigned int count = 0;
	if (!_silent)
		Log("Getting city names...");
	std::map<City, unsigned long> selected_cities;
	for (auto iter = m_cities.begin(); iter != m_cities.end(); ++iter){
		auto selected_iter = index_selected.find(count);
		if (selected_iter != index_selected.end()){
			for (auto add = 0; add < selected_iter->second; ++add){
				auto find_iter = selected_cities.find((*iter));
				if (find_iter == selected_cities.end()){
					selected_cities[(*iter)] = 1;
					if (_max_hit <1)
						_max_hit = 1;
					if (_min_hit >1)
						_min_hit = 1;
				}
				else{
					(*find_iter).second = (*find_iter).second + 1;
					if (_max_hit <(*find_iter).second)
						_max_hit = (*find_iter).second;
					if (_min_hit > (*find_iter).second)
						_min_hit = (*find_iter).second;
				}
			}
			
		}
		count++;
	}
	if (_max_hit < _min_hit)
		_max_hit = _min_hit = 0;
	return selected_cities;

}


void Cities::print_cities()const{
	for (auto x : m_cities)
		x.print();
}

std::string Cities::country_name(const City &_city)const{
	return m_countries.find_by_id(_city.country_id()).name();
}

unsigned int Cities::country_id(const std::string &_name)const{
	return m_countries.find_by_name(_name).id();
}

Countries & Cities::countries(){
	return m_countries;

}

void Cities::generate_white_city_database(const std::string &_white_city_list,
	const std::string &_output_file)const{
	std::ifstream ifs_allowed(_white_city_list);
	std::string temp;
	std::set<std::string> allowed_cities;
	while (std::getline(ifs_allowed, temp)){
		boost::trim(temp);
		boost::to_lower(temp);
		allowed_cities.insert(temp);
	}
	ifs_allowed.close();
	std::vector<City> all_cities;
	cities(all_cities);
	std::sort(all_cities.begin(), all_cities.end(), [](const City &_a, const City &_b){
		return _a.name() < _b.name();
	});
	std::ofstream ofs(_output_file);
	auto previousCity = City();
	bool isduplicate_printed = false;
	for (auto iter = all_cities.begin(); iter != all_cities.end(); ++iter){
		if (allowed_cities.find((*iter).name()) != allowed_cities.end()){
			if (boost::iequals((*iter).name(), previousCity.name())){
				if (isduplicate_printed == false){
					ofs << "----Duplicate----" << std::endl;
					isduplicate_printed = true;
				}
			}
			else
				isduplicate_printed = false;
			previousCity = *iter;
			ofs << country_name(*iter) << (*iter);
		}
	}
	ofs.close();
}

void SearchCities::set_cities(std::vector<City> &_cities){
	std::swap(m_cities, _cities);
}

City SearchCities::FindClosest(const float &_latitude,
	const float &_longitude)const{

	auto current_min = std::numeric_limits<double>::max();
	City closet_city;
	for (auto iter = m_cities.begin(); iter != m_cities.end(); ++iter){
		auto current_distance = (*iter).distance(_latitude, _longitude);
		if (current_distance < current_min){
			current_min = current_distance;
			closet_city = *iter;
			if (current_min == 0)
				break;
		}
	}
	return closet_city;
}

void SearchCityNames::tokenize_and_add(const std::string &_name,
	City &_city,
	std::set<city_name_token> &_container){
	std::vector<std::string> tokens;
	StringUtils::split(_name, tokens, ' ');
	if (tokens.size() > 0){
		city_name_token city_name_token(tokens[0]);
		auto iter = _container.find(city_name_token);
		if (iter == _container.end()){
			_container.insert(city_name_token);
			iter = _container.find(city_name_token);
		}
		auto next_token_iter = tokens.begin();
		next_token_iter++;
		struct city_name_token *city_token = const_cast<struct city_name_token*>(&(*iter));

		if (next_token_iter == tokens.end()){
			struct city_name_token t("");
			auto eoc_iter = city_token->p_next_tokens.find(t);
			if (eoc_iter == city_token->p_next_tokens.end()){
				t.p_cities.push_back(_city);
				city_token->p_next_tokens.insert(t);
			}
			else{
				const_cast<struct city_name_token*>(&(*eoc_iter))->p_cities.push_back(_city);
			}

			
		}
		else{
			std::string sub_str = *next_token_iter;
			++next_token_iter;
			for (; next_token_iter != tokens.end(); ++next_token_iter){
				sub_str = sub_str + " " + (*next_token_iter);
			}
			tokenize_and_add(sub_str, _city, city_token->p_next_tokens);
		}
		

	}
}

void SearchCityNames::add_cities(Countries &_countries,
								const std::vector<City> &_cities_all,
								const std::string &_allowed_cities_file,
								const std::string &_false_positives_file){
	std::ifstream ifs_allowed(_allowed_cities_file);
	std::string temp;
	City city;
	while (std::getline(ifs_allowed, temp)){
		boost::trim(temp);
		boost::to_lower(temp);
		if (temp != "----Duplicate----"){
			if (Cities::create_city(temp, _countries, city)){
				m_allowed_cities.insert(city);
			}
		}
		
	}
	ifs_allowed.close();
	std::for_each(_cities_all.begin(), _cities_all.end(), [this](City _city){
		std::string name = _city.name();
		if (this->m_allowed_cities.find(_city) != this->m_allowed_cities.end() || this->m_allowed_cities.empty())
			tokenize_and_add(name,_city, this->m_all_first_level_tokens);
	});

	std::ifstream ifs(_false_positives_file);

	while (std::getline(ifs, temp)){
		boost::trim(temp);
		boost::to_lower(temp);
		m_false_positives.insert(temp);
	}
	ifs.close();
}

void SearchCityNames::find_from_token(std::vector<std::string>::iterator _iter,
									std::vector<std::string>::iterator _end,
									 const std::string &_current_name,
									const std::set<city_name_token> &_container,
									std::vector<City>&_found_vals,
									unsigned int &_city_name_size)const{
	if (_iter == _end)
		return;
	city_name_token city_token(*_iter);
	city_name_token eocity_name("");
	auto iter = _container.find(city_token);
	
	if (iter != _container.end()){
		auto eocity_iter = (*iter).p_next_tokens.find(eocity_name);
		if (eocity_iter != (*iter).p_next_tokens.end()){
			std::copy(eocity_iter->p_cities.begin(), eocity_iter->p_cities.end(), std::back_inserter(_found_vals));
		}
		else
			_city_name_size++;
		++_iter;
		if (_current_name.empty())
			return find_from_token(_iter, _end, (*iter).p_token, (*iter).p_next_tokens, _found_vals, _city_name_size);
		else
			return find_from_token(_iter, _end, _current_name + " " + (*iter).p_token, (*iter).p_next_tokens, _found_vals, _city_name_size);

	}

}


void SearchCityNames::find(std::map<City,unsigned long> &_found_cities,
							unsigned int &_max_hit,
							unsigned int &_min_hit,
						  std::string _text)const{
	boost::trim(_text);
	boost::to_lower(_text);
	std::vector<std::string> tokens;
	StringUtils::split(_text, tokens, ' ');
	_max_hit = std::numeric_limits<unsigned int>::min();
	_min_hit = std::numeric_limits<unsigned int>::max();
	for (auto iter = tokens.begin(); iter != tokens.end();){
		std::vector<City>found_vals;
		unsigned int max_city_name_size = 1;
		find_from_token(iter, tokens.end(), "", m_all_first_level_tokens, found_vals,max_city_name_size);
		iter = iter + max_city_name_size;
		for (auto v : found_vals){
			if (m_false_positives.find(v.name()) == m_false_positives.end()){
				auto inner_iter = _found_cities.find(v);
				if (inner_iter != _found_cities.end()){
					(*inner_iter).second = (*inner_iter).second + 1;
					if (_max_hit < (*inner_iter).second)
						_max_hit = (*inner_iter).second;
					if (_min_hit >(*inner_iter).second)
						_min_hit = (*inner_iter).second;
				}
				else{
					_found_cities[v] = 1;
					if (_max_hit <1)
						_max_hit = 1;
					if (_min_hit >1)
						_min_hit = 1;
				}
			}
			
		}
	}
	if (_max_hit < _min_hit)
		_max_hit = _min_hit = 0;
}