#ifndef CG_CONSTANTS_3D_H
#define CG_CONSTANTS_3D_H
#include <boost\algorithm\string.hpp>
namespace CG3DContants{
	enum OCTANTS{
		PLUS_PLUS_PLUS = 1, MINUS_PLUS_PLUS = 2, MINUS_MINUS_PLUS = 4, PLUS_MINUS_PLUS = 8,
		PLUS_PLUS_MINUS = 16, MINUS_PLUS_MINUS = 32, MINUS_MINUS_MINUS = 64, PLUS_MINUS_MINUS = 128,
		POSITIVE_Z = 15, NEGATIVE_Z = 240,
		POSITIVE_X = 153, NEGATIVE_X = 102,
		POSITIVE_Y = 51, NEGATIVE_Y = 204,
		ALL = 255,
		UNKNOWN_OCTANT = 0
	};
	static const std::string OCTANT_NAMES[15] = { "+++", "-++", "--+", "+-+",
		"++-", "-+-", "---", "+--",
		"+z", "-z",
		"+x", "-x",
		"+y", "-y",
		"*"
	};

	OCTANTS static inline GetOctant(const std::string &_str){
		if (boost::iequals(OCTANT_NAMES[0], _str))
			return OCTANTS::PLUS_PLUS_PLUS;

		if (boost::iequals(OCTANT_NAMES[1], _str))
			return OCTANTS::MINUS_PLUS_PLUS;

		if (boost::iequals(OCTANT_NAMES[2], _str))
			return OCTANTS::MINUS_MINUS_PLUS;

		if (boost::iequals(OCTANT_NAMES[3], _str))
			return OCTANTS::PLUS_MINUS_PLUS;

		if (boost::iequals(OCTANT_NAMES[4], _str))
			return OCTANTS::PLUS_PLUS_MINUS;

		if (boost::iequals(OCTANT_NAMES[5], _str))
			return OCTANTS::MINUS_PLUS_MINUS;

		if (boost::iequals(OCTANT_NAMES[6], _str))
			return OCTANTS::MINUS_MINUS_MINUS;

		if (boost::iequals(OCTANT_NAMES[7], _str))
			return OCTANTS::PLUS_MINUS_MINUS;

		if (boost::iequals(OCTANT_NAMES[8], _str))
			return OCTANTS::POSITIVE_Z;

		if (boost::iequals(OCTANT_NAMES[9], _str))
			return OCTANTS::NEGATIVE_Z;

		if (boost::iequals(OCTANT_NAMES[10], _str))
			return OCTANTS::POSITIVE_X;

		if (boost::iequals(OCTANT_NAMES[11], _str))
			return OCTANTS::NEGATIVE_X;

		if (boost::iequals(OCTANT_NAMES[12], _str))
			return OCTANTS::POSITIVE_Y;

		if (boost::iequals(OCTANT_NAMES[13], _str))
			return OCTANTS::NEGATIVE_Y;

		if (boost::iequals(OCTANT_NAMES[14], _str))
			return OCTANTS::ALL;

		return OCTANTS::UNKNOWN_OCTANT;
	};
}

#endif