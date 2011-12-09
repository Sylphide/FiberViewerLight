#include "FVDistributionGUI.h"

FVDistributionGUI::FVDistributionGUI(QWidget* Parent, FiberDisplay* Display) : FVPanelGUI(Parent,Display)
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

	m_L_Distribution=new QLabel("Computes classes distribution", this);
	m_L_ChooseThreshold=new QLabel("Choose threshold", this);
	m_L_Min=new QLabel("Min", this);
	m_L_NbBars=new QLabel("NbBars", this);
	m_L_Max=new QLabel("Max", this);
	m_L_Threshold=new QLabel("Threshold", this);
	m_LE_Min=new QLineEdit(this);
	m_LE_Min->setText("0");
	m_LE_NbBars=new QLineEdit(this);
	m_LE_NbBars->setText("10");
	m_LE_Max=new QLineEdit(this);

	m_LE_Threshold=new QLineEdit(this);
	m_LE_Threshold->setText("0.5");
	m_PB_ComputeDistribution=new QPushButton("Computes distribution",this);
	
	QFrame* F_HLine1=new QFrame;
	QFrame* F_HLine2=new QFrame;
	F_HLine1->setFrameShadow(QFrame::Plain);
	F_HLine1->setFrameShape(QFrame::HLine);
	F_HLine2->setFrameShadow(QFrame::Plain);
	F_HLine2->setFrameShape(QFrame::HLine);
	
	QGridLayout* GL_Distribution=new QGridLayout;
	QVBoxLayout* VL_PanelLayout=new QVBoxLayout;
	QHBoxLayout* HL_Threshold=new QHBoxLayout;
	QHBoxLayout* HL_UndoNext=new QHBoxLayout;
	
	VL_PanelLayout->addWidget(m_L_Distribution);
	VL_PanelLayout->addWidget(F_HLine1);
	GL_Distribution->addWidget(m_L_Min, 0, 0);
	GL_Distribution->addWidget(m_LE_Min, 0, 1);
	GL_Distribution->addWidget(m_L_NbBars, 0, 2);
	GL_Distribution->addWidget(m_LE_NbBars, 0, 3);
	GL_Distribution->addWidget(m_L_Max, 0, 4);
	GL_Distribution->addWidget(m_LE_Max, 0, 5);
	GL_Distribution->addWidget(m_PB_ComputeDistribution, 1, 0, 5, 0);
	VL_PanelLayout->addLayout(GL_Distribution);
	VL_PanelLayout->addWidget(m_HistPlot);
	VL_PanelLayout->addWidget(m_L_ChooseThreshold);
	VL_PanelLayout->addWidget(F_HLine2);
	HL_Threshold->addWidget(m_L_Threshold);
	HL_Threshold->addWidget(m_LE_Threshold);
	VL_PanelLayout->addLayout(HL_Threshold);
	HL_UndoNext->addWidget(m_PB_Undo);
	HL_UndoNext->addWidget(m_PB_Next);
	VL_PanelLayout->addLayout(HL_UndoNext);
	setLayout(VL_PanelLayout);
	
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(NextAction()));
	connect(m_PB_ComputeDistribution, SIGNAL(clicked()), this, SLOT(Plot()));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
}

void FVDistributionGUI::ClearHist()
{
	m_Hist->setSamples(QVector<QwtIntervalSample>());
	m_HistPlot->replot();
	m_Com.clear();
	m_Mark.clear();
	m_Distance.clear();
}

void FVDistributionGUI::InitDistance(int NbFibers)
{
	m_Distance.clear();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		std::vector<double> Line;
		//for all the other fibers
		for (int j = 0 ; j < NbFibers ; j++)
			Line.push_back(0);
		m_Distance.push_back(Line);
	}
}

void FVDistributionGUI::SetMethod(std::string Sender)
{
	m_Sender=Sender;
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	int NbFibers=PolyData->GetNumberOfCells();
	InitDistance(NbFibers);
	if(m_Sender=="Gravity")
		ApplyGravity(NbFibers);
	else if(m_Sender=="Hausdorff")
		ApplyHausdorff(NbFibers);
	else if(m_Sender=="Mean")
		ApplyMean(NbFibers);
	
	int Max=(int)GetMaxDistance();
	std::ostringstream text;
	text<<Max;
	m_LE_Max->setText(text.str().c_str());
}

void FVDistributionGUI::ApplyGravity(int NbFibers)
{
	int CountProgress=0;
	ComputeCOM();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		//for all the other fibers
		for (int j = i ; j < NbFibers ; j++)
		{
			m_Distance[i][j] = ComputeGravDist(i,j);
			m_Distance[j][i] = m_Distance[i][j];
			CountProgress++;
			emit Progress(CountProgress*200/(NbFibers*NbFibers));
		}
	}
}

void FVDistributionGUI::ApplyHausdorff(int NbFibers)
{
	int CountProgress=0;
	//This loop will be done as many times as the number of fiber
	for (int i=0;i<NbFibers;i++)
	{
		//the index of the column depends on the number of the client and the line. Here is the complexe function that we have found.
		for (int j=i;j<NbFibers;j++)
		{
			//As each client compute the i,j and the j,i MaxMin, the Hausdorff distance is simply the max from these both.
			double x1 = ComputeHausDist(j,i);
			double x2 = ComputeHausDist(i,j);
			double Max = x1;
			if (x2 > Max)
				Max = x2;
			//result has the kth hausdorff distance for this client
			m_Distance[i][j] =  Max;
			m_Distance[j][i] =  Max;
			CountProgress++;
			emit Progress(CountProgress*200/(NbFibers*NbFibers));
		}
	}
}

void FVDistributionGUI::ApplyMean(int NbFibers)
{
	int CountProgress=0;
	for(int i=0; i<NbFibers; i++)
	{
		for(int j=i; j<NbFibers; j++)
		{
			double x1 = ComputeMeanDistance(j,i);
			double x2 = ComputeMeanDistance(i,j);
			double MeanVal = (x1 + x2) / 2;
			m_Distance[i][j] =  MeanVal;
			m_Distance[j][i] =  MeanVal;
			CountProgress++;
			emit Progress(CountProgress*200/(NbFibers*NbFibers));
		}
	}
}

void FVDistributionGUI::Plot()
{
	QwtIntervalSeriesData* HistData=new QwtIntervalSeriesData;
	QVector<QwtIntervalSample> Samples;
	double Min=atof(m_LE_Min->text().toStdString().c_str());
	double Max=atof(m_LE_Max->text().toStdString().c_str());
	double Step=(Max-Min)/atof(m_LE_NbBars->text().toStdString().c_str());
	
	for(double i=Min; i+Step<=Max; i+=Step)
	{
		GenerateClasse(m_Distance, i);
		Samples.push_back(QwtIntervalSample(GetNumberOfClasse(), i, i+Step));
	}
	HistData->setSamples(Samples);
	m_Hist->setData(HistData);
	m_HistPlot->replot();
}

void FVDistributionGUI::GenerateClasse(std::vector<std::vector<double> > Distance, double Threshold)  
{
	m_Mark.clear();
	std::vector<int> m_Norm;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	int Change, NbFibers=PolyData->GetNumberOfCells();
	
	for(int i = 0 ; i < NbFibers ; i++)
		m_Mark.push_back(i);
  
	for(int i = 1 ; i < NbFibers ; i++)
	{
		for(int j = 0 ; j < i ; j++)
		{
			if(Distance[i][j] <= Threshold)
			{
				Change = m_Mark[i];
				for(int k = 0 ; k <= i ; k++)
				{  
					if(m_Mark[k] == Change)  //m_Mark[i]
						m_Mark[k] = m_Mark[j];
				}
			}
		}
	}

  //Normalisation of the mark vector  
	for(int i = 0 ; i < NbFibers ; i++)
		m_Norm.push_back(-1);
  
	int Compt = 1;
	for(int i = 0 ; i < NbFibers ; i++)
	{
		if(m_Norm[m_Mark[i]] == -1)
			m_Norm[m_Mark[i]] = Compt++;
		m_Mark[i] = m_Norm[m_Mark[i]];
	}
}

double FVDistributionGUI::GetMaxDistance()
{
	double Max=-100000;
	for(unsigned int i=0; i<m_Distance.size(); i++)
	{
		for(unsigned int j=i; j<m_Distance[i].size(); j++)
		{
			if(m_Distance[i][j]>Max)
				Max=m_Distance[i][j];
		}
	}
	return Max;
}

int FVDistributionGUI::GetNumberOfClasse()
{
	//Get the max of the mark vector which is the number of classe
	int nbclasse = 0;
	for(unsigned int i = 0 ; i < m_Mark.size() ; i++)
		if(m_Mark[i] > nbclasse)
			nbclasse = m_Mark[i];
	return nbclasse;
}

void FVDistributionGUI::ComputeCOM()
{
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	vtkPoints* Points=PolyData->GetPoints();
	vtkCellArray* Lines=PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	
	double x,y,z;
	Lines->InitTraversal();
	for(int i = 0  ; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		x = y = z = 0;
		//For each point of this fiber
		for(unsigned int j = 0 ; j < NumberOfPoints; j++)
		{
			double Point[3];
			Points->GetPoint(Ids[j], Point);
			x += Point[0];
			y += Point[1];
			z += Point[2];	
		}
		std::vector<double> com;
		com.push_back(x/NumberOfPoints);
		com.push_back(y/NumberOfPoints);
		com.push_back(z/NumberOfPoints);
		
		m_Com.push_back(com);
	}
}

double FVDistributionGUI::ComputeGravDist(int source,int target)  
{
	double x = m_Com[target][0] - m_Com[source][0];
	double y = m_Com[target][1] - m_Com[source][1];
	double z = m_Com[target][2] - m_Com[source][2];
	double dist = sqrt(x*x + y*y + z*z);
	
	return dist;
}

double FVDistributionGUI::ComputeHausDist(int Source,int Target)
{  
	double HausDist = -1;
	double x,y,z;
	double xs,ys,zs;
	double xt,yt,zt;
	
	double Distance, MinDist;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	vtkIdType NbSourcePoints, NbTargetPoints;
	vtkPoints* Points=PolyData->GetPoints();
	vtkIdType* SourceIds;
	vtkIdType* TargetIds;
	vtkCellArray* Lines=PolyData->GetLines();
	
	Lines->InitTraversal();
	for(int i=0; i<=Source; i++)
		Lines->GetNextCell(NbSourcePoints,SourceIds);
	
	Lines->InitTraversal();
	for(int i=0; i<=Target; i++)
		Lines->GetNextCell(NbTargetPoints,TargetIds);
	
	//For each point of the source fiber
	for(int i=0; i<NbSourcePoints; i++)
	{
		double SourcePoint[3]={0,0,0};
		Points->GetPoint(SourceIds[i],SourcePoint);
		MinDist = 999999999;
		//For each point of the target fiber
		xs=SourcePoint[0];
		ys=SourcePoint[1];
		zs=SourcePoint[2];
		
		for(int j=0; j<NbTargetPoints; j++)
		{
			//calculate distance between the two points
			double TargetPoint[3]={0,0,0};
			Points->GetPoint(TargetIds[j],TargetPoint);
			xt = TargetPoint[0];
			yt = TargetPoint[1];
			zt = TargetPoint[2];
			x = (xs - xt);
			y = (ys - yt);
			z = (zs - zt);
			Distance = sqrt(x*x+y*y+z*z);
			//Keep the minimum distance of the distances between the whole points
			//of the target fiber and one point of the source fiber
			if (Distance<MinDist)
				MinDist = Distance;
		}
		 //Finaly, get the maximum of all these minimum
		if (MinDist > HausDist)
			HausDist = MinDist;
	}
	//return the MinMax, to have the Hausdorff distance
	//it just remains to take the max between the couple i,j and j,i
	return HausDist;
}

//Compute the MaxMin between two fibers
double FVDistributionGUI::ComputeMeanDistance(int Source,int Target)
{
	double x,y,z;
	double xs,ys,zs;
	double xt,yt,zt;
	
	double MeanDist, Distance, MinDist, TotalDist = 0;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	vtkIdType NbSourcePoints, NbTargetPoints;
	vtkPoints* Points=PolyData->GetPoints();
	vtkIdType* SourceIds;
	vtkIdType* TargetIds;
	vtkCellArray* Lines=PolyData->GetLines();
	Lines->InitTraversal();
	for(int i=0; i<=Source; i++)
		Lines->GetNextCell(NbSourcePoints,SourceIds);
	
	Lines->InitTraversal();
	for(int i=0; i<=Target; i++)
		Lines->GetNextCell(NbTargetPoints,TargetIds);
	
	//For each point of the source fiber  
	for (unsigned int i=0;i<NbSourcePoints;i++)
	{
		double SourcePoint[3]={0,0,0};
		Points->GetPoint(SourceIds[i],SourcePoint);
		MinDist = 999999999;
		//For each point of the target fiber
		xs=SourcePoint[0];
		ys=SourcePoint[1];
		zs=SourcePoint[2];
		
		for (unsigned int j=0;j<NbTargetPoints;j++)
		{
			//calculate distance between the two points
			double TargetPoint[3]={0,0,0};
			Points->GetPoint(TargetIds[j],TargetPoint);
			xt = TargetPoint[0];
			yt = TargetPoint[1];
			zt = TargetPoint[2];

			x = (xs - xt);
			y = (ys - yt);
			z = (zs - zt);
			Distance = sqrt(x*x+y*y+z*z);
			//Keep the minimum distance of the distances between the whole points
			//of the target fiber and one point of the source fiber
			if (Distance<MinDist)
				MinDist = Distance;
		}
		//Finaly, sum all min
		TotalDist += MinDist;
	}
	MeanDist = TotalDist/NbSourcePoints;
	//return the Meanmin to have the Mean distance
	//it just remains to take the mean between the couple i,j end j,i
	return MeanDist;
}

void FVDistributionGUI::UndoAction()
{
	emit Exit(FVPanelGUI::Undo);
}

void FVDistributionGUI::NextAction()
{
	
	GenerateClasse(m_Distance, atof(m_LE_Threshold->text().toStdString().c_str()));
	emit Exit(FVPanelGUI::Ok);
}
