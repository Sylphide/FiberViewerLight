#include "FiberDisplay.h"

FiberDisplay::FiberDisplay(QWidget* parent) : QWidget(parent)
{
	m_OriginalPolyData=vtkSmartPointer<vtkPolyData>::New();
	m_ModifiedPolyData=vtkSmartPointer<vtkPolyData>::New();
	m_ColorMap=vtkSmartPointer<vtkLookupTable>::New();
	m_VTKW_RenderWin=new QVTKWidget;
	m_VTKW_RenderWin->setMinimumSize(300,300);
	
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
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

vtkRenderer* FiberDisplay::GetRenderer()
{
		return m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
}

vtkPolyData* FiberDisplay::GetOriginalPolyData()
{
	return m_OriginalPolyData;
}

void FiberDisplay::SetOriginalPolyData(vtkPolyData* PolyData)
{
	m_OriginalPolyData=PolyData;
}

void FiberDisplay::SetModifiedPolyData(vtkPolyData* PolyData)
{
	m_ModifiedPolyData=PolyData;
}

vtkPolyData* FiberDisplay::GetModifiedPolyData()
{
	return m_ModifiedPolyData;
}

vtkActor* FiberDisplay::GetActor()
{
	return GetRenderer()->GetActors()->GetLastActor();
}

void FiberDisplay::Render()
{
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

/********************************************************************************
 *StarRenderer: First render of a polydata. There is just one actor for the whole
 *	fiber
 ********************************************************************************/

void FiberDisplay::StartRenderer(vtkPolyData* PolyData)
{
	//Copy PolyData in Original in case of step back
	m_OriginalPolyData->DeepCopy(PolyData);
	m_ModifiedPolyData=PolyData;

	//Get actual renderer
	vtkRenderer* Renderer=vtkRenderer::New();
	Renderer=GetRenderer();
	if(Renderer!=NULL)
	{
		//Set mapper's input
		vtkPolyDataMapper* PolyDataMapper=vtkPolyDataMapper::New();
		PolyDataMapper->SetInput(m_ModifiedPolyData);
		
		//Set actor's mapper
		vtkActor* PolyDataActor=vtkActor::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		
		//Add actor and set actor's color
		Renderer->AddActor(PolyDataActor);	
		PolyDataActor->GetProperty()->SetColor(0.85,0,0);
		Renderer->ResetCamera();
		Render();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
}

/********************************************************************************
 *GetFiberColor: Color Mapping, coef is a value between 0 and 1 and color is 
 *	the RGB results also between 0 and 1. Doing a linear interpolation between
 *	purple and red.
 ********************************************************************************/

void FiberDisplay::GetFiberColor(double coef, double color[])
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
		color[i]/=255.0;
}
