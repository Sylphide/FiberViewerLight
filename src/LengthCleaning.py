from __main__ import vtk, qt, ctk, slicer

#
#	LengthStat
#

class LengthStat:
	def __init__(self, parent):
		parent.title = "Length Cleaning"
		parent.categories = ["Diffusion"]
		parent.contributors = ["Jean-Baptiste Berger"]
		self.parent = parent
		
		
class LengthStatWidget:
	def __init__(self, parent=None):
		if not parent:
			self.parent = slicer. qMRMLWidget()
			self.parent.setLayout(qt.QVBoxLayout())
			self.parent.setMRMLScene(slicer.mrmlScene)
		else:
			self.parent = parent
		self.distanceTable = list()
		self.outputNode = None
		if not parent:
			self.setup()
			self.vtkSelector.setMRMLScene(slicer.mrmlScene)
			self.outputSelector.setMRMLScene(slicer.mrmlScene)
			self.parent.show()
			
			
	def setup(self):
		self.vtkSelectorFrame = qt.QFrame(self.parent)
		self.vtkSelectorFrame.setLayout(qt.QHBoxLayout())
		self.parent.layout().addWidget(self.vtkSelectorFrame)
		
		self.vtkSelectorLabel = qt.QLabel("Input Fiber Bundle: ", self.vtkSelectorFrame)
		self.vtkSelectorFrame.layout().addWidget(self.vtkSelectorLabel)
		
		self.vtkSelector = slicer.qMRMLNodeComboBox(self.vtkSelectorFrame)
		self.vtkSelector.nodeTypes = ("vtkMRMLFiberBundleNode","vtkMRMLFiberBundleNode")
		self.vtkSelector.selectNodeUponCreation = False
		self.vtkSelector.addEnabled = False
		self.vtkSelector.removeEnabled = False
		self.vtkSelector.noneEnabled = True
		self.vtkSelector.setMRMLScene(slicer.mrmlScene)
		self.vtkSelector.setToolTip("Select the Fiber Bundle to read")
		self.vtkSelectorFrame.layout().addWidget(self.vtkSelector)
		
		self.outputSelectorFrame = qt.QFrame(self.parent)
		self.outputSelectorFrame.setLayout(qt.QHBoxLayout())
		self.parent.layout().addWidget(self.outputSelectorFrame)
		
		self.outputSelectorLabel = qt.QLabel("Output Fiber Bundle: ", self.outputSelectorFrame)
		self.outputSelectorFrame.layout().addWidget(self.outputSelectorLabel)
		
		self.outputSelector = slicer.qMRMLNodeComboBox(self.outputSelectorFrame)
		self.outputSelector.nodeTypes = ("vtkMRMLFiberBundleNode","vtkMRMLFiberBundleNode")
		self.outputSelector.selectNodeUponCreation = False
		self.outputSelector.addEnabled = True
		self.outputSelector.removeEnabled = True
		self.outputSelector.noneEnabled = True
		self.outputSelector.setMRMLScene(slicer.mrmlScene)
		self.outputSelector.setToolTip("Select the Fiber Bundle to write on")
		self.outputSelectorFrame.layout().addWidget(self.outputSelector)
		
		self.thresholdFrame = qt.QFrame(self.parent)
		self.thresholdFrame.setLayout(qt.QHBoxLayout())
		self.parent.layout().addWidget(self.thresholdFrame)
		
		self.thresholdMinLabel = qt.QLabel("Min: ",self.thresholdFrame)
		self.thresholdFrame.layout().addWidget(self.thresholdMinLabel)
		
		self.thresholdMin = qt.QSpinBox(self.thresholdFrame)
		self.thresholdMin.setSingleStep(1)
		self.thresholdMin.setRange(0,1000)
		self.thresholdMin.setValue(0)
		self.thresholdMin.enabled = False
		self.thresholdFrame.layout().addWidget(self.thresholdMin)
		
		self.thresholdMaxLabel = qt.QLabel("Max: ",self.thresholdFrame)
		self.thresholdFrame.layout().addWidget(self.thresholdMaxLabel)
		
		self.thresholdMax = qt.QSpinBox(self.thresholdFrame)
		self.thresholdMax.setSingleStep(1)
		self.thresholdMax.setRange(0,1000)
		self.thresholdMax.setValue(1)
		self.thresholdMax.enabled = False
		self.thresholdFrame.layout().addWidget(self.thresholdMax)
		
		self.calculateLengthButton = qt.QPushButton("Calculate Length")
		self.calculateLengthButton.enabled = False
		self.parent.layout().addWidget(self.calculateLengthButton)
		
		self.applyThresholdButton = qt.QPushButton("Apply Threshold")
		self.applyThresholdButton.enabled = False
		self.parent.layout().addWidget(self.applyThresholdButton)
		
		self.parent.layout().addStretch(1)
		
		self.calculateLengthButton.connect('clicked()', self.onCalculateLength)
		self.applyThresholdButton.connect('clicked()',self.onApplyThreshold)
		self.vtkSelector.connect('nodeActivated(vtkMRMLNode*)',self.onVtkSelect)
		self.outputSelector.connect('nodeActivated(vtkMRMLNode*)',self.onOutputSelect)
		

	def onVtkSelect(self, node):
		self.vtkNode = node
		if node != None:
			self.calculateLengthButton.enabled = True
		else:
			self.calculateLengthButton.enabled = False
		
	def onOutputSelect(self, node):
		self.outputNode = node
		self.outputNode.CreateDefaultDisplayNodes()
		self.outputPolyData = vtk.vtkPolyData()
		
	def onCalculateLength(self):
		self.inputPolyData = self.vtkNode.GetPolyData()
		points = self.inputPolyData.GetPoints()
		lines = self.inputPolyData.GetLines()
		lines.InitTraversal()
		
		i = 0
		for i in range(self.inputPolyData.GetNumberOfCells()):
			fiberLength = 0
			ids = vtk.vtkIdList()
			lines.GetNextCell(ids)
			for j in range(ids.GetNumberOfIds()-1):
				point1 = [0,0,0]
				point2 = [0,0,0]
				points.GetPoint(ids.GetId(j), point1)
				points.GetPoint(ids.GetId(j+1), point2)
				x = point2[0] - point1[0]
				y = point2[1] - point1[1]
				z = point2[2] - point1[2]
				step = (x*x + y*y + z*z)**.5
				fiberLength += step
			self.distanceTable.append(fiberLength)
			i += 1
			
		min,max=self.getDistanceBound()
		
		self.thresholdMin.setValue(min)
		self.thresholdMin.enabled = True
		
		self.thresholdMax.setValue(max+1)
		self.thresholdMax.enabled = True
		
		self.applyThresholdButton.enabled = True
	
	
	def getDistanceBound(self):
		max = -1
		min = 100000
		for length in self.distanceTable:
			if length>max:
				max = length
			if length<min:
				min = length
		return min,max
	
	
	def onApplyThreshold(self):
		min,max = self.getDistanceBound()
		
		if self.outputNode != None:
			newPoints = vtk.vtkPoints()
			newLines = vtk.vtkCellArray()
			newTensors = vtk.vtkFloatArray()
			newTensors.SetNumberOfComponents(9)
			newScalars = vtk.vtkFloatArray()
			
			points = self.inputPolyData.GetPoints()
			lines = self.inputPolyData.GetLines()
			tensors = self.inputPolyData.GetPointData().GetTensors()
			lines.InitTraversal()
			
			i = 0
			newId = 0
			for length in self.distanceTable:
				if length<self.thresholdMax.value and length>self.thresholdMin.value:
					ids = vtk.vtkIdList()
					lines.GetNextCell(ids)
					newLine = vtk.vtkPolyLine()
					newLine.GetPointIds().SetNumberOfIds(ids.GetNumberOfIds())
					print(((length-min)/(max-min))*100)
					for i in range(ids.GetNumberOfIds()): 
						newPoints.InsertNextPoint(points.GetPoint(ids.GetId(i)))
						newLine.GetPointIds().SetId(i,newId)
						newScalars.InsertNextValue(((length-min)/(max-min))*100)
						newId += 1
						if(tensors != None):
							tensorValue = [0]*9
							for j in range(9):
								tensorValue[j] = tensors.GetComponent(ids.GetId(i),j)
							newTensors.InsertNextTuple(tensorValue)
					newLines.InsertNextCell(newLine)
					
			self.outputPolyData.SetPoints(newPoints)
			self.outputPolyData.SetLines(newLines)
			self.outputPolyData.GetPointData().SetTensors(newTensors)
			newScalars.SetName("Length")
			self.outputPolyData.GetPointData().AddArray(newScalars)
			self.outputNode.SetAndObservePolyData(self.outputPolyData)
			
		
		layoutNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLLayoutNode')
		layoutNodes.InitTraversal()
		layoutNode = layoutNodes.GetNextItemAsObject()
		layoutNode.SetViewArrangement(slicer.vtkMRMLLayoutNode.SlicerLayoutConventionalQuantitativeView)
		
		chartViewNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLChartViewNode')
		chartViewNodes.InitTraversal()
		chartViewNode = chartViewNodes.GetNextItemAsObject()
		
		arrayNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLDoubleArrayNode())
		array = arrayNode.GetArray()
		array.SetNumberOfTuples(10)
		
		step = (max-min)/10
		interMin = min
		interMax = min+step
		for i in range(10):
			numberOfFibers = 0
			for length in self.distanceTable:
				if length<interMax and length>interMin and length<self.thresholdMax.value and length>self.thresholdMin.value:
					numberOfFibers += 1
			array.SetComponent(i,0,(interMin+interMax)/2)
			array.SetComponent(i,1,numberOfFibers)
			array.SetComponent(i,2,0)
			interMin += step
			interMax += step
		
		chartNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLChartNode())
		chartNode.AddArray("Length",arrayNode.GetID())
		
		chartViewNode.SetChartNodeID(chartNode.GetID())
		
		chartNode.SetProperty('default', 'title', 'Length Distribution')
		chartNode.SetProperty('default', 'xAxisLabel', 'Length')
		chartNode.SetProperty('default', 'yAxisLabel', 'Distribution')
		chartNode.SetProperty('default', 'type', 'Bar')
		
	
		
		
		
