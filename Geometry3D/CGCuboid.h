#ifndef CGCUBOID_H
#define CGCUBOID_H
#include <CGPoint3D.h>
#include <CGTriangle3D.h>
class CGLine3D;
class CGCuboid
{
	CGPoint3D m_origin;
	CGPoint3D m_corner;
	
	void add_face(const CGPoint3D &_p1,
		const CGPoint3D &_p2,
		const CGPoint3D &_p3,
		const CGPoint3D &_p4,
		std::vector<CGTriangle3D> &_faces) const;
public:
	CGCuboid();

	CGCuboid(const CGPoint3D &_origin,
		const CGPoint3D &_corner);

	CGCuboid(const CGPoint3D &_center,
		const double &_size_x,
		const double &_size_y,
		const double &_size_z);
	
	CGCuboid(const CGCuboid&);

	CGCuboid& operator = (const CGCuboid&);

	void Set(const CGPoint3D &_origin,
		const CGPoint3D &_corner);

	void Set(const CGPoint3D &_center,
		const double &_size_x,
		const double &_size_y,
		const double &_size_z);

	void SetOrigin(const CGPoint3D &_origin);

	void SetCorner(const CGPoint3D &_corner);

	CGPoint3D GetOrigin()const;

	CGPoint3D GetCorner()const;

	double GetMinX()const;
	
	double GetMinY()const;

	double GetMinZ()const;

	double GetMaxX()const;

	double GetMaxY()const;

	double GetMaxZ()const;

	void GetAllVertices(std::vector<CGPoint3D> &)const;

	double GetLengthAlongX()const;
	
	double GetLengthAlongY()const;
	
	double GetLengthAlongZ()const;

	CGPoint3D GetCenter()const;

	bool IsInside(const CGPoint3D &)const;

	bool IsInside(const CGLine3D &)const;

	bool IsInside(const CGCuboid &)const;

	bool Overlap(const CGPoint3D &)const;

	bool Overlap(const CGLine3D &)const;

	bool Overlap(const CGCuboid &)const;

	double Distance(const CGPoint3D &)const;

	double Distance(const CGLine3D &)const;

	double Distance(const CGCuboid &)const;

	double GetVolume()const;

	bool Intersects(const CGLine3D &)const;

	bool Intersects(const CGCuboid &)const;

	void IntersectionPoints(const CGLine3D &, 
							std::vector<CGPoint3D> &)const;

	void IntersectionPoints(const CGCuboid &, 
							std::vector<CGPoint3D> &)const;

	void Merge(const CGCuboid &);

	void GetFaces(std::vector<CGTriangle3D> &_faces)const;
	
	~CGCuboid();
};

#endif