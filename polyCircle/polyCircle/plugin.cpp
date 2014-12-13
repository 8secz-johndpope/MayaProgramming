//
//  plugin.cpp
//  polyCircle
//
//  Created by Tetsushi on 2014/12/12.
//    Copyright (c) 2014年 Tetsushi. All rights reserved.
//

//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include <maya/MFnPlugin.h>

class polyCircle : public MPxCommand{
    
public:
    polyCircle(){};
    virtual ~polyCircle(){};
    static void* creator();
    MStatus doIt(const MArgList &args);
    MStatus redoIt();
    MStatus undoIt();
    bool isUndoable() const {return true;};
private:
    MObject m_transform;    //作成したメッシュ
    MDGModifier m_dgModifier;   //
    int m_div;
};


void* polyCircle::creator(){
    return new polyCircle();
}


MStatus polyCircle::doIt(const MArgList &args){
    MGlobal::displayInfo(MString("doIt"));
    
    //m_div　は円周上の分割数。デフォルト値を3にする
    m_div = 3;
    if(args.length() > 0){
        m_div = args.asInt(0);
    }
    if (m_div < 3) {
        //ワーニングを表示
        MGlobal::displayWarning(MString("minumun argments length is 3."));
        m_div =  3;
    }
    return redoIt();
}

MStatus polyCircle::redoIt(){
    MFloatPointArray pnt;
    pnt.append(MFloatPoint(0,0,0));//0,0,0を中心の頂点として設定
    double theta = 2 * M_PI / m_div;//隣同士の頂点のなす角度
    for (int ii=0; ii<m_div; ii++) {
        double x = cos(theta*ii);
        double y = 0;
        double z = sin(theta*ii);
        pnt.append(MFloatPoint(x,y,z));
    }
    
    int num_pnt = m_div + 1;//頂点数
    
    MIntArray face_count;
    MIntArray face_connection;
    for (int ii=1; ii < m_div; ii++) {
        face_connection.append(0);//起点はすべて0から
        face_connection.append(ii);
        face_connection.append(ii+1);
        face_count.append(3);//3頂点使用します
    }
    //最後のポリゴンだけは0->div->1となるためforの外で記入
    face_connection.append(0);
    face_connection.append(m_div);
    face_connection.append(1);
    face_count.append(3);
  
    int num_faces = m_div;//フェース数
    
    //ここまで平面の設定
    //ここから実際にフェースを作る
    MFnMesh fnPoly;
    m_transform = fnPoly.create(num_pnt, num_faces, pnt, face_count, face_connection, MObject::kNullObj);
    fnPoly.updateSurface();//メッシュ情報が変更されたことをMayaに伝える
    
    //ここからシェーダーの割当。MELを使って割り当てる
    MString mel_cmd("sets -e -fe initialShadingGroup ");
    
    mel_cmd += fnPoly.name();
    m_dgModifier.commandToExecute(mel_cmd);//MELの実行
    
    //作成したオブジェクトを選択する
    MFnDagNode fnDagNode(m_transform);
    mel_cmd = "select ";
    mel_cmd += fnDagNode.name();
    m_dgModifier.commandToExecute(mel_cmd);
    
    //名前を変更
    m_dgModifier.renameNode(m_transform, "pCircle1");
    m_dgModifier.doIt();
    
    return MStatus::kSuccess;
}

MStatus polyCircle::undoIt(){
    MGlobal::displayInfo(MString("undoIt"));
    //redoItでのdoIt()をアンドゥする
    m_dgModifier.undoIt();
    
    //meshノードを削除するためMObjectを取り出す
    MFnDagNode dagNode(m_transform);
    MObject mesh = dagNode.child(0);
    
    //ノードを削除する
    //MDGModifierを使用する
    MDGModifier dgMod;
    dgMod.deleteNode(mesh);
    dgMod.deleteNode(m_transform);
    return dgMod.doIt();
}

//-----------------------------------------------------------------------------
MStatus initializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is loaded into Maya. It 
	//- registers all of the services that this plug-in provides with Maya.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj, _T("Vendor Name"), _T("Version"), _T("Any")) ;
    plugin.registerCommand("createPoly", polyCircle::creator);
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
    plugin.deregisterCommand("createPoly");
	//- Examples:
	//NodeUnregisterOk(plugin.deregisterNode (myNode::id)) ;
	//NodeUnregisterOk(myNode::unregisterMe (plugin)) ;
	
	//-{{MAYA_UNREGISTER
	//-MAYA_UNREGISTER}}

	return (MS::kSuccess) ;
}
