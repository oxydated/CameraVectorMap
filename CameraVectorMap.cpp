#include "CameraVectorMap.h"
#include "ShadeContextProxy.h"
#include "quatRotation.h"

using namespace MaxSDK::Graphics;

class CameraVectorMapClassDesc : public ClassDesc2 {
public:
	int IsPublic() { return true; }
	void* Create(BOOL loading = FALSE) { return new CameraVectorMap(); }
	const TCHAR* ClassName() { return GetString(IDS_CLASS_NAME); }
	const TCHAR* NonLocalizedClassName() { return _T("CameraVectorMap"); }
	SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID ClassID() { return CAMERAVECTORMAP_CLASS_ID; }
	const TCHAR* Category() { return GetString(IDS_CATEGORY); }

	const TCHAR* InternalName() { return _T("CameraVectorMap"); }
	HINSTANCE HInstance() { return hInstance; }
};

static CameraVectorMapClassDesc CameraVectorMapDesc;
ClassDesc2* GetCameraVectorMapDesc() { return &CameraVectorMapDesc; }

enum {
	CAMERAVECTORMAP_VIEWVECTOR_ON,
	CAMERAVECTORMAP_REFLECTED_ON,
	CAMERAVECTORMAP_INVERTED_ON,
	CAMERAVECTORMAP_DIFFUSE_ON,
	CAMERAVECTORMAP_OBSERVER_ON,
	CAMERAVECTORMAP_NORMAL_MAP,
	CAMERAVECTORMAP_MAP,
};

static ParamBlockDesc2 cameravectormap_param_blk(gnormal_params, _T("params"), 0, &CameraVectorMapDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF,
	//rolout
	IDD_PARAM_PANEL,
	IDS_PARAMS,
	0, 0, NULL,
	//params
	CAMERAVECTORMAP_NORMAL_MAP, _T("normalmap"), TYPE_TEXMAP, 0, IDS_NORMAL_MAP,
	p_refno, 1,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_NORMAL_MAP,
	p_end,
	//params
	CAMERAVECTORMAP_MAP, _T("map"), TYPE_TEXMAP, 0, IDS_MAP,
	p_refno, 1,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_MAP,
	p_end,
	CAMERAVECTORMAP_VIEWVECTOR_ON, _T("mapEnabled"), TYPE_BOOL, 0, IDS_VIEWVECTOR_ON,
	p_default, 2.2f,
	p_range, 1.0f, 5.0f,
	p_ui, TYPE_SINGLECHECKBOX, IDC_VIEWVECTOR_ON,
	p_end,
	//0, 0, NULL,
	//params
	CAMERAVECTORMAP_DIFFUSE_ON, _T("diffusionEnabled"), TYPE_BOOL, 0, IDS_DIFFUSE_ON,
	p_default, 2.2f,
	p_range, 1.0f, 5.0f,
	p_ui, TYPE_SINGLECHECKBOX, IDC_DIFFUSE_ON,
	p_end,
	CAMERAVECTORMAP_REFLECTED_ON, _T("reflectionEnabled"), TYPE_BOOL, 0, IDS_REFLECTED_ON,
	p_default, 2.2f,
	p_range, 1.0f, 5.0f,
	p_ui, TYPE_SINGLECHECKBOX, IDC_REFLECTED_ON,
	p_end,
	CAMERAVECTORMAP_INVERTED_ON, _T("invertionEnabled"), TYPE_BOOL, 0, IDS_INVERTED_ON,
	p_default, 2.2f,
	p_range, 1.0f, 5.0f,
	p_ui, TYPE_SINGLECHECKBOX, IDC_INVERTED_ON,
	p_end,
	CAMERAVECTORMAP_OBSERVER_ON, _T("ObserverSpaceEnabled"), TYPE_BOOL, 0, IDS_OBSERVER_ON,
	p_default, 2.2f,
	p_range, 1.0f, 5.0f,
	p_ui, TYPE_SINGLECHECKBOX, IDC_OBSERVER_ON,
	p_end,
	p_end
);

MaxSDK::Graphics::IShaderManager* CameraVectorMap::GetShaderManager() {
	if (mpShaderManager == NULL) {
		mpShaderManager = IShaderManagerCreator::GetInstance()->CreateShaderManager(
			IShaderManager::ShaderTypeAMG,
			_M("max_CameraVectorMap"),
			_M(""),
			this);
	}
	return mpShaderManager;
}

CameraVectorMap::CameraVectorMap() {
	mpPblock = NULL;
	mpSubTex = NULL;
	mpNormalMap = NULL;
	texHandle = NULL;
	mpShaderManager = NULL;
	CameraVectorMapDesc.MakeAutoParamBlocks(this);
	Init();
}

CameraVectorMap::~CameraVectorMap() {
	DiscardTexHandle();
	IShaderManagerCreator::GetInstance()->DeleteShaderManager(mpShaderManager);
	mpShaderManager = NULL;
}

void CameraVectorMap::Reset() {
	DeleteReference(1);
	Init();
	ivalid.SetEmpty();
}

void CameraVectorMap::Init() {
	ivalid.SetEmpty();
	mapValid.SetEmpty();
}

void CameraVectorMap::InvalidateUI() {
	cameravectormap_param_blk.InvalidateUI(mpPblock->LastNotifyParamID());
}

//----------------------------------------------------------------------------------------
//-- From Animatable

void CameraVectorMap::GetClassName(MSTR& s, bool localized) const {
	s = localized ? GetString(IDS_CLASS_NAME_IMP) : _T("Camera Vector Map");
}

Animatable* CameraVectorMap::SubAnim(int i) {
	switch (i) {
	case 0:
		return mpPblock;

	case 1:
		return mpNormalMap;

	default:
		return mpSubTex;
	}
}

TSTR CameraVectorMap::SubAnimName(int i, bool localized) {
	switch (i) {
	case 0:
		return localized ? GetString(IDS_PARAMS) : _T("Parameters");
	default:
		return GetSubTexmapTVName(i, localized);
	}
}

//----------------------------------------------------------------------------------------
//-- From ReferenceMaker

#define MTL_HDR_CHUNK 0x4000

IOResult CameraVectorMap::Save(ISave* isave) {
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res != IO_OK)return res;
	isave->EndChunk();
	return IO_OK;
}

IOResult CameraVectorMap::Load(ILoad* iload){
	IOResult res;
	int id;
	while (IO_OK == (res = iload->OpenChunk())) {
		switch (id = iload->CurChunkID()) {
		case MTL_HDR_CHUNK:
			res = MtlBase::Load(iload);
			break;
		}
		iload->CloseChunk();
		if (res != IO_OK)
			return res;
	}
	return IO_OK;
}

RefTargetHandle CameraVectorMap::GetReference(int i) {
	switch (i) {
	case PBLOCK_REF:
		return mpPblock;

	case NORMAL_REF:
		return mpNormalMap;

	default:
		return mpSubTex;
	}
}

void CameraVectorMap::SetReference(int i, RefTargetHandle rtarg) {
	switch (i) {
	case PBLOCK_REF:
		mpPblock = (IParamBlock2*)rtarg;
		break;

	case NORMAL_REF:
		mpNormalMap = (Texmap*)rtarg;
		break;

	default:
		mpSubTex = (Texmap*)rtarg;
		break;
	}
}

RefResult CameraVectorMap::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) {
	switch (message) {
	case REFMSG_CHANGE:
		ivalid.SetEmpty();
		mapValid.SetEmpty();
		if (hTarget == mpPblock) {
			ParamID changing_param = mpPblock->LastNotifyParamID();
			cameravectormap_param_blk.InvalidateUI(changing_param);
			if (changing_param != -1) {
				DiscardTexHandle();
			}
		}
		break;
	}
	return(REF_SUCCEED);
}

//----------------------------------------------------------------------------------------
//-- From ReferenceMaker

RefTargetHandle CameraVectorMap::Clone(RemapDir& remap) {
	CameraVectorMap* mnew = new CameraVectorMap();
	*((MtlBase*)mnew) = *((MtlBase*)this); //copy superclass stuff
	mnew->ReplaceReference(PBLOCK_REF, remap.CloneRef(mpPblock));
	mnew->ivalid.SetEmpty();
	mnew->mapValid.SetEmpty();
	mnew->mpSubTex = NULL;
	mnew->mpNormalMap = NULL;
	mnew->mReflectedOn = mReflectedOn;
	mnew->mDiffuseOn = mDiffuseOn;
	mnew->mInvertedOn = mInvertedOn;
	mnew->mObserverSpaceOn = mObserverSpaceOn;
	mnew->mViewVectorOn = mViewVectorOn;
	//mnew->mReverseGamma = mReverseGamma;
	//mnew->mSolidColor = mSolidColor;
	//mnew->mGamma = mGamma;
	//mnew->mGain = mGain;

	if (mpSubTex) {
		mnew->ReplaceReference(MAP_REF, remap.CloneRef(mpSubTex));
	}

	if (mpNormalMap) {
		mnew->ReplaceReference(NORMAL_REF, remap.CloneRef(mpNormalMap));
	}

	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

//-----------------------------------------------------------------------------
//-- From MtlBase

ULONG CameraVectorMap::LocalRequirements(int subMtlNum) {
	ULONG retval = 0;
	if (mpSubTex) {
		retval |= mpSubTex->LocalRequirements(subMtlNum);
	}
	return retval;
}

void CameraVectorMap::LocalMappingsRequired(int subMtlNum, BitArray& mapreq, BitArray& bumpreq) {
	if (mpSubTex) {
		mpSubTex->LocalMappingsRequired(subMtlNum, mapreq, bumpreq);
	}
}

ParamDlg* CameraVectorMap::CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp) {
	IAutoMParamDlg* dlg = CameraVectorMapDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	return dlg;
}

void CameraVectorMap::Update(TimeValue t, Interval& valid) {
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();
		//mpPblock->GetValue(CAMERAVECTORMAP_SOLID_COLOR, t, mSolidColor, ivalid);
		mpPblock->GetValue(CAMERAVECTORMAP_VIEWVECTOR_ON, t, mViewVectorOn, ivalid);
		mpPblock->GetValue(CAMERAVECTORMAP_REFLECTED_ON, t, mReflectedOn, ivalid);
		mpPblock->GetValue(CAMERAVECTORMAP_INVERTED_ON, t, mInvertedOn, ivalid);
		mpPblock->GetValue(CAMERAVECTORMAP_DIFFUSE_ON, t, mDiffuseOn, ivalid);
		mpPblock->GetValue(CAMERAVECTORMAP_OBSERVER_ON, t, mObserverSpaceOn, ivalid);
		//mpPblock->GetValue(CAMERAVECTORMAP_GAMMA, t, mGamma, ivalid);
		//mpPblock->GetValue(CAMERAVECTORMAP_GAIN, t, mGain, ivalid);
		//mpPblock->GetValue(CAMERAVECTORMAP_REVERSE_GAMMA, t, mReverseGamma, ivalid);
		NotifyDependents(FOREVER, PART_TEXMAP, REFMSG_DISPLAY_MATERIAL_CHANGE);
	}

	if (!mapValid.InInterval(t)) {
		mapValid.SetInfinite();
		if (mpSubTex) {
			mpSubTex->Update(t, mapValid);
		}
		if (mpNormalMap) {
			mpNormalMap->Update(t, mapValid);
		}
	}
	valid &= mapValid;
	valid &= ivalid;
}

void CameraVectorMap::SetSubTexmap(int i, Texmap* m) {
	switch (i) {
	case 0:
		ReplaceReference(NORMAL_REF, m);
		cameravectormap_param_blk.InvalidateUI(CAMERAVECTORMAP_NORMAL_MAP);
		mapValid.SetEmpty();
		break;
	default:
		ReplaceReference(MAP_REF, m);
		cameravectormap_param_blk.InvalidateUI(CAMERAVECTORMAP_MAP);
		mapValid.SetEmpty();
	}
}

TSTR CameraVectorMap::GetSubTexmapSlotName(int i, bool localized) {
	switch (i) {
	case 0:
		return localized ? GetString(IDS_NORMAL_MAP) : _T("Normal Map");
		break;

	default:
		return localized ? GetString(IDS_MAP) : _T("Map");
	}
}

DWORD_PTR CameraVectorMap::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t)) {
			return texHandle->GetHandle();
		}
		else {
			DiscardTexHandle();
		}
	}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t, thmaker, texHandleValid));
	return texHandle->GetHandle();
}

void CameraVectorMap::ActivateTexDisplay(BOOL onoff) {
	if (!onoff) {
		DiscardTexHandle();
	}
}

void CameraVectorMap::DiscardTexHandle() {
	if (texHandle) {
		texHandle->DeleteThis();
		texHandle = NULL;
	}
}

BITMAPINFO* CameraVectorMap::GetVPDisplayDIB(TimeValue t, TexHandleMaker& thmaker, Interval& valid, BOOL mono, BOOL forceW, BOOL forceH) {
	if (TestMtlFlag(MTL_HW_TEX_ENABLED)) {
		BITMAPINFO* cache = NULL;
		//use a cached bitmapinfo
		BitmapInfo bi;
		static MaxSDK::AssetManagement::AssetUser bitMapAssetUser;
		if (bitMapAssetUser.GetId() == MaxSDK::AssetManagement::kInvalidId) {
			bitMapAssetUser = MaxSDK::AssetManagement::IAssetManager::GetInstance()->GetAsset(_T("CameraVectorMapTemp"), MaxSDK::AssetManagement::kBitmapAsset);
		}
		bi.SetAsset(bitMapAssetUser);
		bi.SetWidth(1);
		bi.SetHeight(1);
		bi.SetType(BMM_TRUE_32);
		Bitmap* bm = TheManager->Create(&bi);
		cache = thmaker.BitmapToDIB(bm, 1, 0, forceW, forceH);
		bm->DeleteThis();
		return cache;
	}

	Interval v;
	Update(t, v);
	BitmapInfo bi;
	static MaxSDK::AssetManagement::AssetUser bitMapAssetUser;
	if (bitMapAssetUser.GetId() == MaxSDK::AssetManagement::kInvalidId) {
		bitMapAssetUser = MaxSDK::AssetManagement::IAssetManager::GetInstance()->GetAsset(_T("CameraVectorMapTemp"), MaxSDK::AssetManagement::kBitmapAsset);
	}
	bi.SetAsset(bitMapAssetUser);
	bi.SetWidth(thmaker.Size());
	bi.SetHeight(thmaker.Size());
	bi.SetType(BMM_TRUE_32);
	Bitmap* bm = TheManager->Create(&bi);
	GetCOREInterface()->RenderTexmap(this, bm);
	BITMAPINFO* bmi = thmaker.BitmapToDIB(bm, 1, 0, forceW, forceH);
	bm->DeleteThis();
	valid.SetInfinite();

	return bmi;
}

//-----------------------------------------------------------------------------
//-- From Texmap

AColor CameraVectorMap::EvalColor(ShadeContext& sc) {
	AColor retval;
	float gamma = 1.0;

	ShadeContextProxy scp(sc);


	Point3 baryCoord = sc.BarycentricCoords();
	Point3 viewVector = Normalize(sc.OrigView());
	Point3 normalVector = sc.OrigNormal();

	if (mpNormalMap) {

		float &nx = normalVector.x;
		float &ny = normalVector.y;
		float &nz = normalVector.z;

		AColor normalColor = mpNormalMap->EvalColor(sc);

		float sx = normalColor.r;
		float sy = normalColor.g;
		float sz = normalColor.b;

		float rx, ry, rz;

		transformFromTangentSpaceToWorld( nx, ny, nz, sx, sy, sz, rx, ry, rz);

		normalVector.x = rx;
		normalVector.y = ry;
		normalVector.z = rz;
	}

	retval.r = 0.0;
	retval.g = 0.0;
	retval.b = 0.0;

	float ox = viewVector.x;
	float oy = viewVector.y;
	float oz = viewVector.z;

	if (mViewVectorOn) {
		retval.r = viewVector.x;
		retval.g = viewVector.y;
		retval.b = viewVector.z;
	}
	else 
	{
		Point3 dir;
		float dotL;
		float diffuse;
		LightDesc* ld = sc.Light(0);
		if (ld != nullptr) {
			Color lightColor;
			if (ld->Illuminate(sc, normalVector, lightColor, dir, dotL, diffuse)) {

				Point3 nDir = Normalize(dir);

				float vx = nDir.x;
				float vy = nDir.y;
				float vz = nDir.z;

				float nx = normalVector.x;
				float ny = normalVector.y;
				float nz = normalVector.z;

				if (mDiffuseOn) {

					float NormalDotView = (nx * vx + ny * vy + nz * vz);

					if (mpSubTex) {
						Point3 oUVW;

						oUVW.x = NormalDotView;
						oUVW.y = 0.5;
						oUVW.z = 0.0f;
						

								scp.SetUVW(oUVW);
								retval = mpSubTex->EvalColor(scp);
					}
					else {
						retval.r = NormalDotView;
						retval.g = NormalDotView;
						retval.b = NormalDotView;
					}
				}
				else {

					if (mReflectedOn) {
						//{-vx + 2*nx*(nx*vx + ny*vy + nz*vz),-vy + 2*ny*(nx*vx + ny*vy + nz*vz),-vz + 2*nz*(nx*vx + ny*vy + nz*vz)}

						float rx = -vx + 2 * nx * (nx * vx + ny * vy + nz * vz);
						float ry = -vy + 2 * ny * (nx * vx + ny * vy + nz * vz);
						float rz = -vz + 2 * nz * (nx * vx + ny * vy + nz * vz);

						if (mObserverSpaceOn) {

							float& cx = rx;
							float& cy = ry;
							float& cz = rz;

							float& ix = ox;
							float& iy = oy;
							float& iz = oz;

							float cx2 = cx * cx;
							float cy2 = cy * cy;
							float cz2 = cz * cz;

							float obsx = (cy2 * ix + cx2 * cz * ix + cx * cy * (-1 + cz) * iy) / (cx2 + cy2) - cx * iz;
							float obsy = (cx * cy * (-1 + cz) * ix + cx2 * iy + cy2 * cz * iy) / (cx2 + cy2) - cy * iz;
							float obsz = cx * ix + cy * iy + cz * iz;

							if (mpSubTex) {
								Point3 oUVW;

								oUVW.x = (float)(obsx + 1) / 2;
								oUVW.y = (float)(obsy + 1) / 2;
								oUVW.z = 0.0f;

								scp.SetUVW(oUVW);
								retval = mpSubTex->EvalColor(scp);
							}
							else
							{
								retval.r = obsx;
								retval.g = obsy;
								retval.b = obsz;
							}
						}
						else
						{
							retval.r = rx;
							retval.g = ry;
							retval.b = rz;
						}
					}
					else
					{
						retval.r = nDir.x;
						retval.g = nDir.y;
						retval.b = nDir.z;
					}
				}
			}
		}

	}

	//scp.SetUVW(sc.UVW()/2.0);

	//if (mpSubTex) {
	//	retval = mpSubTex->EvalColor(scp);
	//}

	return mInvertedOn ? retval * -1 : retval;
}

float CameraVectorMap::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
}

Point3 CameraVectorMap::EvalNormalPerturb(ShadeContext& sc) {
	//! Undefined currently
	if (mpSubTex) {
		return mpSubTex->EvalNormalPerturb(sc);
	}
	else {
		Point3 defaultNormal(0, 0, 0);
		return defaultNormal;
	}
}

BaseInterface* CameraVectorMap::GetInterface(Interface_ID id) {
	if (ISHADER_MANAGER_INTERFACE_ID == id) {
		return GetShaderManager();
	}
	else if (IPARAMETER_TRANSLATOR_INTERFACE_ID == id) {
		return static_cast<IParameterTranslator*>(this);
	}
	else {
		return Texmap::GetInterface(id);
	}
}

bool CameraVectorMap::GetParameterValue(const TimeValue t, const MCHAR* shaderParamName, void* value, IParameterTranslator::ShaderParameterType shaderParamType) {
	//if (_tcscmp(shaderParamName, _M("gain")) == 0) {
	//	*(float*)value = mGain;
	//}
	//else if (_tcscmp(shaderParamName, _M("gamma")) == 0) {
	//	*(float*)value = mGamma;
	//}
	//else if (_tcscmp(shaderParamName, _M("input")) == 0) {
	//	//! AColor has the same layout as D3COLORVALUE
	//	*(AColor*)value = mSolidColor;
	//}
	//else if (_tcscmp(shaderParamName, _M("reverse")) == 0) {
	//	*(BOOL*)value = mReverseGamma;
	//}
	return TRUE;
}

bool CameraVectorMap::GetShaderInputParameterName(SubMtlBaseType type, int subMtlBaseIndex, MSTR& shaderInputParamName) {
	if (mpSubTex) {
		shaderInputParamName = _M("input");
	}
	return true;
}

bool CameraVectorMap::OnPreParameterTranslate() {
	Interval valid;
	Update(GetCOREInterface()->GetTime(), valid);
	return true;
}