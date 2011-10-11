#ifndef FVLENGTHGUI_H
#define FVLENGTHGUI_H

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

#include "FiberDisplay.h"

class FVLengthGUI : public QWidget
{
	Q_OBJECT
	
	public:
		FVLengthGUI(QWidget* parent, FiberDisplay* Display);
		void LengthCalculation();
		std::vector<int> GetThresholdedIds(int LowerTh, int UpperTh);
		bool IntIsIn(int x, std::vector<int> List);
		double GetMaxLength();
		double GetMinLength();
		void InitLengthColorMap();
		void SetFiberOpacity(vtkIdType Id, double Alpha);
		void SetDisplay(FiberDisplay* Display);
		
	protected slots:
		void LengthComputation();
		void PreviousAction();
		
	signals:
		void PreviousClicked();
		
	private:
		QSpinBox* m_SB_NbBars;
		QLabel* m_L_NbBars;
		QSpinBox* m_SB_LowerTh;
		QSpinBox* m_SB_UpperTh;
		QLabel* m_L_LowerTh;
		QLabel* m_L_UpperTh;
		QPushButton* m_PB_LengthComputation;
		QPushButton* m_PB_Previous;
		std::vector<double> m_Length;
		vtkSmartPointer<vtkLookupTable> m_ColorMap;
		FiberDisplay* m_Display;
		QwtPlot* m_HistPlot;
		QwtPlotHistogram* m_Hist;
		
};

#endif
