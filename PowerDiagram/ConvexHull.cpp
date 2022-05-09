#include <stdlib.h>
#include <time.h>

#include "ConvexHull.h"

namespace ConvexHull
{
CConvexHull::CConvexHull() : m_max_vertex_id(0), m_max_face_id(0)
{
}

void CConvexHull::init(size_t num_pts)
{
    // 1. randomly generate the input points
    srand((unsigned) time(NULL)); 
    double n[3];
    for (int i = 0; i < num_pts; ++i)
    {
        n[0] = rand() / double(RAND_MAX);
        n[1] = rand() / double(RAND_MAX);
        n[2] = 2.0 * M_PI * rand() / double(RAND_MAX);
        
        // refer: http://datagenetics.com/blog/january32020/index.html
        double r = std::pow(n[0], 1.0 / 3.0);
        double phi = std::acos((2.0 * n[1]) - 1.0);
        double theta = n[2];
        double x = r * std::sin(phi) * std::cos(theta);
        double y = r * std::sin(phi) * std::sin(theta);
        double z = r * std::cos(phi);

        CPoint* p = new CPoint(x, y, z);
        //(*p) /= p->norm();
        m_sites.push_back(p);
    }

    // 2. construst an initial convex hull with double faces
    using M = CConvexHullMesh;
    m_max_vertex_id = 0;
    m_max_face_id   = 0;

    std::vector<int> face_vids;
    for (int i = 0; i < 3; ++i)
    {
        CPoint* p = m_sites[i];
        M::CVertex * pV = m_pMesh.insert_vertex(++m_max_vertex_id);
        pV->point() = *p;
    }
    
    M::CFace* pF = NULL;
    face_vids = {1, 2, 3};
    pF = m_pMesh.insert_face(face_vids, ++m_max_face_id);
    m_pMesh.compute_normal(pF);
    face_vids = {2, 1, 3};
    pF = m_pMesh.insert_face(face_vids, ++m_max_face_id);
    m_pMesh.compute_normal(pF);
}

void CConvexHull::init(std::vector<CPoint*>& sites) 
{
    // 1. read the sites
    for (int i = 0; i < sites.size(); ++i)
    {
        CPoint* p = sites[i];
        m_sites.push_back(p);
    }

    // 2. construst an initial convex hull with double faces
    using M = CConvexHullMesh;
    m_max_vertex_id = 0;
    m_max_face_id = 0;

    std::vector<int> face_vids;
    for (int i = 0; i < 3; ++i)
    {
        CPoint* p = m_sites[i];
        M::CVertex* pV = m_pMesh.insert_vertex(++m_max_vertex_id);
        pV->point() = *p;
    }

    M::CFace* pF = NULL;
    face_vids = {1, 2, 3};
    pF = m_pMesh.insert_face(face_vids, ++m_max_face_id);
    m_pMesh.compute_normal(pF);
    face_vids = {2, 1, 3};
    pF = m_pMesh.insert_face(face_vids, ++m_max_face_id);
    m_pMesh.compute_normal(pF);
}

void CConvexHull::insert(const CPoint& p) 
{
    if (!_inside(p))
    {
        _remove_visiable(p);
        _close_cap(p);
    }
}

void CConvexHull::construct()
{
    for (int i = 3; i < m_sites.size(); ++i)
    {
        const auto& p = m_sites[i];
        insert(*p);
        printf("\r%.2f%%", 100.0 * (i + 1) / m_sites.size());
    }
    printf("\n");
}

int CConvexHull::_volume_sign(CConvexHullMesh::CFace* f, const CPoint& p) 
{
    CConvexHullMesh::CVertex* v[3];
    CConvexHullMesh::CDart* pD = m_pMesh.face_dart(f);
    for (int i = 0; i < 3; ++i)
    {
        v[i] = m_pMesh.dart_vertex(pD);
        pD   = m_pMesh.dart_next(pD);
    }

    //insert your code here
    return -1;
}

bool CConvexHull::_inside(const CPoint& p)
{ 
    using M = CConvexHullMesh;
    //insert your code here
    return true;
}

void CConvexHull::_remove_visiable(const CPoint& p) 
{
    using M = CConvexHullMesh;
    std::vector<M::CFace*> visiable_faces;
    //insert your code here
    _remove_faces(visiable_faces);
}

void CConvexHull::_remove_faces(const std::vector<CConvexHullMesh::CFace*>& faces) 
{
    if (faces.size() == 0) return;

    using M = CConvexHullMesh;
    for (M::FaceIterator fiter(&m_pMesh); !fiter.end(); ++fiter)
    {
        M::CFace* pF = *fiter;
        pF->touched() = false;
    }

    std::vector<M::CFace*> remove_faces;
    std::queue<M::CFace*> fqueue;
    fqueue.push(faces[0]);
    faces[0]->touched() = true;
    while (!fqueue.empty())
    {
        M::CFace* pF = fqueue.front();
        fqueue.pop();
        remove_faces.push_back(pF);

        M::CDart* pD0 = m_pMesh.face_dart(pF);
        M::CDart* pD = pD0;
        do
        {
            if (!m_pMesh.boundary(pD))
            {
                M::CFace* pSymF = m_pMesh.dart_face(m_pMesh.dart_sym(pD));
                if (!pSymF->touched() && std::find(faces.begin(), faces.end(), pSymF) != faces.end())
                {
                    fqueue.push(pSymF);
                    pSymF->touched() = true;
                }
            }
            pD = m_pMesh.dart_next(pD);
        } while (pD != pD0);
    }

    for (int i = 0; i < remove_faces.size(); ++i)
    {
        M::CFace* f = remove_faces[i];
        if (!m_pMesh.remove_face(f))
        {
            int j = (i + 1);
            std::swap(remove_faces[i], remove_faces[j]);
            --i;
        }
    }
}

void CConvexHull::_close_cap(const CPoint& p)
{
    using M = CConvexHullMesh;

    M::CVertex* pV = m_pMesh.insert_vertex(++m_max_vertex_id);
    pV->point() = p;

    M::CBoundary boundary(&m_pMesh);
    auto loops = boundary.loops();
    assert(loops.size() == 1);

    auto loop = loops[0];
    for (auto pD : loop->halfedges())
    {
        M::CVertex* pS = m_pMesh.dart_source(pD);
        M::CVertex* pT = m_pMesh.dart_target(pD);

        //insert your code here
        //using function m_pMesh.insert_face
    }
}

}
