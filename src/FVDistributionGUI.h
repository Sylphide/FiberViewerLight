#ifndef FVGLOBALGUI_H
#define FVGLOBALGUI_H

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QApplication>
#include <QSpinBox>
#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_histogram.h"
#include "qwt_series_data.h"
#include "qwt_interval.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include <vtkPolyDataWriter.h>

#include "FVPanelGUI.h"
#include "FVDisplayClassGUI.h"

class FVDistributionGUI : public FVPanelGUI
{
	Q_OBJECT
	
	public:
		FVDistributionGUI(QWidget* Parent, FiberDisplay* Display);
		void ClearHist();
		void InitDistance();
		void SetMethod(std::string Sender);
		std::vector<int> GetMark(){return m_Mark;}
		void ApplyGravity(int NbFibers);
		void ApplyHausdorff(int NbFibers);
		void ApplyMean(int NbFibers);
		void GenerateClasse(std::vector<std::vector<double> > Distance, double Threshold);
		int GetNumberOfClasse();
		void ComputeCOM();
		double ComputeGravDist(int source,int target);
		double ComputeHausDist(int Source,int Target);
		double ComputeMeanDistance(int Source,int Target);
		
	protected slots:
		void NextAction();
		void Plot();
		void UndoAction();
		
	private:
		QLabel* m_L_Min;
		QLabel* m_L_Step;
		QLabel* m_L_Max;
		QLabel* m_L_Threshold;
		QLabel* m_L_Distribution;
		QLabel* m_L_ChooseThreshold;
		QLineEdit* m_LE_Min;
		QLineEdit* m_LE_Step;
		QLineEdit* m_LE_Max;
		QLineEdit* m_LE_Threshold;
		QPushButton* m_PB_ComputeDistribution;
		QwtPlot* m_HistPlot;
		QwtPlotHistogram* m_Hist;
		
		std::string m_Sender;
		std::vector<std::vector<double> >m_Com;
		std::vector<int> m_Mark;
		std::vector<std::vector<double> >m_Distance;
		
};

#endif
