#pragma once
#include <CGPoint3D.h>
class CGVector3D
{
	CGPoint3D m_data;
	
	friend class CGVector3D;

public:
	CGVector3D();

	CGVector3D(const CGVector3D &);

	CGVector3D(const CGPoint3D &_data);

	CGVector3D(const double &);

	bool operator==(const CGVector3D &)const;

	CGVector3D &operator=(const CGVector3D &);

	CGVector3D operator -(const CGVector3D &)const;

	CGVector3D operator +(const CGVector3D &)const;

	CGPoint3D operator +(const CGPoint3D &)const;

	CGVector3D operator *(const CGVector3D &)const;

	CGVector3D operator -(const double &)const;

	CGVector3D operator +(const double &)const;

	CGVector3D operator /(const double &)const;

	CGVector3D operator *(const double &)const;

	double dot(const CGVector3D &)const;

	CGVector3D cross(const CGVector3D &)const;

	double x()const;
	
	double y()const;

	double z()const;

	void set_x(const double &_x);

	void set_y(const double &_y);

	void set_z(const double &_z);

	void set(const double &_x,
		const double &_y,
		const double &_z);

	void set(const CGVector3D &);

	CGVector3D UnitVector()const;

	double magnitude()const;

	double angle(const CGVector3D &_vec)const;

	CGPoint3D data()const;

	~CGVector3D();
};

