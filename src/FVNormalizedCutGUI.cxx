#include "FVNormalizedCutGUI.h"

FVNormCutGUI::FVNormCutGUI(QWidget* Parent, FiberDisplay* Display):FVPanelGUI(Parent,Display)
{
	m_L_Cluster=new QLabel("Number of Cluster", this);
// 	m_L_Scale=new QLabel("Scale", this);
	m_SB_Cluster=new QSpinBox(this);
	m_SB_Cluster->setSingleStep(1);
	m_SB_Cluster->setRange(2,1000);
	m_SB_Cluster->setValue(2);
// 	m_S_Scale=new QSlider(Qt::Horizontal, this);
// 	m_S_Scale->setRange(0,100);
// 	m_S_Scale->setValue(20);
// 	m_LCD_Scale=new QLCDNumber(this);
// 	m_LCD_Scale->display(0.2);
// 	m_LCD_Scale->setSegmentStyle(QLCDNumber::Flat);
	
	
	QGridLayout* GL_NormCut=new QGridLayout;
	QHBoxLayout* HL_Navigation=new QHBoxLayout;
	
	GL_NormCut->addWidget(m_L_Cluster,0,0,1,0);
	GL_NormCut->addWidget(m_SB_Cluster,1,0,1,0);
// 	GL_NormCut->addWidget(m_L_Scale,2,0,1,0);
// 	GL_NormCut->addWidget(m_S_Scale,3,0);
// 	GL_NormCut->addWidget(m_LCD_Scale,3,1);
	HL_Navigation->addWidget(m_PB_Undo);
	HL_Navigation->addWidget(m_PB_Next);
	GL_NormCut->addLayout(HL_Navigation,2,0,1,0);
	GL_NormCut->setRowStretch(3,1);

	setLayout(GL_NormCut);
	
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(NextAction()));	
// 	connect(m_S_Scale, SIGNAL(valueChanged(int)), this, SLOT(setLCDValue(int)));
}

// void FVNormCutGUI::setLCDValue(int Value)
// {
// 	m_LCD_Scale->display(double(Value)/100.0);
// }

void FVNormCutGUI::InitWeight()
{
	m_Weight.clear();
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	int NbFibers=PolyData->GetNumberOfCells();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		std::vector<double> Line;
		//for all the other fibers
		for (int j = 0 ; j < NbFibers ; j++)
			Line.push_back(0);
		m_Weight.push_back(Line);
	}
}

void FVNormCutGUI::ApplyWeight()
{
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetModifiedPolyData();
	int NbFibers=PolyData->GetNumberOfCells();
	int CountProgress=0;
	InitWeight();
	for(int i=0; i<NbFibers; i++)
	{
		for(int j=i; j<NbFibers; j++)
		{
			double x1 = ComputeMeanDistance(j,i);
			double x2 = ComputeMeanDistance(i,j);
			double MeanVal = (x1 + x2) / 2;
			if(MeanVal!=0)
			{
				m_Weight[i][j] = 1/MeanVal;
				m_Weight[j][i] = 1/MeanVal;
			}
			else
			{
				m_Weight[i][j]=0;
				m_Weight[j][i]=0;
			}
			CountProgress++;
			emit Progress(CountProgress*200/(NbFibers*NbFibers));
		}
	}
}

double FVNormCutGUI::ComputeMeanDistance(int Source,int Target)
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

double FVNormCutGUI::Assoc(int BeginId, int EndId, bool All)
{
	double SumWeight=0;
	for(int i=BeginId; i<=EndId; i++)
	{
		if(!All)
		{
			for(int j=BeginId; j<=EndId; j++)
				SumWeight+=m_Weight[i][j];
		}
		else
		{
			for(unsigned int j=0; j<m_Weight.size() ; j++)
				SumWeight+=m_Weight[i][j];
		}
	}
	return SumWeight;
}

void FVNormCutGUI::GenerateClass()
{
	int ClusterDone=1;
	m_Mark=FillMark(m_Weight,ClusterDone,m_SB_Cluster->value());
}

std::vector<int> FVNormCutGUI::FillMark(std::vector<std::vector<double> > Weight, int& ClusterDone, int NumberOfCluster)
{
	std::vector<int> Mark;
	if(Weight.size()==1 || NumberOfCluster==1)
	{
		for(unsigned int i=0; i<Weight.size(); i++)
			Mark.push_back(ClusterDone);
		return Mark;
	}
	else
	{
		double MinCut=1000;
		int CutId=-1;
		for(unsigned int i=0; i<Weight.size()-1; i++)
		{
			double cut=Cut(i,i+1);
			double NCut=cut/Assoc(0,i,true)+cut/Assoc(i+1,Weight.size()-1,true);
			if(NCut<MinCut)
			{
				CutId=i;
				MinCut=NCut;
			}
		}
		std::vector<std::vector<double> > SubWeight;
		for(int i=0; i<=CutId; i++)
		{
			std::vector<double>Line;
			for(int j=0; j<=CutId; j++)
				Line.push_back(Weight[i][j]);
			SubWeight.push_back(Line);
		}
		Mark=FillMark(SubWeight,ClusterDone,NumberOfCluster/2);
		
		
		ClusterDone++;
		SubWeight.clear();
		for(unsigned int i=CutId+1; i<Weight.size(); i++)
		{
			std::vector<double>Line;
			for(unsigned int j=CutId+1; j<=Weight.size(); j++)
				Line.push_back(Weight[i][j]);
			SubWeight.push_back(Line);
		}
		std::vector<int> MarkTemp;
		
		if(NumberOfCluster%2==0)
			MarkTemp=FillMark(SubWeight,ClusterDone,NumberOfCluster/2);
		else
			MarkTemp=FillMark(SubWeight,ClusterDone,NumberOfCluster/2+1);
		for(unsigned int i=0; i<MarkTemp.size(); i++)
			Mark.push_back(MarkTemp[i]);
		return Mark;
	}
}

double FVNormCutGUI::Cut(int Previous, int Next)
{
	double Cut=0;
	for(int i=0; i<=Previous; i++)
	{
		for(unsigned int j=Next; j<m_Weight.size(); j++)
			Cut+=m_Weight[i][j];
	}
	return Cut;
}

int FVNormCutGUI::GetNumberOfClasse()
{
	//Get the max of the mark vector which is the number of classe
	int nbclasse = 0;
	for(unsigned int i = 0 ; i < m_Mark.size() ; i++)
		if(m_Mark[i] > nbclasse)
			nbclasse = m_Mark[i];
	return nbclasse;
}

void FVNormCutGUI::UndoAction()
{
	emit Exit(FVPanelGUI::Undo);
}

void FVNormCutGUI::NextAction()
{
	GenerateClass();
	emit Exit(FVPanelGUI::Ok);
}
