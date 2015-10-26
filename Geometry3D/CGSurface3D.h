#ifndef CGSURFACE3D_H
#define CGSURFACE3D_H
#include <CGTriangle3D.h>
class CGPolygon;
class CGSurface3D
{
	std::vector<CGTriangle3D*> m_triangles;
	
	mutable double m_area;
public:

	CGSurface3D(std::vector<CGTriangle3D*> &_boundary);

	CGSurface3D(const CGSurface3D &);

	CGSurface3D& operator=(const CGSurface3D &_t);

	bool Intersects(const CGLine3D&)const;

	bool Intersects(const CGSurface3D &)const;

	bool Intersects(const CGTriangle3D &)const;

	bool Overlaps(const CGLine3D &)const;

	bool Overlaps(const CGTriangle3D &)const;

	bool Overlaps(const CGSurface3D &)const;

	void IntersectionPoints(const CGLine3D&,
							std::vector<CGPoint3D> &)const;

	void IntersectionPoints(const CGSurface3D&,
							std::vector<CGPoint3D> &)const;

	void IntersectionPoints(const CGTriangle3D&,
							std::vector<CGPoint3D> &)const;

	double Distance(const CGPoint3D&)const;

	double Distance(const CGLine3D&)const;

	double Distance(const CGTriangle3D&)const;

	double Distance(const CGSurface3D&)const;

	bool IsInside(const CGPoint3D&)const;

	bool IsInside(const CGLine3D&)const;

	bool IsInside(const CGTriangle3D&)const;

	bool IsInside(const CGSurface3D&)const;

	void GetTriangles(std::vector<const CGTriangle3D*> &_triangles)const;

	double Area()const;

	CGSurface3D* Merge(const CGTriangle3D &_t)const;

	CGSurface3D* Merge(const CGSurface3D &_s)const;

	~CGSurface3D();
};
#endif

