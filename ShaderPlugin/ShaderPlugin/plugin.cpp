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

//アトリビュートを入力可能にするマクロ
#define MAKE_INPUT(__x__) \
    __x__.setKeyable(true);\
    __x__.setStorable(true);\
    __x__.setWritable(true);\
    __x__.setReadable(true);

class MyShader : public MPxNode{
public:
    MyShader(){};
    virtual ~MyShader(){};
    static void* creator();
    virtual MStatus compute(const MPlug &plug, MDataBlock &block);
    static MStatus initialize();
    
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
    
    aColor = nAttr.createColor("color", "c");//color input attribute
    
   
    nAttr.setDefault(0.5,0.5,0.5);
    addAttribute(aColor);
    
    aRayDirection = nAttr.createPoint("rayDirection", "n"); //レイの方向ベクトル
    
    return MStatus::kSuccess;
}

//レンダリング計算を行う（レイとサーフェスが交差する）たびに呼び出される。
MStatus MyShader::compute(const MPlug &plug, MDataBlock &block){
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
