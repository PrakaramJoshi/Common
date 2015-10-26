#ifndef CGLINE3D_H
#define CGLINE3D_H
#include <CGPoint3D.h>
#include <CGCuboid.h>
#include <CGLine.h>
#include <vector>
#include <CGVector3D.h>
class CGLine3D
{
	CGPoint3D m_start;
	CGPoint3D m_end;
	CGCuboid  m_cuboid;
	CGVector3D m_unit_vector;

	CGLine3D();

	void calculate_bounding_cuboid();

	void construct_projections();
public:
	CGLine3D(const CGPoint3D &_start,
		const CGPoint3D &_end);

	CGLine3D(const CGLine3D &);

	CGLine3D& operator =(const CGLine3D &);
	
	void Set(const CGPoint3D &_start,
		const CGPoint3D &_end);

	void Set(const CGLine3D &_line);

	void SetStart(const CGPoint3D &);

	void SetEnd(const CGPoint3D &);

	const CGPoint3D& GetStart()const;

	const CGPoint3D& GetEnd()const;

	CGLine3D *GetCopy()const;

	double Distance(const CGPoint3D &_pt)const;

	double Distance(const CGLine3D &_line)const;

	bool Intersects(const CGLine3D &_line)const;

	bool IntersectsInMiddle(const CGLine3D &_line)const;

	void IntersectionPoints(const CGLine3D &_line, 
							std::vector<CGPoint3D> &_pts)const;

	bool IsOn(const CGPoint3D &_pt)const;

	double GetLength() const;

	double Angle(const CGLine3D& _geometry) const;

	bool IsParallel(const CGLine3D &_seg) const;

	double GetMinX()const;

	double GetMaxX()const;
	
	double GetMinY()const;

	double GetMaxY()const;
	
	double GetMinZ()const;

	double GetMaxZ()const;

	void extend(const double &_length, 
				bool _from_start=true);

	bool IsEndPoint(const CGPoint3D &_pt)const;

	CGVector3D UnitVector()const;

	CGPoint3D MidPoint()const;

	void flip();

	void BreakLineAtPoints(std::vector<CGPoint3D>&_pts,
		std::vector<CGLine3D*> &_lines)const;

	~CGLine3D();
};

#endif