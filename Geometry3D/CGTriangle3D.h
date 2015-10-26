#ifndef CGTRIANGLE3D_H
#define CGTRIANGLE3D_H
#include <CGPoint3D.h>
#include <CGVector3D.h>

class CGPolygon;
class CGTriangle3D;
class CGLine3D;
class CGOutline3D{
	std::vector<CGLine3D*> m_outline;

public:

	bool AddLine(CGLine3D*_l);

	bool SharesEndPoint(const CGLine3D *_l)const;
	
	bool SharesEndPoint(const CGOutline3D *_outline)const;

	const CGPoint3D& GetStart()const;

	const CGPoint3D& GetEnd()const;

	bool Merge(CGOutline3D *_outline);

	bool IsClosed()const;

	void flip();

	void Tesselate(std::vector<CGTriangle3D*> &_triangles, const CGTriangle3D &_t1, const CGTriangle3D &_t2);

	void donate_edges(std::vector<CGLine3D*>&_lines);

	~CGOutline3D();
};
class CGTriangle3D
{
	enum EDGETYPE{ OUT_OUT, OUT_INT, INT_INT_IN, INT_INT_OUT, INT_INT_ON, INT_IN, IN_IN, MIXED, UNKNOWN };

	CGLine3D *m_l1;
	CGLine3D *m_l2;
	CGLine3D *m_l3;

	CGVector3D m_normal_unit_vector;

	void calculate_projections();

	void intersect3D_RayTriangle(const CGLine3D & R, std::vector<CGPoint3D> &_pts)const;

	void classify_edges(const CGTriangle3D &_t,
		std::vector<CGLine3D*> &_lines,
		std::map<EDGETYPE,std::vector<CGLine3D*> > &_classified)const;
	
	void FilterEdges(std::set<EDGETYPE> &_edge_types,
		std::map<EDGETYPE, std::vector<CGLine3D*> > &_classified,
		std::vector<CGLine3D*> &_lines)const;

	void Merge_out_of_plane(const CGTriangle3D &_t,
		std::vector<CGTriangle3D*>&_result)const;

	void Merge_coplanar(const CGTriangle3D &_t,
		std::vector<CGTriangle3D*> &_result)const;
public:
	CGTriangle3D(const CGPoint3D&, const CGPoint3D&, const CGPoint3D&);

	CGTriangle3D(const CGTriangle3D &);

	CGTriangle3D& operator =(const CGTriangle3D &);

	const CGPoint3D& GetP1()const;

	const CGPoint3D& GetP2()const;

	const CGPoint3D& GetP3()const;

	const CGLine3D * GetEdge1()const;

	const CGLine3D * GetEdge2()const;

	const CGLine3D * GetEdge3()const;

	void Set(const CGPoint3D&, const CGPoint3D&, const CGPoint3D&);

	void SetEdge1(const CGLine3D &_l);

	void SetEdge2(const CGLine3D &_l);

	void SetEdge3(const CGLine3D &_l);

	CGTriangle3D* GetCopy()const;

	void GetVertices(std::vector<CGPoint3D> &_pts)const;

	double GetArea()const;

	double GetMinX()const;

	double GetMinY()const;

	double GetMinZ()const;

	double GetMaxX()const;

	double GetMaxY()const;

	double GetMaxZ()const;

	double GetLengthAlongX()const;

	double GetLengthAlongY()const;

	double GetLengthAlongZ()const;

	CGPoint3D GetCenteroid()const;

	bool IsInside(const CGPoint3D &)const;

	bool IsInside(const CGLine3D &)const;

	bool IsInside(const CGTriangle3D &)const;

	bool Overlap(const CGPoint3D &)const;

	bool Overlap(const CGLine3D &)const;

	bool Overlap(const CGTriangle3D &)const;

	double Distance(const CGPoint3D &)const;

	double Distance(const CGLine3D &)const;

	double Distance(const CGTriangle3D &)const;

	bool Intersects(const CGLine3D &)const;

	bool Intersects(const CGTriangle3D &)const;

	void IntersectionPoints(const CGLine3D &, std::vector<CGPoint3D> &)const;

	void IntersectionPoints(const CGTriangle3D &, std::vector<CGPoint3D> &)const;

	CGLine3D GetNormal()const;

	CGLine3D GetUnitNormal()const;

	CGVector3D GetNormalVector()const;

	CGVector3D GetUnitNormalVector()const;

	void GetEdges(std::vector<const CGLine3D*> &_edges)const;

	void Merge(const CGTriangle3D &_t,
		std::vector<CGTriangle3D*>&_result)const;

	bool IsOnEdge(const CGPoint3D &_pt)const;

	void BreakTriangle(CGLine3D _l,
		std::vector<CGTriangle3D*> &_r)const;

	void GetOutlinesForBooleanOr(const CGTriangle3D &_t, std::vector<CGOutline3D*> &_outlines)const;

	void GetLinesForBooleanOr(const CGTriangle3D &_t, std::vector<CGLine3D*> &_outlines)const;

	double angle(const CGTriangle3D &_triangle)const;

	~CGTriangle3D();
};
#endif

