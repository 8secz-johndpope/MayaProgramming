#coding: UTF-8
'''
Created on 2015/05/14

@author: tetsushi2
'''
import pymel.core as pm
import maya.cmds as cmds

class dtLib():
    '''
    classdocs
    '''


    def __init__(self, params):
        '''
        Constructor
        '''
        
    '''
    ======================================================
    
    ジョイント関係の処理 
    
    ======================================================
    '''   
    @staticmethod
    def addManyJoint(firstJoint, secondJoint, num=1):
         jointRadius = firstJoint.getRadius()
         firstPoint = firstJoint.getTranslation(space='world')
         secondPoint = secondJoint.getTranslation(space='world')
         for n in range(0,num):
            newJoint = pm.insertJoint(firstJoint)
            #追加したジョイントの位置を計算する
            px = secondPoint[0] + (((firstPoint[0]-secondPoint[0])/(num+1))*(n+1))
            py = secondPoint[1] + (((firstPoint[1]-secondPoint[1])/(num+1))*(n+1))
            pz = secondPoint[2] + (((firstPoint[2]-secondPoint[2])/(num+1))*(n+1))
            #newJointはunicode型なのでpyMelの機能が使えない。ここではcmdsを使用する
            cmds.joint(newJoint, e=True, component=True, position=[px, py, pz], radius=jointRadius)
    
    '''
    ======================================================
    
    カーブ関係の処理 
    
    ======================================================
    '''    
    '''
    四角形カーブを生成する
    出力: 作成された四角カーブ
    '''
    @staticmethod
    def makeSquare():
        crv = pm.nurbsSquare()
        ch = crv[0].getChildren()
        for n in range(1,len(ch)):
            current = ch[n]
            pm.parent(current.getShape(), ch[0], relative=True, shape=True)
            pm.delete(current)
        pm.parent(ch[0], crv, world=True)
        pm.delete(crv)
        ch[0].rename(u'nurbsSquare')
        return ch[0]
    
    '''
    立方体カーブを生成する
    出力: 作成された四角キューブ
    '''
    @staticmethod
    def makeCube():
        squareCrv = dtLib.makeSquare()
        squareCrv2 = dtLib.makeSquare()
        squareCrv2.setRotation([90, 0,0])
        squareCrv2.setTranslation([0,0.5,-0.5])
        pm.makeIdentity(apply=True, translate=True, rotate=True, scale=True, normal=1)
        
        crvs = squareCrv2.getShapes()
        
        for current in crvs:
            pm.parent(current, squareCrv, relative=True, shape=True)
        pm.delete(squareCrv2)
        
        pm.select(squareCrv)
        dupCrv = pm.duplicate()
        dupCrv[0].setRotation([180, 0, 0])
        dupCrv[0].setTranslation([0,0,-1])
        pm.makeIdentity(apply=True, translate=True, rotate=True, scale=True, normal=1)
        crvs = dupCrv[0].getShapes()
        for current in crvs:
            pm.parent(current, squareCrv, relative=True, shape=True)
            
        pm.delete(dupCrv)
        #中央にピポットを移動
        pm.select(squareCrv)
        pm.xform(cp=True)
        return pm.selected()
    
    '''
    指定したカーブにオブジェクトを合わせる
    target: 対象のカーブ
    object: 合わせるもの
    position: (オプション)指定位置、デフォルトでは0に設定
    '''
    @staticmethod
    def setToCurve(target, objectData, position=0):
        #pathAnimationを指定.fractionMode=Trueにすることで0~1にu値を正規化
         motionPathName=pm.pathAnimation(objectData, curve=target, fractionMode=True)
         #文字列をstr型で使いたいためAsciiコードに変換
         ascPathName = motionPathName.encode('ascii')
         pm.setAttr(ascPathName+".uValue", position)
         pm.disconnectAttr(ascPathName+".uValue")
    
    
    
    '''
    ==================================================================
    
    ファイル関係の処理
    
    ==================================================================
    '''
        
    '''
    入力: ロードするファイル名
    出力:　読み込まれたファイルのTransformとMeshのリスト
    '''
    @staticmethod
    def loadFbx(filePath):
        newNodes = pm.importFile(filePath, returnNewNodes=True, type=u"FBX")
        objectNode = [i for i in newNodes if isinstance(i, pm.nodetypes.Transform) or isinstance(i, pm.nodetypes.Mesh)]
        return objectNode
         
        