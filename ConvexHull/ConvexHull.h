#ifndef _CONVEX_HULL_H_
#define _CONVEX_HULL_H_

#include <vector>
#include <list>

#include "ConvexHullMesh.h"


namespace ConvexHull 
{

class CConvexHull
{
  public:
    /*!
     *  Construct function
     */
    CConvexHull();

    /*!
     *  Initialize some points in a unit ball randomly.
     *  \param [in] num_pts: the number of the points.
     */
    void init(size_t num_pts);

    /*!
     *  Insert one point, the convex hull will be updated if necessary.
     *  \param [in] p: a point which will be inserted.
     */
    void insert(const CPoint & p);

    /*!
     *  Construct the convex hull.
     */
    void construct();

    /*!
     *  The input sites
     *  \return the reference
     */
    std::vector<CPoint*>& sites() { return m_sites; };
    
    /*!
     *  The convex hull represented by a mesh.
     *  \return the reference
     */
    CConvexHullMesh& hull()       { return m_pMesh; };

  protected:
    /*!
     *  Determine the sign of the volume constructed by a face and a point
     *  \param [in] pF: a face pointer
     *  \param [in]  p: a point reference
     *  \return +1, -1 or 0
     */
    int _volume_sign(CConvexHullMesh::CFace* pF, const CPoint& p);

    /*!
     *  Determine whether a point lies inside the current convex hull
     *  \param [in] p: a point reference
     *  \return true if p lies inside the convex hull, or false
     */
    bool _inside(const CPoint & p);

    /*!
     *  Remove the faces which are visiable viewing from point p
     *  \param [in] p: the observer
     */
    void _remove_visiable(const CPoint & p);

    /*!
     *  Remove a list of faces.
     *    It removes face carefully because the process of removing face may
     *    produce non-manifold, so it skillfully arrange the order of removing
     *    faces.
     *    However, in some situdations, it always produces non-manifold, so we
     *    assume that if we remove the faces, the remaining part is a manifold.
     *    The essential reason is that the DartLib does not support for
     *    representing non-manifold, maybe it will be the future work.
     *
     *  \param [in] faces: a list of faces which forms a topological disk.
     */
    void _remove_faces(const std::vector<CConvexHullMesh::CFace*> & faces);

    /*!
     *  Close the cap to form a new convex hull
     *  \param [in] p: a point which will connect to the boundary of the cap.
     */
    void _close_cap(const CPoint & p);

  protected:
    
    /*!
     *  the sites will be used to construct the convex hull
     */
    std::vector<CPoint*> m_sites;

    /*!
     *  A mesh used to hold the convex hull
     */
    CConvexHullMesh m_pMesh;

    /*!
     *  Current maximal vertex id in the convex hull mesh
     */
    int m_max_vertex_id;

    /*!
     *  Current maximal face id in the convex hull mesh
     */
    int m_max_face_id;
};
}
#endif //! _CONVEX_HULL_H_
