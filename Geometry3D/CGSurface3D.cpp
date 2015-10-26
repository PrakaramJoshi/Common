#include "CGSurface3D.h"
#include <CGLine3D.h>
#include <CGAlgorithms.h>

CGSurface3D::CGSurface3D(std::vector<CGTriangle3D*> &_boundary){
	std::swap(m_triangles, _boundary);
	m_area = -1;
}

CGSurface3D::CGSurface3D(const CGSurface3D &_s){
	m_triangles.reserve(_s.m_triangles.size());
	for (auto t : _s.m_triangles)
		m_triangles.push_back(t->GetCopy());
	m_area = _s.Area();
}

CGSurface3D& CGSurface3D::operator = (const CGSurface3D &_s){
	m_triangles.reserve(_s.m_triangles.size());
	for (auto t : _s.m_triangles)
		m_triangles.push_back(t->GetCopy());
	m_area = _s.Area();
	return *this;
}

bool CGSurface3D::Intersects(const CGLine3D &_l)const{
	for (auto t : m_triangles){
		if (t->Intersects(_l))
			return true;
	}
	return false;
}

bool CGSurface3D::Intersects(const CGSurface3D &_s)const{
	for (auto t : m_triangles){
		if (_s.Intersects(*t))
			return true;
	}
	return false;
}

bool CGSurface3D::Intersects(const CGTriangle3D &_t)const{
	for (auto t : m_triangles){
		if (t->Intersects(_t))
			return true;
	}
	return false;
		
}

bool CGSurface3D::Overlaps(const CGLine3D &_l)const{
	for (auto t : m_triangles){
		if (t->Overlap(_l))
			return true;
	}
	return false;
}

bool CGSurface3D::Overlaps(const CGTriangle3D &_t)const{
	for (auto t : m_triangles){
		if (t->Overlap(_t))
			return true;
	}
	return false;
}

bool CGSurface3D::Overlaps(const CGSurface3D &_s)const{
	for (auto t : _s.m_triangles){
		if (Overlaps(*t))
			return true;
	}
	return false;
}

void CGSurface3D::IntersectionPoints(const CGLine3D &_l,
									std::vector<CGPoint3D> &_intxPts)const{
	for (auto t : m_triangles){
		t->IntersectionPoints(_l, _intxPts);
	}
}

void CGSurface3D::IntersectionPoints(const CGSurface3D &_s,
									std::vector<CGPoint3D> &_intxPts)const{
	std::vector<CGPoint3D> pts;
	for (auto t : _s.m_triangles){
		IntersectionPoints(*t, pts);
	}
	CGAlgorithms::Algorithms::RemoveDuplicates(pts);
	CGAlgorithms::Algorithms::CopyTo(pts, _intxPts);
	
}

void CGSurface3D::IntersectionPoints(const CGTriangle3D &_t,
									std::vector<CGPoint3D> &_intxPts)const{
	for (auto t : m_triangles){
		t->IntersectionPoints(_t, _intxPts);
	}
}

double CGSurface3D::Distance(const CGPoint3D &_pt)const{
	double distance = std::numeric_limits<double>::max();
	for (auto t : m_triangles){
		double current_dist = t->Distance(_pt);
		if (current_dist < distance){
			distance = current_dist;
			if (CGMath::GeoZero(distance, CGConstants::GEO_ERR3D))
				return 0;
		}
	}
	return distance;
}

double CGSurface3D::Distance(const CGLine3D &_l)const{
	double distance = std::numeric_limits<double>::max();
	for (auto t : m_triangles){
		double current_dist = t->Distance(_l);
		if (current_dist < distance){
			distance = current_dist;
			if (CGMath::GeoZero(distance, CGConstants::GEO_ERR3D))
				return 0;
		}
	}
	return distance;
}

double CGSurface3D::Distance(const CGTriangle3D &_t)const{
	double distance = std::numeric_limits<double>::max();
	for (auto t : m_triangles){
		double current_dist = t->Distance(_t);
		if (current_dist < distance){
			distance = current_dist;
			if (CGMath::GeoZero(distance, CGConstants::GEO_ERR3D))
				return 0;
		}
	}
	return distance;
}

double CGSurface3D::Distance(const CGSurface3D &_s)const{
	double distance = std::numeric_limits<double>::max();
	for (auto t : _s.m_triangles){
		double current_dist = Distance(*t);
		if (current_dist < distance){
			distance = current_dist;
			if (CGMath::GeoZero(distance, CGConstants::GEO_ERR3D))
				return 0;
		}
	}
	return distance;
}

bool CGSurface3D::IsInside(const CGPoint3D &_pt)const{
	for (auto t : m_triangles){
		if (t->IsInside(_pt))
			return true;
	}
	return false;
}

bool CGSurface3D::IsInside(const CGTriangle3D &_t)const{
	std::vector<const CGLine3D*> edges;
	_t.GetEdges(edges);
	for (auto e : edges){
		if (!IsInside(*e))
			return false;
	}
	return true;
}

bool CGSurface3D::IsInside(const CGLine3D &_l)const{
	std::vector<CGPoint3D> intxPts;
	IntersectionPoints(_l, intxPts);
	std::vector<CGLine3D*> lines;
	_l.BreakLineAtPoints(intxPts, lines);
	bool isInside = true;
	for (auto l : lines){
		if (!IsInside(l->MidPoint()))
		{
			isInside = false;
			break;
		}
	}

	for (auto l : lines)
		delete l;
	return isInside;
}

bool CGSurface3D::IsInside(const CGSurface3D &_s)const{
	for (auto t : m_triangles){
		if (!_s.IsInside(*t)){
			return false;
		}
	}
	return true;
}

double CGSurface3D::Area()const{
	if (m_area < 0){
		m_area = 0;
		for (auto t : m_triangles)
			m_area += t->GetArea();
	}
	return m_area;
}

CGSurface3D* CGSurface3D::Merge(const CGTriangle3D &_t)const{
	std::list<CGTriangle3D*>triangles;
	triangles.push_back(_t.GetCopy());

	std::vector<CGTriangle3D*> triangles_merged;

	for (auto iter = m_triangles.begin(); iter != m_triangles.end(); ++iter){
		
		bool merged = false;
		for (auto innerIter = triangles.begin(); innerIter != triangles.end(); ++innerIter){
			if (!*innerIter)
				continue;
			std::vector<CGTriangle3D*> merged_triangles;
			(*innerIter)->Merge(**iter, merged_triangles);
			if (!merged_triangles.empty()){
				merged = true;
				delete *innerIter;
				*innerIter = nullptr;
				std::copy(merged_triangles.begin(), merged_triangles.end(), std::back_inserter(triangles));
				break;
			}
		}
		if (!merged)
			triangles_merged.push_back((*iter)->GetCopy());
	}

	for (auto t : triangles){
		if (t)
			triangles_merged.push_back(t);
	}
	return new CGSurface3D(triangles_merged);

}

CGSurface3D* CGSurface3D::Merge(const CGSurface3D &_s)const{
	std::list<CGTriangle3D*>triangles;
	std::vector<const CGTriangle3D*>st;
	_s.GetTriangles(st);
	for (auto t :st)
		triangles.push_back(t->GetCopy());

	std::vector<CGTriangle3D*> triangles_merged;

	for (auto iter = m_triangles.begin(); iter != m_triangles.end(); ++iter){

		bool merged = false;
		for (auto innerIter = triangles.begin(); innerIter != triangles.end(); ++innerIter){
			if (!*innerIter)
				continue;
			std::vector<CGTriangle3D*> merged_triangles;
			(*innerIter)->Merge(**iter, merged_triangles);
			if (!merged_triangles.empty()){
				merged = true;
				delete *innerIter;
				*innerIter = nullptr;
				std::copy(merged_triangles.begin(), merged_triangles.end(), std::back_inserter(triangles));
				break;
			}
		}
		if (!merged)
			triangles_merged.push_back((*iter)->GetCopy());
	}

	for (auto t : triangles){
		if (t)
			triangles_merged.push_back(t);
	}
	return new CGSurface3D(triangles_merged);
}

void CGSurface3D::GetTriangles(std::vector<const CGTriangle3D*> &_triangles)const{
	std::copy(m_triangles.begin(), m_triangles.end(), std::back_inserter(_triangles));
}

CGSurface3D::~CGSurface3D(){
	for (auto t : m_triangles)
		delete t;
	m_triangles.clear();
}