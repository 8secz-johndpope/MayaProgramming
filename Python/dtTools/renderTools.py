#coding: UTF-8
'''
Created on 2015/05/17

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
uiFile = selfDirectory + "/renderView.ui"

form_class, base = uic.loadUiType(uifile=uiFile)

class renderTools(base, form_class):
    
    def __init__(self, parent=getMayaMainWindow()):
        super(base,self).__init__(parent)
        self.setupUi(self)
        self.setObjectName("RenderTools")
        self.setDockNestingEnabled(True)
        self.Establish_Connections()
        
    def Establish_Connections(self):
        QtCore.QObject.connect(self.renderButton, QtCore.SIGNAL("clicked()"), self.renderButton_push)
        
    def renderButton_push(self):
        imagePath = "/Users/tetsushi2/Pictures/sBPHTEd.jpeg"
        qPixmap = QtGui.QPixmap(imagePath)
        item = QtGui.QGraphicsPixmapItem(qPixmap)
        scene = QtGui.QGraphicsScene()
        scene.addItem(item)
        self.renderView.setScene(scene)
        self.renderView.show()

def main():
    global ui
    ui = renderTools()
    ui.show()
    
    
if __name__=="__main__":
    main()