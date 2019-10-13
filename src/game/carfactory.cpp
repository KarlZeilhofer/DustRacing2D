// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "carfactory.hpp"

#include <MCAssetManager>

CarPtr CarFactory::buildCar(int index, int numCars, Game & game)
{
    const int defaultPower = 200000; // This in Watts

    const float airDensity = 1.25; // kg/m³
    const float cw = 0.25f; // form dependent factor for stream lined drag
    const float A = 1.8f; // m², cross section area of car
    const float defaultQuadraticDrag = airDensity * cw * A / 2.0f; // in N/(m/s)²

    const float defaultFriction = 0.60; // rear wheel drive

    static const int NUM_CARS = numCars;
    static std::map<int, std::string> carImageMap = {
        { NUM_CARS - 1, "carBlack" },
        { NUM_CARS - 2, "carOrange" },
        { NUM_CARS - 3, "carRed" },
        { NUM_CARS - 4, "carBlue" },
        { NUM_CARS - 5, "carDarkGreen" },
        { NUM_CARS - 6, "carBrown" },
        { NUM_CARS - 7, "carCyan" },
        { NUM_CARS - 8, "carViolet" },
        { NUM_CARS - 9, "carGreen" },
        { NUM_CARS - 10, "carDarkRed" },
        { 1, "carGrey" },
        { 0, "carPink" }
    };

    Car::Description desc;

    // Select car image
    std::string carImage("carYellow");
    if (carImageMap.count(index)) {
        carImage = carImageMap[index];
    }

    CarPtr car;
    if (index == 0 || (index == 1 && game.hasTwoHumanPlayers())) {
        desc.power = defaultPower * Game::instance().difficultyProfile().powerMultiplier(true);
        desc.dragQuadratic = defaultQuadraticDrag;

        // rear wheel drive:
        desc.accelerationFriction = defaultFriction * Game::instance().difficultyProfile().accelerationFrictionMultiplier(true);
        // all wheel drive:
        //desc.accelerationFriction = 1.00f * Game::instance().difficultyProfile().accelerationFrictionMultiplier(true);

        car.reset(new Car(desc, MCAssetManager::surfaceManager().surface(carImage), index, true));
    } else if (game.hasComputerPlayers()) {
        // Introduce some variance to the power of computer players so that the
        // slowest cars have less power than the human player and the fastest
        // cars have more power than the human player.
        desc.power = defaultPower * Game::instance().difficultyProfile().powerMultiplier(true) * randomFactor(0.8f, 1.2f);
        desc.accelerationFriction = defaultFriction * randomFactor(0.8f, 1.2f) * Game::instance().difficultyProfile().accelerationFrictionMultiplier(false);
        desc.dragQuadratic = defaultQuadraticDrag * randomFactor(0.8f, 1.2f);

        car.reset(new Car(desc, MCAssetManager::surfaceManager().surface(carImage), index, false));
    }

    return car;
}

float CarFactory::randomFactor(float from, float to)
{
    return (rand() % 1001) / 1000.0f * (to - from) + from;
}
