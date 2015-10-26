#include <CGCuboid.h>
#include <CGLine3D.h>

CGCuboid::CGCuboid()
{
	m_origin.Set(0, 0, 0);
	m_corner.Set(0, 0, 0);
}


CGCuboid::~CGCuboid()
{
}

CGCuboid::CGCuboid(const CGPoint3D &_origin,
	const CGPoint3D &_corner){
	Set(_origin, _corner);
}

CGCuboid::CGCuboid(const CGPoint3D &_center,
	const double &_size_x,
	const double &_size_y,
	const double &_size_z){

	Set(_center, _size_x, _size_y, _size_z);
}

CGCuboid::CGCuboid(const CGCuboid&_cuboid){
	Set(_cuboid.GetOrigin(), _cuboid.GetCorner());
}

CGCuboid& CGCuboid::operator = (const CGCuboid&_cuboid){
	Set(_cuboid.GetOrigin(), _cuboid.GetCorner());
	return *this; 
}

void CGCuboid::Set(const CGPoint3D &_origin,
	const CGPoint3D &_corner){
	SetOrigin(_origin);
	SetCorner(_corner);
}

void CGCuboid::Set(const CGPoint3D &_center,
	const double &_size_x,
	const double &_size_y,
	const double &_size_z){

	CGPoint3D origin(_center.GetX() - (_size_x / 2.0),
					_center.GetY() - (_size_y / 2.0), 
					_center.GetZ() - (_size_z / 2.0));

	CGPoint3D corner(_center.GetX() + (_size_x / 2.0), 
					_center.GetY() + (_size_y / 2.0), 
					_center.GetZ() + (_size_z / 2.0));

	Set(origin, corner);

}

void CGCuboid::add_face(const CGPoint3D &_p1,
	const CGPoint3D &_p2,
	const CGPoint3D &_p3,
	const CGPoint3D &_p4,
	std::vector<CGTriangle3D> &_faces)const{

	_faces.push_back(CGTriangle3D(_p1, _p2, _p3));
	_faces.push_back(CGTriangle3D(_p1, _p4, _p3));

}

void CGCuboid::GetFaces(std::vector<CGTriangle3D> &_faces)const{
	CGPoint3D xy_corner(m_corner.GetX(), m_corner.GetY(), m_origin.GetZ());
	CGPoint3D origin_y(m_origin.GetX(), m_corner.GetY(), m_origin.GetZ());
	CGPoint3D origin_x(m_corner.GetX(), m_origin.GetY(), m_origin.GetZ());
	CGPoint3D zx_corner(m_corner.GetX(), m_origin.GetY(), m_corner.GetZ());
	CGPoint3D origin_z(m_origin.GetX(), m_origin.GetY(), m_corner.GetZ());
	CGPoint3D yz_corner(m_origin.GetX(), m_corner.GetY(), m_corner.GetZ());

	add_face(m_origin, origin_x, xy_corner, origin_y, _faces);  // face on xy plane								face id 1 
	add_face(m_corner, yz_corner, origin_z, zx_corner, _faces); // face parallel to xy face						face id 2 (opposite to 1)

	add_face(m_origin, origin_z, yz_corner, origin_y, _faces); // face on yz plane							face id 3
	add_face(m_corner, zx_corner, origin_x, xy_corner, _faces);// face parallel to yz face					face id 4 (opposite to 3)

	add_face(m_origin, origin_z, zx_corner, origin_x, _faces); // face on zx plane							face id 5
	add_face(m_corner, yz_corner, origin_y, xy_corner, _faces);// face parallel to zx face					face id 6 (opposite to 5)
		
}

void CGCuboid::SetOrigin(const CGPoint3D &_origin){
	m_origin = _origin;
}

void CGCuboid::SetCorner(const CGPoint3D &_corner){
	m_corner = _corner;
}

CGPoint3D CGCuboid::GetOrigin()const{
	return m_origin;
}

CGPoint3D CGCuboid::GetCorner()const{
	return m_corner;
}

double CGCuboid::GetMinX()const{
	return m_origin.GetX();
}

double CGCuboid::GetMinY()const{
	return m_origin.GetY();
}

double CGCuboid::GetMinZ()const{
	return m_origin.GetZ();
}

double CGCuboid::GetMaxX()const{
	return m_corner.GetX();
}

double CGCuboid::GetMaxY()const{
	return m_corner.GetY();
}


double CGCuboid::GetMaxZ()const{
	return m_corner.GetZ();
}

void CGCuboid::GetAllVertices(std::vector<CGPoint3D> &_vertices)const{
	_vertices.push_back(m_origin);
	_vertices.push_back(CGPoint3D(m_corner.GetX(), m_origin.GetY(), m_origin.GetZ()));
	_vertices.push_back(CGPoint3D(m_corner.GetX(), m_corner.GetY(), m_origin.GetZ()));
	_vertices.push_back(CGPoint3D(m_origin.GetX(), m_corner.GetY(), m_origin.GetZ()));

	_vertices.push_back(CGPoint3D(m_origin.GetX(), m_corner.GetY(), m_corner.GetZ()));
	_vertices.push_back(CGPoint3D(m_origin.GetX(), m_origin.GetY(), m_corner.GetZ()));

	_vertices.push_back(CGPoint3D(m_corner.GetX(), m_origin.GetY(), m_origin.GetZ()));
	_vertices.push_back(m_corner);

}

double CGCuboid::GetLengthAlongX()const{
	return m_corner.GetX() - m_origin.GetX();
}

double CGCuboid::GetLengthAlongY()const{
	return m_corner.GetY() - m_origin.GetY();
}

double CGCuboid::GetLengthAlongZ()const{
	return m_corner.GetZ() - m_origin.GetZ();
}

CGPoint3D CGCuboid::GetCenter()const{
	return m_origin.GetMidPoint(m_corner);
}

bool CGCuboid::IsInside(const CGPoint3D &_pt)const{

	return CGMath::GeoGreaterThanEqual(_pt.GetX(), GetMinX()) &&
		CGMath::GeoGreaterThanEqual(_pt.GetY(), GetMinY()) &&
		CGMath::GeoGreaterThanEqual(_pt.GetZ(), GetMinZ()) &&
		CGMath::GeoLessThanEqual(_pt.GetX(), GetMaxX()) &&
		CGMath::GeoLessThanEqual(_pt.GetY(), GetMaxY()) &&
		CGMath::GeoLessThanEqual(_pt.GetZ(), GetMaxZ());

}

bool CGCuboid::IsInside(const CGLine3D &_line)const{
	return IsInside(_line.GetStart()) && IsInside(_line.GetEnd());
}

bool CGCuboid::IsInside(const CGCuboid &_cuboid)const{
	return IsInside(_cuboid.GetOrigin()) &&
		IsInside(_cuboid.GetCorner());
}

bool CGCuboid::Overlap(const CGPoint3D &_pt)const{
	return IsInside(_pt);
}

bool CGCuboid::Overlap(const CGLine3D &_line)const{

	if (IsInside(_line.GetStart()))
		return true;
	if (IsInside(_line.GetEnd()))
		return true;
	std::vector<CGPoint3D> pts;
	IntersectionPoints(_line,pts);
	return !pts.empty();

}

bool CGCuboid::Overlap(const CGCuboid &_cuboid)const{
	if (CGMath::GeoLessThan(GetMaxX(), _cuboid.GetMinX()) ||
		CGMath::GeoLessThan(_cuboid.GetMaxX(), GetMinX()))
		return false;

	if (CGMath::GeoLessThan(GetMaxY(), _cuboid.GetMinY()) ||
		CGMath::GeoLessThan(_cuboid.GetMaxY(), GetMinY()))
		return false;

	if (CGMath::GeoLessThan(GetMaxZ(), _cuboid.GetMinZ()) ||
		CGMath::GeoLessThan(_cuboid.GetMaxZ(), GetMinZ()))
		return false;

	return true;
}

double CGCuboid::Distance(const CGPoint3D &_pt)const{
	if (IsInside(_pt))
		return 0;
	std::vector<CGTriangle3D> triangles;
	GetFaces(triangles);
	double min_distance = std::numeric_limits<double>::max();
	for (auto t : triangles){
		double distance = t.Distance(_pt);
		if (distance < min_distance)
			min_distance = distance;
	}
	return min_distance;
}

double CGCuboid::Distance(const CGLine3D &_line)const{
	if (Overlap(_line))
		return 0;
	std::vector<CGTriangle3D> triangles;
	GetFaces(triangles);
	double min_distance = std::numeric_limits<double>::max();
	for (auto t : triangles){
		double distance = t.Distance(_line);
		if (distance < min_distance)
			min_distance = distance;
	}
	return min_distance;
}

double CGCuboid::Distance(const CGCuboid &_cuboid)const{
	return 0;
}

double CGCuboid::GetVolume()const{
	return GetLengthAlongX()*GetLengthAlongY()*GetLengthAlongZ();
}

bool CGCuboid::Intersects(const CGLine3D &_line)const{
	std::vector<CGPoint3D> pts;
	IntersectionPoints(_line, pts);
	return !pts.empty();
}

bool CGCuboid::Intersects(const CGCuboid &_cuboid)const{
	std::vector<CGPoint3D> pts;
	IntersectionPoints(_cuboid, pts);
	return !pts.empty();
}

void CGCuboid::IntersectionPoints(const CGLine3D &_line,
	std::vector<CGPoint3D> &_pts)const{

	std::vector<CGTriangle3D> triangles;
	GetFaces(triangles);
	for (auto t : triangles)
		t.IntersectionPoints(_line, _pts);
}

void CGCuboid::IntersectionPoints(const CGCuboid &_cuboid,
	std::vector<CGPoint3D> &_pts)const{

	std::vector<CGTriangle3D> triangles;
	GetFaces(triangles);
	std::vector<CGTriangle3D> triangles2;
	_cuboid.GetFaces(triangles2);

	for (auto t1 : triangles){
		for (auto t2 : triangles2)
			t1.IntersectionPoints(t2, _pts);
	}
}

void CGCuboid::Merge(const CGCuboid &_cuboid){
	CGPoint3D origin = GetOrigin();
	CGPoint3D origin2 = _cuboid.GetOrigin();
	origin.SetX(std::min(origin.GetX(), origin2.GetX()));
	origin.SetY(std::min(origin.GetY(), origin2.GetY()));
	origin.SetZ(std::min(origin.GetZ(), origin2.GetZ()));

	CGPoint3D corner = GetCorner();
	CGPoint3D corner2 = _cuboid.GetCorner();

	corner.SetX(std::max(corner.GetX(), corner2.GetX()));
	corner.SetY(std::max(corner.GetY(), corner2.GetY()));
	corner.SetZ(std::max(corner.GetZ(), corner2.GetZ()));

	Set(origin, corner);
}
