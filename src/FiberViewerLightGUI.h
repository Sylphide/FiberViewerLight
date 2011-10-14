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
#include <QMessageBox>

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
#include "FVGlobalGUI.h"



class FiberViewerLightGUI: public QWidget
{
	Q_OBJECT
			
	public:
		FiberViewerLightGUI(QWidget* parent=0);
		void InitWidgets();
		void InitRedMap(vtkPolyData* PolyData);
		vtkSmartPointer<vtkPolyData> LoadVTK(std::string FileName);
		void SetAtLastAlphas();
		
	protected slots:
		void BrowserVTKInput();
		void EnterVTKInput();
		void BrowserSaveVTK();
		void OpenLengthPanel();
		void OpenGlobalPanel();
		void CloseLengthPanel();
		void Undo();
		
	private:
		QGroupBox* m_GB_ActionPanel;
		QLineEdit* m_LE_VTKInput;
		QToolButton* m_TB_BrowserVTKInput;
		QLabel* m_L_SelectFiber;
		QPushButton* m_PB_Length;
		QPushButton* m_PB_Gravity;
		QPushButton* m_PB_Hausdorff;
		QPushButton* m_PB_Mean;
		QPushButton* m_PB_Undo;
		QPushButton* m_PB_SaveVTK;
		std::string m_VtkFileName;
		
		FVLengthGUI* m_LengthGUI;
		FVGlobalGUI* m_GlobalGUI;
		vtkSmartPointer<vtkLookupTable> m_RedMap;
		FiberDisplay* m_Display;
};

#endif
