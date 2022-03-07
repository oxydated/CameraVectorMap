#include <cmath>

void rotateVectorByQuat(float qs, float qx, float qy, float qz, float sx, float sy, float sz, float& rx, float& ry, float& rz)
{
	float qs2 = qs * qs;
	float qx2 = qx * qx;
	float qy2 = qy * qy;
	float qz2 = qz * qz;

	rx = (1 - 2 * qy2 - 2 * qz2) * sx + 2 * (qx * qy + qs * qz) * sy + (-2 * qs * qy + 2 * qx * qz) * sz;
	ry = 2 * (qx * qy - qs * qz) * sx + (1 - 2 * qx2 - 2 * qz2) * sy + 2 * (qs * qx + qy * qz) * sz;
	rz = 2 * (qs * qy + qx * qz) * sx + (-2 * qs * qx + 2 * qy * qz) * sy + (1 - 2 * qx2 - 2 * qy2) * sz;
}

void transformFromTangentSpaceToWorld(float nx, float ny, float nz, float sx, float sy, float sz, float& rx, float& ry, float& rz) {
	float nx2 = nx * nx;
	float ny2 = ny * ny;
	float nz2 = nz * nz;

	rx = -(((-1 + nz) * (ny2 * sx - nx * ny * sy + (1 + nz) * (nz * sx + nx * sz))) / (nx2 + ny2));
	ry = -(((-1 + nz) * (-(nx * ny * sx) + (1 - ny2 + nz) * sy + ny * (1 + nz) * sz)) / (nx2 + ny2));
	rz = -(nx * sx) - ny * sy + nz * sz;
}