#include <CGTriangle3D.h>
#include <CGPolygon.h>
#include <CGLine3D.h>
#include <CGCuboid.h>
#include <CGAlgorithms.h>
#include<CGVector3D.h>

bool CGOutline3D::AddLine(CGLine3D*_l){
	if (m_outline.empty()){
		if (_l)
			m_outline.push_back(_l);
		return true;
	}
	bool returnVal = false;
	if (m_outline.front()->GetStart() == _l->GetStart()){
		flip();
		returnVal = true;
	}
	else if (m_outline.front()->GetStart() == _l->GetEnd()){
		flip();
		_l->flip();
		returnVal = true;
	}
	else if (m_outline.back()->GetEnd() == _l->GetStart()){
		returnVal = true;
	}
	else if (m_outline.back()->GetEnd() == _l->GetEnd()){
		_l->flip();
		returnVal = true;
	}
	if (returnVal){
		m_outline.push_back(_l);
		return true;
	}
	return false;
	
}

void CGOutline3D::donate_edges(std::vector<CGLine3D*>&_lines){
	std::copy(m_outline.begin(), m_outline.end(), std::back_inserter(_lines));
	m_outline.clear();
}

void CGOutline3D::Tesselate(std::vector<CGTriangle3D*> &_triangles, const CGTriangle3D &_t1, const CGTriangle3D &_t2){

	if (GetStart() == GetEnd() && m_outline.size() > 2){
		while (!m_outline.empty()){
			if (m_outline.size() == 3){
				CGTriangle3D *t = new CGTriangle3D(m_outline[0]->GetStart(), m_outline[0]->GetEnd(), m_outline[1]->GetEnd());
				_triangles.push_back(t);
				for (auto l : m_outline)
					delete l;
				m_outline.clear();
			}
			for (auto iter = m_outline.begin(); iter != m_outline.end(); ++iter){
				CGLine3D *l1 = *iter;
				CGLine3D *l2 = nullptr;
				auto l2_iter = iter;
				if ((iter + 1) != m_outline.end()){
					l2_iter = iter + 1;
					l2 = *(iter + 1);
				}
				else{
					l2_iter = m_outline.begin();
					l2 = m_outline.front();
				}

				CGLine3D l3(l1->GetStart(), l2->GetEnd());
				bool ear = true;
				for (auto l : m_outline){
					if (l != l1&&l != l2){
						if (l->IntersectsInMiddle(l3)){
							ear = false;
							break;
						}
					}
				}

				if (ear){
					if (_t1.IsInside(l3.MidPoint()) || _t2.IsInside(l3.MidPoint())){
						CGTriangle3D *t = new CGTriangle3D(l1->GetStart(), l1->GetEnd(), l2->GetEnd());
						_triangles.push_back(t);
						l1->SetEnd(l2->GetEnd());
						delete l2;
						l2 = nullptr;
					}
				}

				if (!l2){
					m_outline.erase(l2_iter);
					break;
				}

			}
		}
		
	}

}

void CGOutline3D::flip(){
	for (auto l:m_outline){
		l ->flip();
	}
	std::reverse(m_outline.begin(), m_outline.end());
}

const CGPoint3D& CGOutline3D::GetStart()const{
	return m_outline.front()->GetStart();
}

const CGPoint3D& CGOutline3D::GetEnd()const{
	return m_outline.back()->GetEnd();
}

bool CGOutline3D::SharesEndPoint(const CGLine3D *_l)const{
	if (!m_outline.empty())
		return m_outline.front()->GetStart() == _l->GetStart() ||
		m_outline.front()->GetStart() == _l->GetEnd() ||
		m_outline.back()->GetEnd() == _l->GetStart() ||
		m_outline.back()->GetEnd() == _l->GetEnd();
	return false;
}

bool CGOutline3D::SharesEndPoint(const CGOutline3D *_outline)const{
	if (!m_outline.empty()&&_outline->m_outline.empty())
		return m_outline.front()->GetStart() == _outline->GetStart() ||
		m_outline.front()->GetStart() == _outline->GetEnd() ||
		m_outline.back()->GetEnd() == _outline->GetStart() ||
		m_outline.back()->GetEnd() == _outline->GetEnd();
	return false;
}

bool CGOutline3D::Merge(CGOutline3D *_outline){
	if (m_outline.empty()){
		if (_outline){
			std::swap(m_outline, _outline->m_outline);
		}
		return true;
	}
	bool returnVal = false;
	if (m_outline.front()->GetStart() == _outline->GetStart()){
		flip();
		returnVal = true;
	}
	else if (m_outline.front()->GetStart() == _outline->GetEnd()){
		flip();
		_outline->flip();
		returnVal = true;
	}
	else if (m_outline.back()->GetEnd() == _outline->GetStart()){
		returnVal = true;
	}
	else if (m_outline.back()->GetEnd() == _outline->GetEnd()){
		_outline->flip();
		returnVal = true;
	}
	if (returnVal){
		std::copy(_outline->m_outline.begin(), _outline->m_outline.end(), std::back_inserter(m_outline));
		_outline->m_outline.clear();
		return true;
	}
	return false;
}

CGOutline3D::~CGOutline3D(){
	for (auto o : m_outline)
		delete o;

}

bool CGOutline3D::IsClosed()const{
	if (!m_outline.empty())
		return m_outline.front()->GetStart() == m_outline.back()->GetEnd();
	return false;
}

CGTriangle3D::~CGTriangle3D()
{
	delete m_l1;
	delete m_l2;
	delete m_l3;
}

CGTriangle3D::CGTriangle3D(const CGPoint3D &_p1, const CGPoint3D &_p2, const CGPoint3D &_p3){
	m_l1 = nullptr;
	m_l2 = nullptr;
	m_l3 = nullptr;
	Set(_p1, _p2, _p3);
}

CGTriangle3D::CGTriangle3D(const CGTriangle3D &_triangle){
	m_normal_unit_vector = _triangle.GetUnitNormalVector();
	
	m_l1 = _triangle.GetEdge1()->GetCopy();
	m_l2 = _triangle.GetEdge2()->GetCopy();
	m_l3 = _triangle.GetEdge3()->GetCopy();
}

CGTriangle3D& CGTriangle3D::operator =(const CGTriangle3D &_triangle){
	m_normal_unit_vector = _triangle.GetUnitNormalVector();
	if (!m_l1)
		m_l1 = _triangle.GetEdge1()->GetCopy();
	else
		m_l1->Set(*_triangle.GetEdge1());

	if (!m_l2){
		m_l2 = _triangle.GetEdge2()->GetCopy();
	}
	else
		m_l2->Set(*_triangle.GetEdge2());

	if (!m_l3){
		m_l3 = _triangle.GetEdge3()->GetCopy();
	}
	else
		m_l3->Set(*_triangle.GetEdge3());
	
	
	return *this;
}

const CGLine3D * CGTriangle3D::GetEdge1()const{
	return m_l1; 
}

const CGLine3D * CGTriangle3D::GetEdge2()const{
	return m_l2;
}

const CGLine3D *CGTriangle3D::GetEdge3()const{
	return m_l3;
}

const CGPoint3D& CGTriangle3D::GetP1()const{
	return m_l1->GetStart();
}

const CGPoint3D& CGTriangle3D::GetP2()const{
	return m_l1->GetEnd();
}

const CGPoint3D& CGTriangle3D::GetP3()const{
	return m_l1->IsEndPoint(m_l2->GetEnd()) ? m_l2->GetStart() : m_l2->GetEnd();
}

void CGTriangle3D::Set(const CGPoint3D&_p1, const CGPoint3D&_p2, const CGPoint3D&_p3){
	if (!m_l1){
		m_l1 = new CGLine3D(_p1, _p2);
	}
	else{
		m_l1->SetStart(_p1);
		m_l1->SetEnd(_p2);
	}
	
	if (!m_l2){
		m_l2 = new CGLine3D(_p2, _p3);
	}
	else{
		m_l2->SetStart(_p2);
		m_l2->SetEnd(_p3);
	}
	
	if (!m_l3){
		m_l3 = new CGLine3D(_p3, _p1);
	}
	else{
		m_l3->SetStart(_p3);
		m_l3->SetEnd(_p1);
	}
	
	calculate_projections();
}

void CGTriangle3D::calculate_projections(){

	m_normal_unit_vector = GetNormalVector().UnitVector();

}

void CGTriangle3D::SetEdge1(const CGLine3D &_l){
	(*m_l1) = _l;
	if (m_l2->GetStart() == m_l3->GetStart()){
		m_l2->SetEnd(m_l1->GetStart());
		m_l3->SetEnd(m_l1->GetEnd());
	}
	else if (m_l2->GetStart() == m_l3->GetEnd()){
		m_l2->SetEnd(m_l1->GetStart());
		m_l3->SetStart(m_l1->GetEnd());
	}
	else if (m_l2->GetEnd() == m_l3->GetEnd()){
		m_l2->SetStart(m_l1->GetStart());
		m_l3->SetStart(m_l1->GetEnd());
	}
	else {
		m_l2->SetStart(m_l1->GetStart());
		m_l3->SetEnd(m_l1->GetEnd());
	}

	calculate_projections();
}

void CGTriangle3D::SetEdge2(const CGLine3D &_l){
	(*m_l2) = _l;
	if (m_l1->GetStart() == m_l3->GetStart()){
		m_l1->SetEnd(m_l2->GetStart());
		m_l3->SetEnd(m_l2->GetEnd());
	}
	else if (m_l1->GetStart() == m_l3->GetEnd()){
		m_l1->SetEnd(m_l2->GetStart());
		m_l3->SetStart(m_l2->GetEnd());
	}
	else if (m_l1->GetEnd() == m_l3->GetEnd()){
		m_l1->SetStart(m_l2->GetStart());
		m_l3->SetStart(m_l2->GetEnd());
	}
	else {
		m_l1->SetStart(m_l2->GetStart());
		m_l3->SetEnd(m_l2->GetEnd());
	}
	calculate_projections();
}

void CGTriangle3D::SetEdge3(const CGLine3D &_l){
	(*m_l3) = _l;
	if (m_l1->GetStart() == m_l2->GetStart()){
		m_l1->SetEnd(m_l3->GetStart());
		m_l2->SetEnd(m_l3->GetEnd());
	}
	else if (m_l1->GetStart() == m_l2->GetEnd()){
		m_l1->SetEnd(m_l3->GetStart());
		m_l2->SetStart(m_l3->GetEnd());
	}
	else if (m_l1->GetEnd() == m_l2->GetEnd()){
		m_l1->SetStart(m_l3->GetStart());
		m_l2->SetStart(m_l3->GetEnd());
	}
	else {
		m_l1->SetStart(m_l3->GetStart());
		m_l2->SetEnd(m_l3->GetEnd());
	}
	calculate_projections();
}

void CGTriangle3D::GetVertices(std::vector<CGPoint3D> &_pts)const{
	_pts.push_back(GetP1());
	_pts.push_back(GetP2());
	_pts.push_back(GetP3());
}

double CGTriangle3D::GetArea()const{
	CGVector3D l1(m_l1->GetEnd() - m_l1->GetStart());
	CGVector3D l2(m_l2->GetEnd() - m_l2->GetStart());
	double area = std::fabs(l1.cross(l2).magnitude()) / 2.0;
	return area;
}

double CGTriangle3D::GetMinX()const{
	return std::min(std::min(m_l1->GetMinX(), m_l2->GetMinX()), m_l3->GetMinX());
}

double CGTriangle3D::GetMinY()const{
	return std::min(std::min(m_l1->GetMinY(), m_l2->GetMinY()), m_l3->GetMinY());
}

double CGTriangle3D::GetMinZ()const{
	return std::min(std::min(m_l1->GetMinZ(), m_l2->GetMinZ()), m_l3->GetMinZ());
}

double CGTriangle3D::GetMaxX()const{
	return std::max(std::max(m_l1->GetMaxX(), m_l2->GetMaxX()), m_l3->GetMaxX());
}

double CGTriangle3D::GetMaxY()const{
	return std::max(std::max(m_l1->GetMaxY(), m_l2->GetMaxY()), m_l3->GetMaxY());
}

double CGTriangle3D::GetMaxZ()const{
	return std::max(std::max(m_l1->GetMaxZ(), m_l2->GetMaxZ()), m_l3->GetMaxZ());
}

double CGTriangle3D::GetLengthAlongX()const{
	return GetMaxX() - GetMinX();
}

double CGTriangle3D::GetLengthAlongY()const{
	return GetMaxY() - GetMinY();
}

double CGTriangle3D::GetLengthAlongZ()const{
	return GetMaxZ() - GetMinZ();
}

CGPoint3D CGTriangle3D::GetCenteroid()const{
	return (GetP1() + GetP2() + GetP3()) / 3.0;
}

bool CGTriangle3D::IsInside(const CGPoint3D &_pt)const{
	if (IsOnEdge(_pt))
		return true;
	CGPoint3D pt_on_triangle = GetP1();
	if (_pt == GetP1())
		pt_on_triangle = GetP2();
	auto dir = CGVector3D(pt_on_triangle - _pt);
	dir = dir / dir.magnitude();
	if (CGMath::GeoZero(dir.dot(m_normal_unit_vector.data()), CGConstants::GEO_ERR4)){
		CGLine3D line(_pt, GetCenteroid());
		std::vector<CGPoint3D> pts;
		GetEdge1()->IntersectionPoints(line, pts);
		if (!pts.empty())
			return false;
		GetEdge2()->IntersectionPoints(line, pts);
		if (!pts.empty())
			return false;
		GetEdge3()->IntersectionPoints(line, pts);
		if (!pts.empty())
			return false;
		return true;
	}
	return false;
}

bool CGTriangle3D::IsInside(const CGLine3D &_line)const{
	if (CGMath::GeoZero(_line.UnitVector().dot(m_normal_unit_vector), CGConstants::GEO_ERR4)){

		return IsInside(_line.GetStart()) && IsInside(_line.GetEnd());

	}
	return false;
}

bool CGTriangle3D::IsInside(const CGTriangle3D &_triangle)const{
	std::vector<CGPoint3D> vertices;
	_triangle.GetVertices(vertices);

	for (auto v : vertices){
		if (!IsInside(v))
			return false;
	}

	return true;
}

bool CGTriangle3D::Overlap(const CGPoint3D &_pt)const{
	return IsInside(_pt);
}

double CGTriangle3D::Distance(const CGPoint3D &_pt)const{
	if (IsInside(_pt))
		return 0;
	double d1 = GetEdge1()->Distance(_pt);
	double d2 = GetEdge2()->Distance(_pt);
	double d3 = GetEdge3()->Distance(_pt);

	// distance to the plane 

	CGVector3D vec(_pt- GetP1());
	double d = vec.dot(GetNormal().UnitVector());

	return std::min(d,std::min(std::min(d1, d2), d3));
}

double CGTriangle3D::Distance(const CGLine3D &_line)const{
	double d1 = Distance(_line.GetStart());
	double d2= Distance(_line.GetEnd());

	double d3 = GetEdge1()->Distance(_line);
	double d4 = GetEdge2()->Distance(_line);
	double d5 = GetEdge3()->Distance(_line);

	double distance = std::min(d1, d2);
	distance = std::min(distance, d3);
	distance = std::min(distance, d4);
	distance = std::min(distance, d5);
	return distance;
}

double CGTriangle3D::Distance(const CGTriangle3D &_triangle)const{
	std::set<double> distances;
	distances.insert(GetEdge1()->Distance(*_triangle.GetEdge1()));
	distances.insert(GetEdge1()->Distance(*_triangle.GetEdge2()));
	distances.insert(GetEdge1()->Distance(*_triangle.GetEdge3()));

	distances.insert(GetEdge2()->Distance(*_triangle.GetEdge1()));
	distances.insert(GetEdge2()->Distance(*_triangle.GetEdge2()));
	distances.insert(GetEdge2()->Distance(*_triangle.GetEdge3()));

	distances.insert(GetEdge3()->Distance(*_triangle.GetEdge1()));
	distances.insert(GetEdge3()->Distance(*_triangle.GetEdge2()));
	distances.insert(GetEdge3()->Distance(*_triangle.GetEdge3()));

	return *distances.begin();
}

bool CGTriangle3D::Intersects(const CGLine3D &_line)const{
	std::vector<CGPoint3D> pts;
	IntersectionPoints(_line, pts);
	return !pts.empty();
}

bool CGTriangle3D::Intersects(const CGTriangle3D &_triangle)const{
	std::vector<CGPoint3D> pts;
	IntersectionPoints(_triangle, pts);
	return !pts.empty();
}

bool CGTriangle3D::Overlap(const CGTriangle3D &_t)const{
	std::vector<const CGLine3D*> edges1, edges2;
	GetEdges(edges1);
	_t.GetEdges(edges2);
	for (auto e : edges2){
		if (Overlap(*e))
			return true;
	}

	for (auto e : edges1){
		if (_t.Overlap(*e))
			return true;
	}

	return false;
}

bool CGTriangle3D::Overlap(const CGLine3D &_l)const{
	if (Intersects(_l))
		return true;
	if (IsInside(_l.GetStart()))
		return true;
	if (IsInside(_l.GetEnd()))
		return true;
	return false;
}
CGLine3D CGTriangle3D::GetNormal()const{
	
	auto n = GetNormalVector();
	auto center = GetCenteroid();
	return CGLine3D(CGPoint3D(0,0,0)+center, CGPoint3D(n.x(), n.y(), n.z())+center);
}

CGLine3D CGTriangle3D::GetUnitNormal()const{

	auto n = GetUnitNormalVector();
	auto center = GetCenteroid();
	return CGLine3D(CGPoint3D(0, 0, 0) + center, CGPoint3D(n.x(), n.y(), n.z()) + center);
}


CGVector3D CGTriangle3D::GetNormalVector()const{
	CGVector3D    u, v, n;              // triangle vectors
	u = CGVector3D(GetP2() - GetP1());
	v = CGVector3D(GetP3() - GetP1());
	n = u * v;
	return n;
}

CGVector3D CGTriangle3D::GetUnitNormalVector()const{
	return m_normal_unit_vector;
}
// intersect3D_RayTriangle(): find the 3D intersection of a ray with a triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 =  disjoint (no intersect)
//             1 =  intersect in unique point I1
//             2 =  are in the same plane
void
CGTriangle3D::intersect3D_RayTriangle(const CGLine3D & R, std::vector<CGPoint3D> &_pts)const
{
	CGVector3D    u, v, n;              // triangle vectors
	CGVector3D    dir, w0, w;           // ray vectors
	double     r, a, b;              // params to calc ray-plane intersect

	n = GetUnitNormalVector();              // cross product
	if (n == CGVector3D(0))             // triangle is degenerate
		return;                  // do not deal with this case

	dir = CGVector3D(R.GetEnd() - R.GetStart());              // ray direction vector
	w0 = CGVector3D(R.GetStart() - GetP1());
	a = -n.dot( w0);
	b = n.dot( dir);


	if (fabs(b) < CGConstants::GEO_ERR4) {     // ray is  parallel to triangle plane
		if (CGMath::GeoZero(a, CGConstants::GEO_ERR4)){                 // ray lies in triangle plane
			std::vector<CGPoint3D> intxPts;
			R.IntersectionPoints(*GetEdge1(), intxPts);
			std::copy(intxPts.begin(), intxPts.end(), std::back_inserter(_pts));
			intxPts.clear();

			R.IntersectionPoints(*GetEdge3(), intxPts);
			std::copy(intxPts.begin(), intxPts.end(), std::back_inserter(_pts));
			intxPts.clear();

			R.IntersectionPoints(*GetEdge2(), intxPts);
			std::copy(intxPts.begin(), intxPts.end(), std::back_inserter(_pts));
			/*if (IsInside(R.GetStart()) && IsInside(R.GetEnd())){
				_pts.push_back(R.GetStart());
				_pts.push_back(R.GetEnd());
				return;
			}*/
			return;
		}
		else 
			return ;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		return;                   // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	CGPoint3D pt = (dir*r)+R.GetStart();            // intersect point of ray and plane

	if (IsInside(pt)&&R.IsOn(pt))
		_pts.push_back(pt);
	/*// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu =u.dot(u);
	uv = u.dot( v);
	vv = v.dot( v);
	w = pt - GetP1();
	wu = w.dot( u);
	wv = w.dot( v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return 0;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return 0;

	return 1;                       // I is in T*/
}

void CGTriangle3D::IntersectionPoints(const CGLine3D &_line, std::vector<CGPoint3D> &_pts)const{

	std::vector<CGPoint3D >pts;
	intersect3D_RayTriangle(_line, pts);
	CGAlgorithms::Algorithms::RemoveDuplicates(pts);
	std::copy(pts.begin(), pts.end(), std::back_inserter(_pts));

}

void CGTriangle3D::IntersectionPoints(const CGTriangle3D &_triangle, std::vector<CGPoint3D> &_pts)const{
	std::vector<const CGLine3D*> edges1;
	std::vector<const CGLine3D*> edges2;
	GetEdges(edges1);
	_triangle.GetEdges(edges2);
	std::vector<CGPoint3D> intersections_pts;

	for (const auto & e1 : edges1){
		_triangle.IntersectionPoints(*e1, intersections_pts);
	}

	for (const auto & e2 : edges2){
		IntersectionPoints(*e2, intersections_pts);
	}

	CGAlgorithms::Algorithms::RemoveDuplicates(intersections_pts);
	std::copy(intersections_pts.begin(), intersections_pts.end(), std::back_inserter(_pts));
}

void CGTriangle3D::GetEdges(std::vector<const CGLine3D*> &_edges)const{
	_edges.push_back(m_l1);
	_edges.push_back(m_l2);
	_edges.push_back(m_l3);
}

CGTriangle3D* CGTriangle3D::GetCopy()const{
	return new CGTriangle3D(*this);
}


void CGTriangle3D::classify_edges(const CGTriangle3D &_t,
	std::vector<CGLine3D*> &_lines,
	std::map<EDGETYPE, std::vector<CGLine3D*> > &_classified)const{
	for (auto l : _lines){
		if (_t.IsOnEdge(l->GetStart())){
			if (_t.IsOnEdge(l->GetEnd())){
				if (_t.IsOnEdge(l->MidPoint())){
					_classified[EDGETYPE::INT_INT_ON].push_back(l);
				}
				else if (_t.IsInside(l->MidPoint())){
					_classified[EDGETYPE::INT_INT_IN].push_back(l);
				}
				else
					_classified[EDGETYPE::INT_INT_OUT].push_back(l);
			
			}
			else if (_t.IsInside(l->GetEnd())){
				_classified[EDGETYPE::INT_IN].push_back(l);
			}
			else 
				_classified[EDGETYPE::OUT_INT].push_back(l);
		}
		else if (_t.IsOnEdge(l->GetEnd())){
			if (_t.IsInside(l->GetStart())){
				_classified[EDGETYPE::INT_IN].push_back(l);
			}
			else
				_classified[EDGETYPE::OUT_INT].push_back(l);
		}
		else if (_t.IsInside(l->GetStart())){
			_classified[EDGETYPE::IN_IN].push_back(l);
		}
		else
			_classified[EDGETYPE::OUT_OUT].push_back(l);
	}
}
void CGTriangle3D::FilterEdges(std::set<EDGETYPE> &_edge_types,
	std::map<EDGETYPE, std::vector<CGLine3D*> > &_classified,
	std::vector<CGLine3D*> &_lines)const{

	for (auto e : _edge_types){
		auto iter = _classified.find(e);
		if (iter != _classified.end()){
			std::copy((*iter).second.begin(), (*iter).second.end(), std::back_inserter(_lines));
			(*iter).second.clear();
		}
	}
}

void CGTriangle3D::GetOutlinesForBooleanOr(const CGTriangle3D &_t, std::vector<CGOutline3D*> &_outlines)const{
	std::vector<CGPoint3D> intersectionPts;
	IntersectionPoints(_t, intersectionPts);
	CGAlgorithms::Algorithms::RemoveDuplicates(intersectionPts);
	std::vector<const CGLine3D*> edges, edges2;
	GetEdges(edges);
	_t.GetEdges(edges2);


	std::vector<CGLine3D*> final_Edges, final_edges2;
	for (const auto &e : edges){
		std::vector<CGPoint3D> pts;
		for (const auto &p : intersectionPts){
			if (e->IsOn(p))
				pts.push_back(p);
		}
		e->BreakLineAtPoints(pts, final_Edges);
	}

	for (const auto &e : edges2){
		std::vector<CGPoint3D> pts;
		for (const auto &p : intersectionPts){
			if (e->IsOn(p))
				pts.push_back(p);
		}
		e->BreakLineAtPoints(pts, final_edges2);
	}

	std::map<EDGETYPE, std::vector<CGLine3D* > >classified_edges1, classified_edges2;
	classify_edges(_t, final_Edges, classified_edges1);
	classify_edges(*this, final_edges2, classified_edges2);

	std::set<EDGETYPE> filters;
	filters.insert(EDGETYPE::INT_INT_OUT);
	filters.insert(EDGETYPE::OUT_INT);
	filters.insert(EDGETYPE::OUT_OUT);

	std::vector<CGLine3D*> selected_edges;
	FilterEdges(filters, classified_edges1, selected_edges);
	filters.insert(EDGETYPE::INT_INT_ON);
	FilterEdges(filters, classified_edges2, selected_edges);
	for (auto iter = selected_edges.begin(); iter != selected_edges.end(); ++iter){
		auto outline = new CGOutline3D();
		outline->AddLine(*iter);
		_outlines.push_back(outline);
	}

	for (auto e : classified_edges1){
		for (auto l : e.second)
			delete l;
	}
	for (auto e : classified_edges2){
		for (auto l : e.second)
			delete l;
	}
}

void CGTriangle3D::GetLinesForBooleanOr(const CGTriangle3D &_t, std::vector<CGLine3D*> &_outlines)const{
	std::vector<CGOutline3D*> outlines;
	GetOutlinesForBooleanOr(_t,outlines);
	for (auto o : outlines){
		o->donate_edges(_outlines);
		delete o;
	}
}

void CGTriangle3D::BreakTriangle(CGLine3D _l,
	std::vector<CGTriangle3D*> &_r)const{
	CGLine3D l = _l;
	std::vector<CGPoint3D> intx;
	intx.clear();
	double max_length = std::max(std::max(m_l1->GetLength(), m_l2->GetLength()), m_l3->GetLength());
	l.extend(std::ceil(max_length) + 1);
	l.extend(std::ceil(max_length) + 1, false);
	IntersectionPoints(l, intx);
	
	if (intx.size() != 2)
		throw "unhandled condition";
	std::vector<CGPoint3D> pt1, pt2;
	const CGLine3D *l1 = nullptr, *l2 = nullptr, *l3 = nullptr;
	if (m_l1->IsOn(intx[0]) && !m_l1->IsEndPoint(intx[0])){
		l1 = m_l1;
	}

	if (m_l1->IsOn(intx[1]) && !m_l1->IsEndPoint(intx[1])){
		l2 = m_l1;
	}

	if (m_l2->IsOn(intx[0]) && !m_l2->IsEndPoint(intx[0])){
		l1 = m_l2;
	}

	if (m_l2->IsOn(intx[1]) && !m_l2->IsEndPoint(intx[1])){
		l2 = m_l2;
	}

	if (m_l3->IsOn(intx[0]) && !m_l3->IsEndPoint(intx[0])){
		l1 = m_l3;
	}

	if (m_l3->IsOn(intx[1]) && !m_l3->IsEndPoint(intx[1])){
		l2 = m_l3;
	}
	if (!l1 || !l2)
		return;
	pt1.push_back(intx[0]);
	pt1.push_back(intx[1]);

	pt2.push_back(intx[0]);
	pt2.push_back(intx[1]);

	if (l1->GetStart() == l2->GetStart()){
		pt1.push_back(l1->GetStart());
		pt2.push_back(l2->GetEnd());
		pt2.push_back(l1->GetEnd());
	}
	else if (l1->GetStart() == l2->GetEnd()){
		pt1.push_back(l1->GetStart());
		pt2.push_back(l2->GetStart());
		pt2.push_back(l1->GetEnd());
	}
	else if (l1->GetEnd() == l2->GetStart()){
		pt1.push_back(l1->GetEnd());
		pt2.push_back(l2->GetEnd());
		pt2.push_back(l1->GetStart());
	}
	else if (l1->GetEnd() == l2->GetEnd()){
		pt1.push_back(l1->GetStart());
		pt2.push_back(l2->GetStart());
		pt2.push_back(l1->GetStart());
	}
	else
		throw "unhandled condition";

	_r.push_back(new CGTriangle3D(pt1[0], pt1[1], pt1[2]));
	_r.push_back(new CGTriangle3D(pt2[0], pt2[1], pt2[2]));
	_r.push_back(new CGTriangle3D(pt2[0], pt2[2], pt2[3]));
}
void CGTriangle3D::Merge_out_of_plane(const CGTriangle3D &_t,
	std::vector<CGTriangle3D*>&_result)const{
	std::vector<CGPoint3D> intx;
	IntersectionPoints(_t, intx);
	if (intx.size() == 0||intx.size()==1){
		return;
	}
	if (intx.size() != 2)
		throw" unhandled condition";
	CGLine3D l(intx[0], intx[1]);
	BreakTriangle(l, _result);
	_t.BreakTriangle(l, _result);
}

void CGTriangle3D::Merge_coplanar(const CGTriangle3D &_t,
	std::vector<CGTriangle3D*> &_result)const{
	std::vector<CGOutline3D*> outlines;
	GetOutlinesForBooleanOr(_t, outlines);
	std::vector<CGOutline3D*>completed_outlines;
	while (true){
		for (auto iter = outlines.begin(); iter != outlines.end(); ++iter){
			if (*iter){
				for (auto innerIter = outlines.begin(); innerIter != outlines.end(); ++innerIter){
					if (iter != innerIter && (*innerIter)){
						if ((*iter)->Merge(*innerIter)){
							delete *innerIter;
							*innerIter = nullptr;
						}
					}
				}
			}
		}
		bool continueLoop = false;
		for (auto iter = outlines.begin(); iter != outlines.end(); ++iter){
			if ((*iter)){
				if ((*iter)->GetStart() == (*iter)->GetEnd()){
					completed_outlines.push_back(*iter);
					*iter = nullptr;
				}
				else
					continueLoop = true;
			}
		}
		if (!continueLoop)
			break;
	}

	for (auto o : completed_outlines){
		if (o){
			o->Tesselate(_result, *this, _t);
			delete o;
		}
	}
}
void CGTriangle3D::Merge(const CGTriangle3D &_t,
	std::vector<CGTriangle3D*>&_result)const{
	
	if (!Overlap(_t))
		return;
	auto unit_vector = GetUnitNormalVector();
	auto unit_vector2 = _t.GetUnitNormalVector();

	if (!CGMath::GeoZero(unit_vector.cross(unit_vector2).magnitude(), CGConstants::GEO_ERR4)){
		Merge_out_of_plane(_t, _result);
		return;
	}

	Merge_coplanar(_t, _result);

}

double CGTriangle3D::angle(const CGTriangle3D &_triangle)const{

	auto cos_theta = GetUnitNormalVector().dot(_triangle.GetUnitNormalVector());
	return std::acos(cos_theta);

}

bool CGTriangle3D::IsOnEdge(const CGPoint3D &_pt)const{

	return m_l1->IsOn(_pt) || m_l2->IsOn(_pt) || m_l3->IsOn(_pt);
}