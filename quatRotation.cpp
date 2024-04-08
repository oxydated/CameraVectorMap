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

	bool nanResult = std::isnan(rx) || std::isnan(ry) || std::isnan(rz);

	return;
}

void crossProduct(float nx, float ny, float nz, float ux, float uy, float uz, float& rx, float& ry, float& rz){
	rx = -(nz * uy) + ny * uz;
	ry = nz* ux - nx * uz;
	rz = -(ny * ux) + nx * uy;

	if (std::isnan(rx) || std::isnan(ry) || std::isnan(rz)) {
		float someX = rx;
	}

}

void bitangentFromNormalAndUVector(float nx, float ny, float nz, float ux, float uy, float uz, float& rx, float& ry, float& rz) {
	float nx2 = nx * nx;
	float ny2 = ny * ny;
	float nz2 = nz * nz;

	float ux2 = ux * ux;
	float uy2 = uy * uy;
	float uz2 = uz * uz;

	rx = (ux - nx * (nx * ux + ny * uy + nz * uz)) / std::sqrt(ny2 * (ux - uy) * (ux + uy) + uy2 - 2 * nx * nz * ux * uz + nz2 * (ux - uz) * (ux + uz) - 2 * ny * uy * (nx * ux + nz * uz) + uz2);
	ry = (uy - ny * (nx * ux + ny * uy + nz * uz)) / std::sqrt(ny2 * (ux - uy) * (ux + uy) + uy2 - 2 * nx * nz * ux * uz + nz2 * (ux - uz) * (ux + uz) - 2 * ny * uy * (nx * ux + nz * uz) + uz2);
	rz = (uz - nz * (nx * ux + ny * uy + nz * uz)) / std::sqrt(ny2 * (ux - uy) * (ux + uy) + uy2 - 2 * nx * nz * ux * uz + nz2 * (ux - uz) * (ux + uz) - 2 * ny * uy * (nx * ux + nz * uz) + uz2);

	if (std::isnan(rx) || std::isnan(ry) || std::isnan(rz)) {
		float someX = rx;
	}
}

void transformToBasis(float nx, float ny, float nz, float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz, float& rx, float& ry, float& rz){
	rx = nx* xx + ny * yx + nz * zx;
	ry = nx* xy + ny * yy + nz * zy;
	rz = nx* xz + ny * yz + nz * zz;

	if (std::isnan(rx) || std::isnan(ry) || std::isnan(rz)) {
		float someX = rx;
	}
}

void normalTransformed(float nx, float ny, float nz, float mx, float my, float mz, float ux, float uy, float uz, float& rx, float& ry, float& rz){
	float btx, bty, btz;

	bitangentFromNormalAndUVector(nx, ny, nz, ux, uy, uz, btx, bty, btz);

	float bnx, bny, bnz;

	crossProduct(nx, ny, nz, btx, bty, btz, bnx, bny, bnz);

	transformToBasis(mx, my, mz, btx, bty, btz, bnx, bny, bnz, mx, my, mz, rx, ry, rz);

	if (std::isnan(rx) || std::isnan(ry) || std::isnan(rz)) {
		float someX = rx;
	}
}