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
#include <vtkPolyDataWriter.h>

class FiberDisplay: public QWidget
{
	public:
		FiberDisplay(QWidget* parent);
		void InitRenderer();
		void InitAlphas();
		
		vtkPolyData* GetOriginalPolyData();
		vtkPolyData* GetModifiedPolyData();
		vtkRenderer* GetRenderer();
		vtkActor* GetActor();
		std::vector<int> GetLastAlpha();
		void GetFiberColor(double coef, double color[]);
		
		void SetOriginalPolyData(vtkPolyData* PolyData);
		void SetModifiedPolyData(vtkPolyData* PolyData);
		void SetLookupTable(vtkLookupTable* RedMap);
		void SetLastAlpha(std::vector<int> Alphas);
		
		void PushBackAlpha(std::vector<int> Alpha);
		void PopBackAlpha();
		void StartRenderer(vtkPolyData* PolyData);
		void Render();
		void DuplicateLastAlpha();
		void UpdateCells();
		
	private:
		QVTKWidget* m_VTKW_RenderWin;
		vtkSmartPointer<vtkPolyData> m_OriginalPolyData;
		vtkSmartPointer<vtkPolyData> m_ModifiedPolyData;
		std::vector<std::vector<int> > m_Alphas;
};

#endif
