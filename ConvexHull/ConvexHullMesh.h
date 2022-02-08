#ifndef _CONVEX_HULL_MESH_H_
#define _CONVEX_HULL_MESH_H_

#include "Mesh/Header.h"
#include "Parser/parser.h"

using namespace DartLib;

namespace ConvexHull
{

class CMyFace_2 : public CFace_2
{
  public:
    CMyFace_2() : m_touched(false){};

    ADD_TRAIT(CPoint, normal)
    ADD_TRAIT(bool,  touched)
};

template <typename V, typename E, typename F, typename Dart>
class TConvexHullMesh : public TDynamicMesh_2<V, E, F, Dart>
{
  public:    
    void compute_normal(F* pF);
};

using CConvexHullMesh = TConvexHullMesh<CVertex_2, CEdge_2, CMyFace_2, CDart_2>;

template <typename V, typename E, typename F, typename Dart>
void TConvexHullMesh<V, E, F, Dart>::compute_normal(F* pF)
{
    CPoint p[3];
    CConvexHullMesh::CDart* pD = this->face_dart(pF);
    for (int k = 0; k < 3; k++)
    {
        p[k] = this->dart_target(pD)->point();
        pD = this->dart_next(pD);
    }

    CPoint fn = (p[1] - p[0]) ^ (p[2] - p[0]);
    pF->normal() = fn / fn.norm();
}

} // namespace ConvexHull

#endif //! _CONVEX_HULL_MESH_H_
