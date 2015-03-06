#keyRotation.py
#coding: UTF-8

import maya.cmds as cmds

def keyFullRotation(pObjectName, pStartTime, pEndTime, pTargetAttribute):
    #objectName: 対象にするオブジェクト
       #time: 時間の範囲、タプル
       #attribute: カット, 追加する対象のアトリビュート
       cmds.cutKey(pObjectName, time=(pStartTime, pEndTime), attribute=pTargetAttribute)
       #キーの追加
       cmds.setKeyframe(pObjectName, time=pStartTime, attribute=pTargetAttribute, value=0)
       cmds.setKeyframe(pObjectName, time=pEndTime, attribute=pTargetAttribute, value=360)
       #キーの選択
       cmds.selectKey(pObjectName, time=(pStartTime, pEndTime), attribute=pTargetAttribute, keyframe=True)
       #キータンジェントの変更
       cmds.keyTangent(inTangentType="linear", outTangentType="linear")  


selectionList = cmds.ls(selection=True, type="transform")

if len(selectionList) >= 1:
    #print "Selected items: %s"%selectionList
    
    #minTime=Trueでタイムスライダの最初の時間、
    #maxTime=Trueでタイムスライダの最後の時間
    startTime = cmds.playbackOptions(query=True, minTime=True)
    endTime = cmds.playbackOptions(query=True, maxTime=True)
    for objectName in selectionList:
       # objectTypeResult = cmds.objectType(objectName)
       # print "%s type: %s"%(objectName, objectTypeResult)
       keyFullRotation(objectName, startTime, endTime, "rotateY")
       
else:
    print "Please select at lease one object"
    
# referenced by https://www.youtube.com/watch?v=ijMyo-6T8Ow