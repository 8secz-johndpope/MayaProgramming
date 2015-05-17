import maya.cmds as cmds
#import fileBrowserCommand

def pushButton_action(lineEditField):
	lineEditText = cmds.textField(lineEditField, query=True, text=True)
	print "lineEditField: %s"%lineEditText

dialog=cmds.loadUI(f="QtTest.ui")
layout1=cmds.paneLayout(cn="single", p=dialog)
cmds.dockControl(aa="all", a="right", fl=0, con=layout1, l="Main Window")
cmds.control(dialog, e=True, p=layout1)