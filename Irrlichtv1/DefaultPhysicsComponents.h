#pragma once
#include "PhysicsComponentInterface.h"
#include "CommunicationInterface.h"
#include "PhysicsManager.h"


class Wind : public PhysicsComponentInterface {
	vector3 force;

	vector3 compute_force(const vector3& direction, float velocity);
	void verify_direction(vector3&);

public:
	using vector3 = PhysicsComponentInterface::vector3;
	virtual const vector3& getForce() const override;
	Wind();
	Wind(const vector3& direction, const float velocity);
	void makeRandom();
	void setForce(const vector3& direction, float velocity);
};

class DronePhysicsManager : public PhysicsComponentInterface {
public:
	struct DroneAttributes {
		float	maxForwardVelocity, /*in m/s*/
			maxUpwardVelocity, /*in m/s*/
			maxDownwordsVelocity, /*in m/s*/

			/*maxAccelerationInKPH and maxAccelerationTimeInSeconds corespond to the acceleration, ex 0-100 km/h in 2 seconds as given in the drone sheet*/
			maxAccelerationInKPH, /*in m/s*/
			maxAccelerationTimeInSeconds, /*in s*/

			maxAltitude, /*in m*/
			mass/*in kg*/;
		float maxRightRotationSpeed = 20;
	};

	static const float coeficient; // stub til i fix the problem of velocity depending on the scale of the object.

	DronePhysicsManager(const vector3& forwardsDirection, const DroneAttributes& attrs);

	bool computeNewParameters(const default_ReturnedValueFromStript& inputs, float deltaTime, bool bExistExternalForces = true);

	virtual const vector3& getForce() const override;
	const vector3& getVelocity() const;
	const vector3& getAcceleration() const;

	void reset();

private:
	void acceleration_corection(vector3& acceleration, const default_ReturnedValueFromStript& inputs, float deltatime); // function apply inertia, friction and gravoty effects to acceleration for more acurate values

private:
	using vector3 = PhysicsComponentInterface::vector3;
	const float mass; //in kilograms
	const float maxLinearAcceleration;
	vector3 crt3DAcceleration;
	const float maxPlaneVelocity;
	const float maxUpVelocity;
	const float maxDownVelocity;
	vector3 crt3DVelocity;
	const vector3& forwardsDirection;
	PhysicsManager* phyMgr;
};