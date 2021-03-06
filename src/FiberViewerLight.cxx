#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "QVTKWidget.h"
#include "FiberViewerLightGUI.h"
#include "FiberViewerLightCLP.h"


int main(int argc, char* argv[])
{
	PARSE_ARGS;
	
	QApplication app(argc, argv);
	
	FiberViewerLightGUI* GUIWin=new FiberViewerLightGUI(input,output);
	GUIWin->show();
	
	return app.exec();
}
