#ifndef _DARTLIB_DYNAMIC_MESH_2D_H_
#define _DARTLIB_DYNAMIC_MESH_2D_H_

#include <queue>
#include <vector>

#include "BaseMesh_2.h"

#define T_TYPENAME template <typename tVertex, typename tEdge, typename tFace, typename tDart>
#define T_DYNAMIC_MESH TDynamicMesh_2<tVertex, tEdge, tFace, tDart>
#define T_BASE typename TBaseMesh_2<tVertex, tEdge, tFace, tDart>

namespace DartLib
{

T_TYPENAME
class TDynamicMesh_2 : public TBaseMesh_2<tVertex, tEdge, tFace, tDart>
{
    /*===============================================================
                          Insert/Delete operation
    ===============================================================*/
    
  public:
    using T_BASE::CVertex;
    using T_BASE::CEdge;
    using T_BASE::CFace;
    using T_BASE::CDart;
    
    using T_BASE::CLoop;
    using T_BASE::CBoundary;

    using T_BASE::VertexIterator;
    using T_BASE::EdgeIterator;
    using T_BASE::FaceIterator;
    using T_BASE::DartIterator;

    using T_BASE::VertexInDartIterator;
    using T_BASE::VertexFaceIterator;
    using T_BASE::VertexEdgeIterator;
    using T_BASE::VertexVertexIterator;

    using T_BASE::FaceVertexIterator;
    using T_BASE::FaceEdgeIterator;
    
    /*!
     *  Insert a vetex, it is the same with create_vertex.
     *  \param [in] vid: the vertex id will be assigned to the new vertex
     *  \return the pointer of the created vertex
     */
    tVertex* insert_vertex(int vid);
    
    /*!
     *  Insert a new face, the difference between create_face and 
     *    insert_face is that the latter will modify the dart linking
     *    to the boundary vertex to a boundary dart.
     *  \return the pointer of the created face
     */
    tFace* insert_face(const std::vector<int>& vert_ids, int fid);

    /*!
     *  Remove a vertex.
     *    It will remove all surrounding faces and then release itself.
     *  \param [in] pV: the input vertex
     */
    void remove_vertex(CVertex* pV);

    /*!
     *  Remove an edge.
     *    It will remove all surrounding faces and then release itself.
     *  \param [in] pE: the input edge
     */
    void remove_edge(CEdge* pE);

    /*!
     *  Remove a face, this is a key operation.
     *    It will remove a face and maybe release some vertices and edges
     *    if necessary, then release itself.
     *  \return false if the mesh becomes a non-manifold after removing the face,
     *          or true if not.
     */
    bool remove_face(CFace* pF);

  protected:
    /*!
     *  Release a vertex.
     *    It only releases its memory space.
     *  \param [in] pV: the input vertex
     */
    void release_vertex(CVertex* pV);

    /*!
     *  Release an edge.
     *    It only releases its memory space.
     *  \param [in] pE: the input edge
     */
    void release_edge(CEdge* pE);

    /*!
     *  Release a face.
     *    It only releases its memory space.
     *  \param [in] pF: the input face
     */
    void release_face(CFace* pF);

    /*!
     *  Release a dart.
     *    It modifies the beta_2 function which the dart affects it,
     *    then releases its memory space.
     * 
     *  NOTE: We do not release a dart solely, often we remove a face
     *    and need to release all dart on the face.
     * 
     *  \param [in] pD: the input dart
     */
    void release_dart(CDart* pD);

    /*===============================================================
                          Dynamic operation
    ===============================================================*/
  public:
    // TODO

    //CEdge* edge_swap(CEdge* pE);
    //CVertex edge_collapse(CEdge* pE);
    //CEdge* vertex_split(CVertex* pV);
    //CVertex* edge_split(CEdge* pE);
    //CVertex* face_split(CFace* pF);
};

T_TYPENAME
tVertex* T_DYNAMIC_MESH::insert_vertex(int vid)
{
    return this->create_vertex(vid);
}

T_TYPENAME
tFace* T_DYNAMIC_MESH::insert_face(const std::vector<int>& indices, int fid)
{
    CDart* pD = this->create_face(indices, fid);

    // TODO, modify the dart linking to the boundary vertex to a boundary dart
    // 

    return this->dart_face(pD);
}

T_TYPENAME
void T_DYNAMIC_MESH::remove_vertex(CVertex* pV)
{
    for (VertexFaceIterator vfiter(pV); !vfiter.end(); ++vfiter)
    {
        CFace* pF = *vfiter;
        remove_face(pF);
    }

    release_vertex(pV); // not necessary
}

T_TYPENAME
void T_DYNAMIC_MESH::remove_edge(CEdge* pE)
{
    CFace* faces[2] = {NULL, NULL};
    for (int i = 0; i < 2; ++i)
        faces[i] = this->edge_face(pE, i);

    for (int i = 0; i < 2; ++i)
        remove_face(faces[i]);

    release_edge(pE); // not necessary
}

T_TYPENAME
bool T_DYNAMIC_MESH::remove_face(CFace* pF)
{
    if (pF == NULL)
        return true;

    // check non-manifold, if we remove face ABC, then there will 
    //   form non-manifold.
    /* 
            A------C
            / \  / \
           /   \/   \
           ----B-----       
    */
    std::vector<CVertex*> vs;
    for (FaceVertexIterator fviter(pF); !fviter.end(); ++fviter)
        vs.push_back(*fviter);

    for (int i = 0; i < vs.size(); ++i)
    {
        CVertex* b = vs[i];
        if (this->boundary(b))
        {
            CVertex* a = vs[(i - 1 + vs.size()) % vs.size()];
            CVertex* c = vs[(i + 1) % vs.size()];
            if (!this->boundary(this->vertex_dart(a, b)) &&
                !this->boundary(this->vertex_dart(b, c)))
                return false;
        }
    }

    CDart* pD0 = this->face_dart(pF);
    CDart* pD = pD0;
    std::vector<CDart*> darts;
    std::vector<CEdge*> edges;
    std::vector<CVertex*> verts;
    do
    {
        darts.push_back(pD);
        edges.push_back(this->dart_edge(pD));
        verts.push_back(this->dart_target(pD));
        pD = this->dart_next(pD);
    } while (pD != pD0);

    for (int i = 0; i < edges.size(); ++i)
    {
        CEdge* pE = edges[i];
        if (!this->boundary(darts[i]) && pE->dart() == darts[i])
            pE->dart() = this->dart_sym(darts[i]); // edge_dart(pE, 1)

        if (this->boundary(darts[i]))
            release_edge(pE);
    }

    for (auto v : verts)
    {
        bool b = this->boundary(v);
        int valance = 0;
        for (VertexFaceIterator vfiter(v); !vfiter.end(); ++vfiter)
            ++valance;

        if (std::find(darts.begin(), darts.end(), v->dart()) != darts.end())
        {
            v->dart() = v->dart()->beta(1)->beta(2);
        }
        else
        {
            if (!b)
            {
                for (auto d : darts)
                {
                    if (this->dart_target(d) == v)
                    {
                        v->dart() = this->dart_sym(this->dart_next(d));
                        break;
                    }
                }
            }
        }

        // if (v->dart() == NULL)
        if (valance == 1)
            release_vertex(v);
    }

    release_face(pF);

    for (auto d : darts)
    {
        release_dart(d);
    }

    return true;
}

T_TYPENAME
void T_DYNAMIC_MESH::release_vertex(CVertex* pV)
{
    this->m_vertices.remove(pV);
    this->m_map_vertex.erase(pV->id());

    delete pV;
    pV = NULL;
}

T_TYPENAME
void T_DYNAMIC_MESH::release_edge(CEdge* pE)
{
    if (pE == NULL)
        return;

    int vid0 = this->edge_vertex(pE, 0)->id();
    int vid1 = this->edge_vertex(pE, 1)->id();
    std::vector<int> indices = {vid0, vid1};
    this->m_map_edge_keys.erase(&indices[0]);

    this->m_edges.remove(pE);

    delete pE;
    pE = NULL;
}

T_TYPENAME
void T_DYNAMIC_MESH::release_face(CFace* pF)
{
    this->m_faces.remove(pF);
    this->m_map_face.erase(pF->id());

    delete pF;
    pF = NULL;
}

T_TYPENAME
void T_DYNAMIC_MESH::release_dart(CDart* pD)
{
    if (pD->beta(2) != NULL) // not on boundary
    {
        CDart* pSymDart = this->dart_sym(pD);
        pSymDart->beta(2) = NULL;
    }

    // necessary
    for (int i = 0; i <= 2; ++i)
        pD->cell(i) = NULL;
    for (int i = 1; i <= 2; ++i)
        pD->beta(i) = NULL;

    this->m_darts.remove(pD);
    delete pD;
    pD = NULL;
}

} // namespace DartLib
#undef T_TYPENAME
#undef T_DYNAMIC_MESH
#undef T_BASE
#endif //! _DARTLIB_DYNAMIC_MESH_2D_H_
