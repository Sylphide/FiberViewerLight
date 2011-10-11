#ifndef FIBERDISPLAY_H
#define FIBERDISPLAY_H

#include <QWidget>
#include <QGridLayout>
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
#include <vtkPointData.h>

class FiberDisplay: public QWidget
{
	public:
		FiberDisplay(QWidget* parent);
		void InitRenderer();
		void StartRenderer(vtkPolyData* PolyData);
		void Render();
		vtkActor* GetActor();
		vtkPolyData* GetOriginalPolyData();
		vtkPolyData* GetModifiedPolyData();
		void SetOriginalPolyData(vtkPolyData* PolyData);
		void SetModifiedPolyData(vtkPolyData* PolyData);
		vtkRenderer* GetRenderer();
		void GetFiberColor(double coef, double color[]);
		
	private:
		QVTKWidget* m_VTKW_RenderWin;
		vtkSmartPointer<vtkPolyData> m_OriginalPolyData;
		vtkSmartPointer<vtkPolyData> m_ModifiedPolyData;
		vtkSmartPointer<vtkLookupTable> m_ColorMap;
};

#endif
