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

	connect(m_LE_VTKInput, SIGNAL(editingFinished()), this, SLOT(EnterVTKInput()));
	connect(m_TB_BrowserVTKInput, SIGNAL(clicked()), this, SLOT(BrowserVTKInput()));
	connect(m_PB_SaveVTK, SIGNAL(clicked()), this, SLOT(BrowserSaveVTK()));
	connect(m_PB_Length, SIGNAL(clicked()), this, SLOT(OpenLengthPanel()));
	connect(m_LengthGUI, SIGNAL(PreviousClicked()), this, SLOT(CloseLengthPanel()));
}


/********************************************************************************
 *InitWidgets: Initialize all graphic widgets
 ********************************************************************************/

void FiberViewerLightGUI::InitWidgets()
{
	m_GB_ActionPanel=new QGroupBox("Fiber Viewer Light 1.0");
	m_GB_ActionPanel->setMinimumSize(300,450);
	m_GB_ActionPanel->setMaximumSize(300,450);
	m_LE_VTKInput=new QLineEdit(this);
	m_LE_VTKInput->setFocusPolicy(Qt::StrongFocus);
	m_TB_BrowserVTKInput=new QToolButton(this);
	m_TB_BrowserVTKInput->setText("...");
	m_L_SelectFiber=new QLabel("Select fiber", this);
	m_L_SelectFiber->setFrameShadow(QFrame::Plain);
	m_PB_SaveVTK=new QPushButton("Save VTK File",this);
	m_PB_Length=new QPushButton("Length",this);
	
	m_Display=new FiberDisplay(this);
	m_LengthGUI=new FVLengthGUI(this, m_Display);
	m_LengthGUI->hide();
	
	QFrame* F_HLine=new QFrame;
	F_HLine->setFrameShadow(QFrame::Plain);
	F_HLine->setFrameShape(QFrame::HLine);
	
	QGroupBox* GB_LengthFilter=new QGroupBox("Filter");

	QHBoxLayout* HL_MainLayout=new QHBoxLayout;
	QVBoxLayout* VL_MainPanel=new QVBoxLayout;
	QVBoxLayout* VL_LengthPanel=new QVBoxLayout;
	QVBoxLayout* VL_ActionPanel=new QVBoxLayout;
	QHBoxLayout* HL_FiberLoad=new QHBoxLayout;
	
	
	//Layout settings
	VL_ActionPanel->addWidget(m_L_SelectFiber);
	VL_ActionPanel->addWidget(F_HLine);
	HL_FiberLoad->addWidget(m_LE_VTKInput);
	HL_FiberLoad->addWidget(m_TB_BrowserVTKInput);
	VL_ActionPanel->addLayout(HL_FiberLoad);
	VL_ActionPanel->addWidget(m_PB_SaveVTK);
	VL_ActionPanel->addWidget(m_PB_Length);
	VL_ActionPanel->addStretch(1);
	m_GB_ActionPanel->setLayout(VL_ActionPanel);
	m_GB_ActionPanel->setFixedSize(300,450);
	VL_MainPanel->addWidget(m_GB_ActionPanel);
	VL_MainPanel->addStretch(1);
	HL_MainLayout->addLayout(VL_MainPanel);
	VL_LengthPanel->addWidget(m_LengthGUI);
	VL_LengthPanel->addStretch(1);
	HL_MainLayout->addLayout(VL_LengthPanel);
	HL_MainLayout->addWidget(m_Display);

	setLayout(HL_MainLayout);
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
 *EnterVTKInput: If the VTK file entered is correctly loaded, initialize m_Fibers
 *	the renderer and m_Length table.
 ********************************************************************************/

void FiberViewerLightGUI::EnterVTKInput()
{
	if(m_LE_VTKInput->text().toStdString()!="")
	{
		vtkSmartPointer<vtkPolyData> PolyData;
		PolyData=LoadVTK(m_LE_VTKInput->text().toStdString());
		if(PolyData!=NULL)
			m_Display->StartRenderer(PolyData);
	}
}

/********************************************************************************
 *LoadVTK: Load a VTK file and fill m_PolyData.
 ********************************************************************************/

vtkSmartPointer<vtkPolyData> FiberViewerLightGUI::LoadVTK(std::string FileName)
{
	if(m_VtkFileName!=FileName)
	{
		std::cout<<"Reading VTK data..."<<std::endl;
		vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
		reader->SetFileName(FileName.c_str());
		m_VtkFileName=FileName;
		if(reader->IsFilePolyData())
		{
			vtkSmartPointer<vtkPolyData> PolyData;
			reader->Update();
			PolyData=reader->GetOutput();
			std::cout<<"VTK File read successfuly."<<std::endl;
			return PolyData;
		}
		else
			std::cout<<"Error reading VTK File. Check VTK Format."<<std::endl;
		return NULL;
	}
	else
		return NULL;
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

/********************************************************************************
 *OpenLengthPanel: Option to save the actual VTK
 ********************************************************************************/

void FiberViewerLightGUI::OpenLengthPanel()
{
	m_LengthGUI->SetDisplay(m_Display);
	m_GB_ActionPanel->hide();
	m_LengthGUI->show();
	
}

void FiberViewerLightGUI::CloseLengthPanel()
{
	m_LengthGUI->hide();
	m_GB_ActionPanel->show();
}
