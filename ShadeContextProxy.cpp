#include "ShadeContextProxy.h"

ShadeContextProxy::ShadeContextProxy(ShadeContext &in_sc): 
	ShadeContext(in_sc),
	sc(&in_sc),
	hasCustomUVW(false)
{}

ShadeContextProxy::~ShadeContextProxy()
{
	sc = nullptr;
}

BOOL ShadeContextProxy::InMtlEditor() {
	return sc->InMtlEditor();
}

LightDesc* ShadeContextProxy::Light(int n) {
	return sc->Light(n);
}

TimeValue 	ShadeContextProxy::CurTime() {
	return sc->CurTime();
}

int 	ShadeContextProxy::FaceNumber() {
	return sc->FaceNumber();
}

Point3 	ShadeContextProxy::Normal() {
	return sc->Normal();
}

Point3 	ShadeContextProxy::GNormal() {
	return sc->GNormal();
}

Point3 	ShadeContextProxy::V() {
	return sc->V();
}

void 	ShadeContextProxy::SetView(Point3 p) {
	return sc->SetView(p);
}

Point3 	ShadeContextProxy::ReflectVector() {
	return sc->ReflectVector();
}

Point3 	ShadeContextProxy::RefractVector(float ior) {
	return sc->RefractVector(ior);
}

Point3 	ShadeContextProxy::CamPos() {
	return sc->CamPos();
}

Point3 	ShadeContextProxy::P() {
	return sc->P();
}

Point3 	ShadeContextProxy::DP() {
	return sc->DP();
}

Point3 	ShadeContextProxy::PObj() {
	return sc->PObj();
}

Point3 	ShadeContextProxy::DPObj() {
	return sc->DPObj();
}

Box3 	ShadeContextProxy::ObjectBox() {
	return sc->ObjectBox();
}

Point3 	ShadeContextProxy::PObjRelBox() {
	return sc->PObjRelBox();
}

Point3 	ShadeContextProxy::DPObjRelBox() {
	return sc->DPObjRelBox();
}

void 	ShadeContextProxy::ScreenUV(Point2& uv, Point2& duv) {
	sc->ScreenUV(uv, duv);
}

IPoint2 	ShadeContextProxy::ScreenCoord() {
	return sc->ScreenCoord();
}

Point3 	ShadeContextProxy::UVW(int channel) {
	if (hasCustomUVW) {
		hasCustomUVW = false;
		return customUVW;
	}
	else {
		return sc->UVW(channel);
	}
}

Point3 	ShadeContextProxy::DUVW(int channel) {
	return sc->DUVW(channel);
}

void ShadeContextProxy::SetUVW(Point3 uvw)
{
	hasCustomUVW = true;
	customUVW.x = uvw.x;
	customUVW.y = uvw.y;
	customUVW.z = uvw.z;
}

void 	ShadeContextProxy::DPdUVW(Point3 dP[3], int channel) {
	sc->DPdUVW(dP, channel);
}

void 	ShadeContextProxy::GetBGColor(Color& bgcol, Color& transp, BOOL fogBG) {
	sc->GetBGColor(bgcol, transp, fogBG);
}

Point3 	ShadeContextProxy::PointTo(const Point3& p, RefFrame ito) {
	return sc->PointTo(p, ito);
}

Point3 	ShadeContextProxy::PointFrom(const Point3& p, RefFrame ifrom) {
	return sc->PointFrom(p, ifrom);
}

Point3 	ShadeContextProxy::VectorTo(const Point3& p, RefFrame ito) {
	return sc->VectorTo(p, ito);
}

Point3 	ShadeContextProxy::VectorFrom(const Point3& p, RefFrame ifrom) {
	return sc->VectorFrom(p, ifrom);
}
