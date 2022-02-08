#ifndef _DARTLIB_BOUNDARY_3_H_
#define _DARTLIB_BOUNDARY_3_H_

#include <string>
#include <set>
#include <map>
#include <vector>
#include <fstream>

#include "Iterators_3.h"
#include "Iterators_2.h"
#include "../Geometry/Point.h"

namespace DartLib
{

/*===================================================================
  Usage:

    M3 tmesh;
    M2 surface;
    std::vector<typename M3::CFace*> faces;

    CBoundary<M3> boundary(&tmesh);

    boundary.exact_surface<M2>(surface);    // method one
    boundary.exact_surface(surface);        //   or omit the template

    boundary.exact_surface(faces);          // method two
    
    faces = boundary.faces();               // method three
===================================================================*/

/*! 
 *  \class TBoundary_3 BaseMesh_3.h "BaseMesh_3.h"
 *  \brief TBaseMesh_3, base class for all types of 3d-mesh classes
 *
 *  \tparam CVertex vertex class, derived from DartLib::CVertex class
 *  \tparam CEdge   edge   class, derived from DartLib::CEdge   class
 *  \tparam CFace   face   class, derived from DartLib::CFace   class
 *  \tparam CVolume volume class, derived from DartLib::CVolume class
 *  \tparam CDart   dart   class, derived from DartLib::CDart   class
 */
template <class M3>
class TBoundary_3
{
  public:
    TBoundary_3(M3* pMesh);

    template <class M2>
    void exact_surface(M2& surface);

    void exact_surface(std::vector<typename M3::CFace*>& faces);

    std::vector<typename M3::CFace*>& faces() { return m_faces; };

  protected:
    M3* m_pMesh;

    std::vector<typename M3::CFace*> m_faces;
};

template <class M3>
TBoundary_3<M3>::TBoundary_3(M3* pMesh)
{
    m_pMesh = pMesh;

    for (Dim3::FaceIterator<M3> fiter(m_pMesh); !fiter.end(); ++fiter)
    {
        typename M3::CFace* pF = *fiter;
        if (m_pMesh->boundary(pF))
            m_faces.push_back(pF);
    }
}

template <class M3>
template <class M2>
void TBoundary_3<M3>::exact_surface(M2& surface)
{
    std::vector<CPoint>           points;
    std::vector<std::vector<int>> faces;

    std::map<int, int>            vert_id_idx; // vid  -> vidx

    // collect vertices
    std::set<typename M3::CVertex*> vSet;
    for (auto pF : m_faces)
    {
        for (Dim3::FaceVertexIterator<M3> fviter(pF); !fviter.end(); ++fviter)
        {
            typename M3::CVertex* pV = *fviter;
            vSet.insert(pV);
        }
    }

    // collect points
    for (auto pV : vSet)
    {
        int vidx = points.size();
        points.push_back(pV->point());
        vert_id_idx.insert(std::make_pair(pV->id(), vidx));
    }

    // collect face -> vertex indeces
    for (auto pF : m_faces)
    {
        std::vector<int> vert_indices;
        for (Dim3::FaceVertexIterator<M3> fviter(pF); !fviter.end(); ++fviter)
        {
            typename M3::CVertex* pV = *fviter;
            int vidx = vert_id_idx[pV->id()];
            vert_indices.push_back(vidx);
        }
        faces.push_back(vert_indices);
    }

    // build the surface
    surface.load(points, faces);

    // assign vertex id and string for surface mesh
    for (auto id_idx : vert_id_idx)
    {
        int vid  = id_idx.first;
        int vidx = id_idx.second;

        typename M2::CVertex* v_2 = surface.vertex(vidx);
        typename M3::CVertex* v_3 = m_pMesh->id_vertex(vid);

        v_2->id() = vid;

        v_3->to_string();   // update string attribute
        v_2->string() = v_3->string();
        v_2->from_string(); // update attributes from the string
    }

    // assign face id for surface mesh
    int fid = 1;
    for (auto pF : surface.faces())
    {
        pF->id() = fid++;
    }
}

template <class M3>
void TBoundary_3<M3>::exact_surface(std::vector<typename M3::CFace*>& faces)
{
    for (auto pF : m_faces)
    {
        faces.push_back(pF);
    }
}

} // namespace DartLib
#endif // !_DARTLIB_BOUNDARY_3_H_
