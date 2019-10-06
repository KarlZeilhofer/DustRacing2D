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

#ifndef MCCOLLISIONDETECTOR_HH
#define MCCOLLISIONDETECTOR_HH

#include "mcmacros.hh"

#include <vector>
#include <map>
#include <set>

class MCCircleShape;
class MCObject;
class MCObjectGrid;
class MCRectShape;

//! Collision detector and contact generator.
class MCCollisionDetector
{
public:

    //! Constructor.
    MCCollisionDetector();

    //! Destructor.
    virtual ~MCCollisionDetector() {}

    void clear();

    void remove(MCObject & object);

    //! Detect collisions and generate contacts. Contacts are stored to MCObject.
    unsigned int detectCollisions(MCObjectGrid & objectGrid);

    //! Iterate current collisions and generate contacts. Contacts are stored to MCObject.
    unsigned int iterateCurrentCollisions();

private:

    DISABLE_COPY(MCCollisionDetector);
    DISABLE_ASSI(MCCollisionDetector);

    bool areCurrentlyColliding(MCObject & object1, MCObject & object2);

    bool processPossibleCollision(MCObject & object1, MCObject & object2);

    bool testRectAgainstRect(MCRectShape & object1, MCRectShape & object2);

    bool testRectAgainstCircle(MCRectShape & object1, MCCircleShape & object2);

    bool testCircleAgainstCircle(MCCircleShape & object1, MCCircleShape & object2);

    using CollisionMap = std::map<MCObject *, std::set<MCObject *>>;
    CollisionMap m_currentCollisions;
};

#endif // MCCOLLISIONDETECTOR_HH
