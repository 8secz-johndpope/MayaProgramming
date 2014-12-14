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
    
    //出力カラー
    static MObject  aOutColor;  //outColorアトリビュート
};

//IDを定義。このIDは正しくはAutodeskから取得しなければならない
MTypeId MyShader::id(0xab33);

void *MyShader::creator(){
    return new MyShader();
}

//シェーダーの初期化
//アトリビュートの宣言や追加、入出力アトリビュートの定義
MStatus MyShader::initialize(){
    //アトリビュートを定義する。nAttrは”どのような名称のアトリビュートを定義するか”
    MFnNumericAttribute nAttr;
    
    //入力色を指定する
    aColor = nAttr.createColor("color", "c");//color input attribute
        MAKE_INPUT(nAttr);
        nAttr.setDefault(0.5,0.5,0.5);
    addAttribute(aColor);//Mayaに反映させる
    
    aBorder = nAttr.createColor("border", "b");
        MAKE_INPUT(nAttr);
        nAttr.setDefault(0.0,0.0,0.0);
    addAttribute(aBorder);7

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
    aRayDirection = nAttr.createPoint("rayDirection", "n"); //レイの方向ベクトル
        MAKE_INPUT(nAttr);
        nAttr.setHidden(true);
    addAttribute(aRayDirection);
    
    //ここまでが設定
    
    //ここから影響関係の設定
    //入力アトリビュートと出力アトリビュートの関係を宣言
    //ある入力が出力に影響するならば、宣言しなければならない
    attributeAffects(aColor, aOutColor);
    attributeAffects(aBorder, aOutColor);
    attributeAffects(aBorderArea, aBorder);
    attributeAffects(aNormalCamera, aOutColor);
    attributeAffects(aRayDirection, aOutColor);
    
    return MStatus::kSuccess;
}

//レンダリング計算を行う（レイとサーフェスが交差する）たびに呼び出される。
//plug :　どのアトリビュートのための計算処理か
//block : シーンデータ
MStatus MyShader::compute(const MPlug &plug, MDataBlock &block){
    
    //もし計算対象のアトリビュートがaOutColorに対してでは無い場合は強制終了
    if(plug != aOutColor){
        return MStatus::kUnknownParameter;
    }
    
    MFloatVector &rayDirection = block.inputValue(aRayDirection).asFloatVector();
    
}

//-----------------------------------------------------------------------------
MStatus initializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is loaded into Maya. It 
	//- registers all of the services that this plug-in provides with Maya.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj, _T("Vendor Name"), _T("Version"), _T("Any"));
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

	//- Examples:
	//NodeUnregisterOk(plugin.deregisterNode (myNode::id)) ;
	//NodeUnregisterOk(myNode::unregisterMe (plugin)) ;
	
	//-{{MAYA_UNREGISTER
	//-MAYA_UNREGISTER}}

	return (MS::kSuccess) ;
}
