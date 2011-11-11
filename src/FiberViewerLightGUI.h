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
#include <QProgressBar>

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
#include "FVDistributionGUI.h"
#include "FVNormalizedCutGUI.h"
#include "PlanSetting.h"

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
		void BrowserVTKOutput();
		void SaveVTK();
		void OpenLengthPanel();
		void OpenDistributionPanel();
		void OpenNormCutPanel();
		void CloseLengthPanel(FVPanelGUI::ExitSignal Direction);
		void CloseDistributionPanel(FVPanelGUI::ExitSignal Direction);
		void CloseDisplayClass(FVPanelGUI::ExitSignal Direction);
		void CloseNormCut(FVPanelGUI::ExitSignal Direction);
		void OpenPlanSetting();
		void UndoAction();
		void RedoAction();
		
	private:
		QGroupBox* m_GB_ActionPanel;
		QGroupBox* m_GB_LengthPanel;
		QGroupBox* m_GB_DistributionPanel;
		QGroupBox* m_GB_DisplayClassPanel;
		QGroupBox* m_GB_NormCutPanel;
		QLineEdit* m_LE_VTKInput;
		QLineEdit* m_LE_VTKOutput;
		QToolButton* m_TB_BrowserVTKInput;
		QToolButton* m_TB_BrowserVTKOutput;
		QLabel* m_L_VTKInput;
		QLabel* m_L_VTKOutput;
		QLabel* m_L_IO;
		QLabel* m_L_Processing;
		QLabel* m_L_Navigation;
		QPushButton* m_PB_Length;
		QPushButton* m_PB_Gravity;
		QPushButton* m_PB_Hausdorff;
		QPushButton* m_PB_Mean;
		QPushButton* m_PB_NormCut;
		QPushButton* m_PB_Undo;
		QPushButton* m_PB_Redo;
		QPushButton* m_PB_SaveVTK;
		QPushButton* m_PB_Plane;
		QProgressBar* m_ProgressBar;
		std::string m_VtkFileName;
		
		FVLengthGUI* m_LengthGUI;
		FVDistributionGUI* m_DistributionGUI;
		FVDisplayClassGUI* m_DisplayClassGUI;
		FVNormCutGUI* m_NormCutGUI;
		vtkSmartPointer<vtkLookupTable> m_RedMap;
		FiberDisplay* m_Display;
		PlanSetting* m_PlanSetting;
};

#endif
