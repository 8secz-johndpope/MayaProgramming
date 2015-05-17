#coding: UTF-8
'''
Created on 2015/05/17

@author: tetsushi2
Reference: http://melpystudio.blog82.fc2.com/blog-entry-138.html
'''
from PyQt4 import QtCore, QtGui

class imageViewScene(QtGui.QGraphicsScene):
    '''
    拡張したグラフィックスビュー
    '''
    currentImage=None
    def __init__( self, *argv, **kwargs ):
        super( imageViewScene, self ).__init__( *argv, **kwargs )
        
    def setFile(self, imagePath):
        # イメージをアイテムとしてシーンに追加するためのメソッド。
        pixmap = QtGui.QPixmap( imagePath )
        # 既にシーンにPixmapアイテムがある場合は削除する。
        if self.currentImage:
            self.removeItem( self.currentImage )
  
  
        # 与えられたイメージをPixmapアイテムとしてシーンに追加する。-----------
        item = QtGui.QGraphicsPixmapItem( pixmap )
        # アイテムを移動可能アイテムとして設定。
        item.setFlags(
            QtGui.QGraphicsItem.ItemIsMovable
        )
        self.addItem( item )
        self.currentImage = item
        # ---------------------------------------------------------------------
  
        self.fitImage()
    def imageItem( self ):
        return self.currentImage
  
    def fitImage( self ):
        # イメージをシーンのサイズに合わせてフィットするためのメソッド。
        # アスペクト比によって縦にフィットするか横にフィットするかを自動的に
        # 決定する。
        if not self.imageItem():
            return
  
        # イメージの元の大きさを持つRectオブジェクト。
        boundingRect = self.imageItem().boundingRect()
        # シーンの現在の大きさを持つRectオブジェクト。
        sceneRect    = self.sceneRect()
  
        itemAspectRatio  = boundingRect.width() / boundingRect.height()
        sceneAspectRatio = sceneRect.width() / sceneRect.height()
  
        # 最終的にイメージのアイテムに適応するためのTransformオブジェクト。
        transform        = QtGui.QTransform()
  
        if itemAspectRatio >= sceneAspectRatio:
            # 横幅に合わせてフィット。
            scaleRatio = sceneRect.width() / boundingRect.width()
        else:
            # 縦の高さに合わせてフィット。.
            scaleRatio = sceneRect.height() / boundingRect.height()
  
        # アスペクト比からスケール比を割り出しTransformオブジェクトに適応。
        transform.scale( scaleRatio, scaleRatio )
        # 変換されたTransformオブジェクトをイメージアイテムに適応。
        self.imageItem().setTransform( transform )
        
        
class GraphicsEllipseButton(QtGui.QGraphicsEllipseItem):
    callBackFunction=None
    
    def __init__(self, *argv, **kwargs):
        '''
        QGraphicsEllipseItemと同じ情報
        callback: このアイテムを押したときに実行される内容
        '''
        super( GraphicsEllipseButton, self ).__init__( *argv, **kwargs )
    
    def setCallback(self, callback):
        self.callBackFunction = callback
    
    def mousePressEvent(self, *args, **kwargs):
        if(self.callBackFunction != None):
            self.callBackFunction()
        else:
            print "Please set callbackFunction by using \"setCallback\""
        