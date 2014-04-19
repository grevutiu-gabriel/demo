#######################################################################################################
#  FromHoudini2Nuke V1.0 - Date: 21.10.2010 - Created by Jan Oberhauser - jan.oberhauser@gmail.com    #
#  Exports Polygon-Objects, Cameras, Lights and Locators from Houdini to Nuke                         #
#  Check for a updateted Version at http://janoberhauser.gute-filme-enden-nie.de                      #
#######################################################################################################

# In order to use this script do the following:
# 1.create a shelf
# 2.add a new tool with the following script:
#	execfile(hou.findFile('location/of/this/file'))
#	export = ExportScene()
#
#

#Imports the necessary stuff
import os, sys, inspect, ntpath
import subprocess
from types import *

def getThisScriptPath():
	filename = inspect.getframeinfo(inspect.currentframe()).filename
	path = os.path.dirname(os.path.abspath(filename))
	return path

#make sure path of this python script is registered so that acompanied files are found
#in import
sys.path.insert(0,getThisScriptPath()) 
import binary_json

def path_leaf(path):
	head, tail = ntpath.split(path)
	return tail or ntpath.basename(head)

	
	
#
# main class for exporting scene content
#
class ExportScene:
	def __init__(self):

		#set Default-Values
		self.defaultExportPath = 'C:\\projects\\demo\\git\\bin\\data'
		self.defaultExportScriptName = path_leaf(hou.hipFile.name()).replace(".hipnc", ".scn").replace(".hip", ".scn")
		self.defaultStartFrameValue = int(hou.hscriptStringExpression("$RFSTART"))
		self.defaultEndFrameValue = int(hou.hscriptStringExpression("$RFEND"))

		self.selected = hou.selectedNodes()
		if len(self.selected) == 0:
			self.selected=[]
			# objects
			objs = hou.node("/obj").children()
			
			for obj in objs:
				#ignore export config object
				if obj.name()=="export_config":
					continue
				# ignore ipr cameras...dont know what they are for
				if "ipr_camera" in obj.name():
					continue
				# add object to selection...
				self.selected.append(obj)
			print(self.selected)
			
			# we want to export everything
			#hou.ui.displayMessage("Nothing is selected.\nPlease select the Objects, Cameras, Lights and Locators you want to export.\n", buttons=('OK',), severity=hou.severityType.Message, default_choice=0,title="FromHoudini2Nuke - ERROR") 

		# check if export configuration node exists
		if hou.node( '/obj/export_config/export_config' ) == None:
			# no configuration exist: show ui
			self.showParameterWindow()
		else:
			# start ----
			# remember current frame
			self.frameScriptStart = hou.frame()
			# ??
			self.checkParentParameters = ['tx', 'ty', 'tz']
			# initialize lists of things we want to export...
			self.objects = []
			self.cameras = []
			self.lights = []
			self.locators = []
			self.inSubnet = {}
			# run export
			self.startExportProcedure()

				
	def startExportProcedure(self):
		print("startExportProcedure")

		parameterNode = hou.node("/obj/export_config/export_config")
		geoParameters = hou.node("/obj/export_config")

		#supposed to close the panel: this crashes houdini12.1 and 13.0 ----
		#thisPaneTab = None
		#for paneTab in hou.ui.paneTabs():
		#	print(paneTab.name())
		#	if paneTab.name() == 'exportScene':
		#		thisPaneTab = paneTab
		#		break 
		#thisPaneTab.close()

		self.startFrame = parameterNode.parm('frameRangeFieldx').evalAsInt()
		self.endFrame = parameterNode.parm('frameRangeFieldy').evalAsInt()
		self.exportScriptName = str(parameterNode.parm('exportScriptNameField').eval()) 
		self.exportPath = str(parameterNode.parm('exportPathField').eval()) 
		self.exportPath = self.exportPath.replace('\\', '/')
		if self.exportPath[len(self.exportPath)-1] != '/':
			self.exportPath += '/'

		self.readObjects(self.selected) 

		outfile = open( self.exportPath + self.exportScriptName, "w" )
		writer = binary_json.Writer( outfile, False, 6 )

		writer.jsonBeginMap()
		#self.exportObjects()
		self.exportCameras( writer )
		#self.exportLights()
		self.exportLocators( writer )
		writer.jsonEndMap()
		
	 
		#if self.deleteExport == True:
			#self.delteCameraFiles()
			#self.delteLightFiles()
			#self.delteLocatorFile()
		
		# close the file
		outfile.close()
		
		# revert to initial frame
		hou.setFrame(self.frameScriptStart)

 

	def showParameterWindow(self):

		# get root node of objects
		obj = hou.node("/obj")
		
		# getcreate a geo node containing a null node. We will store our parameters there
		self.geoParameters = obj.createNode("geo", "export_config", run_init_scripts = False)
		self.parameterNode = self.geoParameters.createNode('null', 'export_config')

		# now attach parameters for all configuration info 
		parameterTemplate = hou.StringParmTemplate('exportPathField', 'Export Path: ', 1, default_value=([self.defaultExportPath]))
		self.parameterNode.addSpareParmTuple(parameterTemplate)

		parameterTemplate = hou.StringParmTemplate('exportScriptNameField', 'Export Script-Name: ', 1, default_value=([self.defaultExportScriptName]))
		self.parameterNode.addSpareParmTuple(parameterTemplate)
		
		parameterTemplate = hou.IntParmTemplate('frameRangeField', 'Frame-Range: ', 2, (self.defaultStartFrameValue,self.defaultEndFrameValue))
		self.parameterNode.addSpareParmTuple(parameterTemplate)
		
		parameterTemplate = hou.ButtonParmTemplate('starExport', 'Export', tags={"script_callback": "execfile(hou.findFile('"+getThisScriptPath()+"\\exportScene.py')); export = ExportScene()", "script_callback_language": "python"})
		self.parameterNode.addSpareParmTuple(parameterTemplate)
		
		self.pane_tab = hou.ui.curDesktop().createFloatingPaneTab(hou.paneTabType.Parm)
		self.pane_tab.setName('exportScene')
		self.pane_tab.setCurrentNode(self.parameterNode)
		self.pane_tab.setPin(True)
		
	def readObjects(self, selected):
		
		for thisNode in selected:
			thisObject = hou.node(thisNode.path())
			thisObjectType = thisObject.type().name() 
			
			if thisObjectType == 'cam':
				self.cameras[:0] = [thisNode]
			elif thisObjectType == 'hlight':
				self.lights[:0] = [thisNode]
			elif thisObjectType == 'geo':
				self.objects[:0] = [thisNode]
			elif thisObjectType == 'subnet':
				childrenNodes = hou.node(thisNode.path())
				childrenNodes = childrenNodes.children()
				for childNode in childrenNodes:
					self.inSubnet.update({childNode:thisNode})
				self.readObjects(childrenNodes)
			elif thisObjectType == 'null':
				self.locators[:0] = [thisNode]

				
				
				
				
				
				
				
				
				
	def exportCameras(self, writer):
		channelMatch = {'transform.tx':'tx', 'transform.ty':'ty', 'transform.tz':'tz', 'transform.rx':'rx', 'transform.ry':'ry', 'transform.rz':'rz', 'transform.rotateOrder':'rOrd', 'camera.fl':'focal', 'camera.horizontalFilmAperture':'aperture'}

		writer.jsonKeyToken("cameras")
		writer.jsonBeginMap()
		for camera in self.cameras:
			object = hou.node(camera.path())
			objectName = object.name()
			writer.jsonKeyToken(objectName)
			self.exportNode(camera, channelMatch, self.startFrame, self.endFrame, writer)
			#self.exportData(camera, channelMatch, self.startFrame, self.endFrame, self.exportPath + camera.name() + '.fm2n')
			#self.exportData2(camera, channelMatch, self.startFrame, self.endFrame, writer)
			print "Exported: " + camera.name()
		writer.jsonEndMap()

	def exportLocators(self, writer):
		channelMatch = {'transform.tx':'tx', 'transform.ty':'ty', 'transform.tz':'tz', 'transform.rx':'rx', 'transform.ry':'ry', 'transform.rz':'rz', 'transform.rotateOrder':'rOrd'}

		writer.jsonKeyToken("locators")
		writer.jsonBeginMap()
		for locator in self.locators:
			object = hou.node(locator.path())
			objectName = object.name()
			writer.jsonKeyToken(objectName)
			self.exportNode(locator, channelMatch, self.startFrame, self.endFrame, writer)
			print "Exported: " + locator.name()
		writer.jsonEndMap()	
			
	def getRealParmValue(self, object, parameter, frame=None):
		if frame == None:
			frame = self.frameScriptStart
		thisValue = object.parm(parameter).evalAsFloatAtFrame(frame)
		thisObject = object
		
		while True:
			parent = thisObject.parent()
			if str(parent) == 'obj' or parent == None:
				break
			else:
				thisValue += parent.parm(parameter).evalAsFloatAtFrame(frame)
				thisObject = parent
		
		return thisValue
			
			
	def getIfAnimated(self, object, channel, channels):
		isAnimated = object.parm(channels[channel]).isTimeDependent()
		while isAnimated == False:
			parent = object.parent()
			if str(parent) == 'obj' or parent == None:
				break
			else:
				isAnimated = parent.parm(channels[channel]).isTimeDependent()
				object = parent
		
		return isAnimated
	

	def exportChannel(self, object, channelName, channel, startF, endF, writer):
		objectPath = object.path()
	
		#write channel ---
		writer.jsonBeginMap()
		
		writer.jsonKeyToken( "nsamples" )
		writer.jsonInt( (endF+1)-startF )
		
		writer.jsonKeyToken( "data" )
		writer.jsonBeginArray()
		for frame in range(startF, (endF+1)):
			thisValue = hou.parm(objectPath+"/"+channel).evalAsFloatAtFrame(frame)
			if channel in self.checkParentParameters :
				thisValue = self.getRealParmValue(object, channel, frame)
			writer.jsonReal32( thisValue )
		writer.jsonEndArray()
		
		
		writer.jsonEndMap()

	
	def exportNode(self, object, channels, startF, endF, writer):
		object = hou.node(object.path())
		objectPath = object.path()
		objectName = object.name()
		objectType = object.type().name()
		
		print("exporting "+objectName)
	
		channelsAnimated = []
		channelsNotAnimated = []
		
		writer.jsonBeginMap()

		objectNameWrite = objectName

		if objectType == 'cam':
			objectNodeTypeWrite = 'camera'
		elif objectType == 'null':
			objectNodeTypeWrite = 'locator'
		elif objectType == 'hlight':
			if object.parm('light_type').eval() == 0:
				objectNodeTypeWrite = 'point' 
			elif object.parm('light_type').eval() == 1:
				objectNodeTypeWrite = 'spot'
			elif object.parm('light_type').eval() == 2: 
				objectNodeTypeWrite = 'directional'

		for channel in channels:
			if channels[channel] in self.checkParentParameters :
				isAnimated = self.getIfAnimated(object, channel, channels)
			else:
				isAnimated = hou.parm(objectPath+"/"+channels[channel]).isTimeDependent()
			
			if isAnimated == True:
				channelsAnimated.append(channel)
			else:
				channelsNotAnimated.append(channel)		



		# write non-animated channels as name value pairs ---
		for channel in channelsNotAnimated:
			thisValue = hou.parm(objectPath+"/"+channels[channel]).eval()
			
			if channels[channel] in self.checkParentParameters :
				thisValue = self.getRealParmValue(object, channels[channel])
			if channel == 'transform.rotateOrder':
				thisValue = hou.parm(objectPath+"/"+channels[channel]).evalAsString().upper()
			if channel == 'camera.fl':
				focalUnit = hou.parm(objectPath+"/focalunits").evalAsString()
				thisValue = float(thisValue)
				if focalUnit == 'm':
					thisValue = thisValue*1000 
				elif focalUnit == 'nm':
					thisValue = thisValue/1000000
				elif focalUnit == 'in':
					thisValue = thisValue*25.4
				elif focalUnit == 'ft':
					thisValue = thisValue*304.8
				thisValue = thisValue
			
			writer.jsonKey( channel )			
			if type(thisValue) is IntType:
				writer.jsonInt( thisValue )
			elif type(thisValue) is FloatType:
				writer.jsonReal32( thisValue )
			elif type(thisValue) is StringType:
				writer.jsonString( thisValue )
			else:
				writer.jsonString( "error" )
				
			
			
		
		if objectType == 'cam':
			resy = float(object.parm('resy').eval())
			resx = float(object.parm('resx').eval())
			apx = float(object.parm('aperture').eval())
			asp = float(object.parm('aspect').eval()) 
			verticalAperture = (resy*apx) / (resx*asp)		
			
			writer.jsonKey( "resx" )
			writer.jsonInt( resx );
			writer.jsonKey( "resy" )
			writer.jsonInt( resy );

		
		#writeOut += '+++++Animated+++++\n'
	
		# write animated channels into channels ---
		writer.jsonKeyToken( "channels" )
		writer.jsonBeginMap()
		if len(channelsAnimated) > 0:
			for channel in channelsAnimated:
				writer.jsonKeyToken( channel )
				self.exportChannel( object, channel, channels[channel], startF, endF, writer )
		writer.jsonEndMap() #channels

		writer.jsonEndMap() #object
		
	
	def exportData2(self, object, channels, startF, endF, writer):
		object = hou.node(object.path())
		objectPath = object.path()
		objectName = object.name()
		objectType = object.type().name()
	
		channalsAnimated = []
		channalsNotAnimated = []
		
		writer.jsonBeginMap()

		objectNameWrite = objectName

		if objectType == 'cam':
			objectNodeTypeWrite = 'camera'
		elif objectType == 'null':
			objectNodeTypeWrite = 'locator'
		elif objectType == 'hlight':
			if object.parm('light_type').eval() == 0:
				objectNodeTypeWrite = 'point' 
			elif object.parm('light_type').eval() == 1:
				objectNodeTypeWrite = 'spot'
			elif object.parm('light_type').eval() == 2: 
				objectNodeTypeWrite = 'directional'

		for channel in channels:		
			if channels[channel] in self.checkParentParameters :
				isAnimated = self.getIfAnimated(object, channel, channels)
			else:
				isAnimated = hou.parm(objectPath+"/"+channels[channel]).isTimeDependent()
			
			if isAnimated == True:
				channalsAnimated.append(channel)
			else:
				channalsNotAnimated.append(channel)		
		
		writeOut = objectNameWrite + '\t'  + objectNodeTypeWrite + '\t\n'
				
		# write non-animated channels as name value pairs ---
		for channel in channalsNotAnimated:
			thisValue = hou.parm(objectPath+"/"+channels[channel]).eval()
			
			if channels[channel] in self.checkParentParameters :
				thisValue = self.getRealParmValue(object, channels[channel])
			if channel == 'transform.rotateOrder':
				thisValue = hou.parm(objectPath+"/"+channels[channel]).evalAsString().upper()
			if channel == 'camera.fl':
				focalUnit = hou.parm(objectPath+"/focalunits").evalAsString()
				thisValue = float(thisValue)
				if focalUnit == 'm':
					thisValue = thisValue*1000 
				elif focalUnit == 'nm':
					thisValue = thisValue/1000000
				elif focalUnit == 'in':
					thisValue = thisValue*25.4
				elif focalUnit == 'ft':
					thisValue = thisValue*304.8
				thisValue = thisValue
			
			writer.jsonKey( channel )			
			if type(thisValue) is IntType:
				writer.jsonInt( thisValue )
			elif type(thisValue) is FloatType:
				writer.jsonReal32( thisValue )
			elif type(thisValue) is StringType:
				writer.jsonString( thisValue )
			else:
				writer.jsonString( "error" )
				
			
			
		
		if objectType == 'cam':
			resy = float(object.parm('resy').eval())
			resx = float(object.parm('resx').eval())
			apx = float(object.parm('aperture').eval())
			asp = float(object.parm('aspect').eval()) 
			verticalAperture = (resy*apx) / (resx*asp)		
			
			writer.jsonKey( "resx" )
			writer.jsonInt( resx );
			writer.jsonKey( "resy" )
			writer.jsonInt( resy );

		
		writeOut += '+++++Animated+++++\n'
	
	
		# write animated channels into tracks ---
		writer.jsonKeyToken( "tracks" )
		writer.jsonBeginMap()
		if len(channalsAnimated) > 0:
			for channel in channalsAnimated:
				writer.jsonKeyToken( channel )
				self.exportTrack( object, channel, channels[channel], startF, endF, writer )
		writer.jsonEndMap() #tracks
		
		writer.jsonEndMap() #object
		
		#f = open(exportFile, 'w') 
		#f.write(writeOut)
		#f.close()
	
	def exportData(self, object, channels, startF, endF, exportFile):
		object = hou.node(object.path())
		objectPath = object.path()
		objectName = object.name()
		objectType = object.type().name()
	
		channalsAnimated = []
		channalsNotAnimated = []
		
		objectNameWrite = objectName

		if objectType == 'cam':
			objectNodeTypeWrite = 'camera'
		elif objectType == 'null':
			objectNodeTypeWrite = 'locator'
		elif objectType == 'hlight':
			if object.parm('light_type').eval() == 0:
				objectNodeTypeWrite = 'point' 
			elif object.parm('light_type').eval() == 1:
				objectNodeTypeWrite = 'spot'
			elif object.parm('light_type').eval() == 2: 
				objectNodeTypeWrite = 'directional'

		for channel in channels:		
			if channels[channel] in self.checkParentParameters :
				isAnimated = self.getIfAnimated(object, channel, channels)
			else:
				isAnimated = hou.parm(objectPath+"/"+channels[channel]).isTimeDependent()
			
			if isAnimated == True:
				channalsAnimated.append(channel)
			else:
				channalsNotAnimated.append(channel)		
		
		writeOut = objectNameWrite + '\t'  + objectNodeTypeWrite + '\t\n'
				
		for channel in channalsNotAnimated:
			thisValue = hou.parm(objectPath+"/"+channels[channel]).evalAsString()
			
			if channels[channel] in self.checkParentParameters :
				thisValue = str(self.getRealParmValue(object, channels[channel]))
			if channel == 'transform.rotateOrder':
				thisValue = thisValue.upper()
			if channel == 'camera.fl':
				focalUnit = hou.parm(objectPath+"/focalunits").evalAsString()
				thisValue = float(thisValue)
				if focalUnit == 'm':
					thisValue = thisValue*1000 
				elif focalUnit == 'nm':
					thisValue = thisValue/1000000
				elif focalUnit == 'in':
					thisValue = thisValue*25.4
				elif focalUnit == 'ft':
					thisValue = thisValue*304.8
				thisValue = str(thisValue)
				
			writeOut += channel + '\t' + thisValue + '\n'
		
		if objectType == 'cam':
			resy = float(object.parm('resy').eval())
			resx = float(object.parm('resx').eval())
			apx = float(object.parm('aperture').eval())
			asp = float(object.parm('aspect').eval()) 
			verticalAperture = (resy*apx) / (resx*asp)		
			writeOut += 'camera.verticalFilmAperture\t' + str(verticalAperture) + '\n'
		
		writeOut += '+++++Animated+++++\n'
	
		if len(channalsAnimated) > 0:
			writeOut += 'Frame'
			for channel in channalsAnimated:
				writeOut += '\t' + channel
			writeOut += '\n'
				
			for frame in range(startF, (endF+1)):
				writeOut += str(frame)
				for channel in channalsAnimated:
					thisValue = hou.parm(objectPath+"/"+channels[channel]).evalAsStringAtFrame(frame)
					if channels[channel] in self.checkParentParameters :
						thisValue = str(self.getRealParmValue(object, channels[channel], frame))
					writeOut += '\t' + thisValue
				writeOut += '\n'
		
		f = open(exportFile, 'w') 
		f.write(writeOut)
		f.close()
		
	def delteCameraFiles(self):
		for camera in self.cameras:
			os.remove(self.exportPath + camera.name() + '.fm2n')