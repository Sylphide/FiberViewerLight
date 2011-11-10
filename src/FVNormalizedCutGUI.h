#ifndef FVNORMCUTGUI_H
#define FVNORMCUTGUI_H

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

#include "FVPanelGUI.h"

class FVNormCutGUI: public FVPanelGUI
{
	Q_OBJECT
	
	public:
		FVNormCutGUI(QWidget* Parent, FiberDisplay* Display);
		void InitWeight();
		void ApplyWeight();
		double ComputeMeanDistance(int Source,int Target);
		std::vector<int> GetMark(){return m_Mark;}
		int GetNumberOfClasse();
		void GenerateClass();
		double Assoc(int BeginId, int EndId, bool All=false);
		double Cut(int Previous, int Next);
		std::vector<int> FillMark(std::vector<std::vector<double> > Weight, int& ClusterDone, int NumberOfCluster);
		
	public slots:
		void UndoAction();
		void NextAction();
// 		void setLCDValue(int Value);
		
	private:
		std::vector<int> m_Mark;
		std::vector<std::vector<double> > m_Weight;
		QLabel* m_L_Cluster;
// 		QLabel* m_L_Scale;
		QSpinBox* m_SB_Cluster;
// 		QSlider* m_S_Scale;
// 		QLCDNumber* m_LCD_Scale;
};

#endif
