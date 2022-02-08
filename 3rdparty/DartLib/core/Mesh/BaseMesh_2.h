#ifndef _DARTLIB_MESH_2D_H_
#define _DARTLIB_MESH_2D_H_

#include <time.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <tuple>
#include <queue>
#include <string>
#include <fstream>

#include "../Geometry/Point.h"
#include "../Parser/strutil.h"
#include "../Utils/IO.h"
#include "Iterators_2.h"
#include "Boundary_2.h"

#include "MapKeys.h"

#define MAX_LINE 1024

#define T_TYPENAME template <typename tVertex, typename tEdge, typename tFace, typename tDart>
#define T_BASEMESH TBaseMesh_2<tVertex, tEdge, tFace, tDart> 

namespace DartLib
{
/*! \class TBaseMesh_2 BaseMesh_2.h "BaseMesh_2.h"
 *  \brief TBaseMesh_2, base class for all types of 2d-mesh classes
 *
 *  \tparam tVertex: vertex class, derived from DartLib::CVertex_2 class
 *  \tparam tEdge  : edge   class, derived from DartLib::CEdge_2   class
 *  \tparam tFace  : face   class, derived from DartLib::CFace_2   class
 *  \tparam tDart  : dart   class, derived from DartLib::CDart_2   class
 */
T_TYPENAME
class TBaseMesh_2
{
  public:
    using CVertex              = tVertex;
    using CEdge                = tEdge;
    using CFace                = tFace;
    using CDart                = tDart;
    
    using CLoop                = TLoop      <T_BASEMESH>;
    using CBoundary            = TBoundary_2<T_BASEMESH>;

    using VertexIterator       = Dim2::VertexIterator<T_BASEMESH>;
    using EdgeIterator         = Dim2::EdgeIterator  <T_BASEMESH>;
    using FaceIterator         = Dim2::FaceIterator  <T_BASEMESH>;
    using DartIterator         = Dim2::DartIterator  <T_BASEMESH>;

    using VertexInDartIterator = Dim2::VertexInDartIterator<T_BASEMESH>;
    using VertexFaceIterator   = Dim2::VertexFaceIterator  <T_BASEMESH>;
    using VertexEdgeIterator   = Dim2::VertexEdgeIterator  <T_BASEMESH>;
    using VertexVertexIterator = Dim2::VertexVertexIterator<T_BASEMESH>;

    using FaceVertexIterator   = Dim2::FaceVertexIterator  <T_BASEMESH>;
    using FaceEdgeIterator     = Dim2::FaceEdgeIterator    <T_BASEMESH>;
    
    /*!
     *  Constructor function
     */
    TBaseMesh_2(){};

    /*!
     *  Destructor function
     */
    ~TBaseMesh_2() { unload(); };

    /*=============================================================
                   Build the dart data structure
    =============================================================*/
    
    /*!
     *  Unload data
     */
    void unload();

    /*!
     *  Load mesh from the input file
     *  \param input: the input name of the mesh file
     */
    void read(const std::string& input);

    /*!
     *  Write mesh to the output file
     *  \param output: the output name of the mesh file
     */
    void write(const std::string& output);

    /*!
     *  Load mesh from point cloud and faces
     *  \param vert_id_point: a map with key(vid) and value(point)
     *  \param  face_id_vids: a map with key(fid) and value(the vertex ids to construct faces)
     */
    void load(const std::map<int, CPoint>& vert_id_point, 
              const std::map<int, std::vector<int>>& face_id_vids);

    /*!
     *  Load attributes for darts and cells
     *  \param vert_id_idx: vertex id -> vertex index
     *  \param vert_id_str: vertex id -> vertex string
     *  \param face_id_idx:   face id -> face index
     *  \param face_id_str:   face id -> face string
     *  \param  edge_attrs:   (vid1, vid2) -> string
     *  \param corner_attrs:  (vid,   fid) -> string
     */
    void load_attributes(
        const std::map<int, std::string>& vert_id_str,
        const std::map<int, std::string>& face_id_str,
        const std::vector<std::tuple<int, int, std::string>>& edge_attrs,
        const std::vector<std::tuple<int, int, std::string>>& corner_attrs);
 
    /*!
     *  Create a vertex cell.
     *  The vertex dart will be set in the processing of 
     *    creating 1-cell, so it returns a vertex pointer here.
     *  \param vid: vertex id
     *  \return the added vertex pointer
     */
    CVertex* create_vertex(int vid);

    /*!
     *  Create a face cell
     *  \param indices: indices of the vertices to construct a face
     *  \param     fid: face id
     *  \return a dart pointer of the face
     */
    CDart* create_face(const std::vector<int>& indices, int fid);

    /*!
     *  Create a edge cell
     *  \param pF: face pointer the edge attaches to.
     *  \param indices: indices of the vertices to construct an edge
     *  \return a dart pointer of the edge
     */
    CDart* create_edge(CFace* pF, const std::vector<int>& indices);

    /*!
     *  Link the two faces
     *  \param f1: index of the first face
     *  \param f2: index of the second face
     */
    void link_faces(CDart* f1, CDart* f2);

    /*!
     *  Link the two edges
     *  \param e1: index of the first edge
     *  \param e2: index of the second edge
     *
     */
    void link_edges(CDart* e1, CDart* e2);


    /*=============================================================
                 Access dart and i-cell from index or id

        Recommend to use iterators instead of the following usage.
            for(int i = 0; i < darts().size(); ++i)
            {
                CDart * pD = dart(i);
                ...
            }
    =============================================================*/
    /*
    CDart*   dart  (int index) { return m_darts[index];    };
    CVertex* vertex(int index) { return m_vertices[index]; };
    CEdge*   edge  (int index) { return m_edges[index];    };
    CFace*   face  (int index) { return m_faces[index];    };
   */
    CVertex* id_vertex(int id) { return m_map_vertex[id];  };
    CFace*   id_face  (int id) { return m_map_face[id];    };


    /*=============================================================
                   Access container of dart and i-cell

        Recommend to use iterators instead of the container.
      =============================================================*/

    std::list<CDart*>  & darts()   { return m_darts;   };
    std::list<CVertex*>& vertices(){ return m_vertices;};
    std::list<CEdge*>  & edges()   { return m_edges;   };
    std::list<CFace*>  & faces()   { return m_faces;   };

    std::unordered_map<int, CVertex*>& map_vertex(){ return m_map_vertex;};
    std::unordered_map<int,   CFace*>& map_face()  { return m_map_face;  };
    
    
    /*=============================================================
                Access dart from i-cell and vice versa
                              (Part I)
    =============================================================*/
    
    CVertex*C0(CDart* dart)  { return dart != NULL ? (CVertex*)dart->cell(0) : NULL;};
    CEdge*  C1(CDart* dart)  { return dart != NULL ? (CEdge*)  dart->cell(1) : NULL;};
    CFace*  C2(CDart* dart)  { return dart != NULL ? (CFace*)  dart->cell(2) : NULL;};
    CDart*  D (CVertex* vert){ return vert != NULL ? (CDart*)  vert->dart()  : NULL;};
    CDart*  D (CEdge*   edge){ return edge != NULL ? (CDart*)  edge->dart()  : NULL;};
    CDart*  D (CFace*   face){ return face != NULL ? (CDart*)  face->dart()  : NULL;};

    /*!
     *  The beta function in combinatorial map
     *  \param i: represents for function beta_i, i \in {1, 2}
     *  \param dart: the input dart
     */
    CDart* beta(int i, CDart* dart) { return (CDart*) dart->beta(i); };


    /*=============================================================
                Access dart from i-cell and vice versa
                              (Part II)
    =============================================================*/

    CVertex* dart_vertex(CDart* dart) { return C0(dart); };
    CVertex* dart_target(CDart* dart) { return C0(dart); };
    CVertex* dart_source(CDart* dart);
    CEdge  * dart_edge  (CDart* dart) { return C1(dart); };
    CFace  * dart_face  (CDart* dart) { return C2(dart); };
    CDart  * dart_prev  (CDart* dart);
    CDart  * dart_next  (CDart* dart) { return beta(1, dart); };
    CDart  * dart_sym   (CDart* dart) { return beta(2, dart); };
    CDart  * dart       (CVertex* vert, CFace* face);

    CEdge  * vertex_edge(CVertex* v0, CVertex* v1);
    CDart  * vertex_dart(CVertex* vert) { return D(vert); };
    CDart  * vertex_dart(CVertex* source, CVertex* target);
    
    CVertex* edge_vertex(CEdge* edge, int index);
    CFace  * edge_face  (CEdge* edge, int index);
    CDart  * edge_dart  (CEdge* edge, int index);

    CDart  * face_dart  (CFace* face) { return D(face); };


    /*=============================================================
                   Access neighboring dart or i-cell
    =============================================================*/

    CDart* vertex_most_clw_out_dart(CVertex* vertex);
    
    /*=============================================================
                         Boundary detectors
    =============================================================*/

    bool boundary(CDart* dart  ){ return dart->boundary();    };
    bool boundary(CEdge* edge  ){ return D(edge)->boundary(); };
    bool boundary(CVertex* vert){ return boundary(D(vert));   };

    /*=============================================================
                    Number of all kinds of element
    =============================================================*/

    int num_vertices() { return m_vertices.size(); };
    int num_edges()    { return m_edges.size();    };
    int num_faces()    { return m_faces.size();    };
    int num_darts()    { return m_darts.size();    };


    /*=============================================================
                          Metric information
    =============================================================*/
    
    double edge_length(CEdge* edge);

    /*=============================================================
                       Alias methods for MeshLib
    =============================================================*/
    typedef CDart                CHalfEdge;

    typedef VertexIterator       MeshVertexIterator;
    typedef EdgeIterator         MeshEdgeIterator;
    typedef FaceIterator         MeshFaceIterator;
    typedef DartIterator         MeshDartIterator;
    typedef DartIterator         MeshHalfEdgeIterator;
    typedef VertexInDartIterator VertexInHalfEdgeIterator;

    CVertex* idVertex(int id) { return id_vertex(id); };
    CFace*   idFace  (int id) { return id_face(id);   };

    CVertex* halfedgeVertex(CDart* d) { return dart_vertex(d);   };
    CVertex* halfedgeTarget(CDart* d) { return dart_target(d);   };
    CVertex* halfedgeSource(CDart* d) { return dart_source(d);   };
    CEdge  * halfedgeEdge  (CDart* d) { return dart_edge(d);     };
    CFace  * halfedgeFace  (CDart* d) { return dart_face(d);     };
    CDart  * halfedgePrev  (CDart* d) { return dart_prev(d);     };
    CDart  * halfedgeNext  (CDart* d) { return dart_next(d);     };
    CDart  * halfedgeSym   (CDart* d) { return dart_sym(d);      };
    CDart  * halfedge      (CVertex* v, CFace* f) { return dart(v, f); };
    CDart  * corner        (CVertex* v, CFace* f) { return dart(v, f); };

    CEdge* vertexEdge    (CVertex* v0, CVertex* v1){ return vertex_edge(v0, v1);};
    CDart* vertexHalfedge(CVertex* v)              { return vertex_dart(v);     };
    CDart* vertexHalfedge(CVertex* s,  CVertex* t ){ return vertex_dart(s, t);  };
    
    CVertex* edgeVertex  (CEdge* e, int i){ return edge_vertex(e, i); };
    CFace  * edgeFace    (CEdge* e, int i){ return edge_face  (e, i); };
    CDart  * edgeHalfedge(CEdge* e, int i){ return edge_dart  (e, i); };

    CDart  * faceHalfedge(CFace* f)       { return face_dart(f);      };

    CDart  * vertexMostClwOutHalfEdge(CVertex* v) { return vertex_most_clw_out_dart(v); };

    double edgeLength(CEdge* e)   { return edge_length(e); };

    bool isBoundary(CDart* dart)  { return boundary(dart); };
    bool isBoundary(CEdge* edge)  { return boundary(edge); };
    bool isBoundary(CVertex* vert){ return boundary(vert); };

    int numVertices() { return num_vertices(); };
    int numEdges()    { return num_edges();    };
    int numFaces()    { return num_faces();    };
    int numHalfedges(){ return num_darts();    };

  protected:

    void _post_processing();

  protected:
    std::list<CDart*  > m_darts;
    std::list<CVertex*> m_vertices;
    std::list<CEdge*  > m_edges;
    std::list<CFace*  > m_faces;

    std::unordered_map<int, CVertex*> m_map_vertex;
    std::unordered_map<int, CFace*  > m_map_face;

    /*!
     *  This is used to check that whether an edge has been created.
     */
    std::unordered_map<EdgeMapKey, CEdge*, EdgeMapKey_hasher> m_map_edge_keys;
};

T_TYPENAME
void T_BASEMESH::unload()
{
    for (auto v : m_darts)    delete v;
    m_darts.clear();

    for (auto v : m_vertices) delete v;
    m_vertices.clear();

    for (auto v : m_edges)    delete v;
    m_edges.clear();

    for (auto v : m_faces)    delete v;
    m_faces.clear();

    m_map_edge_keys.clear();

    m_map_vertex.clear();
    m_map_face.clear();
}

T_TYPENAME
void T_BASEMESH::read(const std::string& input)
{
    IO::Dim2::read<T_BASEMESH>(this, input);
}

T_TYPENAME
void T_BASEMESH::write(const std::string& output)
{
    IO::Dim2::write<T_BASEMESH>(this, output);
}

T_TYPENAME
void T_BASEMESH::load(
    const std::map<int, CPoint>& vert_id_point,
    const std::map<int, std::vector<int>>& face_id_vids)
{
    // clear mesh
    this->unload();

    // add vertices
    for (auto & kv : vert_id_point)
    {
        int vid = kv.first;
        CVertex* pV = create_vertex(vid);
        pV->point() = kv.second;
    }

    // add faces
    for (auto& kv : face_id_vids)
    {
        int fid = kv.first;
        std::vector<int> vids = kv.second;
        create_face(vids, fid);
    }

    // post processing
    _post_processing();
}

T_TYPENAME
void T_BASEMESH::load_attributes(
  const std::map<int, std::string>& vert_id_str,
  const std::map<int, std::string>& face_id_str,
  const std::vector<std::tuple<int, int, std::string>>& edge_attrs,
  const std::vector<std::tuple<int, int, std::string>>& corner_attrs)
{
    for (const auto& id_str : vert_id_str)
    {
        CVertex* pV = id_vertex(id_str.first);
        pV->string() = id_str.second;

        pV->from_string();
    }

    for (const auto& id_str : face_id_str)
    {
        CFace* pF = id_face(id_str.first);
        pF->string() = id_str.second;

        pF->from_string();
    }

    for (const auto& edge : edge_attrs)
    {
        int vid0 = std::get<0>(edge);
        int vid1 = std::get<1>(edge);
        CVertex* pV0 = id_vertex(vid0);
        CVertex* pV1 = id_vertex(vid1);

        CEdge* pE = vertex_edge(pV0, pV1);
        pE->string() = std::get<2>(edge);

        pE->from_string();
    }

    for (const auto& corner : corner_attrs)
    {
        int vid = std::get<0>(corner);
        int fid = std::get<1>(corner);
        CVertex* pV = id_vertex(vid);
        CFace  * pF = id_face(fid);

        CDart* pD = dart(pV, pF);
        pD->string() = std::get<2>(corner);

        pD->from_string();
    }
}

T_TYPENAME
tVertex* T_BASEMESH::create_vertex(int vid) 
{
    CVertex* pV = new CVertex;
    pV->id() = vid;
    m_vertices.push_back(pV);
    m_map_vertex.insert(std::make_pair(vid, pV));

    return pV;
}

T_TYPENAME
tDart* T_BASEMESH::create_face(const std::vector<int>& indices, int fid)
{
    // add face
    CFace* pF = new CFace;
    pF->id() = fid;
    m_faces.push_back(pF);
    m_map_face.insert(std::make_pair(fid, pF));

    // add edges attached on the face ccwly
    size_t nEdges = indices.size();    // nEdges = nVertices
    std::vector<int> edge_verts(2);
    std::vector<CDart*> edges; // darts on the edges
    for (int i = 0; i < nEdges; ++i)
    {
        for (int j = 0; j < 2; ++j)
            edge_verts[j] = indices[(i + j) % indices.size()];
        CDart* e = create_edge(pF, edge_verts);
        edges.push_back(e);
    }

    // set dart for new face
    pF->dart() = edges[0];

    // link the edges
    for (int i = 0; i < edges.size(); ++i)
    {
        CDart* e1 = edges[i];
        CDart* e2 = edges[(i + 1) % edges.size()];
        link_edges(e1, e2);
    }

    // reutrn the new added dart even if there is no new added face
    return D(pF);
}

T_TYPENAME
tDart* T_BASEMESH::create_edge(CFace* pF, const std::vector<int>& indices)
{
    // add edge
    auto result = m_map_edge_keys.find(&indices[0]);
    bool found = result != m_map_edge_keys.end();
    
    CEdge* pE = NULL;
    if (!found) // not found
    {
        pE = new CEdge;
        m_edges.push_back(pE);

        m_map_edge_keys.insert(std::make_pair(&indices[0], pE));
    }
    else
        pE = result->second;

    // add new dart
    CDart* pD = new CDart;
    pD->cell(0) = id_vertex(indices[1]);
    pD->cell(1) = pE;
    pD->cell(2) = pF;
    m_darts.push_back(pD);

    // assign a dart to the target vertex in the situation of ccwly
    if (D(id_vertex(indices[1])) == NULL) // not assigned yet
        id_vertex(indices[1])->dart() = pD;

    // set dart for the new edge
    if (!found)
        pE->dart() = pD;
    else
        link_faces(D(pE), pD);

    // in the situation of combinatorial map
    // we do not need to link_vertices (set beta0)

    return pD;
}

T_TYPENAME
void T_BASEMESH::link_faces(CDart* d1, CDart* d2)
{
    for (int i = 0; i < 3; ++i) // #edges of f1 = 3
    {
        for (int j = 0; j < 3; ++j) // #edges of f2 = 3
        {
            if (d1->cell(1) == d2->cell(1))
            {
                d1->beta(2) = d2;
                d2->beta(2) = d1;

                // We assume all faces are convex, so only one edge is shared
                // by f1 and f2. But it is not ture if there exists concave faces.
                return;
            }
            d2 = beta(1, d2); // beta1 has been set in link_edges
        }
        d1 = beta(1, d1);
    }
    printf("[ERROR] Should found!\n");
    exit(EXIT_FAILURE);
}

T_TYPENAME
void T_BASEMESH::link_edges(CDart* d1, CDart* d2) { d1->beta(1) = d2; }

T_TYPENAME
tVertex* T_BASEMESH::dart_source(CDart* dart)
{
    if (!boundary(dart))
    {
        return C0(beta(2, dart));
    }
    else
    {
        CDart* d = dart;
        while (true)
        {
            if (beta(1, d) != dart)
                d = beta(1, d);
            else
                return C0(d);
        }
    }
}

T_TYPENAME
tDart* T_BASEMESH::dart_prev(CDart* dart) 
{
    CDart* d = dart;
    while (true)
    {
        if (beta(1, d) != dart)
            d = beta(1, d);
        else
            return d;
    }
}

T_TYPENAME
tDart* T_BASEMESH::dart(CVertex* vert, CFace* face) 
{
    CDart* d = D(face);
    while (true)
    {
        if (C0(d) != vert)
            d = beta(1, d);
        else
            return d;
    }
}

T_TYPENAME
tEdge* T_BASEMESH::vertex_edge(CVertex* v0, CVertex* v1)
{
    CDart* pD = D(v0);
    if (pD->boundary())
    {
        if (dart_source(pD) == v1)
            return C1(pD);
    }

    CDart* pD_start = beta(1, pD);
    pD = pD_start;
    do
    {
        if (dart_target(pD) == v1)
            return C1(pD);

        if (!boundary(pD))
            pD = beta(1, beta(2, pD));
        else
            return NULL;
    } while (pD != pD_start);

    return NULL;
}

T_TYPENAME
tDart* T_BASEMESH::vertex_dart(CVertex* source, CVertex* target)
{
    CDart* pD_start = beta(1, D(source));
    CDart* pD = pD_start;
    do
    {
        if (dart_target(pD) == target)
            return pD;

        if (!boundary(pD))
            pD = beta(1, beta(2, pD));
        else
            return NULL;
    } while (pD != pD_start);

    return NULL;
}

T_TYPENAME
tVertex* T_BASEMESH::edge_vertex(CEdge* edge, int index)
{
    assert(index == 0 || index == 1);

    if (index == 0)
        return C0(D(edge));
    else if (index == 1)
    {
        CDart* pD = D(edge);
        //if (boundary(pD))
        //{
        do
        {
            if (C1(beta(1, pD)) == edge) return C0(pD);
            else                         pD = beta(1, pD);
        } while (true);
        //}
        //else
        //    return C0(beta(2, pD));
    }

    return NULL;
}

T_TYPENAME
tFace* T_BASEMESH::edge_face(CEdge* edge, int index) 
{
    if (index == 0)
    {
        return C2(D(edge));
    }
    else if (index == 1)
    {
        return boundary(edge) ? NULL : C2(beta(2, D(edge)));
    }
    return NULL;
}

T_TYPENAME
tDart* T_BASEMESH::edge_dart(CEdge* edge, int index) 
{
    if (index == 0)
    {
        return D(edge);
    }
    else if (index == 1)
    {
        return boundary(edge) ? NULL : beta(2, D(edge));
    }
    return NULL;
}

T_TYPENAME
tDart* T_BASEMESH::vertex_most_clw_out_dart(CVertex* vertex) 
{
    if (boundary(vertex))
    {
        CDart* pD = beta(1, D(vertex));
        do
        {
            if (boundary(pD))
                return pD;
            else
                pD = beta(1, beta(2, pD));
        } while (true);
    }
    else
        return beta(1, D(vertex));
}


T_TYPENAME
double T_BASEMESH::edge_length(CEdge* edge)
{
    CVertex* v0 = edge_vertex(edge, 0);
    CVertex* v1 = edge_vertex(edge, 1);
    
    return (v0->point() - v1->point()).norm();
}

T_TYPENAME
void T_BASEMESH::_post_processing()
{
    // 1. set boundary darts on boundary vertices
    CVertex* pV;
    for (CDart* pD : m_darts)
    {
        if (pD->boundary())
        {
            pV = C0(pD);
            pV->dart() = pD;
        }
    }
}

} // namespace DartLib

#undef T_TYPENAME
#undef T_BASEMESH
#endif // !_DARTLIB_MESH_2D_H_
