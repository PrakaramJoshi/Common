#include <CGLine3D.h>
#include <CGVector3D.h>
CGLine3D::~CGLine3D()
{
}

CGLine3D::CGLine3D(const CGPoint3D &_start,
	const CGPoint3D &_end){
	Set(_start, _end);
}

CGLine3D::CGLine3D(const CGLine3D &_line){
	Set(_line);
}

void CGLine3D::calculate_bounding_cuboid(){
	double min_x = std::min(m_start.GetX(), m_end.GetX());
	double min_y = std::min(m_start.GetY(), m_end.GetY());
	double min_z = std::min(m_start.GetZ(), m_end.GetZ());
	double max_x = std::max(m_start.GetX(), m_end.GetX());
	double max_y = std::max(m_start.GetY(), m_end.GetY());
	double max_z = std::max(m_start.GetZ(), m_end.GetZ());

	m_cuboid.Set(CGPoint3D(min_x, min_y, min_z), 
				CGPoint3D(max_x, max_y, max_z));
}

void CGLine3D::construct_projections(){

	calculate_bounding_cuboid();

	auto dir = CGVector3D(m_end - m_start);
	m_unit_vector = dir.UnitVector();
}

CGLine3D& CGLine3D::operator = (const CGLine3D &_line){
	Set(_line);
	return *this;
}

CGLine3D *CGLine3D::GetCopy()const
{
	return new CGLine3D(*this);
}

void CGLine3D::Set(const CGLine3D &_line){
	Set(_line.GetStart(), _line.GetEnd());
}

void CGLine3D::Set(const CGPoint3D &_start,
	const CGPoint3D &_end){

	SetStart(_start);
	SetEnd(_end);
}

void CGLine3D::SetStart(const CGPoint3D &_pt){
	m_start = _pt;
	construct_projections();
}

void CGLine3D::SetEnd(const CGPoint3D &_pt){
	m_end = _pt;
	construct_projections();
}

const CGPoint3D& CGLine3D::GetStart()const{
	return m_start;
}

const CGPoint3D& CGLine3D::GetEnd()const{
	return m_end;
}

double CGLine3D::Distance(const CGPoint3D &_pt)const{

	CGPoint3D pt;
	double x1 = GetStart().GetX();
	double y1 = GetStart().GetY();
	double z1 = GetStart().GetZ();
	double x2 = GetEnd().GetX();
	double y2 = GetEnd().GetY();
	double z2 = GetEnd().GetZ();
	double A = _pt.GetX() - x1;
	double B = _pt.GetY() - y1;
	double F = _pt.GetZ() - z1;
	double C = x2 - x1;
	double D = y2 - y1;
	double E = z2 - z1;
	double dot = A*C + B*D +F*E;
	double len_sq = C*C + D*D + E*E;
	if (len_sq == 0)
		pt = CGPoint3D(x1, y1,z1);
	else
	{
		double u = dot / len_sq;
		if (u < 0)
			pt = CGPoint3D(x1, y1,z1);
		else if (u > 1)
			pt = CGPoint3D(x2, y2,z2);
		else
			pt = CGPoint3D(x1 + u*C, y1 + u*D,z1 +u*E);
	}
	return _pt.Distance(pt);
}

double CGLine3D::Distance(const CGLine3D &_line)const{
	//TODO
	return 0;
}

bool CGLine3D::Intersects(const CGLine3D &_line)const{

	std::vector<CGPoint3D> intersections;
	IntersectionPoints(_line, intersections);
	return !intersections.empty();
}

bool CGLine3D::IntersectsInMiddle(const CGLine3D &_line)const{

	std::vector<CGPoint3D> intersections;
	IntersectionPoints(_line, intersections);
	int count = 0;
	for (auto point: intersections){
		if (IsEndPoint(point) && _line.IsEndPoint(point))
			count++;
	}
	return intersections.size() - count > 0;
}

void CGLine3D::IntersectionPoints(const CGLine3D &_line, 
				std::vector<CGPoint3D> &_pts)const{

	_pts.clear();

	const CGPoint3D &thisStart = GetStart();
	const CGPoint3D &thisEnd = GetEnd();
	const CGPoint3D &geomStart = _line.GetStart();
	const CGPoint3D &geomEnd = _line.GetEnd();

	if (IsOn(geomStart))
	{
		_pts.push_back(geomStart);
	}
	if (IsOn(geomEnd))
	{
		_pts.push_back(geomEnd);
	}
	if (_pts.size() == 2)
		return;
	if (_line.IsOn(thisEnd)){
		if (GetEnd() != geomStart&&thisEnd != geomEnd)
			_pts.push_back(thisEnd);
	}
	if (_pts.size() == 2)
		return;
	if (_line.IsOn(thisStart)){
		if (thisStart != geomStart&&thisStart != geomEnd)
			_pts.push_back(thisStart);
	}
	if (!_pts.empty())
		return;
	// http://paulbourke.net/geometry/lineline2d/
	double x11 = GetStart().GetX();
	double y11 = GetStart().GetY();
	double z11 = GetStart().GetZ();
	double x12 = GetEnd().GetX();
	double y12 = GetEnd().GetY();
	double z12 = GetEnd().GetZ();
	double x21 = _line.GetStart().GetX();
	double y21 = _line.GetStart().GetY();
	double z21 = _line.GetStart().GetZ();
	double x22 = _line.GetEnd().GetX();
	double y22 = _line.GetEnd().GetY();
	double z22 = _line.GetEnd().GetZ();

	double denomxy = (x12 - x11)*(y22 - y21) - (x22 - x21)*(y12 - y11);

	double denomxz = (x12 - x11)*(z22 - z21) - (x22 - x21)*(z12 - z11);

	double denomyz = (y12 - y11)*(z22 - z21) - (y22 - y21)*(z12 - z11);
	if (denomxy == 0 || denomxz == 0 || denomyz==0)//||IsParallel(_geometry))
	{
		return;
	}
	double numaxy = (x22 - x21)*(y11 - y21) - (x11 - x21)*(y22 - y21);
	double numbxy = (x12 - x11)*(y11 - y21) - (x11 - x21)*(y12 - y11);

	double numaxz = (x22 - x21)*(z11 - z21) - (x11 - x21)*(z22 - z21);
	double numayz = (y22 - y21)*(z11 - z21) - (y11 - y21)*(z22 - z21);

	double numbxz = (x12 - x11)*(z11 - z21) - (x11 - x21)*(z12 - z11);
	double numbyz = (y12 - y11)*(z11 - z21) - (y11 - y21)*(z12 - z11);

	double uaxy = numaxy / denomxy;
	double ubxy = numbxy / denomxy;

	double uaxz = numaxz / denomxz;
	double ubxz = numbxz / denomxz;

	double uayz = numayz / denomyz;
	double ubyz = numbyz / denomyz;


	bool pointOnLines = false;
	if (uaxy <= 1 && uaxy >= 0 && ubxy >= 0 && ubxy <= 1){
		pointOnLines = true;
	}
	double px = uaxy*(x12 - x11) + x11;
	double py = uaxy*(y12 - y11) + y11;
	double pz = uaxy*(z12 - z11) + z11;
	CGPoint3D pt1(px, py,pz);
	if (IsOn(pt1) && _line.IsOn(pt1)){
		_pts.push_back(pt1);
		return;
	}
	if (pointOnLines){
		{
			double px = ubxy*(x22 - x21) + x21;
			double py = ubxy*(y22 - y21) + y21;
			double pz = ubxy*(z22 - z21) + z21;
			CGPoint3D pt1(px, py, pz);
			if (IsOn(pt1) && _line.IsOn(pt1)){
				_pts.push_back(pt1);
				return;
			}
		}
		{

			double px = uaxz*(x12 - x11) + x11;
			double py = uaxz*(y12 - y11) + y11;
			double pz = uaxz*(z12 - z11) + z11;
			CGPoint3D pt1(px, py, pz);
			if (IsOn(pt1) && _line.IsOn(pt1)){
				_pts.push_back(pt1);
				return;
			}
		}
		{
			double px = ubxz*(x22 - x21) + x21;
			double py = ubxz*(y22 - y21) + y21;
			double pz = ubxz*(z22 - z21) + z21;
			CGPoint3D pt1(px, py, pz);
			if (IsOn(pt1) && _line.IsOn(pt1)){
				_pts.push_back(pt1);
				return;
			}
		}
		{
			double px = uayz*(x12 - x11) + x11;
			double py = uayz*(y12 - y11) + y11;
			double pz = uayz*(z12 - z11) + z11;
			CGPoint3D pt1(px, py, pz);
			if (IsOn(pt1) && _line.IsOn(pt1)){
				_pts.push_back(pt1);
				return;
			}
		}
		{
			double px = ubyz*(x22 - x21) + x21;
			double py = ubyz*(y22 - y21) + y21;
			double pz = ubyz*(z22 - z21) + z21;
			CGPoint3D pt1(px, py, pz);
			if (IsOn(pt1) && _line.IsOn(pt1)){
				_pts.push_back(pt1);
				return;
			}
		}
	}
	
}

bool CGLine3D::IsOn(const CGPoint3D &_pt)const{
	if (_pt == GetStart() || _pt == GetEnd())
		return true;
	double distance = Distance(_pt);
	return CGMath::GeoZero(distance,CGConstants::GEO_ERR3D);
}

double CGLine3D::GetLength() const{
	return m_end.Distance(m_start);
}

double CGLine3D::Angle(const CGLine3D& _geometry) const{
	auto cos_theta = UnitVector().dot(_geometry.UnitVector());
	return std::acos(cos_theta);
	
}

bool CGLine3D::IsParallel(const CGLine3D &_line) const{
	return CGMath::GeoZero(
		_line.UnitVector().dot(UnitVector()), CGConstants::GEO_ERR4);
}

double CGLine3D::GetMinX()const{
	return m_cuboid.GetMinX();
}

double CGLine3D::GetMaxX()const{
	return m_cuboid.GetMaxX();
}

double CGLine3D::GetMinY()const{
	return m_cuboid.GetMinY();
}

double CGLine3D::GetMaxY()const{
	return m_cuboid.GetMaxY();
}

double CGLine3D::GetMinZ()const{
	return m_cuboid.GetMinZ();
}

double CGLine3D::GetMaxZ()const{
	return m_cuboid.GetMaxZ();

}

void CGLine3D::extend(const double &_length, bool _from_start){

	auto dir = CGVector3D(m_end - m_start);
	auto mag = dir.magnitude();

	mag = mag + _length;
	dir = m_unit_vector*mag;
	auto data = dir.data();
	CGPoint3D pt2;
	if (_from_start)
		SetEnd(m_start + data);
	else
		SetStart(m_end - data);
}

CGVector3D CGLine3D:: UnitVector()const{
	return m_unit_vector;
}

bool CGLine3D::IsEndPoint(const CGPoint3D &_pt)const{
	return _pt == m_start || _pt == m_end;
}

void CGLine3D::flip(){
	std::swap(m_start, m_end);

}
void CGLine3D::BreakLineAtPoints(std::vector<CGPoint3D>&_pts,
	std::vector<CGLine3D*> &_lines)const{

	std::map<double, CGPoint3D> distance_sort;

	for (const auto&p : _pts){
		if (!IsEndPoint(p)&&IsOn(p))
			distance_sort[m_start.Distance(p)] = p;
	}
	
	CGPoint3D end = m_start;

	for (auto p = distance_sort.begin(); p != distance_sort.end(); ++p){
		CGPoint3D start = end;
		end = p->second;
		_lines.push_back(new CGLine3D(start, end));
		//end = start;
	}
	_lines.push_back(new CGLine3D(end, m_end));

}
CGPoint3D CGLine3D::MidPoint()const{
	return (m_start + m_end) / 2;

}