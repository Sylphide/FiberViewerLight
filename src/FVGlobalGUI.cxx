#include "FVGlobalGUI.h"

FVGlobalGUI::FVGlobalGUI(QWidget* parent, FiberDisplay* Display) : QWidget(parent)
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
	
	m_L_Distribution=new QLabel("Computes classes distribution", this);
	m_L_ChooseThreshold=new QLabel("Choose threshold", this);
	m_L_Min=new QLabel("Min", this);
	m_L_Step=new QLabel("Step", this);
	m_L_Max=new QLabel("Max", this);
	m_L_Threshold=new QLabel("Threshold", this);
	m_LE_Min=new QLineEdit(this);
	m_LE_Step=new QLineEdit(this);
	m_LE_Max=new QLineEdit(this);
	m_LE_Threshold=new QLineEdit(this);
	m_PB_ComputeDistribution=new QPushButton("Computes distribution",this);
	m_PB_Undo=new QPushButton("Undo",this);
	m_PB_Next=new QPushButton("Next",this);
	
	QFrame* F_HLine1=new QFrame;
	QFrame* F_HLine2=new QFrame;
	F_HLine1->setFrameShadow(QFrame::Plain);
	F_HLine1->setFrameShape(QFrame::HLine);
	F_HLine2->setFrameShadow(QFrame::Plain);
	F_HLine2->setFrameShape(QFrame::HLine);
	
	QGroupBox* GB_GlobalFilter=new QGroupBox("Compute Distribution");
	GB_GlobalFilter->setFixedSize(300,450);
	QGridLayout* GL_Distribution=new QGridLayout;
	QVBoxLayout* VL_PanelLayout=new QVBoxLayout;
	QVBoxLayout* MainLayout=new QVBoxLayout;
	QHBoxLayout* HL_Threshold=new QHBoxLayout;
	QHBoxLayout* HL_UndoNext=new QHBoxLayout;
	
	VL_PanelLayout->addWidget(m_L_Distribution);
	VL_PanelLayout->addWidget(F_HLine1);
	GL_Distribution->addWidget(m_L_Min, 0, 0);
	GL_Distribution->addWidget(m_LE_Min, 0, 1);
	GL_Distribution->addWidget(m_L_Step, 0, 2);
	GL_Distribution->addWidget(m_LE_Step, 0, 3);
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
	VL_PanelLayout->addStretch(1);
	GB_GlobalFilter->setLayout(VL_PanelLayout);
	MainLayout->addWidget(GB_GlobalFilter);
	
	MainLayout->addStretch(1);
	setLayout(MainLayout);
}
