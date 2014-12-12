#pragma once
#include <Kore/Math/Vector.h>
#include "MeshObject.h"
#include "Collision.h"


/** Manages a regular grid which can manage triangle positions. Initialize it with a mesh to save which triangles are inside which grid cell */
class RegularGrid2
{
	vec3 origin;

	float cellSize;

	int numCells;

	int numTrianglesPerCell;

	/// Array of numCells^2 * numTrianglesPerCell
	int* triangles;

	MeshObject* obj;
	
	void ClipTriangle(int index);

	


public:
	
	// Initialize the grid from the mesh
	void SetMesh(MeshObject* obj);

	// Get a list with all indices of triangles that might be colliding with the sphere
	int* GetPotentialTriangles(const SphereCollider& collider);






	RegularGrid2(vec3 origin, float cellSize, int numCells, int numTrianglesPerCell);
	~RegularGrid2(void);
};

