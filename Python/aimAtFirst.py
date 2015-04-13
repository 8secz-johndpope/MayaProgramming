#aimAtFirst.py
#coding: UTF-8
'''
複数項目を選択して実行したとき、最初に選択したオブジェクトの方向に
向くようなコンストレイントを作成する
AAA
'''
import maya.cmds as cmds

selectionList = cmds.ls(orderedSelection=True)

if len(selectionList) >= 2:
    print 'Selected items: %s' %selectionList
    
    targetName = selectionList[0]
    selectionList.remove( targetName )
    for objectName in selectionList:
        print "Constraining %s towards %s"%(objectName, targetName)
        #エイムコンストレイントの設定。
        cmds.aimConstraint(targetName, objectName, aimVector=[0, 1, 0] )
else:
    print 'Please select two or more objects'    


#referenced by https://www.youtube.com/watch?v=d7FIkbzB_N0
