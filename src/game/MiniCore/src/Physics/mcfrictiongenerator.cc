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

#include "mcfrictiongenerator.hh"
#include "mcmathutil.hh"
#include "mcobject.hh"
#include "mcphysicscomponent.hh"
#include "mcshape.hh"

#include <cmath>

static const float ROTATION_DECAY = 0.01f;

MCFrictionGenerator::MCFrictionGenerator(float coeffLin, float coeffRot)
    : m_coeffLinTot(std::fabs(coeffLin * MCWorld::instance().gravity().k())) // Coeff for Coluomb Friction on translations on the surface
    , m_coeffRotTot(std::fabs(coeffRot * MCWorld::instance().gravity().k() * ROTATION_DECAY)) // Coeff for Coluomb Friction on rotations on the surface
{}

void MCFrictionGenerator::updateForce(MCObject & object)
{
    // Simulated friction caused by linear motion.
    MCPhysicsComponent & physicsComponent = object.physicsComponent();
    const float length = physicsComponent.velocity().lengthFast();
    const MCVector2d<float> velocityDirection(physicsComponent.velocity().normalizedFast());
    if (length >= 1.0f)
    {
        physicsComponent.addForce(-velocityDirection * m_coeffLinTot * physicsComponent.mass());
    }
    else
    {
        physicsComponent.addForce(-velocityDirection * length * m_coeffLinTot * physicsComponent.mass());
    }

    // Simulated friction caused by angular torque.
    if (object.shape())
    {
        const float a = physicsComponent.angularVelocity();
		if(a > 0)
			physicsComponent.addTorque(- m_coeffRotTot * physicsComponent.mass() * 1.0f); // TODO: replace constant of 1.0m for average radius
		if(a < 0)
			physicsComponent.addTorque(+ m_coeffRotTot * physicsComponent.mass() * 1.0f); // TODO: replace constant of 1.0m for average radius
    }
}

MCFrictionGenerator::~MCFrictionGenerator()
{
}

