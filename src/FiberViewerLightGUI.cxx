#include "FiberViewerLightGUI.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"


FiberViewerLightGUI::FiberViewerLightGUI(QWidget* parent):QWidget(parent)
{
	InitWidgets();
	InitRenderer();
	
	connect(m_LE_VTKInput, SIGNAL(editingFinished()), this, SLOT(EnterVTKInput()));
	connect(m_TB_BrowserVTKInput, SIGNAL(clicked()), this, SLOT(BrowserVTKInput()));
}

void FiberViewerLightGUI::InitWidgets()
{
	m_LE_VTKInput=new QLineEdit(this);
	m_TB_BrowserVTKInput=new QToolButton(this);
	m_TB_BrowserVTKInput->setText("...");
	m_L_SelectFiber=new QLabel("Select fiber", this);
	m_L_SelectFiber->setFrameShadow(QFrame::Plain);
	m_VTKW_RenderWin=new QVTKWidget;
	m_VTKW_RenderWin->setMinimumSize(300,300);
	vtkSmartPointer<vtkPolyData> m_PolyData(vtkPolyData::New());
	
	QFrame* F_HLine=new QFrame;
	F_HLine->setFrameShadow(QFrame::Plain);
	F_HLine->setFrameShape(QFrame::HLine);
	QGroupBox* GB_ActionPanel=new QGroupBox("Fiber Viewer Light 1.0");
	
	QHBoxLayout* HL_MainLayout=new QHBoxLayout;
	QVBoxLayout* VL_LeftPanel=new QVBoxLayout;
	QVBoxLayout* VL_ActionPanel=new QVBoxLayout;
	QHBoxLayout* HL_FiberLoad=new QHBoxLayout;
	
	VL_ActionPanel->addWidget(m_L_SelectFiber);
	VL_ActionPanel->addWidget(F_HLine);
	HL_FiberLoad->addWidget(m_LE_VTKInput);
	HL_FiberLoad->addWidget(m_TB_BrowserVTKInput);
	VL_ActionPanel->addLayout(HL_FiberLoad);
	VL_ActionPanel->addStretch(1);
	GB_ActionPanel->setLayout(VL_ActionPanel);
	GB_ActionPanel->setFixedSize(300,300);
	VL_LeftPanel->addWidget(GB_ActionPanel);
	VL_LeftPanel->addStretch(1);
	HL_MainLayout->addLayout(VL_LeftPanel);
	HL_MainLayout->addWidget(m_VTKW_RenderWin);
	
	setLayout(HL_MainLayout);
// 	QVTKWidget* render=new QVTKWidget;
// 	QVBoxLayout* Vlayout=new QVBoxLayout;
// 	QPushButton* test=new QPushButton("test");
// 	Vlayout->addWidget(m_VTKW_RenderWin);
// 	Vlayout->addWidget(test);
// 	
// // 	vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
// // 	cubeMapper->SetInput(m_PolyData);
// // 	cubeMapper->SetScalarRange(0,7);
// // 	vtkActor *cubeActor = vtkActor::New();
// // 	cubeActor->SetMapper(cubeMapper);
// // 	vtkCamera *camera = vtkCamera::New();
// // 	camera->SetPosition(1,1,1);
// // 	camera->SetFocalPoint(0,0,0);
// // 	
// // 	vtkRenderer *renderer = vtkRenderer::New();
// // // 	renderer->AddActor(cubeActor);
// // 	renderer->SetActiveCamera(camera);
// // 	camera->Delete();
// // 	cubeMapper->Delete();
// // 	renderer->ResetCamera();
// // 	renderer->SetBackground(0.5,0.5,0.5);
// // 	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
// // 	m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(cubeActor);
// // 	m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
// // // 	renderer->AddActor(cubeActor);
// // // 	renderer->ResetCamera();
// // // 	m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->RemoveItem(0);
// // // 	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
// // 	std::cout<<m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->VisibleActorCount()<<std::endl;
// // 	renderer->Delete();
// 	
// 	setLayout(Vlayout);
}

void FiberViewerLightGUI::InitRenderer()
{
	vtkCamera *camera=vtkCamera::New();
	camera->SetPosition(1,1,1);
	camera->SetFocalPoint(0,0,0);
	
	vtkRenderer* renderer=vtkRenderer::New();
	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	renderer->SetBackground(0.6,0.6,0.6);
	
	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
	Render(m_PolyData);
}

void FiberViewerLightGUI::BrowserVTKInput()
{
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open VTK File", "/", "VTK (*.vtk)",&type);
	m_LE_VTKInput->setText(filename);
	EnterVTKInput();
}

void FiberViewerLightGUI::EnterVTKInput()
{
	if(LoadVTK(m_LE_VTKInput->text().toStdString()))
		Render(m_PolyData);
	else
		std::cout<<"Nothing to display."<<std::endl;
}

bool FiberViewerLightGUI::LoadVTK(std::string FileName)
{
	std::cout<<"Reading VTK data..."<<std::endl;
	vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
	reader->SetFileName(FileName.c_str());
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

void FiberViewerLightGUI::Render(vtkSmartPointer<vtkPolyData> PolyData)
{
	vtkRendererCollection* RendererList=vtkRendererCollection::New();
	RendererList=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers();
	int NumberOfRenderer=RendererList->GetNumberOfItems();
	if(NumberOfRenderer>0)
	{
		vtkPolyDataMapper* PolyDataMapper=vtkPolyDataMapper::New();
		PolyDataMapper->SetInput(PolyData);
	
		vtkActor* PolyDataActor=vtkActor::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		
		vtkRenderer* Renderer=vtkRenderer::New();
		Renderer=RendererList->GetFirstRenderer();
		Renderer->AddActor(PolyDataActor);
		Renderer->ResetCamera();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
}