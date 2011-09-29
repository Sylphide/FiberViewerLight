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
#include "vtkQtRenderWindow.h"
#include "vtkQtRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "QVTKWidget.h"
#include "FiberViewerLightGUI.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
// 	FiberViewerLightGUI* GUIWin=new FiberViewerLightGUI;
// 	GUIWin->show();
	FiberViewerLightGUI fenetre;
// 	QVTKWidget* render=new QVTKWidget;
// 	QVBoxLayout* Vlayout=new QVBoxLayout;
// 	QPushButton* test=new QPushButton("test");
// 	fenetre.setLayout(Vlayout);
// 	int i;
// 	static float x[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
// 									{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
// 	static vtkIdType pts[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
// 									{1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
// 		
// 				// We'll create the building blocks of polydata including data attributes.
// 	vtkPolyData *cube = vtkPolyData::New();
// 	vtkPoints *points = vtkPoints::New();
// 	vtkCellArray *polys = vtkCellArray::New();
// 	vtkFloatArray *scalars = vtkFloatArray::New();
// 	
// 			// Load the point, cell, and data attributes.
// 	for (i=0; i<8; i++) points->InsertPoint(i,x[i]);
// 	for (i=0; i<6; i++) polys->InsertNextCell(4,pts[i]);
// 	for (i=0; i<8; i++) scalars->InsertTuple1(i,i);
// 	
// 			// We now assign the pieces to the vtkPolyData.
// 	cube->SetPoints(points);
// 	points->Delete();
// 	cube->SetPolys(polys);
// 	polys->Delete();
// 	cube->GetPointData()->SetScalars(scalars);
// 	scalars->Delete();
// 	
// 			// Now we'll look at it.
// 	vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
// 	cubeMapper->SetInput(cube);
// 	cubeMapper->SetScalarRange(0,7);
// 	vtkActor *cubeActor = vtkActor::New();
// 	cubeActor->SetMapper(cubeMapper);
// 	
// 	// The usual rendering stuff.
// 	vtkCamera *camera = vtkCamera::New();
// 	camera->SetPosition(1,1,1);
// 	camera->SetFocalPoint(0,0,0);
// 	
// 	vtkRenderer *renderer = vtkRenderer::New();
// // 	vtkQtRenderWindow *renWin = new vtkQtRenderWindow(&render);
// 	vtkRenderWindow *renWin = vtkQtRenderWindow::New();
// 	renWin->AddRenderer(renderer);
// 	
// // 	vtkQtRenderWindowInteractor *iren = vtkQtRenderWindowInteractor::New();
// 	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
// 	iren->SetRenderWindow(renWin);
// 	
// 	renderer->AddActor(cubeActor);
// 	renderer->SetActiveCamera(camera);
// 	renderer->ResetCamera();
// 	renderer->SetBackground(1,1,1);
// 	
// 	renWin->SetSize(1000,1000);
// 	
// 		// interact with data
// 	iren->Start();
// // 	renWin->Render();
// 	
// 	render->GetRenderWindow()->AddRenderer(renderer);
// 	Vlayout->addWidget(render);
// 	Vlayout->addWidget(test);
	fenetre.show();
	
	return app.exec();
}
