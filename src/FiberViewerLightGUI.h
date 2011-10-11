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
#include <vtkPolyDataWriter.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkLookupTable.h>

#include "FiberDisplay.h"
#include "FVLengthGUI.h"



class FiberViewerLightGUI: public QWidget
{
	Q_OBJECT
			
	public:
		FiberViewerLightGUI(QWidget* parent=0);
		void InitWidgets();
		vtkSmartPointer<vtkPolyData> LoadVTK(std::string FileName);
		
		
	protected slots:
		void BrowserVTKInput();
		void EnterVTKInput();
		void BrowserSaveVTK();
		void OpenLengthPanel();
		void CloseLengthPanel();
		
	private:
		QGroupBox* m_GB_ActionPanel;
		QLineEdit* m_LE_VTKInput;
		QToolButton* m_TB_BrowserVTKInput;
		QLabel* m_L_SelectFiber;
		QPushButton* m_PB_SaveVTK;
		QPushButton* m_PB_Length;
		std::string m_VtkFileName;
		
		FVLengthGUI* m_LengthGUI;
		FiberDisplay* m_Display;
};

#endif
