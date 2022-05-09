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

    void to_string()
    {
        CParser parser(m_string);
        parser._removeToken("normal");

        parser._toString(m_string);
        std::stringstream iss;

        iss << "normal=(" << m_normal[0] << " " << m_normal[1] << " " << m_normal[2] << ")";

        if (m_string.length() > 0)
        {
            m_string += " ";
        }
        m_string += iss.str();
    };

    void from_string()
    {
        CParser parser(m_string);
        for (std::list<CToken*>::iterator iter = parser.tokens().begin(); iter != parser.tokens().end(); ++iter)
        {
            CToken* token = *iter;
            if (token->m_key == "normal")
            {
                token->m_value >> m_normal;
            }
        }        
    };

    ADD_TRAIT(CPoint, normal)
    ADD_TRAIT(bool,  touched)
    ADD_TRAIT(CPoint, dual_point)
};

template <typename V, typename E, typename F, typename Dart>
class TConvexHullMesh : public TDynamicMesh_2<V, E, F, Dart>
{
  public:    
    void compute_normal(F* pF);

    void remove_faces(const std::vector<F*>& faces);
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

template <typename V, typename E, typename F, typename Dart>
void TConvexHullMesh<V, E, F, Dart>::remove_faces(const std::vector<F*>& faces)
{
    if (faces.size() == 0)
        return;

    using M = TConvexHullMesh<V, E, F, Dart>;
    for (typename M::FaceIterator fiter(this); !fiter.end(); ++fiter)
    {
        F* pF = *fiter;
        pF->touched() = false;
    }

    std::vector<F*> remove_faces;
    std::queue<F*> fqueue;
    fqueue.push(faces[0]);
    faces[0]->touched() = true;
    while (!fqueue.empty())
    {
        F* pF = fqueue.front();
        fqueue.pop();
        remove_faces.push_back(pF);

        Dart* pD0 = this->face_dart(pF);
        Dart* pD = pD0;
        do
        {
            if (!this->boundary(pD))
            {
                F* pSymF = this->dart_face(this->dart_sym(pD));
                if (!pSymF->touched() && std::find(faces.begin(), faces.end(), pSymF) != faces.end())
                {
                    fqueue.push(pSymF);
                    pSymF->touched() = true;
                }
            }
            pD = this->dart_next(pD);
        } while (pD != pD0);
    }

    for (int i = 0; i < remove_faces.size(); ++i)
    {
        F* f = remove_faces[i];
        if (!this->remove_face(f))
        {
            int j = (i + 1);
            std::swap(remove_faces[i], remove_faces[j]);
            --i;
        }
    }
}

} // namespace ConvexHull

#endif //! _CONVEX_HULL_MESH_H_
