#include "FiberViewerLightGUI.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"


/********************************************************************************
 *Constructor: Initialize widgets and renderer and connect slots and signals
 ********************************************************************************/

FiberViewerLightGUI::FiberViewerLightGUI(QWidget* parent):QWidget(parent)
{
	this->setMouseTracking(true);
	
	InitWidgets();
	InitRenderer();

	connect(m_LE_VTKInput, SIGNAL(editingFinished()), this, SLOT(EnterVTKInput()));
	connect(m_TB_BrowserVTKInput, SIGNAL(clicked()), this, SLOT(BrowserVTKInput()));
	connect(m_PB_SaveVTK, SIGNAL(clicked()), this, SLOT(BrowserSaveVTK()));
	connect(m_PB_Test, SIGNAL(clicked()), this, SLOT(LengthComputation()));
	connect(m_SB_LowerTh, SIGNAL(editingFinished()), this, SLOT(ColorizeFiber()));
	connect(m_SB_UpperTh, SIGNAL(editingFinished()), this, SLOT(ColorizeFiber()));
}


/********************************************************************************
 *InitWidgets: Initialize all graphic widgets
 ********************************************************************************/

void FiberViewerLightGUI::InitWidgets()
{
	m_LE_VTKInput=new QLineEdit(this);
	m_LE_VTKInput->setFocusPolicy(Qt::StrongFocus);
	m_TB_BrowserVTKInput=new QToolButton(this);
	m_TB_BrowserVTKInput->setText("...");
	m_L_SelectFiber=new QLabel("Select fiber", this);
	m_L_SelectFiber->setFrameShadow(QFrame::Plain);
	m_VTKW_RenderWin=new QVTKWidget;
	m_VTKW_RenderWin->setMinimumSize(300,300);
	m_PB_SaveVTK=new QPushButton("Save VTK File",this);
	m_SB_LowerTh=new QSpinBox(this);
	m_SB_LowerTh->setSingleStep(1);
	m_SB_LowerTh->setRange(0,1000);
	m_SB_LowerTh->setEnabled(false);
	m_SB_UpperTh=new QSpinBox(this);
	m_SB_UpperTh->setSingleStep(1);
	m_SB_UpperTh->setRange(0,1000);
	m_SB_UpperTh->setEnabled(false);
	m_L_LowerTh=new QLabel("Lower Threshold", this);
	m_L_UpperTh=new QLabel("Upper Threshold", this);
	m_PB_Test=new QPushButton("Length Colorization",this);
	m_PolyData=vtkSmartPointer<vtkPolyData>::New();
	m_Fibers=vtkSmartPointer<vtkActorCollection>::New();
	
	QFrame* F_HLine=new QFrame;
	F_HLine->setFrameShadow(QFrame::Plain);
	F_HLine->setFrameShape(QFrame::HLine);
	QGroupBox* GB_ActionPanel=new QGroupBox("Fiber Viewer Light 1.0");
	QGroupBox* GB_LengthFilter=new QGroupBox("Filter");
	GB_LengthFilter->setAlignment(Qt::AlignHCenter);

	QHBoxLayout* HL_MainLayout=new QHBoxLayout;
	QVBoxLayout* VL_LeftPanel=new QVBoxLayout;
	QVBoxLayout* VL_ActionPanel=new QVBoxLayout;
	QHBoxLayout* HL_FiberLoad=new QHBoxLayout;
	QGridLayout* GL_LengthFilter=new QGridLayout;

	VL_ActionPanel->addWidget(m_L_SelectFiber);
	VL_ActionPanel->addWidget(F_HLine);
	HL_FiberLoad->addWidget(m_LE_VTKInput);
	HL_FiberLoad->addWidget(m_TB_BrowserVTKInput);
	VL_ActionPanel->addLayout(HL_FiberLoad);
	VL_ActionPanel->addWidget(m_PB_SaveVTK);
	GL_LengthFilter->addWidget(m_L_LowerTh, 0, 0);
	GL_LengthFilter->addWidget(m_SB_LowerTh, 1, 0);
	GL_LengthFilter->addWidget(m_L_UpperTh, 0, 1);
	GL_LengthFilter->addWidget(m_SB_UpperTh, 1, 1);
	GB_LengthFilter->setLayout(GL_LengthFilter);
	VL_ActionPanel->addWidget(GB_LengthFilter);
	VL_ActionPanel->addWidget(m_PB_Test);
	VL_ActionPanel->addStretch(1);
	GB_ActionPanel->setLayout(VL_ActionPanel);
	GB_ActionPanel->setFixedSize(300,300);
	VL_LeftPanel->addWidget(GB_ActionPanel);
	VL_LeftPanel->addStretch(1);
	HL_MainLayout->addLayout(VL_LeftPanel);
	HL_MainLayout->addWidget(m_VTKW_RenderWin);

	setLayout(HL_MainLayout);
}

/********************************************************************************
 *InitRenderer: Create a new renderer and delete previous ones
 ********************************************************************************/

void FiberViewerLightGUI::InitRenderer()
{
	vtkCamera *camera=vtkCamera::New();
	camera->SetPosition(1,1,1);
	camera->SetFocalPoint(0,0,0);
	
	//Delete the renderer if there is already one
	
	if(m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetNumberOfItems()>0)
	{
		vtkRenderer* renderer;
		renderer=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		m_VTKW_RenderWin->GetRenderWindow()->RemoveRenderer(renderer);
	}
	vtkRenderer* renderer=vtkRenderer::New();
	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	renderer->SetBackground(0.6,0.6,0.6);
	
	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

/********************************************************************************
 *BrowserVTKInput: Browser for input VTK file
 ********************************************************************************/

void FiberViewerLightGUI::BrowserVTKInput()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open VTK File", "/", "VTK (*.vtk)",&type);
	m_LE_VTKInput->setText(filename);
	EnterVTKInput();
	
	QApplication::restoreOverrideCursor();	
}

/********************************************************************************
 *ClearFibers: Remove all actors from m_Fibers
 ********************************************************************************/

void FiberViewerLightGUI::ClearFibers()
{
	m_Fibers->InitTraversal();
	while(m_Fibers->GetNumberOfItems()!=0)
		m_Fibers->RemoveItem(m_Fibers->GetLastActor());
}

/********************************************************************************
 *InitFibers: Create one actor per fiber and store them in the vtkActorCollection
 *	called m_Fibers
 ********************************************************************************/

void FiberViewerLightGUI::InitFibers()
{
	ClearFibers();
	for(int i=0; i<m_PolyData->GetNumberOfCells(); i++)
	{
		vtkSmartPointer<vtkPolyData> Fiber(vtkPolyData::New());
		Fiber=GetLineFromPolyData(i, m_PolyData);
		
		vtkPolyDataMapper* PolyDataMapper=vtkPolyDataMapper::New();
		PolyDataMapper->SetInput(Fiber);
			
		vtkActor* PolyDataActor=vtkActor::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		m_Fibers->AddItem(PolyDataActor);
			
		PolyDataActor->GetProperty()->SetColor(0.85,0,0);
	}
}

/********************************************************************************
 *EnterVTKInput: If the VTK file entered is correctly loaded, initialize m_Fibers
 *	the renderer and m_Length table.
 ********************************************************************************/

void FiberViewerLightGUI::EnterVTKInput()
{
	if(m_LE_VTKInput->text().toStdString()!="")
	{
		if(LoadVTK(m_LE_VTKInput->text().toStdString()))
		{
			InitFibers();
			InitRenderer();
			m_Length.clear();
			StartRenderer(m_PolyData);
		}
	}
}

/********************************************************************************
 *LoadVTK: Load a VTK file and fill m_PolyData.
 ********************************************************************************/

bool FiberViewerLightGUI::LoadVTK(std::string FileName)
{
	if(m_VtkFileName!=FileName)
	{
		std::cout<<"Reading VTK data..."<<std::endl;
		vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
		reader->SetFileName(FileName.c_str());
		m_VtkFileName=FileName;
		if(reader->IsFilePolyData())
		{
			reader->Update();
			m_PolyData=reader->GetOutput();
			std::cout<<"VTK File read successfuly."<<std::endl;
			return true;
		}
		else
			std::cout<<"Error reading VTK File. Check VTK Format."<<std::endl;
		return false;
	}
	else
		return false;
}

/********************************************************************************
 *StarRenderer: First render of a polydata. There is just one actor for the whole
 *	fiber
 ********************************************************************************/

void FiberViewerLightGUI::StartRenderer(vtkSmartPointer<vtkPolyData> PolyData)
{
	vtkRendererCollection* RendererList=vtkRendererCollection::New();
	RendererList=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers();
	int NumberOfRenderer=RendererList->GetNumberOfItems();
	if(NumberOfRenderer>0)
	{
		//Get actual renderer
		vtkRenderer* Renderer=vtkRenderer::New();
		Renderer=RendererList->GetFirstRenderer();
		
		LengthCalculation();
		InitLengthColorMap();
		//Set mapper's input
		vtkPolyDataMapper* PolyDataMapper=vtkPolyDataMapper::New();
		PolyDataMapper->SetInput(PolyData);
		PolyDataMapper->SetScalarRange(0, m_ColorMap->GetNumberOfTableValues()-1);
		PolyDataMapper->SetLookupTable(m_ColorMap);
		
		//Set actor's mapper
		vtkActor* PolyDataActor=vtkActor::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		
		//Add actor and set actor's color
		Renderer->AddActor(PolyDataActor);	
		PolyDataActor->GetProperty()->SetColor(0.85,0,0);
		Renderer->ResetCamera();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
}

/********************************************************************************
 *LengthCalculation: Fill m_Length table with length of each fiber
 ********************************************************************************/

void FiberViewerLightGUI::LengthCalculation()
{
	m_Length.clear();
	double FiberLength, Step;
	vtkCellArray* Lines=vtkCellArray::New();
	vtkPoints* Points=vtkPoints::New();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	Points=m_PolyData->GetPoints();
	Lines=m_PolyData->GetLines();
	Lines->InitTraversal();
	while(Lines->GetNextCell(NumberOfPoints, Ids))
	{
		FiberLength=0;
		for(unsigned int pointId=0; pointId+1< NumberOfPoints; pointId++)
		{
			double Point1[3], Point2[3], x, y, z;
			Points->GetPoint(Ids[pointId], Point1);
			Points->GetPoint(Ids[pointId+1], Point2);
			x=Point2[0]-Point1[0];
			y=Point2[1]-Point1[1];
			z=Point2[2]-Point1[2];
			Step=sqrt(x*x+y*y+z*z);
			FiberLength+=Step;
		}
		m_Length.push_back(FiberLength);
	}
}

/********************************************************************************
 *GetActorFromFibers: Return the actor corresponding to the id fiber
 ********************************************************************************/

vtkActor* FiberViewerLightGUI::GetActorFromFibers(int id)
{
	vtkActor* Actor=vtkActor::New();
	m_Fibers->InitTraversal();
	for(int i=0; i<=id; i++)
		Actor=m_Fibers->GetNextActor();
	return Actor;
}

/********************************************************************************
 *GetThresholdedIds: Return each index that corresponds to fiber which length
 *	is between threshold values
 ********************************************************************************/

std::vector<int> FiberViewerLightGUI::GetThresholdedIds(int LowerTh, int UpperTh)
{
	std::vector<int> Ids;
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]<UpperTh && m_Length[i]>LowerTh)
			Ids.push_back(i);
	}
	return Ids;
}

/********************************************************************************
 *LengthComputation: Calculate the length of each fiber and display it with
 *	color depending on its length
 ********************************************************************************/

void FiberViewerLightGUI::LengthComputation()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	//Erase previous renderer and initialize a new one
	InitRenderer();
	std::cout<<std::endl<<std::endl;
	vtkRendererCollection* RendererList=vtkRendererCollection::New();
	RendererList=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers();
	int NumberOfRenderer=RendererList->GetNumberOfItems();
	if(NumberOfRenderer>0 && m_PolyData!=NULL)
	{
		//Fill m_Length table
		LengthCalculation();
		
		//Set default threshold values
		double Min=GetMinLength(), Max=GetMaxLength();
		m_SB_UpperTh->setValue((int)ceil(Max));
		m_SB_LowerTh->setValue((int)floor(Min));
		m_SB_LowerTh->setEnabled(true);
		m_SB_UpperTh->setEnabled(true);
		
		//Get the actual renderer
		vtkRenderer* Renderer=vtkRenderer::New();
		Renderer=RendererList->GetFirstRenderer();
		
		//For each fiber
		for(int i=0; i<m_PolyData->GetNumberOfCells(); i++)
		{
			//Get the corresponding actor
			vtkActor* PolyDataActor=vtkActor::New();
			PolyDataActor=GetActorFromFibers(i);
			
			//Add and set color of the actor
// 			if(i==1)
// 			{
				Renderer->AddActor(PolyDataActor);
				double Color[3];
				std::cout<<m_Length[i]<<" ";
				GetFiberColor((m_Length[i]-Min)/(Max-Min), Color);
				PolyDataActor->GetProperty()->SetColor(Color[0], Color[1], Color[2]);
// 			}
		}
		Renderer->ResetCamera();
		m_VTKW_RenderWin->GetRenderWindow()->Render();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
	
	QApplication::restoreOverrideCursor();	
}

/********************************************************************************
 *IntIsIn: Look if x is in the List vector
 ********************************************************************************/

bool FiberViewerLightGUI::IntIsIn(int x, std::vector<int> List)
{
	for(unsigned int i=0; i<List.size(); i++)
	{
		if(x==List[i])
			return true;
	}
	return false;
}

/********************************************************************************
 *ColorizeFiber: Slot That will colorize each fibers depending on its length and
 *	threshold values
 ********************************************************************************/

void FiberViewerLightGUI::ColorizeFiber()
{
	vtkRenderer* Renderer=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	std::vector<int> ThresholdedIds=GetThresholdedIds(m_SB_LowerTh->value(), m_SB_UpperTh->value());
	double Min=GetMinLength(), Max=GetMaxLength();
	
	//For each fiber
	for(int i=0; i<m_Fibers->GetNumberOfItems(); i++)
	{
		//Get the actor corresponding to the fiber
		vtkActor* Actor=GetActorFromFibers(i);
		
		//Get and set the color depending on its length
		double Color[3];
		GetFiberColor((m_Length[i]-Min)/(Max-Min), Color);
		Actor->GetProperty()->SetColor(Color[0], Color[1], Color[2]);
		
		//If the length is between threshold values then add the actor to the renderer
		if(IntIsIn(i, ThresholdedIds))
		{
			Renderer->RemoveActor(Actor);		//To avoid multiple actor's definition
			Renderer->AddActor(Actor);
		}
		//else remove it from the renderer
		else
			Renderer->RemoveActor(Actor);
	}
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}
			

/********************************************************************************
 *GetMaxLength/GetMinLength: Get Max/Min from the m_Length table
 ********************************************************************************/

double FiberViewerLightGUI::GetMaxLength()
{
	double Max=-100000;
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]>Max)
			Max=m_Length[i];
	}
	return Max;
}

double FiberViewerLightGUI::GetMinLength()
{
	double Min=100000;
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]<Min)
			Min=m_Length[i];
	}
	return Min;
}

void FiberViewerLightGUI::InitLengthColorMap()
{
	std::cout<<"test"<<std::endl;
	double Min=GetMinLength(), Max=GetMaxLength();
	vtkLookupTable* ColorMap=vtkLookupTable::New();
// 	ColorMap->SetTableRange(GetMinLength(), GetMaxLength());
// 	ColorMap->SetRampToLinear();
// 	ColorMap->Build();
	ColorMap->SetNumberOfTableValues(m_PolyData->GetNumberOfCells());
	vtkPoints* Points=m_PolyData->GetPoints();
	vtkCellArray* Lines=m_PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	vtkFloatArray* Colors=vtkFloatArray::New();
// 	Colors->SetNumberOfComponents(4);
	Lines->InitTraversal();
	std::cout<<"test1"<<std::endl;
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		std::cout<<"test2"<<std::endl;
		double Color[4];
// 		ColorMap->GetColor(GetMaxLength()-(m_Length[i]-GetMinLength()), Color);
		std::cout<<m_Length[i]<<" ";
		GetFiberColor((m_Length[i]-Min)/(Max-Min),Color);
// 		if(i!=1)
// 			Color[3]=0;
// 		else
			Color[3]=1;
		std::cout<<"test3"<<std::endl;
		ColorMap->SetTableValue(i, Color);
// 		float FColor[4];
// 		std::cout<<m_Length[i]<<" ";
// 		for(int k=0; k<4; k++)
// 		{
// 			FColor[k]=static_cast<float>(Color[k]);
// 			std::cout<<Color[k]<<" ";
// 		}
		std::cout<<"test4"<<std::endl;
		std::cout<<std::endl;
		for(int j=0; j<NumberOfPoints; j++)
// 			Colors->InsertNextTupleValue(FColor);
			Colors->InsertNextValue(i);
	}
	std::cout<<"test0"<<std::endl;
	m_PolyData->GetPointData()->SetScalars(Colors);
	m_ColorMap=ColorMap;
}
			
	

/********************************************************************************
 *GetFiberColor: Color Mapping, coef is a value between 0 and 1 and color is 
 *	the RGB results also between 0 and 1.
 ********************************************************************************/

void FiberViewerLightGUI::GetFiberColor(double coef, double color[])
{
	coef=(1-coef)*897.0;
	
	if(coef<256)
	{
		color[0]=255; 
		color[1]=int(coef); 
		color[2]=0;
	}
	else if(coef>=256 && coef<512)
	{
		color[0]=255-int(coef)%255;
		color[1]=255;
		color[2]=0;
	}
	else if(coef>=512 && coef<641)
	{
		color[0]=0;
		color[1]=255;
		color[2]=int(coef)%255;
	}
	else if(coef>=641)
	{
		color[0]=int(coef)%255;
		color[1]=0;
		color[2]=255;
	}
	for(int i=0; i<3; i++)
	{
		color[i]/=255.0;
		std::cout<<color[i]<<" ";
	}
	std::cout<<std::endl;
}

// void FiberViewerLightGUI::SetFiberOpacity(vtkIdType NumberOfPoints, vtkIdType* Ids, double Alpha)
// {
// 	for(int i=0; i<NumberOfPoints; i++)
// 	{
// 		double Color[4];
// 		m_ColorMap->GetTableValue(Ids[i], Color);
// 		Color[3]=Alpha;
// 		m_ColorMap->SetTableValue(Ids[i], Color);
// 	}
// }
	

/********************************************************************************
 *GetLineFromPolyData: Create a new polydata containing only one line from 
 *	the whole polydata
 ********************************************************************************/

vtkSmartPointer<vtkPolyData> FiberViewerLightGUI::GetLineFromPolyData(int id, vtkSmartPointer<vtkPolyData> PolyData)
{
	vtkSmartPointer<vtkPolyData> Line(vtkPolyData::New());
	vtkSmartPointer<vtkPoints> Points(vtkPoints::New());
	vtkSmartPointer<vtkPolyLine> PolyLine(vtkPolyLine::New());
	vtkSmartPointer<vtkCellArray> Cells(vtkCellArray::New());
		
	vtkPoints* PointsFromPolyData=PolyData->GetPoints();
	vtkCellArray* CellsFromPolyData=PolyData->GetLines();
	vtkIdType* Indices;
	vtkIdType NumberOfPoints;
	
	//Find the cell corresponding to the id line
	CellsFromPolyData->InitTraversal();
	for(int i=0; i<=id; i++)
		CellsFromPolyData->GetNextCell(NumberOfPoints, Indices);
	
	//Set number of points for this line in the new polyline
	PolyLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
	
	//Copy points from the polydata to the polyline
	for(int i=0; i<NumberOfPoints; i++)
	{
		double point[3];
		PointsFromPolyData->GetPoint(Indices[i], point);
		Points->InsertNextPoint(point);
		PolyLine->GetPointIds()->SetId(i,i);
	}
	
	//Insert arrays in the data.
	Cells->InsertNextCell(PolyLine);
	Line->SetPoints(Points);
	Line->SetLines(Cells);
	return Line;
}
		
		
/********************************************************************************
 *BrowserSaveVTK: Option to save the actual VTK
 ********************************************************************************/

void FiberViewerLightGUI::BrowserSaveVTK()
{
	QString file = QFileDialog::getSaveFileName(this, "Save VTK file", "/", "VTK (*.vtk)");
	
	if(file!=NULL)
		std::cout<<"Fonction Save VTK"<<std::endl;
// 		SaveVTK();
	else
		std::cout<<"ERROR: File name null!"<<std::endl;
}
