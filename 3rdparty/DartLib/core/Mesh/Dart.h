#ifndef _DARTLIB_DART_H_
#define _DARTLIB_DART_H_

#include <assert.h>

#include "Traits.h"

namespace DartLib
{
template <int N>
class TBaseCell;

/*===================================================================
                    Class dart for n-manifold
===================================================================*/

/*!
 *  \class TBaseDart Dart.h "Dart.h"
 *  \beief TBaseDart, dart template class for n-manifold
 *         e.x. TBaseDart<2> means this is a dart defined on 2-manifold

 *  \tparam      N: dimention of the manifold.
 */
template <int N>
class TBaseDart
{
  public:
    
    TBaseDart()
    {
        for (int i = 0; i < N; ++i)
            m_beta[i] = NULL;

        for (int i = 0; i < N + 1; ++i)
            m_cells[i] = NULL;
    };

    /*!
     *  Beta function in combinatorial map.
     *  \param i: index i should be 1, 2, 3, ..., N
     *  \return the reference of the dart pointer
     *
     *  NOTICE:
     *  The value of beta_1 function could be on the next edge or the
     *  previous edge of the same face, but here we assume it always
     *  be on the next counter-clockwise(ccw) edge.
     */
    TBaseDart<N>*& beta(const int i)
    {
        assert(i >= 1 && i <= N);
        return m_beta[i - 1];
    };

    /*!
     *  Access cell-tuple.
     *  \param i: index i ranges from 0 to N
     *  \return the reference of the i-cell pointer
     */
    TBaseCell<N>*& cell(int i)
    {
        assert(i >= 0 && i <= N);
        return m_cells[i];
    };

    /*!
     *  Whether the dart lies on the boundary.
     *  \return true or false
     */
    bool boundary() { return this->beta(N) == NULL; };

  protected:
    /*!
     *  Store the value(dart pointer) of beta_1, beta_2, ..., beta_n
     *  function.
     *
     *  NOTICE:
     *  In the combinatorial map, beta0(dart) = dart,
     *  so we do not need to store this function value.
     */
    TBaseDart<N>* m_beta[N];

    /*!
     *  Store the cell-tuples (0-cell, 1-cell, ..., n-cell) 
     */
    TBaseCell<N>* m_cells[N + 1];
};

/*!
 *  \class TDart Dart.h "Dart.h"
 *  \beief TDart, dart template class for n-manifold with traits
 *         e.x. TDart<2> means this is a dart defined on 2-manifold
 *              TDart<2, Trait::Angle> means this dart owns angle trait
 
 *  \tparam      N: dimention of the manifold.
 *  \tparam Traits: traits of the dart
 */
template <int N, class... Traits>
class TDart : public TBaseDart<N>, public Traits...
{

};

/*===================================================================
                    Class dart for 2-manifold
===================================================================*/

class CDart_2 : public TDart<2>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(std::string, string)
};

/*===================================================================
                    Class dart for 3-manifold
===================================================================*/

class CDart_3 : public TDart<3>
{
  public:
    void   to_string(){};
    void from_string(){};

    ADD_TRAIT(std::string, string)
};

} // namespace DartLib
#endif // !_DARTLIB_DART_H_
