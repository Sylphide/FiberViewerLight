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

#include "FiberDisplay.h"

class FVGlobalGUI : public QWidget
{
	Q_OBJECT
	
	public:
		FVGlobalGUI(QWidget* parent, FiberDisplay* Display);
		
		
	protected slots:
		
	signals:
		
	private:
		FiberDisplay* m_Display;
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
		QPushButton* m_PB_Undo;
		QPushButton* m_PB_Next;
		std::vector<std::vector<int> > m_Gravity;
		vtkSmartPointer<vtkLookupTable> m_ColorMap;
		QwtPlot* m_HistPlot;
		QwtPlotHistogram* m_Hist;
		
		
};

#endif
