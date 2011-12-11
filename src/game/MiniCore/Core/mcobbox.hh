// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCOBBOX_HH
#define MCOBBOX_HH

#include "mcbbox.hh"
#include "mcvector2d.hh"
#include "mctypes.hh"
#include "mctrigonom.hh"
#include "mcmathutil.hh"

#include <algorithm>

/*! \class MCOBBox
 *  \brief Oriented bounding-box template
 *
 * Vertices are defined clock-wise:
 *
 * v1--v2
 * |    |
 * v0--v3
 */
template <typename T>
class MCOBBox
{
public:

  /*! Constructor
   * \param hx    Local half width of the bbox
   * \param hy    Local half height of the bbox
   * \param loc   Location of the center
   */
  MCOBBox(T hx, T hy, const MCVector2d<T> & loc);

  //! Return current angle
  inline MCUint angle() const {return m_a;}

  //! Return current location
  inline const MCVector2d<T> & location() const {return m_p;}

  //! Return current half width
  inline T hx() const {return m_hx;}

  //! Return current half height
  inline T hy() const {return m_hy;}

  //! Return given vertex
  inline MCVector2d<T> vertex(MCUint index) const {return m_v[index & 0x3] + m_p;}

  //! Return bbox of the MCOBBox
  inline MCBBox<T> bbox() const;

  /*! Return true if given point is inside the MCOBBox
   * \param p The point to be tested
   */
  bool contains(MCVector2d<T> p) const;

  /*! Return true if intersects with given MCOBBox
   * \param r The MCOBBox to be tested
   */
  template <typename U>
  bool intersects(const MCOBBox<U> & r) const;

  /*! Return true if intersects with given BBox
   * \param r The BBox to be tested
   */
  template <typename U>
  bool intersects(const MCBBox<U> & r) const;

  /*! Rotate
   * \param a Rotation angle in degrees (0..360)
   */
  void rotate(MCUint a);

  /*! Translate
   * \param p The new location
   */
  void translate(const MCVector2d<T> & p);

  //! Scale
  void scale(T s);

private:

  //! Initial half X-axis
  T m_hx;

  //! Initial half Y-axis
  T m_hy;

  //! Location
  MCVector2d<T> m_p;

  //! Rotation angle
  MCUint m_a;

  //! Vertex vectors
  MCVector2d<T> m_v[4];
};

template <typename T>
MCOBBox<T>::MCOBBox(T newHx, T newHy, const MCVector2d<T> & loc) :
  m_hx(newHx),
  m_hy(newHy),
  m_p(loc),
  m_a(0)
{
  m_v[0] = MCVector2d<T>(-m_hx, -m_hy);
  m_v[1] = MCVector2d<T>(-m_hx,  m_hy);
  m_v[2] = MCVector2d<T>( m_hx,  m_hy);
  m_v[3] = MCVector2d<T>( m_hx, -m_hy);
}

template <typename T>
bool MCOBBox<T>::contains(MCVector2d<T> p) const
{
  // Translate the test point
  p -= m_p;

  // Calculate edge vectors
  const MCVector2d<T> e0 = m_v[1] - m_v[0];
  const MCVector2d<T> e1 = m_v[2] - m_v[1];
  const MCVector2d<T> e2 = m_v[3] - m_v[2];
  const MCVector2d<T> e3 = m_v[0] - m_v[3];

  // Do the test
  const MCMathUtil::SIGN ref = MCMathUtil::sign(e0 * (m_v[1] - p));
  if (MCMathUtil::sign(e1 * (m_v[2] - p)) == ref &&
      MCMathUtil::sign(e2 * (m_v[3] - p)) == ref &&
      MCMathUtil::sign(e3 * (m_v[0] - p)) == ref)
  {
    return true;
  }

  return false;
}

template <typename T>
void MCOBBox<T>::rotate(MCUint a)
{
  if (a != m_a)
  {
    // Store the new angle
    m_a = a;

    // Update vertex vectors
    T newI = MCTrigonom::rotatedX(m_v[0].i(), m_v[0].j(), m_a);
    T newJ = MCTrigonom::rotatedY(m_v[0].i(), m_v[0].j(), m_a);

    m_v[0].setI(newI);
    m_v[0].setJ(newJ);

    newI = MCTrigonom::rotatedX(m_v[1].i(), m_v[1].j(), m_a);
    newJ = MCTrigonom::rotatedY(m_v[1].i(), m_v[1].j(), m_a);

    m_v[1].setI(newI);
    m_v[1].setJ(newJ);

    // Mirror the other two vertices
    m_v[2].setI(-m_v[0].i());
    m_v[2].setJ(-m_v[0].j());

    m_v[3].setI(-m_v[1].i());
    m_v[3].setJ(-m_v[1].j());
  }
}

template <typename T>
void MCOBBox<T>::translate(const MCVector2d<T> & p)
{
  m_p = p;
}

template <typename T>
template <typename U>
bool MCOBBox<T>::intersects(const MCOBBox<U> & r) const
{
  // Test if r includes at least one vertice
  if (r.contains(m_v[0]) || r.contains(m_v[1]) || r.contains(m_v[2]) || r.contains(m_v[3]))
  {
    // Yes: intersection
    return true;
  }
  // Test if this includes at least one of r's vertices
  else if (contains(r.m_v[0]) || contains(r.m_v[1]) || contains(r.m_v[2]) || contains(r.m_v[3]))
  {
    // Yes: intersection
    return true;
  }

  // Didn't intersect
  return false;
}

template <typename T>
template <typename U>
bool MCOBBox<T>::intersects(const MCBBox<U> & r) const
{
  // Test if r includes at least one vertice
  if (r.contains(m_v[0]) || r.contains(m_v[1]) || r.contains(m_v[2]) || r.contains(m_v[3]))
  {
    // Yes: intersection
    return true;
  }
  // Test if this includes at least one of r's vertices
  else if (contains(MCVector2d<U>(r.x1, r.y1)) || contains(MCVector2d<U>(r.x1, r.y2)) ||
           contains(MCVector2d<U>(r.x2, r.y1)) || contains(MCVector2d<U>(r.x2, r.y2)))
  {
    // Yes: intersection
    return true;
  }

  // Didn't intersect
  return false;
}

template <typename T>
MCBBox<T> MCOBBox<T>::bbox() const
{
  const MCVector2d<T> v0p = m_v[0] + m_p;
  const MCVector2d<T> v1p = m_v[1] + m_p;
  const MCVector2d<T> v2p = m_v[2] + m_p;
  const MCVector2d<T> v3p = m_v[3] + m_p;

  return MCBBox<T>(std::min(std::min(v0p.i(), v1p.i()), std::min(v2p.i(), v3p.i())),
                   std::min(std::min(v0p.j(), v1p.j()), std::min(v2p.j(), v3p.j())),
                   std::max(std::max(v0p.i(), v1p.i()), std::max(v2p.i(), v3p.i())),
                   std::max(std::max(v0p.j(), v1p.j()), std::max(v2p.j(), v3p.j())));
}

template <typename T>
void MCOBBox<T>::scale(T s)
{
  m_hx   *= s;
  m_hy   *= s;
  m_v[0] *= s;
  m_v[1] *= s;
  m_v[2] *= s;
  m_v[3] *= s;
}

#endif // MCOBBOX_HH