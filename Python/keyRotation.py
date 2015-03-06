#keyRotation.py
#coding: UTF-8

import maya.cmds as cmds
selectionList = cmds.ls(selection=True)

if len(selectionList) >= 1:
    print "Selected items: %s"%selectionList
    for objectName in selectionList:
        objectTypeResult = cmds.objectType(objectName)
        print "%s type: %s"%(objectName, objectTypeResult)

else:
    print "Please select at lease one object"
    