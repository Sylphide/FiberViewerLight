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
	connect(m_TB_BrowserVTKOutput, SIGNAL(clicked()), this, SLOT(BrowserVTKOutput()));
	connect(m_PB_Length, SIGNAL(clicked()), this, SLOT(OpenLengthPanel()));
	connect(m_PB_Gravity, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
	connect(m_PB_Hausdorff, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
	connect(m_PB_Mean, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
	connect(m_PB_NormCut, SIGNAL(clicked()), this, SLOT(OpenNormCutPanel()));
	connect(m_LengthGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseLengthPanel(FVPanelGUI::ExitSignal)));
	connect(m_DistributionGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseDistributionPanel(FVPanelGUI::ExitSignal)));
	connect(m_DisplayClassGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseDisplayClass(FVPanelGUI::ExitSignal)));
	connect(m_NormCutGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseNormCut(FVPanelGUI::ExitSignal)));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Redo, SIGNAL(clicked()), this, SLOT(RedoAction()));
	connect(m_PB_SaveVTK, SIGNAL(clicked()), this, SLOT(SaveVTK()));
	connect(m_PB_Plane, SIGNAL(clicked()), this, SLOT(OpenPlanSetting()));
	connect(m_DistributionGUI, SIGNAL(Progress(int)), m_ProgressBar, SLOT(setValue(int)));
}


/********************************************************************************
 *InitWidgets: Initialize all graphic widgets
 ********************************************************************************/

void FiberViewerLightGUI::InitWidgets()
{
	
	m_GB_ActionPanel=new QGroupBox("Fiber Viewer Light 1.0");
	m_GB_ActionPanel->setMinimumSize(300,450);
	m_GB_ActionPanel->setMaximumSize(300,450);
	
	m_GB_LengthPanel=new QGroupBox("Length");
	m_GB_LengthPanel->setMinimumSize(300,430);
	m_GB_LengthPanel->setMaximumSize(300,430);
	m_GB_LengthPanel->hide();
	m_GB_DistributionPanel=new QGroupBox("Distribution");
	m_GB_DistributionPanel->setMinimumSize(300,430);
	m_GB_DistributionPanel->setMaximumSize(300,430);
	m_GB_DistributionPanel->hide();
	m_GB_DisplayClassPanel=new QGroupBox("Display Class");
	m_GB_DisplayClassPanel->setMinimumSize(300,430);
	m_GB_DisplayClassPanel->setMaximumSize(300,430);
	m_GB_DisplayClassPanel->hide();
	m_GB_NormCutPanel=new QGroupBox("Normalized Cut");
	m_GB_NormCutPanel->setMinimumSize(300,430);
	m_GB_NormCutPanel->setMaximumSize(300,430);
	m_GB_NormCutPanel->hide();
	
	m_Display=new FiberDisplay(this);
	m_LengthGUI=new FVLengthGUI(this, m_Display);
	m_DistributionGUI=new FVDistributionGUI(this, m_Display);
	m_DisplayClassGUI=new FVDisplayClassGUI(this,m_Display);
	m_NormCutGUI=new FVNormCutGUI(this,m_Display);
	m_PlanSetting=new PlanSetting(this, m_Display);
	m_PlanSetting->hide();
	
	m_ProgressBar=new QProgressBar;
	m_ProgressBar->setValue(0);
	m_LE_VTKInput=new QLineEdit(this);
	m_LE_VTKOutput=new QLineEdit(this);
	m_TB_BrowserVTKInput=new QToolButton(this);
	m_TB_BrowserVTKInput->setText("...");
	m_TB_BrowserVTKOutput=new QToolButton(this);
	m_TB_BrowserVTKOutput->setText("...");
	m_L_VTKInput=new QLabel("VTK Input", this);
	m_L_VTKOutput=new QLabel("VTK Output", this);
	m_L_IO=new QLabel("Select input/output", this);
	m_L_IO->setFrameShadow(QFrame::Plain);
	m_L_Processing=new QLabel("Processing", this);
	m_L_Processing->setFrameShadow(QFrame::Plain);
	m_L_Navigation=new QLabel("Navigation", this);
	m_L_Navigation->setFrameShadow(QFrame::Plain);
	m_PB_Length=new QPushButton("Length",this);
	m_PB_Gravity=new QPushButton("Gravity",this);
	m_PB_Hausdorff=new QPushButton("Hausdorff",this);
	m_PB_Mean=new QPushButton("Mean",this);
	m_PB_NormCut=new QPushButton("Normalized Cut",this);
	m_PB_Undo=new QPushButton("Undo",this);
	m_PB_Undo->setEnabled(false);
	m_PB_Redo=new QPushButton("Redo",this);
	m_PB_Redo->setEnabled(false);
	m_PB_SaveVTK=new QPushButton("Save VTK File",this);
	m_PB_SaveVTK->setEnabled(false);
	m_PB_Plane=new QPushButton("Plane Option", this);
	m_PB_Plane->setEnabled(false);
	
	QFrame* F_HLine1=new QFrame;
	F_HLine1->setFrameShadow(QFrame::Plain);
	F_HLine1->setFrameShape(QFrame::HLine);
	QFrame* F_HLine2=new QFrame;
	F_HLine2->setFrameShadow(QFrame::Plain);
	F_HLine2->setFrameShape(QFrame::HLine);
	QFrame* F_HLine3=new QFrame;
	F_HLine3->setFrameShadow(QFrame::Plain);
	F_HLine3->setFrameShape(QFrame::HLine);
	
	QGridLayout* GL_MainLayout=new QGridLayout;
	QVBoxLayout* VL_ActionPanel=new QVBoxLayout;
	QGridLayout* GL_FiberIO=new QGridLayout;
	QHBoxLayout* HL_Navigation=new QHBoxLayout;
	
	//Layout settings
	VL_ActionPanel->addWidget(m_L_IO);
	VL_ActionPanel->addWidget(F_HLine1);
	GL_FiberIO->addWidget(m_L_VTKInput,0,0);
	GL_FiberIO->addWidget(m_LE_VTKInput,0,1);
	GL_FiberIO->addWidget(m_TB_BrowserVTKInput,0,2);
	GL_FiberIO->addWidget(m_L_VTKOutput,1,0);
	GL_FiberIO->addWidget(m_LE_VTKOutput,1,1);
	GL_FiberIO->addWidget(m_TB_BrowserVTKOutput,1,2);
	VL_ActionPanel->addLayout(GL_FiberIO);
	VL_ActionPanel->addWidget(m_L_Processing);
	VL_ActionPanel->addWidget(F_HLine2);
	VL_ActionPanel->addWidget(m_PB_Length);
	VL_ActionPanel->addWidget(m_PB_Gravity);
	VL_ActionPanel->addWidget(m_PB_Hausdorff);
	VL_ActionPanel->addWidget(m_PB_Mean);
	VL_ActionPanel->addWidget(m_PB_NormCut);
	VL_ActionPanel->addWidget(m_L_Navigation);
	VL_ActionPanel->addWidget(F_HLine3);
	HL_Navigation->addWidget(m_PB_Undo);
	HL_Navigation->addWidget(m_PB_Redo);
	VL_ActionPanel->addLayout(HL_Navigation);
	VL_ActionPanel->addWidget(m_PB_SaveVTK);
	VL_ActionPanel->addWidget(m_ProgressBar);
	VL_ActionPanel->addStretch(1);
	
	m_GB_ActionPanel->setLayout(VL_ActionPanel);
	GL_MainLayout->addWidget(m_GB_ActionPanel,0,0,Qt::AlignTop);

	m_GB_LengthPanel->setLayout(m_LengthGUI->layout());
	GL_MainLayout->addWidget(m_GB_LengthPanel,0,1,Qt::AlignTop);
	m_GB_DistributionPanel->setLayout(m_DistributionGUI->layout());
	GL_MainLayout->addWidget(m_GB_DistributionPanel,0,2,Qt::AlignTop);
	m_GB_DisplayClassPanel->setLayout(m_DisplayClassGUI->layout());
	GL_MainLayout->addWidget(m_GB_DisplayClassPanel,0,3,Qt::AlignTop);
	m_GB_NormCutPanel->setLayout(m_NormCutGUI->layout());
	GL_MainLayout->addWidget(m_GB_NormCutPanel,0,4,Qt::AlignTop);
	
	GL_MainLayout->addWidget(m_PlanSetting,1,0,1,5);
	GL_MainLayout->addWidget(m_PB_Plane,2,5);
	GL_MainLayout->addWidget(m_Display,0,5,2,3);
	GL_MainLayout->setColumnStretch(6,1);

	setLayout(GL_MainLayout);
}

/********************************************************************************
 *BrowserVTKInput: Browser for input VTK file
 ********************************************************************************/

void FiberViewerLightGUI::BrowserVTKInput()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString filename,type;
	QFileDialog FileDialog;
	FileDialog.setViewMode(QFileDialog::Detail);
	filename = FileDialog.getOpenFileName(this, "Open VTK File", "/", "VTK (*.vtk)",&type);
	if(filename!="")
	{
		m_Display->ClearAlphas(FiberDisplay::Previous);
		m_Display->ClearAlphas(FiberDisplay::Next);
		m_VtkFileName="";
	}
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
		{
			InitRedMap(PolyData);
			m_Display->InitRenderer();
			m_Display->StartRenderer(PolyData);
			m_Display->GetActor()->GetMapper()->SetScalarRange(0, PolyData->GetNumberOfCells()-1);
			m_Display->SetLookupTable(m_RedMap);
			m_PB_SaveVTK->setEnabled(true);
			m_PB_Plane->setEnabled(true);
		}
	}
}

/********************************************************************************
 *BrowserVTKOutput: Browser for output VTK file
 ********************************************************************************/

void FiberViewerLightGUI::BrowserVTKOutput()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString file = QFileDialog::getSaveFileName(this, "Save VTK file", "/", "VTK (*.vtk)");
	
	if(file!=NULL)
		m_LE_VTKOutput->setText(file);
	
	QApplication::restoreOverrideCursor();	
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
			std::cout<<"VTK File read successfuly. "<<PolyData->GetNumberOfCells()<<" fibers read."<<std::endl;
			return PolyData;
		}
		else
			std::cout<<"Error reading VTK File. Check VTK Format."<<std::endl;
		return NULL;
	}
	else
		return NULL;
}

void FiberViewerLightGUI::InitRedMap(vtkPolyData* PolyData)
{
	//There is at most one color per fiber
	vtkLookupTable* RedMap=vtkLookupTable::New();
	RedMap->SetNumberOfTableValues(PolyData->GetNumberOfCells());
	
	vtkPoints* Points=PolyData->GetPoints();
	vtkCellArray* Lines=PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	vtkFloatArray* Colors=vtkFloatArray::New();
	
	//For each fiber
	Lines->InitTraversal();
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		//Fill Color array with R G B A (Alpha transparency)
		double Color[4]={0.85,0,0,1};
		
		//Fill the LUT
		RedMap->SetTableValue(i, Color);
		
		//Fill Scalars to be inserted in the PolyData
		for(int j=0; j<NumberOfPoints; j++)
			Colors->InsertNextValue(i);
	}
	PolyData->GetPointData()->SetScalars(Colors);
	m_RedMap=RedMap;
}

/********************************************************************************
 *BrowserSaveVTK: Option to save the actual VTK
 ********************************************************************************/

void FiberViewerLightGUI::SaveVTK()
{
	if(m_LE_VTKOutput->text()=="")
		BrowserVTKOutput();
	if(m_LE_VTKOutput->text()!="")
	{
		vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
		FinalPolyData->DeepCopy(m_Display->GetModifiedPolyData());
		FinalPolyData->GetPointData()->SetScalars(0);
		vtkSmartPointer<vtkPolyDataWriter> writer(vtkPolyDataWriter::New());
		writer->SetFileName(m_LE_VTKOutput->text().toStdString().c_str());
		writer->SetInput(FinalPolyData);
		writer->Update();
	}
}


/********************************************************************************
 *OpenLengthPanel: 
 ********************************************************************************/

void FiberViewerLightGUI::OpenLengthPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		m_GB_ActionPanel->hide();
		m_LengthGUI->InitLengthPanel();
		m_GB_LengthPanel->show();
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::CloseLengthPanel(FVPanelGUI::ExitSignal Type)
{
	m_GB_LengthPanel->hide();
	m_GB_ActionPanel->show();
	
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	
	if(Type==FVPanelGUI::Ok)
	{
		m_PB_Redo->setEnabled(false);
		if(m_Display->IsUnchanged())
		{
			if(m_Display->GetAlphasSize(FiberDisplay::Previous)>1)
				m_PB_Undo->setEnabled(true);
			else
				m_PB_Undo->setEnabled(false);
		}
		else
			m_PB_Undo->setEnabled(true);
	}
	else if(Type==FVPanelGUI::Undo)
	{
		if(m_Display->GetAlphasSize(FiberDisplay::Previous)<=1)
			m_PB_Undo->setEnabled(false);
		else
			m_PB_Undo->setEnabled(true);
	}
}

void FiberViewerLightGUI::OpenDistributionPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		if(sender()==m_PB_Gravity)
			m_DistributionGUI->SetMethod("Gravity");
		else if(sender()==m_PB_Hausdorff)
			m_DistributionGUI->SetMethod("Hausdorff");
		else if(sender()==m_PB_Mean)
			m_DistributionGUI->SetMethod("Mean");
		m_GB_ActionPanel->hide();
		m_GB_DistributionPanel->show();
		m_ProgressBar->setValue(0);
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::OpenNormCutPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		m_GB_ActionPanel->hide();
		m_NormCutGUI->ApplyWeight();
		m_GB_NormCutPanel->show();
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::CloseDistributionPanel(FVPanelGUI::ExitSignal Type)
{
	m_GB_DistributionPanel->hide();
	if(Type==FVPanelGUI::Undo)
	{
		m_DistributionGUI->ClearHist();
		m_GB_ActionPanel->show();
	}
	else if(Type==FVPanelGUI::Ok)
	{
		m_DisplayClassGUI->SetSender(FVDisplayClassGUI::Distribution);
		m_DisplayClassGUI->SetModel(m_DistributionGUI->GetMark(), m_DistributionGUI->GetNumberOfClasse());
		m_GB_DisplayClassPanel->show();
	}
}

void FiberViewerLightGUI::CloseDisplayClass(FVPanelGUI::ExitSignal Type)
{
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	if(Type==FVPanelGUI::Undo)
	{
		m_GB_DisplayClassPanel->hide();
		if(m_DisplayClassGUI->GetSender()==FVDisplayClassGUI::Distribution)
			m_GB_DistributionPanel->show();
		else
			m_GB_NormCutPanel->show();
	}
	else if(Type==FVPanelGUI::Ok)
	{
		m_PB_Redo->setEnabled(false);
		if(m_DisplayClassGUI->GetSender()==FVDisplayClassGUI::Distribution)
			m_DistributionGUI->ClearHist();
		m_GB_DisplayClassPanel->hide();
		m_GB_ActionPanel->show();
		if(m_Display->IsUnchanged())
		{
			if(m_Display->GetAlphasSize(FiberDisplay::Previous)>1)
				m_PB_Undo->setEnabled(true);
			else
				m_PB_Undo->setEnabled(false);
		}
		else
			m_PB_Undo->setEnabled(true);
	}
}

void FiberViewerLightGUI::CloseNormCut(FVPanelGUI::ExitSignal Type)
{
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	m_GB_NormCutPanel->hide();
	if(Type==FVPanelGUI::Ok)
	{
		m_DisplayClassGUI->SetSender(FVDisplayClassGUI::NCut);
		m_DisplayClassGUI->SetModel(m_NormCutGUI->GetMark(), m_NormCutGUI->GetNumberOfClasse());
		m_GB_DisplayClassPanel->show();
	}
	else if(Type==FVPanelGUI::Undo)
		m_GB_ActionPanel->show();
}

void FiberViewerLightGUI::OpenPlanSetting()
{
	if(m_PlanSetting->isVisible())
	{
		m_Display->GetPlan()->Off();
		m_PlanSetting->hide();
	}
	else
	{
		m_Display->GetPlan()->On();
      //Check if there is actually a plan displayed
		if(m_PlanSetting)
		{
			m_PlanSetting->GetParam();
			m_PlanSetting->show();
		}
	}
}

void FiberViewerLightGUI::UndoAction()
{
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Previous),FiberDisplay::Next);
	m_Display->PopBackAlpha(FiberDisplay::Previous);
	m_Display->UpdateCells();
	m_Display->Render();
	m_PB_Redo->setEnabled(true);
	if(m_Display->GetAlphasSize(FiberDisplay::Previous)<=1)
		m_PB_Undo->setEnabled(false);
}

void FiberViewerLightGUI::RedoAction()
{
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Next),FiberDisplay::Previous);
	m_Display->PopBackAlpha(FiberDisplay::Next);
	m_Display->UpdateCells();
	m_Display->Render();
	m_PB_Undo->setEnabled(true);
	if(m_Display->GetAlphasSize(FiberDisplay::Next)<=0)
		m_PB_Redo->setEnabled(false);
}
