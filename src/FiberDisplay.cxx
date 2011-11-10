#include "FiberDisplay.h"
#include "FiberViewerLightGUI.h"

FiberDisplay::FiberDisplay(QWidget* parent) : QWidget(parent)
{
	m_OriginalPolyData=vtkSmartPointer<vtkPolyData>::New();
	m_ModifiedPolyData=vtkSmartPointer<vtkPolyData>::New();
	iren=QVTKInteractor::New();
	m_Plane=vtkImplicitPlaneWidget::New();
	
	m_VTKW_RenderWin=new QVTKWidget;
	m_VTKW_RenderWin->setMinimumSize(300,527);
	
	QGridLayout* MainLayout=new QGridLayout;
	MainLayout->addWidget(m_VTKW_RenderWin);
	setLayout(MainLayout);
	
	InitRenderer();
}

/********************************************************************************
 *InitRenderer: Create a new renderer and delete previous ones
 ********************************************************************************/

void FiberDisplay::InitRenderer()
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
	
	//Create a new renderer
	vtkRenderer* renderer=vtkRenderer::New();
	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	renderer->SetBackground(0.6,0.6,0.6);
	
	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
	iren->SetRenderWindow(m_VTKW_RenderWin->GetRenderWindow());
	vtkInteractorStyleSwitch* style=vtkInteractorStyleSwitch::New();
	style->SetCurrentStyleToTrackballCamera();
	iren->SetInteractorStyle(style);
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

void FiberDisplay::InitAlphas()
{
	m_PreviousAlphas.push_back(std::vector<int>());
	for(int i=0; i<m_OriginalPolyData->GetNumberOfCells(); i++)
		m_PreviousAlphas[0].push_back(1);
}

bool FiberDisplay::IsUnchanged()
{
	if(m_PreviousAlphas.size()>1)
	{
		for(int i=0; i<m_OriginalPolyData->GetNumberOfCells(); i++)
		{
			if(m_PreviousAlphas[m_PreviousAlphas.size()-1][i]!=m_PreviousAlphas[m_PreviousAlphas.size()-2][i])
				return false;
		}
	}
	PopBackAlpha(FiberDisplay::Previous);
	return true;
}

vtkPolyData* FiberDisplay::GetOriginalPolyData()
{
	return m_OriginalPolyData;
}

vtkPolyData* FiberDisplay::GetModifiedPolyData()
{
	return m_ModifiedPolyData;
}

vtkRenderer* FiberDisplay::GetRenderer()
{
		return m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
}

vtkActor* FiberDisplay::GetActor()
{
	vtkActorCollection* Collection=GetRenderer()->GetActors();
	Collection->InitTraversal();
	return Collection->GetNextActor();
}

std::vector<int> FiberDisplay::GetLastAlpha(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		return m_NextAlphas[m_NextAlphas.size()-1];
	else if(Type==FiberDisplay::Previous)
		return m_PreviousAlphas[m_PreviousAlphas.size()-1];
	else
		return std::vector<int>();
}

int FiberDisplay::GetAlphasSize(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		return m_NextAlphas.size();
	else if(Type==FiberDisplay::Previous)
		return m_PreviousAlphas.size();
	else
		return 0;
}
/********************************************************************************
 *GetFiberColor: Color Mapping, coef is a value between 0 and 1 and color is 
 *	the RGB results also between 0 and 1. Doing a linear interpolation between
 *	purple and red.
 ********************************************************************************/

void FiberDisplay::GetFiberColor(double coef, double color[])
{
	coef=(1-coef)*1020.0;
	double contrast=0.9;
	
	if(coef<256)
	{
		color[0]=(int)(255*contrast); 
		color[1]=(int)(int(coef)*contrast); 
		color[2]=0;
	}
	else if(coef>=256 && coef<510)
	{
		color[0]=(int)((255-int(coef)%255)*contrast);
		color[1]=(int)(255*contrast);
		color[2]=0;
	}
	else if(coef>=510 && coef<764)
	{
		color[0]=0;
		color[1]=(int)(255*contrast);
		color[2]=(int)((int(coef)%255+1)*contrast);
	}
	else if(coef>=764 && coef<1021)
	{
		color[0]=0;
		color[1]=(int)((255-int(coef)%255)*contrast);
		color[2]=(int)(255*contrast);
	}
	for(int i=0; i<3; i++)
		color[i]/=255.0;
}

void FiberDisplay::SetOriginalPolyData(vtkPolyData* PolyData)
{
	m_OriginalPolyData=PolyData;
}

void FiberDisplay::SetModifiedPolyData(vtkPolyData* PolyData)
{
	m_ModifiedPolyData=PolyData;
}

	//Set Mapper's LUT Color Map
void FiberDisplay::SetLookupTable(vtkLookupTable* Map)
{
	GetActor()->GetMapper()->SetLookupTable(Map);
}

void FiberDisplay::SetLastAlpha(std::vector<int> Alpha, AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas[m_NextAlphas.size()-1]=Alpha;
	else if(Type==FiberDisplay::Previous)
		m_PreviousAlphas[m_PreviousAlphas.size()-1]=Alpha;	
}


void FiberDisplay::PushBackAlpha(std::vector<int> Alpha, AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.push_back(Alpha);
	else if(Type==FiberDisplay::Previous)
		m_PreviousAlphas.push_back(Alpha);
}

void FiberDisplay::PopBackAlpha(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.pop_back();
	else if(m_PreviousAlphas.size()>1 && Type==FiberDisplay::Previous)
		m_PreviousAlphas.pop_back();
}

void FiberDisplay::ClearAlphas(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.clear();
	else if(Type==FiberDisplay::Previous)
		m_PreviousAlphas.clear();
}

/********************************************************************************
 *StarRenderer: First render of a polydata. There is just one actor for the whole
 *	fiber
 ********************************************************************************/

void FiberDisplay::InitPlaneCoord(double Bounds[])
{
	vtkPoints* Points=m_ModifiedPolyData->GetPoints();
	double MinY=10000, MaxY=-10000, MinZ=10000, MaxZ=-10000, MinX=10000, MaxX=-10000;
	for(int i=0; i<m_ModifiedPolyData->GetNumberOfPoints(); i++)
	{
		double Point[3];
		Points->GetPoint(i,Point);
		if(Point[0]<MinX)
			MinX=Point[0];
		if(Point[0]>MaxX)
			MaxX=Point[0];
		if(Point[1]<MinY)
			MinY=Point[1];
		if(Point[1]>MaxY)
			MaxY=Point[1];
		if(Point[2]<MinZ)
			MinZ=Point[2];
		if(Point[2]>MaxZ)
			MaxZ=Point[2];
	}
	Bounds[0]=MinX;
	Bounds[1]=MaxX;
	Bounds[2]=MinY;
	Bounds[3]=MaxY;
	Bounds[4]=MinZ;
	Bounds[5]=MaxZ;
}
		

void FiberDisplay::StartRenderer(vtkPolyData* PolyData)
{
	//Copy PolyData in Original in case of step back
	m_OriginalPolyData->DeepCopy(PolyData);
	m_ModifiedPolyData=PolyData;
	InitAlphas();
	
	//Get actual renderer
	vtkRenderer* Renderer=vtkRenderer::New();
	Renderer=GetRenderer();
	if(Renderer!=NULL)
	{
		double Bounds[6];
		InitPlaneCoord(Bounds);
		InitPlan(Bounds);
		
		//Set mapper's input
		vtkPolyDataMapper* PolyDataMapper=vtkPolyDataMapper::New();
		PolyDataMapper->SetInput(m_ModifiedPolyData);
		
		//Set actor's mapper
		vtkActor* PolyDataActor=vtkActor::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		
		//Add actor and set actor's color
		Renderer->AddActor(PolyDataActor);	
		Renderer->ResetCamera();
		Render();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
}

void FiberDisplay::Render()
{
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

void FiberDisplay::UpdateCells()
{
	vtkSmartPointer<vtkPolyData> ModifiedPolyData=vtkSmartPointer<vtkPolyData>::New();
	vtkFloatArray* NewScalars=vtkFloatArray::New();
	vtkFloatArray* NewTensors=vtkFloatArray::New();
	NewTensors->SetNumberOfComponents(9);
	vtkDataArray* Scalars=m_OriginalPolyData->GetPointData()->GetScalars();
	vtkDataArray* Tensors=m_OriginalPolyData->GetPointData()->GetTensors();
	vtkPoints* NewPoints=vtkPoints::New();
	vtkCellArray* NewLines=vtkCellArray::New();
	vtkPoints* Points=m_OriginalPolyData->GetPoints();
	vtkCellArray* Lines=m_OriginalPolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	int NewId=0;
	Lines->InitTraversal();
	
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		if(m_PreviousAlphas[m_PreviousAlphas.size()-1][i]==1)
		{
			vtkPolyLine* NewLine=vtkPolyLine::New();
			NewLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
			for(int j=0; j<NumberOfPoints; j++)
			{
				NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
				NewLine->GetPointIds()->SetId(j,NewId);
				NewId++;
				NewScalars->InsertNextValue(Scalars->GetComponent(0,Ids[j]));
				double tensorValue[9];
				for(int k=0; k<9; k++)
					tensorValue[k]=Tensors->GetComponent(Ids[j],k);
				NewTensors->InsertNextTuple(tensorValue);
			}
			NewLines->InsertNextCell(NewLine);
		}
	}
	ModifiedPolyData->SetPoints(NewPoints);
	ModifiedPolyData->GetPointData()->SetTensors(NewTensors);
	ModifiedPolyData->SetLines(NewLines);
	ModifiedPolyData->GetPointData()->SetScalars(NewScalars);
	m_ModifiedPolyData->DeepCopy(ModifiedPolyData);
	Render();
}

vtkImplicitPlaneWidget* FiberDisplay::GetPlan()
{
	return m_Plane;
}

void FiberDisplay::InitPlan(double Bounds[])
{
	m_Plane->SetPlaceFactor(1);
	m_Plane->PlaceWidget(Bounds);
	m_Plane->SetInteractor(iren);
	m_Plane->SetOrigin((Bounds[0]+Bounds[1])/2,(Bounds[2]+Bounds[3])/2,(Bounds[4]+Bounds[5])/2);
	m_Plane->SetNormal(1,0,0);
	m_Plane->UpdatePlacement();
	m_Plane->Off();
}
	
