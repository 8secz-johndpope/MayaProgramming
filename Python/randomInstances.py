#randomInstances.py
#coding: UTF-8
import maya.cmds as cmds
import random

random.seed(1234)

#現在選択しているオブジェクトを取得 
result = cmds.ls( orderedSelection=True )

print "result: %s"%result

transformName = result[0]

#グループの作成
instanceGroupName = cmds.group(empty=True, name=transformName+'_instance_grp#')

for i in range(0,50):
    instanceResult = cmds.instance(transformName, name=transformName+'_instance#')
    #print 'instanceResult: '+str(instanceResult)
    #xformでの移動
    #cmds.xform(instanceResult[0], relative=True, translation=(10,0,0))
    
    cmds.parent(instanceResult, instanceGroupName)
    
    #-10 ~ 10 の範囲で乱数を発生させる
    x = random.uniform(-10, 10)
    y = random.uniform(0, 20)
    z = random.uniform(-10, 10)
    #cmds.move(x, y, z, instanceResult)
    
    xRot = random.uniform(0, 360)
    yRot = random.uniform(0, 360)
    zRot = random.uniform(0, 360)
    #cmds.rotate(xRot, yRot, zRot, instanceResult)
    
    SclFactor = random.uniform(0.3, 1.5)
    #cmds.scale(SclFactor, SclFactor, SclFactor, instanceResult)
    cmds.xform(instanceResult, relative=True, translation=(x,y,z), rotation=(xRot,yRot,zRot), scale=(SclFactor, SclFactor, SclFactor))
cmds.hide(transformName)
cmds.xform(instanceGroupName, centerPivots=True)

#references by https://www.youtube.com/watch?v=d7FIkbzB_N0