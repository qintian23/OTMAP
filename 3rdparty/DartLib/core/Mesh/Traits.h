#ifndef _DARTLIB_TRAITS_H_
#define _DARTLIB_TRAITS_H_

#include <string>

#include "../Geometry/Point.h"
#include "../Geometry/Point2.h"

/*===================================================================
                    Macro definition for traits

  DECLARE_TRAIT: Do NOT recommend to use it in other place.
      ADD_TRAIT: Used to shorten your code

  class CMyVertex :                    class CMyVertex :
        public TVertex<2>                    public TVertex<2>
  {                                    {
    public:                       ===>   public:   
      CMyVertex() : m_id(0) {  };          CMyVertex() : m_id(0) { };
      int& id() { return m_id; };        ADD_TRAIT(int, id)
    protected:                         }
      int m_id;
  };
===================================================================*/

#define DECLARE_TRAIT(TYPE, NAME, name)     \
struct NAME                                 \
{                                           \
  TYPE  m_##name;                           \
  TYPE& name()   { return m_##name; };      \
};

#define ADD_TRAIT(TYPE, name)               \
public:                                     \
  TYPE& name()   { return m_##name; };      \
protected:                                  \
  TYPE  m_##name;                           


namespace DartLib
{
namespace Trait
{
/*
enum Trait
{
    HAS_VERTEX_NORMAL,
    HAS_VERTEX_POINT,
    HAS_VERTEX_UV,
    HAS_VERTEX_ID,
    HAS_VERTEX_INDEX,
};
*/


/*===================================================================
               Pre-define some frequently used traits.
===================================================================*/

class Angle
{
  public:
    Angle() : m_angle(0) { };
    ADD_TRAIT(double, angle)
};

class Area
{
  public:
    Area() : m_area(0){};
    ADD_TRAIT(double, area)
};

class ID
{
  public:
    ID() : m_id(0){};
    ADD_TRAIT(int, id)
};

class Index
{
  public:
    Index() : m_idx(0){};
    ADD_TRAIT(int, idx)
};

class Length
{
  public:
    Length() : m_length(0){};
    ADD_TRAIT(int, length)
};

class Normal
{
    ADD_TRAIT(CPoint, normal)
};

class Point
{
    ADD_TRAIT(CPoint, point)
};

class RGB
{
    ADD_TRAIT(CPoint, rgb)
};

class Sharp
{
  public:
    Sharp() : m_sharp(false){};
    ADD_TRAIT(bool, sharp)
};

class String
{
  public:
    ADD_TRAIT(std::string, string)
};

class Touched
{
  public:
    Touched() : m_touched(false){};
    ADD_TRAIT(bool, touched)
};

class UV
{
    ADD_TRAIT(CPoint2, uv)
};

} // namespace Trait
} // namespace DartLib


#undef DECLARE_TRAIT

#endif // !_DARTLIB_TRAITS_H_
