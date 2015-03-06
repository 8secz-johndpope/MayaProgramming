#keyRotationWithUI.py
#coding: UTF-8
import maya.cmds as cmds
import functools

def createUI(pWindowTitle, pApplyCallback):
    windowID="myWindowID"
    #myWindowIDで開かれたウィンドウが残ってる場合
    if cmds.window(windowID, exists=True):
        cmds.deleteUI(windowID)
    
    #title: window上部のテキスト
    #sizeable: ユーザーによりサイズを変更できるか
    #resizeToFitChildren: 自動的にコンテンツにかぶらないようにする
    cmds.window(windowID, title=pWindowTitle, sizeable=False, resizeToFitChildren=True)
    cmds.rowColumnLayout(numberOfColumns=3, #列の数は3
    columnWidth=[(1, 75), (2, 60), (3, 60)],#第一要素、75pxの幅、...
    columnOffset=[ (1, "right", 3) ])#第一要素、右寄り、3px右側空けて
    
    #第一行目
    cmds.text(label="Time Range: ")
    startTimeField = cmds.intField(value=cmds.playbackOptions(query=True, minTime=True))
    endTimeField = cmds.intField(value=cmds.playbackOptions(query=True, maxTime=True))
    #第二行目
    cmds.text(label="Attribute: ")
    attributeField = cmds.textField(text="rotateY")
    cmds.separator(height=10, style="none")
    #第三行目
    cmds.separator(height=10, style="none")
    cmds.separator(height=10, style="none")
    cmds.separator(height=10, style="none")
    #第四行目
    cmds.separator(height=10, style="none")
    cmds.button(label="Apply", command=functools.partial(pApplyCallback, startTimeField, endTimeField, attributeField))
    
    def cancelCallBack(*pArgs):
            if cmds.window(windowID, exists=True):
              cmds.deleteUI(windowID)
    cmds.button(label="Cancel", command=cancelCallBack)
    cmds.showWindow()

   
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
 

def applyCallback(pStartTimeField, pEndTimeField, pTargetAttributeField, *pArgs):
    startTime = cmds.intField(pStartTimeField, query=True, value=True)
    endTime = cmds.intField(pEndTimeField, query=True, value=True)
    targetText = cmds.textField(pTargetAttributeField, query=True, text=True)
    
    
    selectionList = cmds.ls(selection=True, type="transform")
    
    if len(selectionList) >= 1:
        #print "Selected items: %s"%selectionList
        for objectName in selectionList:
           # objectTypeResult = cmds.objectType(objectName)
           # print "%s type: %s"%(objectName, objectTypeResult)
           keyFullRotation(objectName, startTime, endTime, targetText)
           
    else:
        print "Please select at lease one object"
    
createUI("MyTitle", applyCallback)
   
   
   
   
   