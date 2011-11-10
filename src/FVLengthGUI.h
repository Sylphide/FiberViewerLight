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
#include <vtkPolyDataWriter.h>

#include "FVPanelGUI.h"

class FVLengthGUI : public FVPanelGUI
{
	Q_OBJECT
	
	public:
		FVLengthGUI(QWidget* Parent, FiberDisplay* Display);
		void LengthCalculation();
		double GetMaxLength();
		double GetMinLength();
		void InitLengthColorMap();
		void SetFiberOpacity(vtkIdType Id, int Alpha);
		void InitLengthPanel();
		
	protected slots:
		void LengthComputation();
		void UndoAction();
		void OkAction();
		
	private:
		QSpinBox* m_SB_NbBars;
		QLabel* m_L_NbBars;
		QSpinBox* m_SB_LowerTh;
		QSpinBox* m_SB_UpperTh;
		QLabel* m_L_LowerTh;
		QLabel* m_L_UpperTh;
		QPushButton* m_PB_LengthComputation;
		std::vector<double> m_Length;
		vtkSmartPointer<vtkLookupTable> m_ColorMap;
		QwtPlot* m_HistPlot;
		QwtPlotHistogram* m_Hist;
};

#endif
