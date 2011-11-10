#include "FVPanelGUI.h"

FVPanelGUI::FVPanelGUI(QWidget* Parent, FiberDisplay* Display) : QWidget(Parent)
{
	m_Display=Display;
	m_PB_Undo=new QPushButton("Undo", this);
	m_PB_Next=new QPushButton("Next", this);
}
