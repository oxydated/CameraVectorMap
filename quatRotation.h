#pragma once

void rotateVectorByQuat(float qs, float qx, float qy, float qz, float sx, float sy, float sz, float& rx, float& ry, float& rz);

void transformFromTangentSpaceToWorld(float nx, float ny, float nz, float sx, float sy, float sz, float& rx, float& ry, float& rz);