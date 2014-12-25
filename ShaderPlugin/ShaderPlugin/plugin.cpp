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
   CHECK_MSTATUS( __x__.setKeyable(true));\
   CHECK_MSTATUS( __x__.setStorable(true));\
   CHECK_MSTATUS( __x__.setWritable(true));\
   CHECK_MSTATUS( __x__.setReadable(true));

#define MAKE_LIGHT_INPUT(__x__) \
CHECK_MSTATUS(__x__.setStorable(false));\
CHECK_MSTATUS(__x__.setHidden(true));\
CHECK_MSTATUS(__x__.setReadable(true));\
CHECK_MSTATUS(__x__.setWritable(false));

class MyShader : public MPxNode{
public:
    MyShader(){};//コンストラクタ
    virtual ~MyShader(){};//デストラクタ
    static void* creator();//ノードが作られたときにノードの実体を作って返す
    virtual MStatus compute(const MPlug&, MDataBlock&);//ノードが実行する計算、処理を記述
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
MTypeId MyShader::id(0x81000);

void *MyShader::creator(){
    return new MyShader();
}

void MyShader::postConstructor(){
    setMPSafe(true);
}

//シェーダーの初期化
//アトリビュートの宣言や追加、入出力アトリビュートの定義
MStatus MyShader::initialize(){
    
    MFnNumericAttribute nAttr;
    MFnLightDataAttribute lAttr;
    
    MStatus status;
    //アトリビュートを定義する。nAttrは”どのような名称のアトリビュートを定義するか”
    
    //入力色を指定する
    aColor = nAttr.createColor("color", "c");//color input attribute
        MAKE_INPUT(nAttr);
        nAttr.setDefault(0.5,0.5,0.5);
    addAttribute(aColor);//Mayaに反映させる
    
    aBorder = nAttr.createColor("border", "b");
        MAKE_INPUT(nAttr);
        nAttr.setDefault(0.0,0.0,0.0);
    addAttribute(aBorder);

    //Float型を入力するバーを指定
    aBorderArea = nAttr.create("borderArea", "bar", MFnNumericData::kFloat, 0);
        MAKE_INPUT(nAttr);
        nAttr.setDefault(0.1);//デフォルト値を指定
        nAttr.setMax(1.0);//最大値
        nAttr.setMin(0.0);//最小値
    addAttribute(aBorderArea);
    
    
    //aOutColorは出力用アトリビュート。出力用に属性を指定
    aOutColor = nAttr.createColor("outColor", "oc");
        nAttr.setStorable(false);
        nAttr.setHidden(false);
        nAttr.setReadable(true);//読み込み可能にする
        nAttr.setWritable(false);//書き込みを禁止にする
    addAttribute(aOutColor);
    
    //normalCameraはカメラ座標系における面の法線ベクトル
    aNormalCamera = nAttr.createPoint("normalCamera", "n");
        MAKE_INPUT(nAttr);
        nAttr.setHidden(true);
    addAttribute(aNormalCamera);
    
    //RayDirectionはレンダリングのときのレイの方向
    aRayDirection = nAttr.createPoint("rayDirection", "rd"); //レイの方向ベクトル
        MAKE_INPUT(nAttr);
        nAttr.setHidden(true);
    addAttribute(aRayDirection);
    
    
    //===================ライトアトリビュートの設定===========================
    
    //ライトは最終的にaLightDataがあれば良いが、aLightDataには複数の子アトリビュートがつくためそれを全て宣言する
    
    aLightDirection = nAttr.createPoint("lightDirection", "ld", &status);
    CHECK_MSTATUS(status);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));
    
    aLightIntensity = nAttr.createColor("lightIntensity", "li");
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));
    
    aLightAmbient = nAttr.create("lightAmbient", "la", MFnNumericData::kBoolean,0);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(true));
    
    aLightDiffuse = nAttr.create("lightDiffuse", "ldu", MFnNumericData::kBoolean,0);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(true));
    
    aLightSpecular = nAttr.create("lightSpecular", "ls", MFnNumericData::kBoolean,0);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(true));
    
    aLightShadowFraction = nAttr.create("lightShadowFraction", "lsf", MFnNumericData::kFloat,0);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(1.0f));
    
    aPreShadowIntensity = nAttr.create("preShadowIntensity", "psi", MFnNumericData::kFloat,0);
    MAKE_LIGHT_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setDefault(1.0f));
    
    aLightBlindData = nAttr.createAddr("lightBlindData", "lbld", &status);
    CHECK_MSTATUS(status);
    MAKE_LIGHT_INPUT(nAttr);

    
    aLightData = lAttr.create( "lightDataArray", "ltd", aLightDirection,
                              aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular,
                              aLightShadowFraction, aPreShadowIntensity, aLightBlindData, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS( lAttr.setArray( true ) );
    CHECK_MSTATUS( lAttr.setStorable( false ) );
    CHECK_MSTATUS( lAttr.setHidden( true ) );
    CHECK_MSTATUS( lAttr.setDefault( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    true, true, false, 1.0f, 1.0f, NULL ) );
    
    addAttribute(aLightData);//ライトデータのみ追加する
    
    
    
    //ここまでが設定
    
    //ここから影響関係の設定
    //入力アトリビュートと出力アトリビュートの関係を宣言
    //ある入力が出力に影響するならば、宣言しなければならない
    attributeAffects(aColor, aOutColor);
    attributeAffects(aBorder, aOutColor);
    attributeAffects(aBorderArea, aBorder);
    attributeAffects(aNormalCamera, aOutColor);
    attributeAffects(aRayDirection, aOutColor);
    //ここからライトの入力アトリビュートと出力アトリビュートの関係を宣言する
    
    
    attributeAffects(aLightAmbient, aOutColor);
    attributeAffects(aLightDiffuse, aOutColor);
    attributeAffects(aLightSpecular, aOutColor);
    attributeAffects(aLightDirection, aOutColor);
    attributeAffects(aLightShadowFraction, aOutColor);
    attributeAffects(aPreShadowIntensity, aOutColor);
    attributeAffects(aLightBlindData, aOutColor);
    attributeAffects(aLightData, aOutColor);
    
    
    return MS::kSuccess;
}

//レンダリング計算を行う（レイとサーフェスが交差する）たびに呼び出される。
//plug :　どのアトリビュートのための計算処理か
//block : シーンデータ
MStatus MyShader::compute(const MPlug &plug, MDataBlock &block){
    
    // The plug parameter will allow us to determine which output attribute
    // needs to be calculated.
    
    
    //もし計算対象のアトリビュートがaOutColorに対してでは無い場合は強制終了
    if(plug != aOutColor){
        return MStatus::kUnknownParameter;
    }
    //出力結果格納用変数
    MFloatVector resultColor(0,0,0);
    
    //シーンから必要なデータを取得
    MFloatVector &surfaceNormal = block.inputValue(aNormalCamera).asFloatVector();//法線方向
    MFloatVector &rayDirection = block.inputValue(aRayDirection).asFloatVector();//レイの方向
    
    MFloatVector &surfaceColor = block.inputValue(aColor).asFloatVector();//表面の色
    MFloatVector &borderColor = block.inputValue(aBorder).asFloatVector();//境界線の色
    float borderArea = block.inputValue(aBorderArea).asFloat();//ボーダーの範囲
//
//    
    
    
    //出力アトリビュートのためのハンドルを取得
    MDataHandle outColorHandle = block.outputValue(aOutColor);
    MFloatVector &outColor = outColorHandle.asFloatVector();
    
    
    //出力はアトリビュート"surfaceColor"とする
    resultColor = surfaceColor;
        //Diffusionの処理
        MArrayDataHandle LightHandle = block.inputArrayValue(aLightData);
        int lightCount = LightHandle.elementCount();
    
        for(int ii=0;ii < lightCount;ii++){
            MDataHandle currentLight = LightHandle.inputValue();
            
            MFloatVector &lightIntensity = currentLight.child(aLightIntensity).asFloatVector();
                        
            if(currentLight.child(aLightAmbient).asBool()){
                resultColor += lightIntensity;
            }
            
            if(currentLight.child(aLightDiffuse).asBool()){
                MFloatVector &lightDirection = currentLight.child(aLightDirection).asFloatVector();
                float cosln = lightDirection * surfaceNormal;
               
                
                MFloatVector lambert;
                lambert[0] = cosln * lightIntensity[0];
                lambert[1] = cosln * lightIntensity[1];
                lambert[2] = cosln * lightIntensity[2];
                
                resultColor += lambert;
            }
            
//            if ( ii < lightCount ) {
                LightHandle.next();
//            }
        }
//
    
        
    //ただし、法線ベクトルとレイのベクトルのなす角によっては変更する
        float NR = surfaceNormal * rayDirection;
    
    
        if(NR > -borderArea && NR < borderArea){
         //出力カラーをborderColorにする
            resultColor += borderColor;
        }
    
    
    //最終的な色を出力
    outColor = resultColor;
    
    //ハンドルを通じて通知
    outColorHandle.setClean();
    
   return MS::kSuccess;
}

//-----------------------------------------------------------------------------
MStatus initializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is loaded into Maya. It 
	//- registers all of the services that this plug-in provides with Maya.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj, PLUGIN_COMPANY, "1.0.0", "Any");
    MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
    const MString user_classify("shader/surface");
    
    plugin.registerNode("MyShader",
                        MyShader::id,
                        MyShader::creator,
                        MyShader::initialize,
                        MPxNode::kDependNode,
                        &user_classify);
    
    
    command += user_classify;
    
    command += "\");}\n";
    
    CHECK_MSTATUS( MGlobal::executeCommand( command ) );

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
