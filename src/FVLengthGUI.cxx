#include "FVLengthGUI.h"

FVLengthGUI::FVLengthGUI(QWidget* Parent, FiberDisplay* Display) : FVPanelGUI(Parent,Display)
{
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
	
	m_SB_NbBars=new QSpinBox(this);
	m_SB_NbBars->setSingleStep(1);
	m_SB_NbBars->setRange(0,1000);
	m_SB_NbBars->setValue(10);
	m_L_NbBars=new QLabel("Bars", this);
	m_SB_LowerTh=new QSpinBox(this);
	m_SB_LowerTh->setSingleStep(1);
	m_SB_LowerTh->setRange(0,1000);
	m_SB_UpperTh=new QSpinBox(this);
	m_SB_UpperTh->setSingleStep(1);
	m_SB_UpperTh->setRange(0,1000);
	m_L_LowerTh=new QLabel("Lower Threshold", this);
	m_L_UpperTh=new QLabel("Upper Threshold", this);
	m_PB_LengthComputation=new QPushButton("Apply Threshold",this);
	
	QGridLayout* GL_LengthFilter=new QGridLayout;
	
	GL_LengthFilter->addWidget(m_HistPlot, 0, 0, 1, 0);
	GL_LengthFilter->addWidget(m_L_NbBars, 1, 0);
	GL_LengthFilter->addWidget(m_SB_NbBars, 1, 1);
	GL_LengthFilter->addWidget(m_L_LowerTh, 2, 0);
	GL_LengthFilter->addWidget(m_SB_LowerTh, 3, 0);
	GL_LengthFilter->addWidget(m_L_UpperTh, 2, 1);
	GL_LengthFilter->addWidget(m_SB_UpperTh, 3, 1);
	GL_LengthFilter->addWidget(m_PB_LengthComputation,4,0,1,0);
	GL_LengthFilter->addWidget(m_PB_Undo,5,0);
	GL_LengthFilter->addWidget(m_PB_Next,5,1);
	setLayout(GL_LengthFilter);
	
	connect(m_PB_LengthComputation, SIGNAL(clicked()), this, SLOT(LengthComputation()));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(OkAction()));
}

/********************************************************************************
 *LengthCalculation: Fill m_Length table with length of each fiber
 ********************************************************************************/

void FVLengthGUI::LengthCalculation()
{
	m_Length.clear();
	double FiberLength, Step;
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	vtkCellArray* Lines=vtkCellArray::New();
	vtkPoints* Points=vtkPoints::New();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	Points=m_Display->GetModifiedPolyData()->GetPoints();
	Lines=m_Display->GetModifiedPolyData()->GetLines();
	
	Lines->InitTraversal();
	for(unsigned int i=0; i<Alpha.size(); i++)
	{
		if(Alpha[i]==1)
		{
			Lines->GetNextCell(NumberOfPoints, Ids);
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
		else
			m_Length.push_back(-1);
		emit Progress(i*100/Alpha.size());
	}
}

void FVLengthGUI::InitLengthPanel()
{
	LengthCalculation();		//Fill m_Length table
	InitLengthColorMap();	//Fill m_ColorMap
				
	//Set default threshold values
	m_SB_UpperTh->setValue((int)ceil(GetMaxLength()));
	m_SB_LowerTh->setValue((int)floor(GetMinLength()));
	
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Previous),FiberDisplay::Previous);
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
	
	double Min=GetMinLength(), Max=GetMaxLength();
	
	m_Display->SetLookupTable(m_ColorMap);
	
	std::vector<int> Alpha;
	//For each fiber
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		if(m_Length[i]!=-1)
		{
			if(m_Length[i]>(m_SB_LowerTh->value()) && m_Length[i]<m_SB_UpperTh->value())
				Alpha.push_back(1);
			else
				Alpha.push_back(0);
		}
		else
			Alpha.push_back(0);
	}
	m_Display->SetLastAlpha(Alpha, FiberDisplay::Previous);
	m_Display->UpdateCells();
	m_Display->Render();
	
	QwtIntervalSeriesData* HistData=new QwtIntervalSeriesData;
	QVector<QwtIntervalSample> Samples;
	double Step=(Max-Min)/m_SB_NbBars->value();
	double IntervalMin=Min, IntervalMax=IntervalMin+Step;
	int NbFiber=0;
	for(int i=0; i<m_SB_NbBars->value(); i++)
	{
		for(unsigned int j=0; j<m_Length.size(); j++)
		{
			if(m_Length[j]>=IntervalMin && m_Length[j]<IntervalMax && m_Display->GetLastAlpha(FiberDisplay::Previous)[j]==1 && m_Length[j]!=-1)
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
	
	std::cout<<"End of Length Computation."<<std::endl;
	QApplication::restoreOverrideCursor();	
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
		if(m_Length[i]<Min && m_Length[i]!=-1)
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
	
	//There is at most one color per fiber
	vtkLookupTable* ColorMap=vtkLookupTable::New();
	ColorMap->SetNumberOfTableValues(m_Length.size());	
	
	for(unsigned int i=0; i<m_Length.size(); i++)
	{
		//Fill Color array with R G B A (Alpha transparency)
		if(m_Length[i]!=-1)
		{
			double Color[4];
			m_Display->GetFiberColor((m_Length[i]-Min)/(Max-Min),Color);
			Color[3]=1;
			ColorMap->SetTableValue(i, Color);
		}
		else
			ColorMap->SetTableValue(i, 0, 0, 0);		
	}
	m_ColorMap=ColorMap;
}

/********************************************************************************
 *SetFiberOpacity: Display or not a fiber by turning 0 or 1 the alpha transparency
 ********************************************************************************/

void FVLengthGUI::SetFiberOpacity(vtkIdType Id, int a)
{
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	Alpha[Id]=a;
	m_Display->SetLastAlpha(Alpha, FiberDisplay::Previous);
}

/********************************************************************************
 *UndoAction: Undo last modification on the polydata.
 ********************************************************************************/

void FVLengthGUI::UndoAction()
{
// 	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Previous), FiberDisplay::Next);
	m_Display->PopBackAlpha(FiberDisplay::Previous);
	m_Hist->setSamples(QVector<QwtIntervalSample>());
	m_HistPlot->replot();
	m_Length.clear();
	emit Exit(FVPanelGUI::Undo);
}

void FVLengthGUI::OkAction()
{
	m_Hist->setSamples(QVector<QwtIntervalSample>());
	m_HistPlot->replot();
	m_Length.clear();
	m_Display->ClearAlphas(FiberDisplay::Next);
	emit Exit(FVPanelGUI::Ok);
}
