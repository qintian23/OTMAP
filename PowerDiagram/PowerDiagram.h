#ifndef _POWER_DIAGRAM_H_
#define _POWER_DIAGRAM_H_

#include "ConvexHullMesh.h"
#include "ConvexHull.h"

using namespace ConvexHull;

namespace PowerDiagram
{
using CMesh = CConvexHullMesh;

/*! 
 *  \class CPowerDiagram PowerDiagram.h "PowerDiagram.h"
 *  \brief CPowerDiagram, is used to compute power diagram of 2d points.
 *         1. It lifts the plannar points onto paraboloid z = x^2 + y^2.
 *         2. A 3d convex hull algorithm is applied on them.
 *         3. The upward faces are removed, and the remaining part projected
 *            onto the plane constructs the delaunay triangulation of the points.
 *         4. The dual of the delaunay triangulation is the voronoi diagram.
 */
class CPowerDiagram
{
  public:
    
    /*!
     *  Initial some random points on unit disk
     *  \param [in] num_pts: number of points
     */
    void init(int num_pts);

    /*!
     *  Feed points into PowerDiagram
     *  \param [in] sites: a list of pointers of points
     */
    void init(const std::vector<CPoint*>& points);

    /*!
     *  Compute delaunay triangulation
     */
    void calc_delaunay();

    /*!
     *  Compute voronoi diagram - the dual of the delaunay triangulation
     */
    void calc_voronoi();

    /*!
     *  Reference of the input points
     *  \return the reference of the array of the input points
     */
    std::vector<CPoint*>& points() { return m_pts; };

    /*!
     *  Reference of the mesh used to store delaunay triangulation and its dual.
     *  \return the reference of the mesh
     */
    CMesh& mesh() { return m_mesh; };
  protected:
    /*!
     *  The input points
     */
    std::vector<CPoint*> m_pts;

    /*! 
     *  Used to store delauany trianle mesh and the dual mesh - voronoi diagram
     */
    CMesh m_mesh;
};
}
#endif // !_POWER_DIAGRAM_H_
