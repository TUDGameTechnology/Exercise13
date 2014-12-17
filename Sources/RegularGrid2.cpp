#include "pch.h"
#include "RegularGrid2.h"


void RegularGrid2::ClipTriangle(int index) {
	// Use the algorithm that is inspired by the rasterization algorithm
}

	


void RegularGrid2::SetMesh(MeshObject* obj) {
	// Go through all triangles and clip them
	this->obj = obj;
}

int* RegularGrid2::GetPotentialTriangles(const SphereCollider& collider) {
	return nullptr;
}






	

RegularGrid2::RegularGrid2(vec3 origin, float cellSize, int numCells, int numTrianglesPerCell): origin(origin), cellSize(cellSize), numCells(numCells), numTrianglesPerCell(numTrianglesPerCell)
{
}


RegularGrid2::~RegularGrid2(void)
{
}
