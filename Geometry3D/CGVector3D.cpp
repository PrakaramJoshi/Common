#include "CGVector3D.h"


CGVector3D::CGVector3D()
{
	m_data.Set(0, 0, 0);
}

CGVector3D::CGVector3D(const CGPoint3D &_data){
	m_data = _data;
}

CGVector3D::CGVector3D(const double &_v){
	m_data.Set(_v, _v, _v);
}

CGVector3D::~CGVector3D()
{
}

bool CGVector3D::operator == (const CGVector3D &_v)const{
	return m_data == _v.m_data;
}

CGVector3D::CGVector3D(const CGVector3D &_v){
	set(_v.m_data);
}

CGVector3D &CGVector3D::operator=(const CGVector3D &_v){
	set(_v.m_data);
	return *this;
}

CGVector3D CGVector3D::operator -(const CGVector3D &_v)const{
	return CGVector3D(m_data - _v.m_data);
}

CGVector3D CGVector3D::operator *(const CGVector3D &_v)const{
	return cross(_v);
}

CGVector3D CGVector3D::operator +(const CGVector3D &_v)const{
	return CGVector3D(m_data+_v.m_data);
}

CGPoint3D CGVector3D::operator + (const CGPoint3D &_v)const{
	return m_data + _v;
}

CGVector3D CGVector3D::operator -(const double &_v)const{
	return CGVector3D(m_data-_v);
}

CGVector3D CGVector3D::operator +(const double &_v)const{
	return CGVector3D(m_data+_v);
}

CGVector3D CGVector3D::operator /(const double &_v)const{
	return CGVector3D(m_data/_v);
}

CGVector3D CGVector3D::operator *(const double &_v)const{
	return CGVector3D(m_data*_v);
}

double CGVector3D::dot(const CGVector3D &_v)const{
	return m_data.DotProduct(_v.m_data);
}

CGVector3D CGVector3D::cross(const CGVector3D &_v)const{
	return CGVector3D(m_data*_v.m_data);
}

double CGVector3D::x()const{
	return m_data.GetX();
}

double CGVector3D::y()const{
	return m_data.GetY();
}

double CGVector3D::z()const{
	return m_data.GetZ();
}

void CGVector3D::set_x(const double &_x){
	m_data.SetX(_x);
}

void CGVector3D::set_y(const double &_y){
	m_data.SetY(_y);
}

void CGVector3D::set_z(const double &_z){
	m_data.SetZ(_z);
}

void CGVector3D::set(const double &_x,
	const double &_y,
	const double &_z){

	m_data.Set(_x, _y, _z);
}

void CGVector3D::set(const CGVector3D &_v){
	set(_v.x(), _v.y(), _v.z());

}

CGVector3D CGVector3D::UnitVector()const{

	double mag = magnitude();
	return CGVector3D(m_data / mag);
}

double CGVector3D::magnitude()const{
	return m_data.Distance(CGPoint3D(0, 0, 0));
}

double CGVector3D::angle(const CGVector3D &_vec)const{
	double mag1 = magnitude();
	double mag2 = _vec.magnitude();
	double dot_product = dot(_vec);
	double cos_theta = dot_product / (mag1*mag2);
	double theta = std::acos(cos_theta);
	return theta;
}

CGPoint3D  CGVector3D::data()const{
	return m_data;

}