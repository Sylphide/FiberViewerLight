#ifndef FVDISPLAYCLASSGUI_H
#define FVDISPLAYCLASSGUI_H

#include <QTableView>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStringList>

#include <vtkFloatArray.h>
#include <vtkCellArray.h>

#include "FVPanelGUI.h"

class FVDisplayClassGUI: public FVPanelGUI
{
	Q_OBJECT
	
	public:
		enum Sender{Distribution,NCut};
		FVDisplayClassGUI(QWidget* Parent, FiberDisplay* Display);
		void SetModel(std::vector<int> Mark, int NbClasse);
		std::vector<int> GetNbFibersPerClasse(int NbClasse);
		void CompleteMark();
		void SetSender(FVDisplayClassGUI::Sender sender);
		FVDisplayClassGUI::Sender GetSender();
		
		
	public slots:
		void UndoAction();
		void ColorFiber();
		void NextAction();
		
	private:
		Sender m_Sender;
		QTableView* m_TV_View;
		QPushButton* m_PB_Show;
		vtkLookupTable* m_ColorMap;
		std::vector<int> m_Mark;
		std::vector<int> m_PreviousAlpha;
};

#endif
