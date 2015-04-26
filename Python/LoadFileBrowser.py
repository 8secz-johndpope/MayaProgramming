import maya.cmds as cmds
import fileBrowserCommand

dialog = cmds.loadUI(f="scripts/UI/fileBrowse.ui")
cmds.showWindow(dialog)