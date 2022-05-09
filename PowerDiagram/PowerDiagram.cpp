#include <time.h>
#include "PowerDiagram.h"

void PowerDiagram::CPowerDiagram::init(int num_pts)
{
    std::vector<CPoint*> pts;

    // 1. generate random points
    // https://mathworld.wolfram.com/DiskPointPicking.html
    srand((unsigned) time(NULL));
    double n[2];
    for (int i = 0; i < num_pts; ++i)
    {
        n[0] = rand() / double(RAND_MAX);               // [0, 1]
        n[1] = 2.0 * M_PI * rand() / double(RAND_MAX);  // [0, 2*pi]

        double x = std::sqrt(n[0]);
        double y = x;
        x *= std::cos(n[1]);
        y *= std::sin(n[1]);

        CPoint *p = new CPoint(x, y, 0);
        pts.push_back(p);
    }

    // 2. lift the points onto paraboloid z = (x^2 + y^2)/2

    //insert your code here

    // 3. feed into CPowerDiagram
    init(pts);
}

void PowerDiagram::CPowerDiagram::init(const std::vector<CPoint*>& points) 
{
    m_pts.clear();
    
    for (const auto p : points)
    {
        m_pts.push_back(p);
    }
}

void PowerDiagram::CPowerDiagram::calc_delaunay()
{
    // 1. calculate the convex hull
    CConvexHull ch;
    ch.init(m_pts);
    ch.construct();

    // 2. remove the faces with upward normal vector
    using M = CConvexHullMesh;
    std::vector<M::CFace*> removed_faces;
    CPoint up(0, 0, 1);
    for (M::FaceIterator fiter(&ch.hull()); !fiter.end(); ++fiter)
    {
        M::CFace* pF = *fiter;
        //insert your code here
    }
    
    ch.hull().remove_faces(removed_faces);

    // 3. copy the result
    m_mesh.copy(&ch.hull());
}

void PowerDiagram::CPowerDiagram::calc_voronoi() 
{ 
    for (CMesh::FaceIterator fiter(&m_mesh); !fiter.end(); ++fiter)
    {
        CMesh::CFace* pF = *fiter;
        CMesh::CDart* pD = NULL;
        pD = m_mesh.face_dart(pF);
        CPoint& a = m_mesh.dart_target(pD)->point();
        pD = m_mesh.dart_next(pD);
        CPoint& b = m_mesh.dart_target(pD)->point();
        pD = m_mesh.dart_next(pD);
        CPoint& c = m_mesh.dart_target(pD)->point();

        double x = 0, y = 0, z = 0;
        
        //insert your code here

        pF->dual_point() = CPoint(x,y,z);
    }
}
