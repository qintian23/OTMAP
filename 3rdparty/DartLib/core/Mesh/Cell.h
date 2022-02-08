#ifndef _DARTLIB_CELL_H_
#define _DARTLIB_CELL_H_

#include "Traits.h"

namespace DartLib
{
template <int N>
class TBaseDart;

/*===================================================================
                    Class i-cell for n-manifold
===================================================================*/

/*!
 *  \class TBaseCell Cell.h "Cell.h"
 *  \beief TBaseCell, cell template class for n-manifold
 *         e.x. TBaseCell<3> means this is a cell on 3-manifold
 * 
 *  \tparam N: dimention of the manifold.
 */
template <int N>
class TBaseCell
{
  public:
    TBaseCell() : m_pDart(NULL) {};

    TBaseDart<N>*& dart() { return m_pDart; };

  protected:
    TBaseDart<N>* m_pDart;
};

/*!
 *  \class TCell Cell.h "Cell.h"
 *  \beief TCell, cell template class for n-manifold with traits
 *         e.x. TCell<2, 3> means this is a 2-cell on 3-manifold
 *              TCell<2, 3, Trait::Normal> means this cell owns normal trait 
 * 
 *  \tparam      I: dimention of the cell.
 *  \tparam      N: dimention of the manifold.
 *  \tparam Traits: traits of the cell
 */
template <int I, int N, class... Traits>
class TCell : public TBaseCell<N>, public Traits...
{

};


/*===================================================================
                    Class i-cells for 2-manifold
===================================================================*/

class CVertex_2 : public TCell<0, 2>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(int, id)
    ADD_TRAIT(CPoint, point)
    ADD_TRAIT(std::string, string)
};

class CEdge_2 : public TCell<1, 2>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(std::string, string)
};

class CFace_2 : public TCell<2, 2>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(int, id)
    ADD_TRAIT(std::string, string)
};


/*===================================================================
                    Class i-cells for 3-manifold
===================================================================*/

class CVertex_3 : public TCell<0, 3>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(int, id)
    ADD_TRAIT(CPoint, point)
    ADD_TRAIT(std::string, string)
};

class CEdge_3 : public TCell<1, 3>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(std::string, string)
};

class CFace_3 : public TCell<2, 3>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(std::string, string)
};

class CVolume_3 : public TCell<3, 3>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(int, id)
    ADD_TRAIT(std::string, string)
};

} // namespace DartLib
#endif //! _DARTLIB_CELL_H_
