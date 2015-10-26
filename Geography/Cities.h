#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <ostream>
#include "Country.h"
#include <boost\algorithm\string.hpp>
template<typename T>
class BlockingQueue;
enum PROJECTION{
	MERCATOR, GALL_PETERS, AZIMUTHAL,
	SPHERICAL,
	TOTAL_PROJECTIONS
};
const static std::string projection_str[PROJECTION::TOTAL_PROJECTIONS + 1] = { "MERCATOR", "GALL_PETERS", "AZIMUTHAL",
"SPHERICAL",
"UNKNOWN_PROJECTION" };

PROJECTION inline static get_projection(const std::string &_str){

	for (auto i = 0; i < PROJECTION::TOTAL_PROJECTIONS; i++){
		if (boost::iequals(_str, projection_str[i]))
			return PROJECTION(i);
	}
	return PROJECTION::TOTAL_PROJECTIONS;
};
class City;
namespace GeoGraphyUtils{
	
	static const float earth_radius = 6371.0;
	static const double PI = 3.141592653589793238462643383279502884197169399375105820974944;

	static const double MAP_WIDTH_MERCATOR = 480;
	static const double MAP_HEIGHT_MERCATOR = MAP_WIDTH_MERCATOR;

	static const double MAP_WIDTH_GALL_PETERS = 14152.8035995 * 2; 
	static const double MAP_HEIGHT_GALL_PETERS = 9009.95460588 * 2;



	static const double MAP_WIDTH_AZIMUTHAL= 5;
	static const double MAP_HEIGHT_AZIMUTHAL = MAP_WIDTH_AZIMUTHAL;

	inline double DTR(const double &_degrees){
		return _degrees*PI / 180.0;
	};

	inline double RTD(const double &_radians){
		return _radians * 180.0 / PI;
	};

	static const double tolerance = 0.0001;

	template<typename FT>
	FT kahanVectorSummation(const std::vector<FT> &_v){
		FT sum = 0;
		FT runningCompenstaion = 0;
		for (typename std::vector<FT>::const_iterator v = _v.begin()
			; v != _v.end();
			++v){
			FT y = (*v) - runningCompenstaion;
			FT t = sum + y;
			runningCompenstaion = (t - sum) - y;
			sum = t;
		}
		return sum;
	};
	template<class FT>
	inline FT kahanSum(const FT &_v1, const FT &_v2){
		double t = _v1 + _v2;
		double s = t;
		t = t - _v1;
		t = t - _v2;
		s = s - t;
		return s;
	};
	inline bool equal(const double& _value1, const double& _value2)// const double& _error = CGConstants::GEO_ERR)
	{
		double diff = fabs(_value1 - _value2);
		return diff <= tolerance;
	}
	
	inline bool zero(const double& _value)
	{
		return equal(_value, 0);
	}

	inline bool less_than(const double& _value1, const double& _value2)
	{
		double diff = (_value1 - _value2);
		if (zero(diff))
			return false;
		return diff<0;
	}

	inline bool greater_than(const double& _value1, const double& _value2)
	{
		double diff = (_value1 - _value2);
		if (zero(diff))
			return false;
		return diff>0;
	}

	inline bool greater_than_equal(const double& _value1, const double& _value2)
	{
		double diff = (_value1 - _value2);
		if (zero(diff))
			return true;
		return diff>0;
	}
	
	//--------------- mercator projection
	namespace mercator{
		//------------inverse mercator conversion: 2D XY to latitude and longitude
		
		static inline float longitude(const float &_x){
			return static_cast<float>(_x / (MAP_WIDTH_MERCATOR / 360.0) - 180.0);
		};

		static inline float latitude(const float &_y){
			float mercN = static_cast<float>((((MAP_HEIGHT_MERCATOR / 2.0) - _y)*(2.0*PI) / MAP_WIDTH_MERCATOR));


			float latRad = static_cast<float>((std::atan(std::exp(mercN)) - (PI / 4.0))*2.0);

			return static_cast<float>(latRad*180.0 / PI);
		};

		static inline void coordinate(float &_latitude,
			float &_longitude,
			const float &_x,
			const float &_y){

			_longitude = longitude(_x);
			_latitude = latitude(_y);
		}
		//--------------converts the latitude and the longitude to mercator XY
		static inline float x(const float &_longitude){
			return static_cast<float>((_longitude + 180.0)*(MAP_WIDTH_MERCATOR / 360.0));
		};

		static inline float y(const float &_latitude){
			float latRad = static_cast<float>(_latitude*PI / 180.0);

			float mercN = static_cast<float>(std::log(std::tan((PI / 4) + (latRad / 2.0))));
			return static_cast<float>((MAP_HEIGHT_MERCATOR / 2.0) - (MAP_WIDTH_MERCATOR*mercN / (2.0*PI)));
		};

		static inline void xy(float &_x,
			float &_y,
			const float &_latitude,
			const float &_longitude){
			_y = y(_latitude);
			_x = x(_longitude);
		};

		// conversion of the mercator xy to -1 1 based XY
		static inline float x_glx(const float &_x){
			return static_cast<float>(((_x / MAP_WIDTH_MERCATOR) - 0.5)*2.0);
		};

		static inline float y_gly(const float &_y){
			return static_cast<float>(-((_y / MAP_HEIGHT_MERCATOR) - 0.5)*2.0);
		};

		static inline float gly_y(const float &_y){
			return static_cast<float>(((-_y / 2.0) + 0.5)*MAP_HEIGHT_MERCATOR);
		};

		static inline float glx_x(const float &_x){
			return static_cast<float>(((_x / 2.0) + 0.5)*MAP_WIDTH_MERCATOR);
		};
	};

	//------------Gall-Peters projection
	namespace gall_peters{
		//------------inverse gall peters conversion: 2D XY to latitude and longitude

		static const double max_x = 14152.8035995;
		static const double min_x = -max_x;
		// ------(6371*pi*180)/(180*sqrt(2))

		static const double max_y = 9009.95460588;
		static const double min_y = -max_y;


		static inline float longitude(const float &_x){
			return static_cast<float>((_x*180.0*std::sqrt(2.0)) / (earth_radius*PI)) ;
		};

		static inline float latitude(const float &_y){
			return static_cast<float>(RTD(_y / (earth_radius*std::sqrt(2.0))));
		};

		static inline void coordinate(float &_latitude,
			float &_longitude,
			const float &_x,
			const float &_y){

			_longitude = longitude(_x);
			_latitude = latitude(_y);
		}
		//--------------converts the latitude and the longitude to XY
		// -------------http://en.wikipedia.org/wiki/Gall%E2%80%93Peters_projection
		static inline float x(const float &_longitude){
			return static_cast<float>((earth_radius*PI*_longitude) / (180.0*std::sqrt(2.0)));
		};

		static inline float y(const float &_latitude){
			return static_cast<float>(earth_radius*std::sqrt(2.0)*std::sin(DTR(_latitude)));
		};

		static inline void xy(float &_x,
			float &_y,
			const float &_latitude,
			const float &_longitude){
			_y = y(_latitude);
			_x = x(_longitude);
		};

		// conversion of the mercator xy to -1 1 based XY
		static inline float x_glx(const float &_x){
			return static_cast<float>(((_x / MAP_WIDTH_GALL_PETERS))*2.0);
		};

		static inline float y_gly(const float &_y){
			return static_cast<float>(((_y / MAP_HEIGHT_GALL_PETERS))*2.0);
		};

		static inline float gly_y(const float &_y){
			return static_cast<float>(((_y / 2.0))*MAP_HEIGHT_GALL_PETERS);
		};

		static inline float glx_x(const float &_x){
			return static_cast<float>(((_x / 2.0))*MAP_WIDTH_GALL_PETERS);
		};
	};


	//------------Azimuthal Equidistant projection
	namespace azimuthal_equidistant{

		//-----------http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
		//------------inverse mercator conversion: 2D XY to latitude and longitude

		static const float phi1 = static_cast<float>(PI/2.0);
		static const float lambda0 = 0.0;

		static inline float c(const float &_phi,
			const float &_lambda){
			return static_cast<float>(std::acos((std::sin(phi1)*std::sin(_phi)) + (std::cos(phi1)*std::cos(_phi)*std::cos(_lambda - lambda0))));
		}

		static inline float kprime(const float &_phi,
									const float &_lambda){
			double c_val = c(_phi,_lambda);
			return static_cast<float>(c_val / std::sin(c_val));
		};	

		static inline float longitude(const float &_x,
									  const float &_y){
			float tan_inverse = 0;
			double c_val = std::sqrt((_x*_x) + (_y*_y));
			tan_inverse = static_cast<float>(std::atan2(_x*std::sin(c_val), 
							((c_val*std::cos(phi1)*std::cos(c_val)) - (_y*std::sin(phi1)*std::sin(c_val)))));
			return static_cast<float>(RTD(lambda0 + tan_inverse));

		};

		static inline float latitude(const float &_x,
									const float &_y){
			double c_val = std::sqrt((_x*_x) + (_y*_y));
			return static_cast<float>(RTD(std::asin((std::cos(c_val)*std::sin(phi1)) + (_y*std::sin(c_val)*std::cos(phi1) / c_val))));
		};

		static inline void coordinate(float &_latitude,
			float &_longitude,
			const float &_x,
			const float &_y){

			_longitude = longitude(_x,_y);
			_latitude = latitude(_x,_y);
		}
		//--------------converts the latitude and the longitude to  XY
		static inline float x(const float &_longitude,
							 const float &_latitude){
			float phi = static_cast<float>(DTR(_latitude));
			float lambda = static_cast<float>(DTR(_longitude));
			return static_cast<float>(kprime(phi, lambda)*std::cos(phi)*std::sin(lambda - lambda0));
		};

		static inline float y(const float &_longitude,
								const float &_latitude){
			float phi = static_cast<float>(DTR(_latitude));
			float lambda = static_cast<float>(DTR(_longitude));
			return static_cast<float>(kprime(phi, lambda)*((std::cos(phi1)*std::sin(phi)) - (std::sin(phi1)*std::cos(phi)*std::cos(lambda - lambda0))));
		};

		static inline void xy(float &_x,
			float &_y,
			const float &_latitude,
			const float &_longitude){
			_y = y(_longitude,_latitude);
			_x = x(_longitude,_longitude);
		};


		// conversion of the mercator xy to -1 1 based XY
		static inline float x_glx(const float &_x){
			return static_cast<float>(((_x / MAP_WIDTH_AZIMUTHAL))*2.0);
		};

		static inline float y_gly(const float &_y){
			return static_cast<float>(((_y / MAP_HEIGHT_AZIMUTHAL))*2.0);
		};

		static inline float gly_y(const float &_y){
			return static_cast<float>(((_y / 2.0))*MAP_HEIGHT_AZIMUTHAL);
		};

		static inline float glx_x(const float &_x){
			return static_cast<float>(((_x / 2.0))*MAP_WIDTH_AZIMUTHAL);
		};

	};


	//-----------spherical projection
	namespace spherical{
		static inline float longitude(const float &_x,
									 const float &_y){
			return  static_cast<float>(RTD(std::atan2(_y, _x)));
		};

		static inline float latitude(const float &_x,
									const float &_y){
			return static_cast<float>(RTD(std::acos(std::sqrt((_x*_x) + (_y*_y)))));
		};

		static inline void coordinate(float &_latitude,
			float &_longitude,
			const float &_x,
			const float &_y){
			_latitude = longitude(_x,_y);
			_longitude = latitude(_x, _y);
			
		}
		//--------------converts the latitude and the longitude to XY
		static inline float x(const float &_longitude,
								const float &_latitude){
			return static_cast<float>(std::cos(DTR(_latitude)) * std::cos(DTR(_longitude)));
		};

		static inline float y(const float &_longitude, 
								const float &_latitude){
			return   static_cast<float>(std::cos(DTR(_latitude)) * std::sin(DTR(_longitude)));
		};

		static inline float z(const float &_latitude){
			return  static_cast<float>(std::sin(DTR(_latitude)));
		}

		static inline void xyz(float &_x,
			float &_y,
			float &_z,
			const float &_latitude,
			const float &_longitude){
			_y = y(_longitude,_latitude);
			_x = x(_longitude, _latitude);
			_z = z(_latitude);
		};

		// conversion of the mercator xy to -1 1 based XY
		static inline float x_glx(const float &_x){
			return _x;
		};

		static inline float y_gly(const float &_y){
			return _y;
		};

		static inline float z_glz(const float &_z){
			return _z;
		}

		static inline float gly_y(const float &_y){
			return _y;
		};

		static inline float glx_x(const float &_x){
			return _x;
		};

		static inline float glz_z(const float &_z){
			return _z;
		}
	}


	inline std::wstring Bearing(const float &_latitude,
		const float &_longitude){
		wchar_t bearing[50];
		bearing[49] = L'\0';
		wchar_t ns = _latitude>=0? (L'N') : (L'S');
		wchar_t ew = (_longitude >= 0) ?( L'E') : (L'W');
		swprintf(bearing,50, L"%3.3f° %C %3.3f° %C ", _latitude, ns, _longitude, ew);
		return std::wstring(bearing);
	}
	
	


}
class City{
	std::string m_name;
	float m_latitude;
	float m_longitude;
	unsigned int m_country_id;
public:
	City();

	City(const std::string &_name,
		const float &_latitude,
		const float &_longitude,
		const unsigned int &_country_id);

	~City();

	std::string name()const;

	void set_name(const std::string &_name);

	float longitude()const; 

	float latitude()const;

	void set_latitude(const float &_latitude);

	void set_longitude(const float &_longitude);

	void set(const std::string &_name,
		const float &_latitude,
		const float &_longitude,
		const unsigned int &_id);

	void set_country_id(const unsigned int &_id);

	unsigned int country_id()const;

	bool operator <(const City &_city)const;

	void print()const;

	float distance(const float &_latitude,
		const float &_longitude)const;

	//haversine distance
	float distance(const City &_city)const;


};

inline std::ostream& operator<<(std::ostream& _os, const City &_city){
	_os << _city.name() << "," << _city.name() << ",,," << _city.latitude() << "," << _city.longitude() << std::endl;
	return _os;
};

struct SortByDistance{
	bool operator()(const City &_a, const City &_b)const {
		if (_a.latitude() < _b.latitude())
			return true;
		if (_a.latitude() == _b.latitude())
			return _a.longitude() < _b.longitude();
		return false;
	};
};

class SearchCities{

	std::vector<City> m_cities;
	
public:
	SearchCities(){};

	~SearchCities(){};

	void set_cities(std::vector<City> &_cities);

	City FindClosest(const float &_latitude,
					const float &_longitude)const;
};

struct city_name_token{
	std::string p_token;
	std::set<city_name_token>p_next_tokens;
	std::vector<City> p_cities;
	city_name_token(const std::string &_token) :p_token(_token){ ; };
	bool operator<(const city_name_token &_token)const{
		return p_token < _token.p_token;
	};
};

class SearchCityNames{
	std::set<city_name_token> m_all_first_level_tokens;
	std::set<std::string> m_false_positives;
	std::set<City> m_allowed_cities;

	void tokenize_and_add(const std::string &_name,
						City &_city,
						std::set<city_name_token> &_container);

	void find_from_token(std::vector<std::string>::iterator _iter,
		std::vector<std::string>::iterator _end,
		const std::string &_current_name,
		const std::set<city_name_token> &_container,
		std::vector<City>&_found_vals,
		unsigned int &_city_name_size)const;



public:
	void add_cities(Countries &_countries,
		const std::vector<City> &_cities_all,
		const std::string &_allowed_cities_file,
		const std::string &_false_positves_file);

	void find(std::map<City, unsigned long> &_found_cities,
		unsigned int &_max_hit,
		unsigned int &_min_hit,
		std::string _text)const;

};



class Cities
{
	std::set<City> m_cities;
	unsigned long m_read_cities;
	unsigned long m_collisions;
	Countries m_countries;

	void CreateCities(std::set<City> &_cities,
		Countries &_counties,
		BlockingQueue<std::string> &_q,
		unsigned long &_read_cities,
		unsigned long &_collisions,
		const unsigned int &_read_count);
public:
	Cities();
	~Cities();

	void print_cities()const;

	bool read_cities_dataset(const std::string &_data_file,
							 const unsigned int &_read_count);

	unsigned long get_collisions()const;

	unsigned long get_read_cities()const;

	std::map<City,unsigned long> get_random(float _count_percent,
											unsigned int &_max_hit,
											unsigned int &_min_hit,
											bool _silent=true)const;

	const std::set<City> & cities()const;

	void cities(std::vector<City> &_cities)const;

	bool save_to_file(const std::string &_file_path)const;

	std::string country_name(const City &_city)const;

	unsigned int country_id(const std::string &_name)const;

	Countries & countries();

	void generate_white_city_database(const std::string &_white_city_list,
		const std::string &_output_file)const;

	bool static create_city(const std::string &_str,
							Countries &_countries,
							City &_city);

};

