#include "ObjectFactory.h"

#include <string>
#include <nlohmann/json.hpp>
#include <irrlicht.h>
#include <iostream>

#include "DefaultSensors.h"
#include "DefaultObjectStorage.h"

using std::operator""s;
using Json = DefaultObjectFactory::Json;

using vector3 = irr::core::vector3df;

SensorInterface* DefaultObjectFactory::getSensor(std::string& name, Json& atributes)
{
    SensorInterface* sensor = nullptr;
    std::string type = atributes["type"].get<std::string>();

    if (type == "sensor-DistanceSensor-default") {
        std::string mesh = atributes["mesh"].get <std::string>();
        std::string text = atributes["texture"].get <std::string>();
        vector3 postion(atributes["position"][0].get<float>(), atributes["position"][1].get<float>(), atributes["position"][2].get<float>());
        vector3 rotation(atributes["rotation"][0].get<float>(), atributes["rotation"][1].get<float>(), atributes["rotation"][2].get<float>());
        vector3 scale(atributes["scale"][0].get<float>(), atributes["scale"][1].get<float>(), atributes["scale"][2].get<float>());
        float range = atributes["range"].get<float>();
        sensor = new DistanceSensor(mesh, text, postion, rotation, scale, range, name);
    }
    else if (type == "sensor-GPS-default") {
        sensor = new GPS(name);
    }
    else if (type == "sensor-Altimeter-default") {
        sensor = new Altimeter(name);
    }
    else if (type == "sensor-Velocimometer-default") {
        sensor = new Velocimometer(name);
    }
    else if (type == "sensor-Accelerometer3D-default") {
        sensor = new Accelerometer3D(name);
    }
    else if (type == "sensor-Accelerometer-default") {
        sensor = new Accelerometer(name);
    }
    else throw std::exception("sensor type not found");

    return sensor;
}

Drone* DefaultObjectFactory::getDrone(Json& atributes)
{
    std::string mesh = atributes["mesh"].get<std::string>();
    std::string text = atributes["texture"].get<std::string>();
    vector3 pos(atributes["mesh-location"][0].get<float>(), atributes["mesh-location"][1].get<float>(),  atributes["mesh-location"][2].get<float>());
    vector3 rot(atributes["mesh-rotation"][0].get<float>(), atributes["mesh-rotation"][1].get<float>(),  atributes["mesh-rotation"][2].get<float>());
    vector3 sca(atributes["mesh-scale"][0].get<float>(),    atributes["mesh-scale"][1].get<float>(),     atributes["mesh-scale"][2].get<float>());
    vector3 forward(atributes["drone-forward"][0].get<float>(), atributes["drone-forward"][1].get<float>(), atributes["drone-forward"][2].get<float>());
    Drone::DroneAttributes atribs;
    atribs.maxAltitude = atributes["max-altitude"].get<float>();
    atribs.maxForwardVelocity = atributes["max-forward-velocity"].get<float>();
    atribs.maxUpwardVelocity = atributes["max-upward-velocity"].get<float>();
    atribs.maxDownwordsVelocity = atributes["max-downword-velocity"].get<float>();
    atribs.mass = atributes["mass"].get<float>();
    atribs.maxAltitude = atributes["max-altitude"].get<float>();
    atribs.maxAccelerationInKPH = atributes["max-acceleration-value"].get<float>();
    atribs.maxAccelerationTimeInSeconds = atributes["max-acceleration-time"].get<float>();

    return new Drone(mesh, text, pos, rot, sca, forward, atribs);
}

StaticObject* DefaultObjectFactory::getStaticObject(Json& atributes, const std::string& name, const bool bHasCollision, const bool bAddToRaycast)
{
    Json tmp;
    std::string meshPath = atributes["mesh-path"].get<std::string>();
    std::string texturePath = atributes["texture-path"].get<std::string>();
    tmp = atributes["position"];
    vector3 position = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());
    tmp = atributes["rotation"];
    vector3 rotation = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());
    tmp = atributes["scale"];
    vector3 scale = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());

    return new StaticObject(meshPath, texturePath, position, rotation, scale, bHasCollision, bAddToRaycast, name);
}

DynamicObject* DefaultObjectFactory::getDynamicObject(Json& atributes, const std::string& name, const bool bHasCollision, const bool bAddToRaycast)
{
    Json tmp;
    std::string meshPath = atributes["mesh-path"].get<std::string>();
    std::string texturePath = atributes["texture-path"].get<std::string>();
    tmp = atributes["position"];
    vector3 position = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());
    tmp = atributes["rotation"];
    vector3 rotation = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());
    tmp = atributes["scale"];
    vector3 scale = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());
    tmp = atributes["forward-vector"];
    vector3 forwardVector = vector3(tmp[0].get<float>(), tmp[1].get<float>(), tmp[2].get<float>());

    return new DynamicObject(meshPath, texturePath, position, rotation, forwardVector, scale, bHasCollision, bAddToRaycast, name);
}

ObjectControllerInterface* DefaultObjectFactory::getAI(const std::string& name, Json& atributes)
{
    return nullptr;
}
