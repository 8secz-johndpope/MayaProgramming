import maya.cmds as cmds

def fileBrowse(lineEditName):
    openFile = cmds.fileDialog2(fm=1, dialogStyle=2)
    cmds.textField(lineEditName, edit=True, text=openFile[0])
    return openFile[0]