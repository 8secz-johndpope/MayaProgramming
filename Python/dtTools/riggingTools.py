#coding:UTF-8
'''
Created on 2015/05/09

@author: tetsushi2
'''
import os
import maya.cmds as cmds
import pymel.core as pm
from PyQt4 import QtCore, QtGui, uic
import maya.OpenMayaUI as omau
import sip
import dtLib as dLib
from dtTools import dtLib

def getMayaMainWindow():
    accessMainWindow = omau.MQtUtil.mainWindow()
    return sip.wrapinstance(long(accessMainWindow), QtCore.QObject)

#パッケージ内のfile.uiを呼び出す。
selfDirectory = os.path.dirname(__file__)
uiFile = selfDirectory + "/file.ui"

form_class, base = uic.loadUiType(uifile=uiFile)

class riggingTools(base, form_class):
    baseHead=None
    facialCurves = []
    def __init__(self, parent=getMayaMainWindow()):
        super(base,self).__init__(parent)
        self.setupUi(self)
        self.setObjectName("RiggingTools")
        self.setDockNestingEnabled(True)
        self.Establish_Connections()
    def Establish_Connections(self):
        #シグナルの設定。
        #self.makeCube: シグナルを発生させるもの。Qt上でmakeCubeというpushButtonを作成し、それを参照している
        #QtCore.SIGNAL("clicked()"): イベント処理の種類を設定。今回はclickedイベントを使用する
        #self.specialfk: 実行内容。今回はspecialfkを実行する。
        QtCore.QObject.connect(self.specialFk, QtCore.SIGNAL("clicked()"),self.specialfk)
        QtCore.QObject.connect(self.addJointsButton, QtCore.SIGNAL("clicked()"),self.addJoints)
        QtCore.QObject.connect(self.makeCircleButton, QtCore.SIGNAL("clicked()"), pm.circle)
        QtCore.QObject.connect(self.makeSquareButton, QtCore.SIGNAL("clicked()"), self.makeSquare)
        QtCore.QObject.connect(self.makeCubeButton, QtCore.SIGNAL("clicked()"), self.makeCube)
        QtCore.QObject.connect(self.loadBaseButton, QtCore.SIGNAL("clicked()"), self.loadBaseHead)
        QtCore.QObject.connect(self.faceCurveButton, QtCore.SIGNAL("clicked()"), self.makeBaseFacial)
        QtCore.QObject.connect(self.facialJointButton, QtCore.SIGNAL("clicked()"), self.makeFacialJoint)
    #クリックにより発生される関数
    def specialfk(self):
        selectedList = pm.selected()
        print selectedList
        for current in selectedList:
            #円のカーブの作成
            crv_circle = pm.circle()
            pm.select(crv_circle)
            #オブジェクト座標形でyに90度回転
            pm.rotate([0,0,90])
            #移動、回転、スケールをフリーズする
            pm.makeIdentity(apply=True, translate=True, rotate=True, scale=True, normal=1)
            #カーブのシェイプノードのみをジョイントの子供にする
            #こうすることで、カーブをコントロールすることでジョイントもコントロールできる
            pm.parent(crv_circle[0].getShape(), current,relative=True, shape=True)
            #余分なシェイプノードを削除する
            pm.delete(crv_circle)
    #ジョイントの二点間にジョイントを挿入する
    def addJoints(self):
        #GUI上のnumJointsから値取得
        numValue = self.numJoints.value()
        selectedList = pm.selected()
        firstSelection = selectedList[0]
        tmps = pm.listRelatives(firstSelection,type=u"joint")
        secondSelection = tmps[0]
        dLib.dtLib.addManyJoint(firstSelection, secondSelection, num=numValue)

    #四角形カーブを生成する関数
    def makeSquare(self):
        #ライブラリから四角形生成を呼び出す
        print dLib.dtLib.makeSquare()
    
    #立方体を作成
    def makeCube(self):
        print dLib.dtLib.makeCube()
        
    def loadBaseHead(self):
        self.baseHead = dLib.dtLib.loadFbx(os.path.dirname(__file__) + "/data/obj/baseHead.fbx")
        pm.select(self.baseHead[0])
        self.faceCurveButton.setEnabled(True)
        #cmds.FBXImport(f=filePath)

    def makeBaseFacial(self):
        filePath = os.path.dirname(__file__) + "/data/vertex.csv"
        for line in open(filePath, 'r'):
            selectList = line.split(',')
            pList = []
            for n in selectList[1:]:
                #読み込んだ物はstr型になってるため、intに修正
                ii = int(n)      
                p = self.baseHead[0].vtx[ii].getPosition(space='world')
                pList.append(p)
            
            pList.append(p)
            #２次ベジェ曲線で作成する
            pmCurve=pm.curve(p=pList, degree=2)
            pmCurve.rename(selectList[0])
            self.facialCurves.append(pmCurve)
        self.facialJointButton.setEnabled(True)
        
    def makeFacialJoint(self):
        lipCurve = [i for i in self.facialCurves if i[:].encode("ascii").find("lip") >= 0]
        noseCurve = [i for i in self.facialCurves if i[:].encode("ascii").find("nose") >= 0]
        otherCurves = [i for i in self.facialCurves if i[:].encode("ascii").find("lip") < 0 and i[:].encode("ascii").find("nose") < 0]
        
        locGroup=pm.group(name="locatorGrp")
        
        #鼻のカーブにコントローラを設定
        for currentCurve in noseCurve:
            locatorData = pm.spaceLocator()
            locatorData.setScale([0.5,0.5,0.5])
            pos = 1/float(2)
            dLib.dtLib.setToCurve(currentCurve, locatorData, position=pos)
            #crvをとったprefix部分を取得
            namePrefix = currentCurve[:-3]
            locatorData.rename(namePrefix+"locator")
            #オフセットジョイントの作成
            offsetJoint=pm.joint(p=[0,0,0])
            offsetJoint.rename(namePrefix+"offsetJnt")
            #コントロール用のジョイント作成
            controlJoint = pm.joint(p=[0,0,0])
            controlJoint.rename(namePrefix+"controlJnt")
            
            radius = 0.3
            controlSphere = pm.sphere(r=radius)
            sphereShape=controlSphere[0].getShape()
            pm.setAttr(sphereShape[:]+".primaryVisibility", 0)
            #ペアレント化
            pm.parent(controlSphere[0].getShape(), controlJoint, relative=True, shape=True)
            pm.delete(controlSphere)
            pm.parent(controlJoint, offsetJoint)
            pm.parent(offsetJoint, locatorData)
            offsetJoint.setTranslation([0,0,0])
            pm.parent(locatorData, locGroup)
        #唇のカーブにコントローラーを設定
        for currentCurve in lipCurve:
            for i in range(0,5):
                locatorData = pm.spaceLocator()
                locatorData.setScale([0.5,0.5,0.5])
                pos = i / float(4)
                dLib.dtLib.setToCurve(currentCurve, locatorData, position=pos)
                #crvをとったprefix部分を取得
                namePrefix = currentCurve[:-3]
                locatorData.rename(namePrefix+"locator"+str(i+1))
                #オフセットジョイントの作成
                offsetJoint=pm.joint(p=[0,0,0])
                offsetJoint.rename(namePrefix+"offsetJnt"+str(i+1))
                #コントロール用のジョイント作成
                controlJoint = pm.joint(p=[0,0,0])
                controlJoint.rename(namePrefix+"controlJnt"+str(i+1))
                
                radius = 0.3
                controlSphere = pm.sphere(r=radius)
                sphereShape=controlSphere[0].getShape()
                pm.setAttr(sphereShape[:]+".primaryVisibility", 0)
                #ペアレント化
                pm.parent(controlSphere[0].getShape(), controlJoint, relative=True, shape=True)
                pm.delete(controlSphere)
                pm.parent(controlJoint, offsetJoint)
                pm.parent(offsetJoint, locatorData)
                offsetJoint.setTranslation([0,0,0])
                pm.parent(locatorData, locGroup)
        #他のカーブにコントローラーを設定
        for currentCurve in otherCurves:
            for i in range(0,3):
                locatorData = pm.spaceLocator()
                locatorData.setScale([0.5,0.5,0.5])
                pos = i / float(2)
                dLib.dtLib.setToCurve(currentCurve, locatorData, position=pos)
                #crvをとったprefix部分を取得
                namePrefix = currentCurve[:-3]
                locatorData.rename(namePrefix+"locator"+str(i+1))
                #オフセットジョイントの作成
                offsetJoint=pm.joint(p=[0,0,0])
                offsetJoint.rename(namePrefix+"offsetJnt"+str(i+1))
                #コントロール用のジョイント作成
                controlJoint = pm.joint(p=[0,0,0])
                controlJoint.rename(namePrefix+"controlJnt"+str(i+1))
                
                radius = 0.3
                controlSphere = pm.sphere(r=radius)
                sphereShape=controlSphere[0].getShape()
                pm.setAttr(sphereShape[:]+".primaryVisibility", 0)
                #ペアレント化
                pm.parent(controlSphere[0].getShape(), controlJoint, relative=True, shape=True)
                pm.delete(controlSphere)
                pm.parent(controlJoint, offsetJoint)
                pm.parent(offsetJoint, locatorData)
                offsetJoint.setTranslation([0,0,0])
                pm.parent(locatorData, locGroup)
        
#main


def main():
    global ui
    ui = riggingTools()
    ui.show()
    
    
if __name__=="__main__":
    main()