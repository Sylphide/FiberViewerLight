#ifndef FIBERVIEWERLIGHTGUI_H
#define FIBERVIEWERLIGHTGUI_H


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

#include "QVTKWidget.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPolyDataReader.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkLookupTable.h>



class FiberViewerLightGUI: public QWidget
{
	Q_OBJECT
			
	public:
		FiberViewerLightGUI(QWidget* parent=0);
		void InitWidgets();
		void InitRenderer();
		void InitLengthColorMap();
		void StartRenderer(vtkSmartPointer<vtkPolyData> PolyData);
		bool LoadVTK(std::string FileName);
		vtkSmartPointer<vtkPolyData> GetLineFromPolyData(int id, vtkSmartPointer<vtkPolyData> PolyData);
		void GetFiberColor(double coef, double color[]);
		void LengthCalculation();
		double GetMaxLength();
		double GetMinLength();
		std::vector<int> GetThresholdedIds(int LowerTh, int UpperTh);
		bool IntIsIn(int x, std::vector<int> List);
		void SetFiberOpacity(vtkIdType Id, double Alpha);
		
	protected slots:
		void BrowserVTKInput();
		void EnterVTKInput();
		void BrowserSaveVTK();
		void LengthComputation();
		
	private:
		QLineEdit* m_LE_VTKInput;
		QToolButton* m_TB_BrowserVTKInput;
		QLabel* m_L_SelectFiber;
		QPushButton* m_PB_SaveVTK;
		QSpinBox* m_SB_LowerTh;
		QSpinBox* m_SB_UpperTh;
		QLabel* m_L_LowerTh;
		QLabel* m_L_UpperTh;
		QPushButton* m_PB_Test;
		QVTKWidget* m_VTKW_RenderWin;
		vtkSmartPointer<vtkPolyData> m_PolyData;
		vtkSmartPointer<vtkPolyData> m_ModifiedPolyData;
		vtkSmartPointer<vtkLookupTable> m_ColorMap;
		std::vector<double> m_Length;
		std::string m_VtkFileName;
};

#endif
