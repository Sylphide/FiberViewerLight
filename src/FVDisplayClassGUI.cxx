#include "FVDisplayClassGUI.h"

FVDisplayClassGUI::FVDisplayClassGUI(QWidget* Parent, FiberDisplay* Display):FVPanelGUI(Parent,Display)
{
	m_TV_View=new QTableView;
	m_PB_Show=new QPushButton("Show", this);
	m_ColorMap=vtkLookupTable::New();
	
	QGridLayout* GL_DisplayClass=new QGridLayout;
	
	GL_DisplayClass->addWidget(m_TV_View,1,0,1,0);
	GL_DisplayClass->addWidget(m_PB_Show,2,0,1,0);
	GL_DisplayClass->addWidget(m_PB_Undo,3,0);
	GL_DisplayClass->addWidget(m_PB_Next,3,1);

	setLayout(GL_DisplayClass);
	
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Show, SIGNAL(clicked()), this, SLOT(ColorFiber()));
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(NextAction()));
}

void FVDisplayClassGUI::SetSender(FVDisplayClassGUI::Sender sender)
{
	m_Sender=sender;
}

FVDisplayClassGUI::Sender FVDisplayClassGUI::GetSender()
{
	return m_Sender;
}

void FVDisplayClassGUI::SetModel(std::vector<int> Mark, int NbClasse)
{
	m_Mark=Mark;
	std::vector<int> NbFibersPerClasse=GetNbFibersPerClasse(NbClasse);
	CompleteMark();
	
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Previous),FiberDisplay::Previous);
	
	QStandardItemModel* Model=new QStandardItemModel(NbClasse,2);
	QStringList Headers;
	Headers.append("Nb Element");
	Headers.append("Color");
	Model->setHorizontalHeaderLabels(Headers);
	
	vtkLookupTable* ColorMap=vtkLookupTable::New();
	ColorMap->SetNumberOfTableValues(m_Display->GetOriginalPolyData()->GetNumberOfCells());
	
	for(int i=0; i<NbClasse; i++)
	{
		Model->setItem(i,0, new QStandardItem(QString::number(NbFibersPerClasse[i])));
		QStandardItem* Background=new QStandardItem;
		double Color[4];
		m_Display->GetFiberColor(double(i+1)/(double)(NbClasse+1), Color);
		Color[3]=1;
		for(unsigned int j=0; j<m_Mark.size(); j++)
		{
			if(m_Mark[j]-1==i)
				ColorMap->SetTableValue(j,Color);
			else if(m_Mark[j]-1==-1)
				ColorMap->SetTableValue(j,0,0,0);
		}
		Color[0]*=255; Color[1]*=255; Color[2]*=255; 
		
		Background->setBackground(QColor((int)Color[0],(int)Color[1],(int)Color[2]));
		Background->setSelectable(false);
		Model->setItem(i,1,Background);
	}
	
	m_ColorMap=ColorMap;
	m_Display->SetLookupTable(m_ColorMap);
	
	m_Display->Render();
	m_TV_View->setModel(Model);
	m_TV_View->setSelectionMode(QAbstractItemView::MultiSelection);
	m_TV_View->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

std::vector<int> FVDisplayClassGUI::GetNbFibersPerClasse(int NbClasse)
{
	//Array with the number of element of each class
	std::vector <int> NbFibersPerClasse(NbClasse,0);
	for(unsigned int i = 0 ; i < m_Mark.size() ; i++)
		NbFibersPerClasse[m_Mark[i]-1]++;
	return NbFibersPerClasse;
}

void FVDisplayClassGUI::CompleteMark()
{
	std::vector<int> PreviousAlpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	std::vector<int> CompletedMark;
	int MarkId=0;
	for(unsigned int i=0; i<PreviousAlpha.size(); i++)
	{
		if(PreviousAlpha[i]==0)
			CompletedMark.push_back(0);
		else
		{
			CompletedMark.push_back(m_Mark[MarkId]);
			MarkId++;
		}
	}
	m_Mark=CompletedMark;
}

void FVDisplayClassGUI::UndoAction()
{
	m_Display->PopBackAlpha(FiberDisplay::Previous);
	emit Exit(FVPanelGUI::Undo);
}

void FVDisplayClassGUI::ColorFiber()
{
	QItemSelectionModel* Selection=m_TV_View->selectionModel();
	QModelIndexList SelectionList=Selection->selectedIndexes();
	std::vector<int> Alpha;
	bool Found=false;
	for(unsigned int i=0; i<m_Mark.size(); i++)
	{
		if(m_Mark[i]!=0)
		{
			for(int j=0; j<SelectionList.size(); j++)
			{
				if(m_Mark[i]-1==SelectionList[j].row())
				{
					Found=true;
					Alpha.push_back(1);
				}
			}
			if(!Found)
				Alpha.push_back(0);
			Found=false;
		}
		else
			Alpha.push_back(0);
	}
	m_Display->SetLastAlpha(Alpha,FiberDisplay::Previous);
	m_Display->UpdateCells();
}

void FVDisplayClassGUI::NextAction()
{
	m_Display->ClearAlphas(FiberDisplay::Next);
	emit Exit(FVPanelGUI::Ok);
}
