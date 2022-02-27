#pragma once

#include <bmmlib.h>
#include <iparamm2.h>
#include <macrorec.h>
#include <ifnpub.h>
#include <AssetManagement/iassetmanager.h>
#include <Graphics/IParameterTranslator.h>
#include <Graphics/IShaderManager.h>
#include "resource.h"

class ShadeContextProxy : virtual public ShadeContext {
public:
	ShadeContextProxy(ShadeContext &in_sc);

	~ShadeContextProxy();

    virtual BOOL 	InMtlEditor() override;

    virtual LightDesc* Light(int n) override;

    virtual TimeValue 	CurTime() override;

    virtual int 	FaceNumber() override;

    virtual Point3 	Normal() override;

    virtual Point3 	GNormal() override;

    virtual Point3 	V() override;

    virtual void 	SetView(Point3 p) override;

    virtual Point3 	ReflectVector() override;

    virtual Point3 	RefractVector(float ior) override;

    virtual Point3 	CamPos() override;

    virtual Point3 	P() override;

    virtual Point3 	DP() override;

    virtual Point3 	PObj() override;

    virtual Point3 	DPObj() override;

    virtual Box3 	ObjectBox() override;

    virtual Point3 	PObjRelBox() override;

    virtual Point3 	DPObjRelBox() override;

    virtual void 	ScreenUV(Point2& uv, Point2& duv) override;

    virtual IPoint2 	ScreenCoord() override;

    virtual Point3 	UVW(int channel = 0) override;

    virtual Point3 	DUVW(int channel = 0) override;

    virtual void SetUVW(Point3 uvw);

    virtual void 	DPdUVW(Point3 dP[3], int channel = 0) override;

    virtual void 	GetBGColor(Color& bgcol, Color& transp, BOOL fogBG = TRUE) override;

    virtual Point3 	PointTo(const Point3& p, RefFrame ito) override;

    virtual Point3 	PointFrom(const Point3& p, RefFrame ifrom) override;

    virtual Point3 	VectorTo(const Point3& p, RefFrame ito) override;

    virtual Point3 	VectorFrom(const Point3 & p, RefFrame ifrom) override;

private:

	ShadeContext* sc;
    bool hasCustomUVW;
    Point3 customUVW;
};
