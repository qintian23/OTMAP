#ifndef _DARTLIB_BOUNDARY_2_H_
#define _DARTLIB_BOUNDARY_2_H_

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <vector>

#include "Iterators_2.h"

namespace DartLib
{
/*!
        \brief TLoopSegment Boundary loop  segment class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
*/

template <class M>
class TLoopSegment
{
  public:
    /*!
            Constructor of the TLoopSegment
            \param pMesh  pointer to the current mesh
            \param pH halfedge on the boundary loop
    */
    TLoopSegment(M* pMesh, std::vector<typename M::CHalfEdge*>& pHs)
    {
        m_pMesh = pMesh;

        for (size_t i = 0; i < pHs.size(); i++)
        {
            m_halfedges.push_back(pHs[i]);
        }
    }
    /*!
           Destructor of TLoop.
    */
    ~TLoopSegment() { m_halfedges.clear(); };

    /*!
    The vector of haledges on the current boundary loop segment.
    */
    std::vector<typename M::CHalfEdge*>& halfedges() { return m_halfedges; }
    /*!
            Starting vertex
    */
    typename M::CVertex* start()
    {
        if (m_halfedges.size() == 0)
            return NULL;
        typename M::CHalfEdge* he = m_halfedges[0];
        return m_pMesh->halfedgeSource(he);
    }
    /*!
            ending vertex
    */
    typename M::CVertex* end()
    {
        if (m_halfedges.size() == 0)
            return NULL;
        size_t n = m_halfedges.size();
        typename M::CHalfEdge* he = m_halfedges[n - 1];
        return m_pMesh->halfedgeTarget(he);
    }

  protected:
    /*!
            The mesh pointer
    */
    M* m_pMesh;

    /*!
            The vector of consecutive halfedges along the boundary loop.
    */
    std::vector<typename M::CHalfEdge*> m_halfedges;
};

/*!
        \brief TLoop Boundary loop  class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
*/

template <class M>
class TLoop
{
    typedef TLoopSegment<M> TSegment;

  public:
    /*!
            Constructor of the TLoop
            \param pMesh  pointer to the current mesh
            \param pH halfedge on the boundary loop
    */
    TLoop(M* pMesh, typename M::CHalfEdge* pH);
    /*!
            Constructor of the TLoop
            \param pMesh  pointer to the current mesh
    */
    TLoop(M* pMesh)
    {
        m_pMesh = pMesh;
        m_length = 0;
        m_pHalfedge = NULL;
    };
    /*!
           Destructor of TLoop.
    */
    ~TLoop();

    /*!
    The list of haledges on the current boundary loop.
    */
    std::vector<typename M::CHalfEdge*>& halfedges() { return m_halfedges; }

    /*!
            The length of the current boundary loop.
    */
    double length() { return m_length; }
    /*!
     *  Output to a file
     */
    void write(const char* file);
    /*!
     *  Input from a file
     */
    void read(const char* file);
    /*!
            The vector of segments on this loop
    */
    std::vector<TSegment*>& segments() { return m_segments; }
    /*!
            divide the loop to segments
            \param markers, the array of markers, the first marker is the starting one
    */
    void divide(std::vector<typename M::CVertex*>& markers);

  protected:
    /*!
            Pointer to the current mesh.
    */
    M* m_pMesh;
    /*! The length of the current boundary loop.
     */
    double m_length;
    /*!
            Starting halfedge of the current boundary loop.
    */
    typename M::CHalfEdge* m_pHalfedge;
    /*!
            List of consecutive halfedges along the boundary loop.
    */
    std::vector<typename M::CHalfEdge*> m_halfedges;
    /*!
            Vector of segments
    */
    std::vector<TSegment*> m_segments;
};

/*!
        \brief TBoundary_2 Boundary  class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
*/

template <class M>
class TBoundary_2
{
    typedef TLoop<M> CLoop;

  public:
    /*!
    TBoundary_2 constructor
    \param pMesh pointer to the current mesh
    */
    TBoundary_2(M* pMesh);
    /*!
    TBoundary_2 destructor
    */
    ~TBoundary_2();
    /*!
    The list of boundary loops.
    */
    std::vector<CLoop*>& loops() { return m_loops; }

  protected:
    /*!
            Pointer to the current mesh.
    */
    M* m_pMesh;
    /*!
            List of boundary loops.
    */
    typename std::vector<CLoop*> m_loops;
    /*!
            Bubble sort the loops
            \param loops the vector of loops
    */
    void _bubble_sort(std::vector<CLoop*>& loops);
};

/*!
        TLoop constructure, trace the boundary loop, starting from the halfedge pH.
        \param pMesh pointer to the current mesh
        \param pH  halfedge on the current boundary loop
*/
template <class M>
TLoop<M>::TLoop(M* pMesh, typename M::CHalfEdge* pH)
{
    m_pMesh = pMesh;
    m_pHalfedge = pH;

    m_length = 0;
    typename M::CHalfEdge* he = pH;
    // trace the boundary loop
    do
    {
        typename M::CVertex* v = pMesh->halfedgeTarget(he);
        //if (v->id() == 543 && pMesh->num_vertices() == 612)
        //    m_pMesh->write("T:/1.m");
        he = m_pMesh->vertexMostClwOutHalfEdge(v);
        m_halfedges.push_back(he);
        m_length += m_pMesh->edgeLength(m_pMesh->halfedgeEdge(he));
    } while (he != m_pHalfedge);
}

/*!
TLoop destructor, clean up the list of halfedges in the loop
*/
template <class M>
TLoop<M>::~TLoop()
{
    m_halfedges.clear();

    for (size_t i = 0; i < m_segments.size(); i++)
    {
        delete m_segments[i];
    }
    m_segments.clear();
}

/*!
        _bubble_sort
        bubble sort  a vector of boundary loop objects, according to their lengths
        \param loops vector of loops
*/
template <class M>
void TBoundary_2<M>::_bubble_sort(std::vector<CLoop*>& loops)
{
    int i, j, flag = 1;                            // set flag to 1 to start first pass
    CLoop* temp; // holding variable
    int numLength = (int) loops.size();
    for (i = 1; (i <= numLength) && flag; i++)
    {
        flag = 0;
        for (j = 0; j < (numLength - 1); j++)
        {
            if (loops[j + 1]->length() > loops[j]->length()) // ascending order simply changes to <
            {
                temp = loops[j]; // swap elements
                loops[j] = loops[j + 1];
                loops[j + 1] = temp;
                flag = 1; // indicates that a swap occurred.
            }
        }
    }
}

/*!
        TBoundary_2 constructor
        \param pMesh the current mesh
*/
template <class M>
TBoundary_2<M>::TBoundary_2(M* pMesh)
{
    m_pMesh = pMesh;
    // collect all boundary halfedges
    std::set<typename M::CHalfEdge*> boundary_hes;
    for (typename M::MeshEdgeIterator eiter(m_pMesh); !eiter.end(); eiter++)
    {
        typename M::CEdge* e = *eiter;
        if (!m_pMesh->isBoundary(e))
            continue;

        typename M::CHalfEdge* he = m_pMesh->edgeHalfedge(e, 0);
        boundary_hes.insert(he);
    }
    // trace all the boundary loops
    while (!boundary_hes.empty())
    {
        // get the first boundary halfedge
        typename std::set<typename M::CHalfEdge*>::iterator siter = boundary_hes.begin();
        typename M::CHalfEdge* he = *siter;
        // trace along this boundary halfedge
        TLoop<M>* pL = new TLoop<M>(m_pMesh, he);
        assert(pL);
        m_loops.push_back(pL);
        // remove all the boundary halfedges, which are in the same boundary loop as the head, from the halfedge list
        for (typename std::vector<typename M::CHalfEdge*>::iterator hiter = pL->halfedges().begin(); hiter != pL->halfedges().end(); hiter++)
        {
            typename M::CHalfEdge* he = *hiter;
            siter = boundary_hes.find(he);
            if (siter == boundary_hes.end())
                continue;
            boundary_hes.erase(siter);
        }
    }

    // std::sort( vlps.begin(), vlps.end(), loop_compare<CVertex,CFace,CEdge,CHalfEdge> );
    _bubble_sort(m_loops);
}

/*!	TBoundary_2 destructor, delete all boundary loop objects.
 */
template <class M>
TBoundary_2<M>::~TBoundary_2()
{
    for (int i = 0; i < (int) m_loops.size(); i++)
    {
        TLoop<M>* pL = m_loops[i];
        delete pL;
    }
};

/*!
        Output the loop to a file
        \param file_name the name of the file
*/
template <class M>
void TLoop<M>::write(const char* file_name)
{
    std::ofstream myfile;
    myfile.open(file_name);
    for (typename std::vector<typename M::CHalfEdge*>::iterator hiter = m_halfedges.begin(); hiter != m_halfedges.end(); hiter++)
    {
        typename M::CHalfEdge* pH = *hiter;
        typename M::CVertex* pV = m_pMesh->halfedgeSource(pH);
        typename M::CVertex* pW = m_pMesh->halfedgeTarget(pH);
        myfile << pV->id() << " " << pW->id() << std::endl;
    }
    myfile.close();
};

/*!
        Output the loop to a file
        \param file_name the name of the file
*/
template <class M>
void TLoop<M>::read(const char* file_name)
{
    std::ifstream myfile;
    myfile.open(file_name);

    if (myfile.is_open())
    {
        while (myfile.good())
        {
            int source, target;
            myfile >> source >> target;
            typename M::CVertex  * pS = m_pMesh->idVertex(source);
            typename M::CVertex  * pT = m_pMesh->idVertex(target);
            typename M::CEdge    * pE = m_pMesh->vertexEdge(pS, pT);
            typename M::CHalfEdge* pH = m_pMesh->edgeHalfedge(pE, 0);
            m_halfedges.push_back(pH);
        }
        myfile.close();
    }
};

/*!
        Divide the loop to segments
        \param markers
*/
template <class M>
void TLoop<M>::divide(std::vector<typename M::CVertex*>& markers)
{
    std::deque<typename M::CHalfEdge*> queue;
    for (typename std::vector<typename M::CHalfEdge*>::iterator hiter = m_halfedges.begin(); hiter != m_halfedges.end(); hiter++)
    {
        typename M::CHalfEdge* ph = *hiter;
        queue.push_back(ph);
    }

    size_t n = 0;
    while (true)
    {
        typename M::CHalfEdge* ph = queue.front();
        if (m_pMesh->halfedgeSource(ph) == markers[0])
            break;
        queue.pop_front();
        queue.push_back(ph);
        n++;
        if (n > queue.size())
            break;
    }

    if (n > queue.size())
    {
        std::cerr << "TLoop::divide: can not find the starting vertex " << std::endl;
        return;
    }

    for (size_t i = 0; i < markers.size(); i++)
    {
        std::vector<typename M::CHalfEdge*> hes;
        typename M::CHalfEdge* ph = queue.front();
        queue.pop_front();
        assert(m_pMesh->halfedgeSource(ph) == markers[i]);
        hes.push_back(ph);

        while (m_pMesh->halfedgeTarget(ph) != markers[(i + 1) % markers.size()])
        {
            if (queue.empty())
            {
                std::cerr << "TLoop::divide error" << std::endl;
                return;
            }
            ph = queue.front();
            queue.pop_front();
            hes.push_back(ph);
        }

        TSegment* pS = new TSegment(m_pMesh, hes);
        assert(pS != NULL);
        m_segments.push_back(pS);
    }
};

} // namespace DartLib
#endif //! _DARTLIB_BOUNDARY_2_H_
