#include "FVLengthGUI.h"

FVLengthGUI::FVLengthGUI(QWidget* parent, FiberDisplay* Display) : QWidget(parent)
{
	m_Display=Display;
	setMinimumSize(300,450);
	setMaximumSize(300,450);
	
	m_HistPlot=new QwtPlot;
	m_Hist=new QwtPlotHistogram;
	m_Hist->setPen(QPen(Qt::white,0));
	m_Hist->setBrush(QColor(51,0,153));
	m_Hist->attach(m_HistPlot);
	QwtPlotGrid* Grid=new QwtPlotGrid;
	Grid->enableXMin(true);
	Grid->enableYMin(true);
	Grid->setMajPen(QPen(Qt::black,0,Qt::DotLine));
	Grid->setMinPen(QPen(Qt::white,0,Qt::DotLine));
	Grid->attach(m_HistPlot);
	m_HistPlot->setCanvasBackground(Qt::white);
	m_HistPlot->replot();
	
	firstUse=true;
	m_SB_NbBars=new QSpinBox(this);
	m_SB_NbBars->setSingleStep(1);
	m_SB_NbBars->setRange(0,1000);
	m_SB_NbBars->setValue(10);
	m_L_NbBars=new QLabel("Bars", this);
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
	m_PB_LengthComputation=new QPushButton("Length Computation",this);
	m_PB_Undo=new QPushButton("Undo",this);
	m_PB_Ok=new QPushButton("Ok",this);
	
	QGroupBox* GB_LengthFilter=new QGroupBox("Length");
	GB_LengthFilter->setFixedSize(300,450);
	QGridLayout* GL_LengthFilter=new QGridLayout;
	QVBoxLayout* MainLayout=new QVBoxLayout;
	
	GL_LengthFilter->addWidget(m_HistPlot, 0, 0, 1, 0);
	GL_LengthFilter->addWidget(m_L_NbBars, 1, 0);
	GL_LengthFilter->addWidget(m_SB_NbBars, 1, 1);
	GL_LengthFilter->addWidget(m_L_LowerTh, 2, 0);
	GL_LengthFilter->addWidget(m_SB_LowerTh, 3, 0);
	GL_LengthFilter->addWidget(m_L_UpperTh, 2, 1);
	GL_LengthFilter->addWidget(m_SB_UpperTh, 3, 1);
	GL_LengthFilter->addWidget(m_PB_LengthComputation,4,0,1,0);
	GL_LengthFilter->addWidget(m_PB_Undo,5,0);
	GL_LengthFilter->addWidget(m_PB_Ok,5,1);
	GL_LengthFilter->setRowStretch(6,1);
	GB_LengthFilter->setLayout(GL_LengthFilter);
	MainLayout->addWidget(GB_LengthFilter);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
	
	connect(m_PB_LengthComputation, SIGNAL(clicked()), this, SLOT(LengthComputation()));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Ok, SIGNAL(clicked()), this, SLOT(OkAction()));
}

/********************************************************************************
 *LengthCalculation: Fill m_Length table with length of each fiber
 ********************************************************************************/

void FVLengthGUI::LengthCalculation()
{
	m_Length.clear();
	double FiberLength, Step;
	vtkCellArray* Lines=vtkCellArray::New();
	vtkPoints* Points=vtkPoints::New();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	Points=m_Display->GetModifiedPolyData()->GetPoints();
	Lines=m_Display->GetModifiedPolyData()->GetLines();
	
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
			
			//Distance between to successive points
			Step=sqrt(x*x+y*y+z*z);
			
			//Distance between first point and last calculated one.
			FiberLength+=Step;
		}
		m_Length.push_back(FiberLength);
	}
}

/********************************************************************************
 *GetThresholdedIds: Return each index that corresponds to fiber which length
 *	is between threshold values
 ********************************************************************************/

std::vector<int> FVLengthGUI::GetThresholdedIds(int LowerTh, int UpperTh)
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

void FVLengthGUI::LengthComputation()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	std::cout<<"Length computation..."<<std::endl;
	
	//Get the actual renderer
	vtkRenderer* Renderer=vtkRenderer::New();
	Renderer=m_Display->GetRenderer();
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	if(PolyData!=NULL)
	{
		double Min, Max;
		vtkActor* Actor=vtkActor::New();
		if(firstUse)
		{
			if(m_Length.size()==0)
			{
				LengthCalculation();		//Fill m_Length table
				InitLengthColorMap();	//Fill m_ColorMap
				
				//Set default threshold values
				m_SB_UpperTh->setValue((int)ceil(GetMaxLength()));
				m_SB_LowerTh->setValue((int)floor(GetMinLength()));
			}
			firstUse=false;
			m_SB_LowerTh->setEnabled(true);
			m_SB_UpperTh->setEnabled(true);
			
			m_Display->DuplicateLastAlpha();
			
			//Set Mapper's LUT Color Map
			Actor=m_Display->GetActor();
			Actor->GetMapper()->SetLookupTable(m_ColorMap);
		}
		
		Min=GetMinLength();
		Max=GetMaxLength();
		std::vector<int> ThresholdedIds=GetThresholdedIds(m_SB_LowerTh->value(),m_SB_UpperTh->value());
		
		//For each fiber
		for(int i=0; i<PolyData->GetNumberOfCells(); i++)
		{
			if(IntIsIn(i,ThresholdedIds))
				SetFiberOpacity(i, 1);	//fiber's length is inside the filter->display
			else
				SetFiberOpacity(i, 0);	//fiber's length is outside the filter->don't display 
		}
		m_Display->UpdateCells();
		m_Display->Render();
		
		QwtIntervalSeriesData* HistData=new QwtIntervalSeriesData;
		QVector<QwtIntervalSample> Samples;
		double Step=(Max-Min)/m_SB_NbBars->value();
		double IntervalMin=Min, IntervalMax=IntervalMin+Step;
		int NbFiber=0;
		for(int i=0; i<m_SB_NbBars->value(); i++)
		{
			for(int j=0; j<PolyData->GetNumberOfCells(); j++)
			{
				if(m_Length[j]>=IntervalMin && m_Length[j]<IntervalMax && m_Display->GetLastAlpha()[j]==1)
					NbFiber++;
			}
			Samples.push_back(QwtIntervalSample(NbFiber,IntervalMin,IntervalMax));
			NbFiber=0;
			IntervalMin+=Step;
			IntervalMax+=Step;
		}
		HistData->setSamples(Samples);
		m_Hist->setData(HistData);
		m_HistPlot->replot();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
	
	std::cout<<"End of Length Computation."<<std::endl;
	QApplication::restoreOverrideCursor();	
}

/********************************************************************************
 *IntIsIn: Look if x is in the List vector
 ********************************************************************************/

bool FVLengthGUI::IntIsIn(int x, std::vector<int> List)
{
	for(unsigned int i=0; i<List.size(); i++)
	{
		if(x==List[i])
			return true;
	}
	return false;
}

/********************************************************************************
 *GetMaxLength/GetMinLength: Get Max/Min from the m_Length table
 ********************************************************************************/

double FVLengthGUI::GetMaxLength()
{
	double Max=-100000;
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]>Max)
			Max=m_Length[i];
	}
	return Max;
}

double FVLengthGUI::GetMinLength()
{
	double Min=100000;
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]<Min)
			Min=m_Length[i];
	}
	return Min;
}

/********************************************************************************
 *InitLengthColorMap: Fill m_ColorMap LUT and Scalars in m_PolyData to set 
 *	the color map.
 ********************************************************************************/

void FVLengthGUI::InitLengthColorMap()
{
	double Min=GetMinLength(), Max=GetMaxLength();
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	
	//There is at most one color per fiber
	vtkLookupTable* ColorMap=vtkLookupTable::New();
	ColorMap->SetNumberOfTableValues(PolyData->GetNumberOfCells());	
	
	for(int i=0; i<PolyData->GetNumberOfCells(); i++)
	{
		//Fill Color array with R G B A (Alpha transparency)
		double Color[4];
		m_Display->GetFiberColor((m_Length[i]-Min)/(Max-Min),Color);
		Color[3]=1;
		
		//Fill the LUT
		ColorMap->SetTableValue(i, Color);
	}
	m_ColorMap=ColorMap;
}

/********************************************************************************
 *SetFiberOpacity: Display or not a fiber by turning 0 or 1 the alpha transparency
 ********************************************************************************/

void FVLengthGUI::SetFiberOpacity(vtkIdType Id, int a)
{
	std::vector<int> Alpha=m_Display->GetLastAlpha();
	Alpha[Id]=a;
	m_Display->SetLastAlpha(Alpha);
}

/********************************************************************************
 *UndoAction: Undo last modification on the polydata.
 ********************************************************************************/

void FVLengthGUI::UndoAction()
{
	firstUse=true;
	m_Display->PopBackAlpha();
	emit ExitLength();
}

void FVLengthGUI::OkAction()
{
	firstUse=true;
	emit ExitLength();
}
	

//Fonction de reset des membres appele dans load vtk
