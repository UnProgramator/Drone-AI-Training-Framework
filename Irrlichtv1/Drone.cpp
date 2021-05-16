#include "Drone.h"

#include <irrlicht.h>
#include <iostream>
#include <list>
#include <memory>
#include "DynamicObject.h"
#include "GraphicsManager.h"

using namespace std;

#ifdef drone_debug_on
#error
#endif

#define drone_debug_on true

#if drone_debug_on == true
#define debug_call(x) {std::cerr << x; throw std::exception(x);}
#define debug_call_cond(cond, x) if((cond)) std::cerr << x
#else
#define debug_call(...)
#define debug_call_cond(...)
#endif

Drone::Drone(const irr::core::vector3df& initalPosition, const irr::core::vector3df& initalRotation, const irr::core::vector3df& initalOrientation, PhysicsManager * physiscsMgr):phyMgr{physiscsMgr}
{
    std::cout << "default drone cube should not be used in final apps";
    irr::scene::IMeshSceneNode* node = getCube();
    mesh= new DynamicObject(node, initalPosition, initalRotation, initalOrientation, true);
}

Drone::Drone(const std::string& meshPath, const std::string& textPath, const irr::core::vector3df& initalPosition, const irr::core::vector3df& initalRotation, const irr::core::vector3df& initalOrientation, PhysicsManager* physiscsMgr):
    Drone(meshPath, textPath, initalPosition, initalRotation, irr::core::vector3df(1.f, 1.f, 1.f), initalOrientation, physiscsMgr)
{
}

Drone::Drone(const std::string& meshPath, const std::string& textPath, const irr::core::vector3df& initalPosition, const irr::core::vector3df& initalRotation, const irr::core::vector3df& scale, const irr::core::vector3df& initalOrientation, PhysicsManager* physiscsMgr) :
    phyMgr{ physiscsMgr }
{
    /*irr::scene::IMeshSceneNode* node = getStaticMesh(path + "", path + ".png", 0);
    node->setScale()
    mesh = new DynamicObject(node, initalPosition, initalRotation, initalOrientation, true);*/
    mesh = new DynamicObject(meshPath, textPath, initalPosition, initalRotation, initalOrientation, scale, true, "Drone");
} 

bool Drone::verifyCollision(class StaticObject* otherObject)
{
    return this->mesh->colideWith(otherObject);
}

void Drone::moveForwards(float ratio)
{
    if (ratio > 1.f) {
        debug_call("supraunitary forward movement ration detected");
        ratio = 1.f;
    }
    else if (ratio < -1.f) {
        debug_call("supraunitary forward negative ration movement detected");
        ratio = -1.f;
    }

    debug_call_cond(ratio<=0, "negative forward movement detected");

    forward_ratio = ratio;
}

void Drone::rotateRight(float ratio)
{
    if (ratio > 1.f) {
        debug_call("supraunitary right turm movement ratio detected");
        ratio = 1.f;
    }
    else if (ratio < -1.f) {
        debug_call("supraunitary right negative turm movement ratio detected");
        ratio = -1.f;
    }

    rotation_ratio = ratio;
}

void Drone::moveUp(float ratio)
{
    if (ratio > 1.f) {
        debug_call("supraunitary forward movement ration detected");
        ratio = 1.f;
    }
    else if (ratio < -1.f) {
        debug_call("supraunitary forward negative ration movement detected");
        ratio = -1.f;
    }

    up_ratio = ratio;
}

void Drone::add_sensor(SensorInterface* sensor)
{
    sensor->link_to(this->mesh);
    this->sensor_list.push_back(sensor);
}

void Drone::getSensorReadValues(default_CommunicationInterface& transmiter)
{
    transmiter.init_parser(sensor_list.size());
    for (auto* sensor : sensor_list) {
        sensor->getDetectedValue(transmiter);
    }
}

void Drone::tick(float deltaTime)
{
    if (forward_ratio != 0 || up_ratio != 0) {

        irr::core::vector3df speed = mesh->getForwardVector();
        if (forward_ratio != 0)
            speed *= forward_ratio * maxFrowardSpeed;
        if (up_ratio != 0)
            speed.Z = up_ratio * maxUpSpeed;
        mesh->addInputVector(speed * deltaTime);
        forward_ratio = up_ratio = 0;
    }
    if (rotation_ratio != 0) {
        mesh->rotate(irr::core::vector3df(0.f, 1.f, 0.f) * rotation_ratio * maxRightRotationSpeed * deltaTime);
        rotation_ratio = 0;
    }
}

void Drone::reset(bool toDefault)
{
    this->mesh->reset(toDefault);
    forward_ratio = 0;
    rotation_ratio = 0;
    up_ratio = 0;
}

irr::core::vector3df Drone::getVelocity() const
{
    return phyMgr->getVelocity();
}

float Drone::getForwardVelocity() const
{
    return phyMgr->getVelocity().Z;
}

irr::core::vector3df Drone::getAccelerations() const
{
    return phyMgr->getAcceleration();
}

float Drone::getForwardAcceleration() const
{
    return  phyMgr->getAcceleration().Z;
}


#undef drone_debug_on
#undef debug_call_cond