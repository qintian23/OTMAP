#ifndef _DARTLIB_MESH_3D_H_
#define _DARTLIB_MESH_3D_H_

#include <time.h>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>

#include "../Geometry/Point.h"
#include "../Parser/strutil.h"
#include "../Utils/IO.h"
#include "Iterators_3.h"
#include "Boundary_3.h"

#include "MapKeys.h"

#define MAX_LINE 1024

#define T_TYPENAME template <typename tVertex, typename tEdge, typename tFace, typename tVolume, typename tDart>
#define TBASEMESH TBaseMesh_3<tVertex, tEdge, tFace, tVolume, tDart>

namespace DartLib
{
/*! \class TBaseMesh_3 BaseMesh_3.h "BaseMesh_3.h"
 *  \brief TBaseMesh_3, base class for all types of 3d-mesh classes
 *
 *  \tparam tVertex vertex class, derived from DartLib::CVertex_3 class
 *  \tparam tEdge   edge   class, derived from DartLib::CEdge_3   class
 *  \tparam tFace   face   class, derived from DartLib::CFace_3   class
 *  \tparam tVolume volume class, derived from DartLib::CVolume_3 class
 *  \tparam tDart   dart   class, derived from DartLib::CDart_3   class
 */
T_TYPENAME
class TBaseMesh_3
{
  public:
    using CVertex              = tVertex;
    using CEdge                = tEdge;
    using CFace                = tFace;
    using CVolume              = tVolume;
    using CDart                = tDart;

    using CBoundary            = TBoundary_3<TBASEMESH>;

    using VertexIterator       = Dim3::VertexIterator<TBASEMESH>;
    using EdgeIterator         = Dim3::EdgeIterator  <TBASEMESH>;
    using FaceIterator         = Dim3::FaceIterator  <TBASEMESH>;
    using VolumeIterator       = Dim3::VolumeIterator<TBASEMESH>;
    using DartIterator         = Dim3::DartIterator  <TBASEMESH>;

    using VertexVolumeIterator = Dim3::VertexVolumeIterator<TBASEMESH>;
    using VertexEdgeIterator   = Dim3::VertexEdgeIterator  <TBASEMESH>;
    using VertexVertexIterator = Dim3::VertexVertexIterator<TBASEMESH>;

    using EdgeVolumeIterator   = Dim3::EdgeVolumeIterator  <TBASEMESH>;
    using EdgeFaceIterator     = Dim3::EdgeFaceIterator    <TBASEMESH>;

    using FaceVertexIterator   = Dim3::FaceVertexIterator  <TBASEMESH>;
    using FaceEdgeIterator     = Dim3::FaceEdgeIterator    <TBASEMESH>;

    using VolumeVertexIterator = Dim3::VolumeVertexIterator<TBASEMESH>;
    using VolumeFaceIterator   = Dim3::VolumeFaceIterator  <TBASEMESH>;
    using VolumeedgeIterator   = Dim3::VolumeEdgeIterator  <TBASEMESH>;

    /*!
     *  Constructor function
     */
    TBaseMesh_3(){};

    /*!
     *  Destructor function
     */
    ~TBaseMesh_3() { unload(); };


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
     *  \param vertices: the input point cloud
     *  \param  volumes: the indices of points to construct volumes
     */
    void load(const std::vector<CPoint>            & vertices, 
              const std::vector< std::vector<int> >& volumes);

    /*!
     *  Load attributes for darts and cells
     *  \param   vert_id_idx: vertex id -> vertex index
     *  \param   vert_id_str: vertex id -> vertex string
     *  \param volume_id_idx: volume id -> face index
     *  \param volume_id_str: volume id -> face string
     *  \param    edge_attrs: (vid1, vid2) -> string
     */
    void load_attributes(
        const std::map<int, int>        & vert_id_idx,
        const std::map<int, std::string>& vert_id_str,
        const std::map<int, int>        & volume_id_idx,
        const std::map<int, std::string>& volume_id_str,
        const std::vector<std::tuple<int, int, std::string>>& edge_attrs);

    /*!
     *  Add a vertex cell.
     *  The vertex dart will be set in the processing of 
     *  creating 1-cell, so it returns a vertex pointer here.
     *  \return the added vertex pointer
     */
    CVertex* add_vertex();

    /*!
     *  Add a volume cell
     *  \param indices: indices of the vertices to construct a volume
     *  \return a dart pointer of the volume
     */
    CDart* add_volume(const std::vector<int>& indices);

    /*!
     *  Add a face cell
     *  \param pVol: volume pointer the face attaches to.
     *  \param indices: indices of the vertices to construct a face
     *  \return a dart pointer of the face
     */
    CDart* add_face(CVolume* pVol, const std::vector<int>& indices);

    /*!
     *  Add a edge cell
     *  \param pVol: volume pointer the edge attaches to.
     *  \param pF: face pointer the edge attaches to.
     *  \param indices: indices of the vertices to construct an edge
     *  \return a dart pointer of the edge
     */
    CDart* add_edge(CVolume* pVol, CFace* pF, const std::vector<int>& indices);

    /*!
     *  Link the two faces
     *  \param f1: index of the first face
     *  \param f2: index of the second face     *
     */
    void link_faces(CDart* f1, CDart* f2);

    /*!
     *  Link the two edges
     *  \param e1: index of the first edge
     *  \param e2: index of the second edge
     *
     */
    void link_edges(CDart* e1, CDart* e2);

    /*!
     *  Link the two volumes
     *  \param vol1: index of the first volume
     *  \param vol2: index of the second volume
     *
     */
    void link_volumes(CDart* vol1, CDart* vol2);


    /*=============================================================
                 Access dart and i-cell from index or id

        Recommend to use iterators instead of the following usage.
            for(int i = 0; i < darts().size(); ++i)
            {
                CDart * pD = dart(i);
                ...
            }
    =============================================================*/

    CDart*   dart  (int index) { return m_darts[index];    };
    CVertex* vertex(int index) { return m_vertices[index]; };
    CEdge*   edge  (int index) { return m_edges[index];    };
    CFace*   face  (int index) { return m_faces[index];    };
    CVolume* volume(int index) { return m_volumes[index];  };
   
    CVertex* id_vertex(int id) { return m_map_vertex[id];  };
    CVolume* id_volume(int id) { return m_map_volume[id];  };


    /*=============================================================
                   Access container of dart and i-cell

        Recommend to use iterators instead of the container.
    =============================================================*/

    std::vector<CDart*  >& darts()    { return m_darts;    };
    std::vector<CVertex*>& vertices() { return m_vertices; };
    std::vector<CEdge*  >& edges()    { return m_edges;    };
    std::vector<CFace*  >& faces()    { return m_faces;    };
    std::vector<CVolume*>& volumes()  { return m_volumes;  };

    std::unordered_map<int, CVertex*>& map_vertex(){ return m_map_vertex;};
    std::unordered_map<int, CVolume*>& map_volume(){ return m_map_volume;};
    

    /*=============================================================
                Access dart from i-cell and vice versa
                              (Part I)
    =============================================================*/
    
    CVertex*C0(CDart* dart)  { return dart != NULL ? (CVertex*)dart->cell(0) : NULL;};
    CEdge*  C1(CDart* dart)  { return dart != NULL ? (CEdge*)  dart->cell(1) : NULL;};
    CFace*  C2(CDart* dart)  { return dart != NULL ? (CFace*)  dart->cell(2) : NULL;};
    CVolume*C3(CDart* dart)  { return dart != NULL ? (CVolume*)dart->cell(3) : NULL;};
    CDart*  D (CVertex* vert){ return vert != NULL ? (CDart*)  vert->dart()  : NULL;};
    CDart*  D (CEdge*   edge){ return edge != NULL ? (CDart*)  edge->dart()  : NULL;};
    CDart*  D (CFace*   face){ return face != NULL ? (CDart*)  face->dart()  : NULL;};
    CDart*  D (CVolume*  vol){ return  vol != NULL ? (CDart*)   vol->dart()  : NULL;};

    /*!
     *  The beta function in combinatorial map
     *  \param i: represents for function beta_i, i \in {1, 2, 3}
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
    CVolume* dart_volume(CDart* dart) { return C3(dart); };
    CDart  * dart_prev  (CDart* dart);
    CDart  * dart_next  (CDart* dart) { return beta(1, dart); };
    CDart  * dart_sym   (CDart* dart) { return beta(2, dart); };
    //CDart  * dart       (CVertex* vert, CFace* face);
    //CDart  * dart       (CVertex* vert, CVolume* volume);

    CEdge  * vertex_edge(CVertex* v0, CVertex* v1);
    CDart  * vertex_dart(CVertex* vert) { return D(vert); };
    //CDart  * vertex_dart(CVertex* source, CVertex* target);
    
    CVertex* edge_vertex(CEdge* edge, int index);
    
    CDart  * face_dart  (CFace* face)  { return D(face);};

    CDart  * volume_dart(CVolume* vol) { return D(vol); };


    /*=============================================================
                Access neighbouring darts from i-cell
                              (Part III)
    =============================================================*/

    /*!
     *  Get all darts incident to the given vertex.
     *  \param vertex: current vertex
     *  \return all incident darts
     */
    std::vector<CDart*> vertex_incident_darts(CVertex* vertex);

    /*!
     *  Get all darts incident to the given vertex on volume.
     *  \param vertex: current vertex
     *  \param volume: the volume where the darts attached on
     *  \return all incident darts attached on the volume
     */
    std::vector<CDart*> vertex_incident_darts(CVertex* vertex, CVolume* volume);

    /*!
     *  Get all 2-cells attached on a 3-cell.
     *  \param volume: the input volume
     *  \return the darts represent for the 2-cells.
     */
    std::vector<CDart*> C32(CVolume* volume);

    /*!
     *  Get all 1-cells attached on a 3-cell.
     *  \param volume: the input volume
     *  \return the darts represent for the 1-cells.
     */
    //std::vector<CDart*> C31(CVolume* volume);


    /*=============================================================
                         Boundary detectors
    =============================================================*/

    bool boundary(CDart* dart) { return    dart->boundary(); };
    bool boundary(CEdge* edge) { return D(edge)->boundary(); };
    bool boundary(CFace* face) { return D(face)->boundary(); };
    
    /*=============================================================
                    Number of all kinds of element
    =============================================================*/

    int num_vertices() { return m_vertices.size(); };
    int num_edges()    { return m_edges.size();    };
    int num_faces()    { return m_faces.size();    };
    int num_volumes()  { return m_volumes.size();  };
    int num_darts()    { return m_darts.size();    };


  protected:
    void _post_processing();

  protected:
    std::vector<CDart*>   m_darts;
    std::vector<CVertex*> m_vertices;
    std::vector<CEdge*>   m_edges;
    std::vector<CFace*>   m_faces;
    std::vector<CVolume*> m_volumes;

    std::unordered_map<int, CVertex*> m_map_vertex;
    std::unordered_map<int, CVolume*> m_map_volume;

    std::unordered_map<EdgeMapKey, int, EdgeMapKey_hasher> m_map_edge_keys;
    std::unordered_map<FaceMapKey, int, FaceMapKey_hasher> m_map_face_keys;
};

T_TYPENAME
void TBASEMESH::unload()
{
    for (auto v : m_darts)    delete v;
    m_darts.clear();

    for (auto v : m_vertices) delete v;
    m_vertices.clear();

    for (auto v : m_edges)    delete v;
    m_edges.clear();

    for (auto v : m_faces)    delete v;
    m_faces.clear();

    for (auto v : m_volumes)  delete v;
    m_volumes.clear();

    m_map_edge_keys.clear();
    m_map_face_keys.clear();
}

T_TYPENAME
void TBASEMESH::read(const std::string& input)
{
    IO::Dim3::read<TBASEMESH>(this, input);
}

T_TYPENAME
void TBASEMESH::write(const std::string& output)
{
    IO::Dim3::write<TBASEMESH>(this, output);
}

T_TYPENAME
void TBASEMESH::load(const std::vector<CPoint>          & points, 
                     const std::vector<std::vector<int>>& volumes)
{
    // clear mesh
    this->unload();

    // add vertices
    m_vertices.resize(points.size());
    for (int i = 0; i < points.size(); ++i)
    {
        CVertex* pV = add_vertex();
        m_vertices[i] = pV;
        m_vertices[i]->point() = points[i];
    }

    // add volumes
    int nVolumes = volumes.size();
    for (int i = 0; i < nVolumes; ++i)
    {
        std::vector<int> volume;
        int nVerts = volumes[i].size();
        for (int j = 0; j < nVerts; ++j)
            volume.push_back(volumes[i][j]);

        add_volume(volume);
    }

    // post processing
    _post_processing();
}

T_TYPENAME
void TBASEMESH::load_attributes(
    const std::map<int, int>        & vert_id_idx,
    const std::map<int, std::string>& vert_id_str,
    const std::map<int, int>        & volume_id_idx,
    const std::map<int, std::string>& volume_id_str,
    const std::vector<std::tuple<int, int, std::string>>& edge_attrs)
{
    if (vert_id_idx.size() == num_vertices())
    {
        int idx = 0;
        for (const auto& id_idx : vert_id_idx)
        {
            CVertex* pV = vertex(id_idx.second);
            pV->id() = id_idx.first;
            m_map_vertex.insert(std::make_pair(pV->id(), pV));
        }
    }

    for (const auto& id_str : vert_id_str)
    {
        CVertex* pV = id_vertex(id_str.first);
        pV->string() = id_str.second;

        pV->from_string();
    }

    if (volume_id_idx.size() == num_volumes())
    {
        int idx = 0;
        for (const auto& id_idx : volume_id_idx)
        {
            CVolume* pVol = volume(id_idx.second);
            pVol->id() = id_idx.first;
            m_map_volume.insert(std::make_pair(pVol->id(), pVol));
        }
    }

    for (const auto& id_str : volume_id_str)
    {
        CVolume* pVol = id_volume(id_str.first);
        pVol->string() = id_str.second;

        pVol->from_string();
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
}

T_TYPENAME
tVertex* TBASEMESH::add_vertex() 
{
    return new CVertex;
}

T_TYPENAME
tDart* TBASEMESH::add_volume(const std::vector<int>& indices)
{
    // assert(indices.size() == 4);
    // add volume
    CVolume* pVol = new CVolume;
    m_volumes.push_back(pVol);

    // add faces
    static int tbl_tet_face[4][3] = {{0, 1, 2}, {0, 2, 3}, {0, 3, 1}, {1, 3, 2}};
    CDart* faces[4]; // the darts attached to the faces
    for (int i = 0; i < 4; ++i)
    {
        std::vector<int> face_verts(3);
        for (int j = 0; j < 3; ++j)
        {
            face_verts[j] = indices[tbl_tet_face[i][j]];
        }
        faces[i] = add_face(pVol, face_verts);
    }

    // set dart for new volume
    pVol->dart() = faces[0];

    // link faces
    for (int i = 0; i < 4; ++i)
    {
        for (int j = i + 1; j < 4; ++j)
            link_faces(faces[i], faces[j]);
    }

    return D(pVol);
}

T_TYPENAME
tDart* TBASEMESH::add_face(CVolume* pVol, const std::vector<int>& indices)
{
    // add face
    auto result = m_map_face_keys.find(&indices[0]);
    bool found = result != m_map_face_keys.end();

    CFace* pF = NULL;
    if (!found) // not found
    {
        pF = new CFace;
        m_faces.push_back(pF);

        m_map_face_keys.insert(std::make_pair(&indices[0], m_faces.size() - 1));
    }
    else
        pF = m_faces[result->second];

    // add edges attached on the face ccwly
    static int tbl_face_edge[3][2] = {{0, 1}, {1, 2}, {2, 0}};
    std::vector<int> edge_verts(2);
    std::vector<CDart*> edges; // darts on the edges
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
            edge_verts[j] = indices[tbl_face_edge[i][j]];
        CDart* e = add_edge(pVol, pF, edge_verts);
        edges.push_back(e);
    }

    // set dart for new face
    if (!found)
        pF->dart() = edges[0];

    // link the edges
    for (int i = 0; i < edges.size(); ++i)
    {
        CDart* e1 = edges[i];
        CDart* e2 = edges[(i + 1) % edges.size()];
        link_edges(e1, e2);
    }

    // if the face is shared by two volumes, then link them.
    if (found)
    {
        link_volumes(D(pF), edges[0]);
    }

    // reutrn the new added dart even if there is no new added face
    return edges[0];
}

T_TYPENAME
tDart* TBASEMESH::add_edge(CVolume* pVol, CFace* pF, const std::vector<int>& indices)
{
    // add edge
    auto result = m_map_edge_keys.find(&indices[0]);
    bool found = result != m_map_edge_keys.end();
    
    CEdge* pE = NULL;
    if (!found) // not found
    {
        pE = new CEdge;
        m_edges.push_back(pE);

        m_map_edge_keys.insert(std::make_pair(&indices[0], m_edges.size() - 1));
    }
    else
        pE = m_edges[result->second];

    // add new dart
    CDart* pD = new CDart;
    pD->cell(0) = m_vertices[indices[1]];
    pD->cell(1) = pE;
    pD->cell(2) = pF;
    pD->cell(3) = pVol;
    m_darts.push_back(pD);

    // assign a dart to the target vertex in the situation of ccwly
    if (D(m_vertices[indices[1]]) == NULL) // not assigned yet
        m_vertices[indices[1]]->dart() = pD;

    // set dart for the new edge
    if (!found)
        pE->dart() = pD;

    // in the situation of combinatorial map
    // we do not need to link_vertices (set beta0)

    return pD;
}

T_TYPENAME
void TBASEMESH::link_faces(CDart* d1, CDart* d2)
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
void TBASEMESH::link_edges(CDart* d1, CDart* d2) { d1->beta(1) = d2; }

T_TYPENAME
void TBASEMESH::link_volumes(CDart* d1, CDart* d2)
{
    int found = 0;
    for (int i = 0; i < 3; ++i) // #edges of the shared face = 3
    {
        for (int j = 0; j < 3; ++j) // #edges of the shared face = 3
        {
            if (d1->cell(1) == d2->cell(1))
            {
                d1->beta(3) = d2;
                d2->beta(3) = d1;
                ++found;
            }
            d2 = beta(1, d2);
        }
        d1 = beta(1, d1);
    }

    if (found != 3) // #edges of the shared face = 3
    {
        printf("[ERROR] When we link volumes, there should be three pairs of darts! (%d)\n", found);
        exit(EXIT_FAILURE);
    }
}

T_TYPENAME
tVertex* TBASEMESH::dart_source(CDart* dart) 
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

T_TYPENAME
tDart* TBASEMESH::dart_prev(CDart* dart)
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
tEdge* TBASEMESH::vertex_edge(CVertex* v0, CVertex* v1) 
{
    std::vector<CDart*> darts = vertex_incident_darts(v1);
    for (auto pD : darts)
    {
        if (dart_source(pD) == v0)
            return C1(pD);
    }
    return NULL;
}

T_TYPENAME
tVertex* TBASEMESH::edge_vertex(CEdge* edge, int index)
{
    assert(index == 0 || index == 1);

    if (index == 0)
        return C0(D(edge));
    else if (index == 1)
        return C0(beta(2, D(edge)));
}

T_TYPENAME
std::vector<tDart*> TBASEMESH::vertex_incident_darts(CVertex* vertex)
{
    std::vector<CDart*> darts;

    std::queue<CVolume*> Q;
    std::vector<CVolume*> volumes;

    CVolume* pVol = C3(D(vertex)); // vertex->dart()->volume();
    Q.push(pVol);
    volumes.push_back(pVol);

    while (!Q.empty())
    {
        pVol = Q.front();
        Q.pop();

        std::vector<CDart*> _darts = vertex_incident_darts(vertex, pVol);
        for (auto d : _darts)
        {
            darts.push_back(d);

            CDart* pD = beta(3, d);
            pVol = pD != NULL ? C3(pD) : NULL;

            if (pVol != NULL && std::find(volumes.begin(), volumes.end(), pVol) == volumes.end())
            {
                Q.push(pVol);
                volumes.push_back(pVol);
            }
        }
    }

    return darts;
}

T_TYPENAME
std::vector<tDart*> TBASEMESH::vertex_incident_darts(CVertex* vertex, CVolume* volume)
{
    // CAUTION:
    //   Only be valid on tetrahedron
    std::vector<CDart*> result;

    CDart* dart_start = NULL;
    if (C3(D(vertex)) == volume) // vertex->dart()->volume() 
        dart_start = D(vertex);
    else
    {
        auto darts = C32(volume);
        bool found = false;
        for (auto& d : darts)
        {
            for (int i = 0; i < 3; ++i)
            {
                if ((CVertex*)d->cell(0) == vertex)
                {
                    dart_start = d;
                    found = true;
                    break;
                }
                d = beta(1, d);
            }
            if (found)
                break;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        result.push_back(dart_start);
        dart_start = beta(2, beta(1, dart_start));
    }

    return result;
}

T_TYPENAME
std::vector<tDart*> TBASEMESH::C32(CVolume* volume)
{
    // CAUTION: only work on tet
    std::vector<CDart*> result;
    CDart* pD = D(volume);
    for (int i = 0; i < 3; ++i)
    {
        result.push_back(pD);
        pD = beta(2, beta(1, pD));
    }
    pD = beta(2, beta(1, beta(1, pD)));
    result.push_back(pD);

    return result;
}

T_TYPENAME
void TBASEMESH::_post_processing()
{
    // 1. set boundary darts on boundary edges
    CEdge* pE;
    for (CDart* pD : m_darts)
    {
        if (pD->boundary())
        {
            pE = C1(pD);
            pE->dart() = pD;
        }
    }
}

} // namespace DartLib

#undef T_TYPENAME
#undef TBASEMESH
#endif // !_DARTLIB_MESH_3D_H_
