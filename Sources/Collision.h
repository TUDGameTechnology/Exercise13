#pragma once

#include "pch.h"
#include "MeshObject.h"

using namespace Kore;


// Task 1.3: Implement the box-sphere intersection test you derived for this task. You can use the collider in the main update function
// to test it against the sphere (you can find it via phyics.physicsObject[0]).
class BoxCollider {
public:

};


// A plane is defined as the plane's normal and the distance of the plane to the origin
class PlaneCollider {
public:
	float d;
	vec3 normal;


};


class TriangleCollider {
	void LoadVector(vec3& v, int index, float* vb) {
		v.set(vb[index*8 + 0], vb[index*8 + 1], vb[index*8 + 2]);
	}

public:
	vec3 A;
	vec3 B;
	vec3 C;

	float Area() {
		return 0.5f * ((B-A).cross(C-A)).getLength();
	}

	void LoadFromBuffers(int index, int* ib, float* vb) {
		LoadVector(A, ib[index * 3], vb);
		LoadVector(B, ib[index * 3 + 1], vb);
		LoadVector(C, ib[index * 3 + 2], vb);
	}

	vec3 GetNormal() {
		vec3 n = (B-A).cross(C-A);
		n.normalize();
		return n;
	}

	PlaneCollider GetPlane() {
		vec3 n = GetNormal();
		float d = n.dot(A);

		PlaneCollider p;
		p.d = -d;
		p.normal = n;
		return p;

	}
	

};





class TriangleMeshCollider {
public:
	MeshObject* mesh;

	int lastCollision;
};



// A sphere is defined by a radius and a center.
class SphereCollider {
public:

	
	SphereCollider() {
	}


	vec3 center;
	float radius;

	// Return true iff there is an intersection with the other sphere
	bool IntersectsWith(const SphereCollider& other) {
		float distance = (other.center - center).getLength();
		return distance < other.radius + radius;
	}

	// Collision normal is the normal vector pointing towards the other sphere
	vec3 GetCollisionNormal(const SphereCollider& other) {
		vec3 n = other.center - center;
		n = n.normalize();
		return n;
	}

	// The penetration depth
	float PenetrationDepth(const SphereCollider& other) {
		return other.radius + radius - (other.center - center).getLength();
	}



	bool IntersectsWith(const PlaneCollider& other) {
		return other.normal.dot(center) + other.d <= radius;
	}

	vec3 GetCollisionNormal(const PlaneCollider& other) {
		return other.normal;
	}

	float PenetrationDepth(const PlaneCollider &other) {
		return other.normal.dot(center) + other.d - radius;
	}

	

	vec3 GetCollisionNormal(const TriangleCollider& other) {
		// Use the normal of the triangle plane
		vec3 n = (other.B - other.A).cross(other.C - other.A);
		n.normalize();
		return n;
	}

	float PenetrationDepth(const TriangleCollider& other) {
		// Use the triangle plane
		// The assumption is that we have a mesh and will not collide only on a vertex or an edge

		return 0.0f;
	}

	
	bool IntersectsWith(TriangleMeshCollider& other) {
		TriangleCollider coll;
		int* current = other.mesh->mesh->indices;
		float* currentVertex = other.mesh->mesh->vertices;
		for (int i = 0; i < other.mesh->mesh->numFaces; i++) {
			coll.LoadFromBuffers(i, current, currentVertex);
			if (coll.Area() < 0.1f) continue;
			if (IntersectsWith(coll)) {
				other.lastCollision = i;
				return true;
			}
		}
		return false;
	}


	vec3 GetCollisionNormal(const TriangleMeshCollider& other) {
		TriangleCollider coll;
		coll.LoadFromBuffers(other.lastCollision, other.mesh->mesh->indices, other.mesh->mesh->vertices);
		return coll.GetNormal();
	}

	float PenetrationDepth(const TriangleMeshCollider& other) {
		// Get a collider for the plane of the triangle
		TriangleCollider coll;
		coll.LoadFromBuffers(other.lastCollision, other.mesh->mesh->indices, other.mesh->mesh->vertices);
		PlaneCollider plane = coll.GetPlane();


		return PenetrationDepth(plane);		
	}


	// Find the point where the sphere collided with the triangle mesh
	vec3 GetCollisionPoint(const TriangleMeshCollider& other) {
		// We take the point on the sphere that is closest to the triangle
		vec3 result = center - GetCollisionNormal(other) * radius;
		return result;
	}

	


	// Task 1.2: Find a set of basis vectors such that the provided collision normal x is the first column of the basis matrix,
	// and the other two vectors are perpendicular to the collision normal
	mat3 GetCollisonBasis(vec3 x) {
		return mat3::Identity();
	}


	// Return true if the axis from vertex A of the triangle to the sphere is a separating axis
	bool IsSeparatedByA(const TriangleCollider& other) {
		return false;
	}


	bool IntersectsWith(const TriangleCollider& other) {
		// See http://realtimecollisiondetection.net/blog/?p=103
		// Separating Axes Test
		// 1 Face
		// 3 Vertices
		// 3 Edges
		// No overlap if all of the resulting axes are not touching

		vec3 A = other.A - center;
		vec3 B = other.B - center;
		vec3 C = other.C - center;
		float rr = radius * radius;
		vec3 V = (B - A).cross(C - A);
		float d = A.dot(V);
		float e = V.dot(V);
		bool sep1 = d * d > rr * e;
		float aa = A.dot(A);
		float ab = A.dot(B);
		float ac = A.dot(C);
		float bb = B.dot(B);
		float bc = B.dot(C);
		float cc = C.dot(C);
		bool sep2 = IsSeparatedByA(other);
		bool sep3 = (bb > rr) & (ab > bb) & (bc > bb);
		bool sep4 = (cc > rr) & (ac > cc) & (bc > cc);
		vec3 AB = B - A;
		vec3 BC = C - B;
		vec3 CA = A - C;
		float d1 = ab - aa;
		float d2 = bc - bb;
		float d3 = ac - cc;
		float e1 = AB.dot(AB);
		float e2 = BC.dot(BC);
		float e3 = CA.dot(CA);
		vec3 Q1 = A * e1 - d1 * AB;
		vec3 Q2 = B * e2 - d2 * BC;
		vec3 Q3 = C * e3 - d3 * CA;
		vec3 QC = C * e1 - Q1;
		vec3 QA = A * e2 - Q2;
		vec3 QB = B * e3 - Q3;
		bool sep5 = (Q1.dot(Q1) > rr * e1 * e1) & (Q1.dot(QC) > 0);
		bool sep6 = (Q2.dot(Q2) > rr * e2 * e2) & (Q2.dot(QA) > 0);
		bool sep7 = (Q3.dot(Q3) > rr * e3 * e3) & (Q3.dot(QB) > 0);
		bool separated = sep1 | sep2 | sep3 | sep4 | sep5 | sep6 | sep7;
		return !separated;
	}

};
