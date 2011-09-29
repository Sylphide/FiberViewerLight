#ifndef FIBERVIEWERLIGHTGUI_H
#define FIBERVIEWERLIGHTGUI_H


#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>

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



class FiberViewerLightGUI: public QWidget
{
	Q_OBJECT
			
	public:
		FiberViewerLightGUI(QWidget* parent=0);
		void InitWidgets();
		void InitRenderer();
		void Render(vtkSmartPointer<vtkPolyData> PolyData);
		bool LoadVTK(std::string FileName);
		
	protected slots:
		void BrowserVTKInput();
		void EnterVTKInput();
		
	private:
		QLineEdit* m_LE_VTKInput;
		QToolButton* m_TB_BrowserVTKInput;
		QLabel* m_L_SelectFiber;
		QVTKWidget* m_VTKW_RenderWin;
		vtkSmartPointer<vtkPolyData> m_PolyData;
};

#endif