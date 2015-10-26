#include <CGPoint3D.h>
#include <CGMath.h>

CGPoint3D::~CGPoint3D()
{
}
CGPoint3D::CGPoint3D(){
	m_x = 0;
	m_y = 0;
	m_z = 0;
}

CGPoint3D::CGPoint3D(const double &_x, 
					const double &_y, 
					const double &_z){
	Set(_x, _y, _z);
}

CGPoint3D::CGPoint3D(const CGPoint3D &_pt){
	Set(_pt);
}

CGPoint3D& CGPoint3D::operator = (const CGPoint3D &_pt){

	Set(_pt);
	return *this;
}

CGPoint3D CGPoint3D::operator *(const CGPoint3D &_pt)const{

	return CrossProduct(_pt);
}

CGPoint3D CGPoint3D::operator / (const double &_val)const{

	return CGPoint3D(m_x / _val, m_y / _val, m_z / _val);
}

CGPoint3D CGPoint3D::operator * (const double &_val)const{

	return CGPoint3D(m_x *_val, m_y * _val, m_z * _val);
}

CGPoint3D CGPoint3D::operator + (const double &_val)const{

	return CGPoint3D(m_x + _val, m_y + _val, m_z + _val);
}

CGPoint3D CGPoint3D::operator - (const double &_val)const{

	return CGPoint3D(m_x - _val, m_y - _val, m_z - _val);
}

bool CGPoint3D::operator ==(const CGPoint3D &_pt)const{

	return CGMath::GeoZero(Distance(_pt), CGConstants::GEO_ERR3D);
}

bool CGPoint3D::operator != (const CGPoint3D &_pt)const{

	return !operator==(_pt);
}

CGPoint3D CGPoint3D::operator - (const CGPoint3D &_pt)const{

	CGPoint3D pt(m_x - _pt.GetX(), m_y - _pt.GetY(), m_z - _pt.GetZ());
	return pt;
}

CGPoint3D CGPoint3D::operator + (const CGPoint3D &_pt)const{

	CGPoint3D pt(m_x + _pt.GetX(), m_y + _pt.GetY(), m_z + _pt.GetZ());
	return pt;
}

void CGPoint3D::Set(const CGPoint3D &_pt){
	Set(_pt.GetX(), _pt.GetY(), _pt.GetZ());
}

void CGPoint3D::Set(const double &_x, 
					const double &_y, 
					const double &_z){

	SetX(_x);
	SetY(_y);
	SetZ(_z);
}

void CGPoint3D::SetX(const double &_x){

	m_x = _x;
}

void CGPoint3D::SetY(const double &_y){

	m_y = _y;
}

void CGPoint3D::SetZ(const double &_z){

	m_z = _z;
}

double CGPoint3D::GetX()const{

	return m_x;
}

double CGPoint3D::GetY()const{

	return m_y;
}

double CGPoint3D::GetZ()const{

	return m_z;
}

double CGPoint3D::Distance(const CGPoint3D &_pt)const{

	double d_x = GetX() - _pt.GetX(); 
	double d_y = GetY() - _pt.GetY();
	double d_z = GetZ() - _pt.GetZ();
	return std::sqrt((d_x*d_x) + (d_y*d_y) + (d_z*d_z));
}

CGPoint3D CGPoint3D::CrossProduct(const CGPoint3D &_v)const{

	double x = (m_y*_v.GetZ()) - (m_z*_v.GetY());
	double y = (m_z*_v.GetX()) - (m_x*_v.GetZ());
	double z = (m_x*_v.GetY()) - (m_y*_v.GetX());
	return CGPoint3D(x, y, z);

}

double CGPoint3D::DotProduct(const CGPoint3D &_v)const{

	return (m_x*_v.GetX()) + (m_y*_v.GetY()) + (m_z*_v.GetZ());

}

CGPoint2D CGPoint3D::getPointXY()const{

	return CGPoint2D(m_x, m_y);
}

CGPoint2D CGPoint3D::getPointYZ()const{

	return CGPoint2D(m_y, m_z);
}

CGPoint2D CGPoint3D::getPointZX()const{

	return CGPoint2D(m_z, m_x);
}

CGPoint3D CGPoint3D::GetMidPoint(const CGPoint3D &_pt)const{

	return CGPoint3D((GetX() + _pt.GetX()) / 2.0, 
					(GetY() + _pt.GetY()) / 2.0, 
					(GetZ() + _pt.GetZ()) / 2.0);

}

CGPoint3D CGPoint3D::min(const CGPoint3D &_pt)const{

	return CGPoint3D(std::min(m_x, _pt.GetX()), 
					std::min(m_y, _pt.GetY()), 
					std::min(m_z, _pt.GetZ()));
}

CGPoint3D CGPoint3D::max(const CGPoint3D &_pt)const{

	return CGPoint3D(std::max(m_x, _pt.GetX()), 
					std::max(m_y, _pt.GetY()), 
					std::max(m_z, _pt.GetZ()));
}