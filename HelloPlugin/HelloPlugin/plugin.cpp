//
//  plugin.cpp
//  HelloPlugin
//
//  Created by Tetsushi on 2014/12/11.
//    Copyright (c) 2014年 Tetsushi. All rights reserved.
//

//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include <maya/MFnPlugin.h>
//#include <maya/MArgList.h>
//#include <maya/MString.h>
//#include <maya/MPxCommand.h>

class HelloPlugin:public MPxCommand{
public:
    HelloPlugin(){};
    virtual ~HelloPlugin(){};
    MStatus doIt(const MArgList& args);
    static void* creator();
};

MStatus HelloPlugin::doIt(const MArgList& args){
   const char *arg0 = "world";
    if(args.length() > 0){
        arg0 = args.asString(0).asChar();
    }
    cout << "HelloWorld" << endl;
    
    MGlobal::executeCommand("print(\"Hello Maya CMD\");");
    return MStatus::kSuccess;
}

void* HelloPlugin::creator(){
    return new HelloPlugin;
}
//-----------------------------------------------------------------------------
MStatus initializePlugin (MObject obj) {
	//- this method is called when the plug-in is loaded into Maya. It 
	//- registers all of the services that this plug-in provides with Maya.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj, PLUGIN_COMPANY, "1.0.0", "Any") ;
    
    plugin.registerCommand("hello", HelloPlugin::creator);//plugin情報の登録
	//- Examples:
	//NodeRegisterOk(plugin.registerNode (_T("myNode"), myNode::id, myNode::creator, myNode::initialize)) ;
	//NodeRegisterOk(myNode::registerMe (plugin)) ;

	//-{{MAYA_REGISTER
	//-MAYA_REGISTER}}

	return MStatus::kSuccess;
}

MStatus uninitializePlugin (MObject obj) {
	#pragma EXPORT
	//- this method is called when the plug-in is unloaded from Maya. It 
	//- deregisters all of the services that it was providing.
	//-		obj - a handle to the plug-in object (use MFnPlugin to access it)
	MFnPlugin plugin (obj) ;
    plugin.deregisterCommand("hello");
	//- Examples:
	//NodeUnregisterOk(plugin.deregisterNode (myNode::id)) ;
	//NodeUnregisterOk(myNode::unregisterMe (plugin)) ;
        
	//-{{MAYA_UNREGISTER
	//-MAYA_UNREGISTER}}
	return MStatus::kSuccess ;
}
