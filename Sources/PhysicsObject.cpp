#include "PhysicsObject.h"
#include <Kore/Log.h>

using namespace Kore;

PhysicsObject::PhysicsObject() {
	Accumulator = vec3(0, 0, 0);
	Velocity = vec3(0, 0, 0);
	Collider.radius = 0.1f;
}

void PhysicsObject::HandleCollision(const PlaneCollider& collider, float deltaT) {
	 // Check if we are colliding with the plane
	if (Collider.IntersectsWith(collider)) {

		float restitution = 0.8f;

		// Calculate the separating velocity
		float separatingVelocity = -(collider.normal * Velocity);

		if (separatingVelocity < 0) return;
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;

		
			
			
			
		// Move the object out of the collider
		float penetrationDepth = Collider.PenetrationDepth(collider); 
		Position += collider.normal * penetrationDepth;

		// Calculate the impulse
		// The plane is immovable, so we have to move all the way
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;

		// If the object is very slow, assume resting contact
		if (deltaVelocity > -1.5f) {
			Velocity.set(0, 0, 0);
			Position = vec3(Position.x(), Collider.radius - collider.d, Position.z());
			Collider.center = Position;
			return;
		}

		// Apply the impulse
		vec3 impulse = collider.normal * -deltaVelocity;

		ApplyImpulse(impulse);
			
	}
}


void PhysicsObject::HandleCollision(PhysicsObject* other, float deltaT) {
	// Check if we are colliding with the plane
	if (Collider.IntersectsWith(other->Collider)) {

		//Kore::log(Info, "Intersection");

		float restitution = 0.8f;

		vec3 collisionNormal = Collider.GetCollisionNormal(other->Collider);
			
		float separatingVelocity = -(other->Velocity - Velocity) * collisionNormal;

		if (separatingVelocity < 0) return;

		float newSeparatingVelocity = -separatingVelocity * restitution;


		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
			
		// Move the object out of the collider
		float penetrationDepth = -Collider.PenetrationDepth(other->Collider);
				
		SetPosition(Position - collisionNormal * penetrationDepth * 100.0f);
		other->SetPosition(other->Position + collisionNormal * penetrationDepth * 100.0f);

		vec3 impulse = collisionNormal * -deltaVelocity;

		ApplyImpulse(impulse);
		other->ApplyImpulse(-impulse);
	}
}


void PhysicsObject::ApplyImpulse(vec3 impulse) {
	Velocity += impulse;
}

void PhysicsObject::ApplyForceToCenter(vec3 force) {
	Accumulator += force;
}


void PhysicsObject::Integrate(float deltaT) {
	// Derive a new position based on the velocity (Note: Use SetPosition to also set the collider's values)
	
	// Derive a new Velocity based on the accumulated forces

	// Multiply by a damping coefficient (e.g. 0.98)
	
	// Clear the accumulator
}

void PhysicsObject::UpdateMatrix() {
	// Update the Mesh matrix
		Mesh->M = mat4::Translation(Position.x(), Position.y(), Position.z()) * mat4::Scale(0.2f, 0.2f, 0.2f);
}



