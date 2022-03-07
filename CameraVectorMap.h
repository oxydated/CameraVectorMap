#pragma once

#include <bmmlib.h>
#include <iparamm2.h>
#include <macrorec.h>
#include <ifnpub.h>
#include <AssetManagement/iassetmanager.h>
#include <Graphics/IParameterTranslator.h>
#include <Graphics/IShaderManager.h>
#include "resource.h"

#define NSUBTEX		2
#define PBLOCK_REF	0
#define NORMAL_REF	1
#define MAP_REF		2
#define NUM_REFS	3

#define CAMERAVECTORMAP_CLASS_ID	Class_ID(0x571a6cf8, 0x1e1129d8)

class CameraVectorMap : public Texmap, public MaxSDK::Graphics::IParameterTranslator
{
public:
	CameraVectorMap();
	~CameraVectorMap();

	void Reset();
	void Init();
	void InvalidateUI();

	//-- From Animatable
	void DeleteThis() { delete this; }
	void GetClassName(MSTR& s, bool localized) const override;
	Class_ID ClassID() { return CAMERAVECTORMAP_CLASS_ID; }
	SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }

	int NumSubs() { return NSUBTEX; }
	Animatable* SubAnim(int i);
	TSTR SubAnimName(int i, bool localized) override;
	int	SubNumToRefNum(int subNum) { return subNum; }

	int NumParamBlocks() { return 1; } // num of paramblocks in this instance
	IParamBlock2* GetParamBlock(int i) { return mpPblock; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (mpPblock->ID() == id) ? mpPblock : NULL; }

	//-- From ReferenceMaker
	IOResult Load(ILoad* iload);
	IOResult Save(ISave* isave);

	int NumRefs() { return NUM_REFS; }
	RefTargetHandle GetReference(int i);

	RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	//-- From ReferenceTarget
	RefTargetHandle Clone(RemapDir& remap);

	//-- From ISubMap
	int NumSubTexmaps() { return NSUBTEX; }
	Texmap* GetSubTexmap(int i) {
		switch (i) {
		case 0:
			return mpNormalMap;
			break;

		default:
			return mpSubTex;
		}
	}
	void SetSubTexmap(int i, Texmap* m);
	TSTR GetSubTexmapSlotName(int i, bool localized) override;

	//-- From MtlBase
	ULONG LocalRequirements(int subMNtlNum);
	void LocalMappingsRequired(int subMtlNum, BitArray& mapreq, BitArray& bumpreq);

	void Update(TimeValue t, Interval& valid);
	Interval Validity(TimeValue t) { Interval v = FOREVER; Update(t, v); return v; }
	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp);

	void DiscardTexHandle();
	BOOL SupportTexDisplay() { return TRUE; }
	DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);
	void ActivateTexDisplay(BOOL onoff);
	BITMAPINFO* GetVPDisplayDIB(TimeValue t, TexHandleMaker& thmaker, Interval& vcalid, BOOL mono = FALSE, BOOL forceW = 0, BOOL forceH = 0);

	//-- From Texmap
	AColor EvalColor(ShadeContext& sc);
	float EvalMono(ShadeContext& sc);
	Point3 EvalNormalPerturb(ShadeContext& sc);

	//-- From IParameterTranslator
	virtual bool GetParameterValue(
		const TimeValue t,
		const MCHAR* shaderParamName,
		void* value,
		MaxSDK::Graphics::IParameterTranslator::ShaderParameterType shaderParamType);
	virtual bool GetShaderInputParameterName(SubMtlBaseType type, int subMtlBaseIndex, MSTR& shaderInputParamName);
	virtual bool OnPreParameterTranslate();

	BaseInterface* GetInterface(Interface_ID id);

private:
	virtual void SetReference(int i, RefTargetHandle rtarg);
	IParamBlock2* mpPblock;	//ref 0
	Texmap* mpSubTex;	//ref 1
	Texmap* mpNormalMap;	//ref 2
	Interval ivalid;
	Interval mapValid;
	BOOL mViewVectorOn;
	BOOL mReflectedOn;
	BOOL mDiffuseOn;
	BOOL mInvertedOn;
	BOOL mObserverSpaceOn;
	BOOL mReverseGamma;
	//AColor mSolidColor;
	//float mGamma;
	//float mGain;
	TexHandle* texHandle;
	Interval texHandleValid;
	MaxSDK::Graphics::IShaderManager* mpShaderManager;
	MaxSDK::Graphics::IShaderManager* GetShaderManager();
};

TCHAR* GetString(int id);
extern HINSTANCE hInstance;
