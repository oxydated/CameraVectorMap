#pragma once

void rotateVectorByQuat(float qs, float qx, float qy, float qz, float sx, float sy, float sz, float& rx, float& ry, float& rz);

void transformFromTangentSpaceToWorld(float nx, float ny, float nz, float sx, float sy, float sz, float& rx, float& ry, float& rz);

void crossProduct(float nx, float ny, float nz, float ux, float uy, float uz, float& rx, float& ry, float& rz);

void bitangentFromNormalAndUVector(float nx, float ny, float nz, float ux, float uy, float uz, float& rx, float& ry, float& rz);

void transformToBasis(float nx, float ny, float nz, float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz, float& rx, float& ry, float& rz);

void normalTransformed(float nx, float ny, float nz, float mx, float my, float mz, float ux, float uy, float uz, float& rx, float& ry, float& rz);