//
//  plugin.cpp
//  ShaderPlugin
//
//  Created by Tetsushi on 2014/12/13.
//    Copyright (c) 2014年 Tetsushi. All rights reserved.
//

//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include <maya/MFnPlugin.h>


/**
 アトリビュートを入力可能にするマクロ
 seStorable : inputアトリビュートをシーンファイル保存時に書き込むよう設定
 setWritable : outputアトリビュートを入出力に設定。(falseの場合出力用)
 */
#define MAKE_INPUT(__x__) \
    __x__.setKeyable(true);\
    __x__.setStorable(true);\
    __x__.setWritable(true);\
    __x__.setReadable(true);

class MyShader : public MPxNode{
public:
    MyShader(){};//コンストラクタ
    virtual ~MyShader(){};//デストラクタ
    static void* creator();//ノードが作られたときにノードの実体を作って返す
    virtual MStatus compute(const MPlug &plug, MDataBlock &block);//ノードが実行する計算、処理を記述
    static MStatus initialize();//ノードに必要なアトリビュートの設定、登録
    
    virtual void postConstructor();
    static MTypeId id;
    
protected:
    //入力アトリビュート
    static MObject aColor;  //ベースの色
    static MObject  aBorder;    //境界の色
    static MObject  aBorderArea;
    static MObject  aNormalCamera;//面の法線ベクトル（カメラ座標系）
    static MObject  aRayDirection;  //レイの方向
//    static MObject diffusionCheck;//Diffuseのチェック
    
    //出力カラー
    static MObject  aOutColor;  //outColorアトリビュート
    
    
//==================ライト情報のための入力アトリビュート======================
    static MObject aLightData;
    static MObject aLightDirection;
    static MObject aLightAmbient;
    static MObject aLightDiffuse;
    static MObject aLightSpecular;
    static MObject aLightIntensity;
    static MObject aLightShadowFraction;
    static MObject aPreShadowIntensity;
    static MObject aLightBlindData;
};

//static変数を定義
MObject MyShader::aColor;
MObject MyShader::aBorder;
MObject MyShader::aBorderArea;
MObject MyShader::aNormalCamera;
MObject MyShader::aRayDirection;
MObject MyShader::aOutColor;
//MObject MyShader::diffusionCheck;//Diffusionの可否
//================ライトの入力アトリビュート================
MObject MyShader::aLightData;
MObject MyShader::aLightDirection;
MObject MyShader::aLightAmbient;
MObject MyShader::aLightDiffuse;
MObject MyShader::aLightSpecular;
MObject MyShader::aLightIntensity;
MObject MyShader::aLightShadowFraction;
MObject MyShader::aPreShadowIntensity;
MObject MyShader::aLightBlindData;


//IDを定義。このIDは正しくはAutodeskから取得しなければならない
MTypeId MyShader::id(0xab33);

void *MyShader::creator(){
    return new MyShader();
}

void MyShader::postConstructor(){
    setMPSafe(true);
}

//シェーダーの初期化
//アトリビュートの宣言や追加、入出力アトリビュートの定義
MStatus MyShader::initialize(){
    MStatus status;

    
	// ƒAƒgƒŠƒrƒ…[ƒg‚ð’è‹`‚·‚éBnAttr‚Íu‚Ç‚Ì‚æ‚¤‚È–¼Ì‚ÌƒAƒgƒŠƒrƒ…[ƒg‚ð’è‹`‚·‚é‚©v
	// ‚µ‚©s‚í‚È‚¸AMaya–{‘Ì‚É‚Í”½‰f‚µ‚È‚¢BMaya‚É”½‰f‚·‚é‚É‚ÍAaddAttribute‚ª•K—vB
	MFnNumericAttribute nAttr;
    
	// colorƒAƒgƒŠƒrƒ…[ƒg‚ð’è‹`BƒfƒtƒHƒ‹ƒg’l‚Æ‚µ‚Ä(0.5, 0.5, 0.5)‚ðÝ’èB
	aColor = nAttr.createColor("color", "c");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.5, 0.5, 0.5);
	addAttribute(aColor);
    
	aBorder = nAttr.createColor("border", "b");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1.0, 1.0, 0.8);
	addAttribute(aBorder);
    
	// ƒXƒJƒ‰[‚Å‚ ‚éfloat‚ð’è‹`BsetMinAsetMax‚É‚æ‚èƒXƒ‰ƒCƒ_‚ðŽ©“®“I‚ÉÝ’èB
	aBorderArea = nAttr.create("borderArea", "bar", MFnNumericData::kFloat, 0);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.1);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	addAttribute(aBorderArea);
    
	// aOutColor‚Ío—Í—pƒAƒgƒŠƒrƒ…[ƒgBo—Í—p‚É‘®«‚ðÝ’è‚·‚éB
	aOutColor = nAttr.createColor("outColor", "oc");
	nAttr.setStorable(false);
	nAttr.setHidden(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	addAttribute(aOutColor);
    
	// normalCamera‚ÍAƒJƒƒ‰À•WŒn‚É‚¨‚¯‚é–Ê‚Ì–@üƒxƒNƒgƒ‹B
	aNormalCamera = nAttr.createPoint("normalCamera", "n");
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
	addAttribute(aNormalCamera);
    
	// rayDirection‚ÍƒŒƒ“ƒ_ƒŠƒ“ƒOŽž‚ÌƒŒƒC‚Ì•ûŒü‚ðŽ¦‚·
	aRayDirection = nAttr.createPoint("rayDirection", "rd");
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
	addAttribute(aRayDirection);
    
	// ƒ‰ƒCƒg‚Ì‚½‚ß‚ÌƒAƒgƒŠƒrƒ…[ƒgBƒ‰ƒCƒg‚ÍÅI“I‚ÉaLightData‚ª‚ ‚ê‚Î‚æ‚¢‚ªA
	// aLightData‚É‚Í•¡”‚ÌŽqƒAƒgƒŠƒrƒ…[ƒg‚ª•t‚­B‚»‚ê‚ç‚·‚×‚Ä‚ðéŒ¾‚·‚éB
	aLightDirection = nAttr.createPoint("lightDirection", "ld");
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightIntensity = nAttr.createColor("lightIntensity", "li");
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightAmbient = nAttr.create("lightAmbient", "la", MFnNumericData::kBoolean);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightDiffuse = nAttr.create("lightDiffuse", "ldu", MFnNumericData::kBoolean);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightSpecular = nAttr.create("lightSpecular", "ls", MFnNumericData::kBoolean);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightShadowFraction = nAttr.create("lightShadowFraction", "lsf", MFnNumericData::kFloat);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aPreShadowIntensity = nAttr.create("preShadowIntensity", "psi", MFnNumericData::kFloat);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	aLightBlindData = nAttr.create("lightBlindData", "lbld", MFnNumericData::kLong);
	MAKE_INPUT(nAttr);
	nAttr.setHidden(true);
    
	MFnLightDataAttribute lAttr;
	aLightData = lAttr.create("lightDataArray", "ltd",
                              aLightDirection, aLightIntensity,
                              aLightAmbient, aLightDiffuse, aLightSpecular,
                              aLightShadowFraction, aPreShadowIntensity,
                              aLightBlindData);
	lAttr.setArray(true);		// ˆê‚Â‚ÌƒVƒF[ƒ_‚É‚Í•¡”‚Ìƒ‰ƒCƒg‚ªÚ‘±‚Å‚«‚é
	lAttr.setStorable(false);
	lAttr.setHidden(true);
	lAttr.setDefault(0, 0, 0,  0, 0, 0,  true, true, false,  0, 1,  0);
	addAttribute(aLightData);	// ƒ‰ƒCƒg‚ÍAalightData‚Ì‚Ý‚ð’Ç‰Á‚·‚éB
    
	// “ü—ÍƒAƒgƒŠƒrƒ…[ƒg‚Æo—ÍƒAƒgƒŠƒrƒ…[ƒg‚ÌŠÖŒW‚ðéŒ¾B
	// ‚ ‚é“ü—Í‚ªo—Í‚É‰e‹¿‚·‚é‚È‚ç‚ÎAéŒ¾‚µ‚È‚¯‚ê‚Î‚È‚ç‚È‚¢B
	attributeAffects(aColor, aOutColor);
	attributeAffects(aBorder, aOutColor);
	attributeAffects(aBorderArea, aBorder);
	attributeAffects(aNormalCamera, aOutColor);
	attributeAffects(aRayDirection, aOutColor);
    
	attributeAffects(aLightData, aOutColor);
	attributeAffects(aLightAmbient, aOutColor);
	attributeAffects(aLightDiffuse, aOutColor);
	attributeAffects(aLightSpecular, aOutColor);
	attributeAffects(aLightDirection, aOutColor);
	attributeAffects(aLightShadowFraction, aOutColor);
	attributeAffects(aPreShadowIntensity, aOutColor);
	attributeAffects(aLightBlindData, aOutColor);
    
	return MStatus::kSuccess;
}

//レンダリング計算を行う（レイとサーフェスが交差する）たびに呼び出される。
//plug :　どのアトリビュートのための計算処理か
//block : シーンデータ
MStatus MyShader::compute(const MPlug &plug, MDataBlock &block){
    MStatus status;
    
	if (! (plug == aOutColor) ) {
		return MStatus::kUnknownParameter;
	}
    
	// ƒVƒF[ƒfƒBƒ“ƒOŒ‹‰Ê‚Ì‚½‚ß‚Ì•Ï”
	MFloatVector resultColor(0, 0, 0);
	// ƒV[ƒ“‚©‚ç•K—v‚Èƒf[ƒ^‚ðŽæ“¾‚·‚é
	MFloatVector &surfaceNormal = block.inputValue(aNormalCamera).asFloatVector();
	MFloatVector &surfaceColor = block.inputValue(aColor).asFloatVector();
	MFloatVector &borderColor = block.inputValue(aBorder).asFloatVector();
	MFloatVector &rayDirection = block.inputValue(aRayDirection).asFloatVector();
	float borderArea = block.inputValue(aBorderArea).asFloat();
    
	// o—ÍƒAƒgƒŠƒrƒ…[ƒg‚Ì‚½‚ß‚Ìƒnƒ“ƒhƒ‹‚ðŽæ“¾
	MDataHandle outColorHandle = block.outputValue(aOutColor);
	MFloatVector &outColor = outColorHandle.asFloatVector();
    
	// ƒ‰ƒCƒg“ü—ÍƒAƒgƒŠƒrƒ…[ƒgBƒ‰ƒCƒg‚Í•¡”‚ ‚è‚¤‚é‚Ì‚Å”z—ñ(Array)‚Æ‚È‚é
	MArrayDataHandle lightData = block.inputArrayValue(aLightData);
	// ƒ‰ƒCƒg‚ÌŒÂ”‚ð”‚¦‚é
	int numLights = lightData.elementCount();
	//cout << "light num: " << numLights << endl;
	// o—ÍƒJƒ‰[‚ÍAƒ‰ƒ“ƒo[ƒgŒvŽZ‚ðs‚¤B
	// ‚½‚¾‚µAƒŒƒC‚Ì•ûŒüƒxƒNƒgƒ‹‚ÆƒT[ƒtƒF[ƒX‚Ì–@üƒxƒNƒgƒ‹‚Ìˆ×‚·Šp‚É‚æ‚Á‚Ä‚Í
	// o—ÍƒJƒ‰[‚Í•ÏX‚·‚éB
	// ƒ‰ƒCƒg‚Ì”‚¾‚¯AÆ“xŒvŽZ‚ðs‚¤BƒCƒ‹ƒ~ƒiƒ“ƒX(illuminance)ƒ‹[ƒv‚Æ‚àŒ¾‚¤B
    
    cout << "LightNumbers: " <<numLights << endl;
	for(int ii = 0; ii < numLights; ii++) {
		MDataHandle currentLight = lightData.inputValue();	// Œ»Ý‚Ìƒ‰ƒCƒgî•ñ‚ðŽæ“¾
		MFloatVector &lightIntensity = currentLight.child(aLightIntensity).asFloatVector();
		MFloatVector &lightDirection = currentLight.child(aLightDirection).asFloatVector();
        
		// diffuseŒvŽZ‚ð‚·‚éBŠgŽU”½ŽË“üŽËŒõ–(ƒ‰ƒCƒg‚Ö‚Ì•ûŒüE–Ê‚Ì–@üƒxƒNƒgƒ‹j
		float cosln = lightDirection * surfaceNormal;		// ƒ‰ƒCƒg•ûŒü‚Æ–@ü‚Ì“àÏ
		MFloatVector lambert;
		lambert[0] = cosln * lightIntensity[0];
		lambert[1] = cosln * lightIntensity[1];
		lambert[2] = cosln * lightIntensity[2];
        
		// ŒvŽZŒ‹‰Ê‚ðŒ‹‰Ê‚É‰Á‚¦‚é
		resultColor += lambert;
		// ŽŸ‚Ìƒ‰ƒCƒg‚ÉˆÚ“®
		lightData.next();
	}
    
	// ƒGƒbƒW•”•ª‚ÌF‚ð•Ï‚¦‚é
	float aaa = surfaceNormal * rayDirection;
	// ˆ×‚·Šp‚ªˆê’è”ÍˆÍ‚ð’´‚¦‚Ä‚¢‚ê‚ÎA
	if (aaa < borderArea && aaa > -borderArea) {
		// o—ÍƒJƒ‰[‚ÉborderColor‚ð‰Á‚¦‚é
		resultColor += borderColor;
	}
	outColor = resultColor;
	// ƒnƒ“ƒhƒ‹‚ð’Ê‚¶‚Ä’Ê’m
	outColorHandle.setClean();
    
	return MStatus::kSuccess;
}

//-----------------------------------------------------------------------------
MStatus initializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is loaded into Maya. It 
	//- registers all of the services that this plug-in provides with Maya.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj, _T("Vendor Name"), _T("Version"), _T("Any"));
    
    const MString user_classify("shader/surface");
    
    plugin.registerNode("MyShader",
                        MyShader::id,
                        MyShader::creator,
                        MyShader::initialize,
                        MPxNode::kDependNode,
                        &user_classify);
    //- Examples:
	//NodeRegisterOk(plugin.registerNode (_T("myNode"), myNode::id, myNode::creator, myNode::initialize)) ;
	//NodeRegisterOk(myNode::registerMe (plugin)) ;

	//-{{MAYA_REGISTER
	//-MAYA_REGISTER}}

	return (MS::kSuccess) ;
}

MStatus uninitializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is unloaded from Maya. It 
	//- deregisters all of the services that it was providing.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj) ;
    plugin.deregisterNode(MyShader::id);
	//- Examples:
	//NodeUnregisterOk(plugin.deregisterNode (myNode::id)) ;
	//NodeUnregisterOk(myNode::unregisterMe (plugin)) ;
	
	//-{{MAYA_UNREGISTER
	//-MAYA_UNREGISTER}}

	return (MS::kSuccess) ;
}
