#ifndef CGPOINT3D_H
#define CGPOINT3D_H
#include <CGPoint2D.h>
class CGPoint3D
{

	double m_x;
	double m_y;
	double m_z;
	
public:
	CGPoint3D();
	CGPoint3D(const double &_x,
				const double &_y,
				const double &_z);

	CGPoint3D(const CGPoint3D &);

	bool operator ==(const CGPoint3D &)const;

	bool operator !=(const CGPoint3D &)const;

	CGPoint3D& operator = (const CGPoint3D &);

	CGPoint3D operator -(const CGPoint3D &)const;

	CGPoint3D operator +(const CGPoint3D &)const;

	CGPoint3D operator *(const CGPoint3D &)const;

	CGPoint3D operator +(const double &)const;

	CGPoint3D operator -(const double &)const;

	CGPoint3D operator/(const double &)const;

	CGPoint3D operator*(const double &)const;

	void Set(const CGPoint3D &);

	void Set(const double &_x, 
			const double &_y, 
			const double &_z);

	void SetX(const double &_x);

	void SetY(const double &_y);

	void SetZ(const double &_z);

	double GetX()const;

	double GetY()const;

	double GetZ()const;

	double Distance(const CGPoint3D &_pt)const;

	CGPoint3D CrossProduct(const CGPoint3D &_v)const;

	double DotProduct(const CGPoint3D &_v)const;

	CGPoint2D getPointXY()const;

	CGPoint2D getPointYZ()const;

	CGPoint2D getPointZX()const;

	CGPoint3D GetMidPoint(const CGPoint3D &_pt)const;

	CGPoint3D min(const CGPoint3D &_pt)const;

	CGPoint3D max(const CGPoint3D &_pt)const;

	~CGPoint3D();
};

#endif