#include "PlanSetting.h"

PlanSetting::PlanSetting(QWidget* Parent, FiberDisplay* Display) : QWidget(Parent)
{
	m_Display=Display;
	m_PB_UpdatePlane=new QPushButton("Update Plan", this);
	m_PB_UpdatePlane->setGeometry(0,70,130,30);
	m_PB_GetParam=new QPushButton("Get Plan Param", this);
	m_PB_GetParam->setGeometry(135,70,130,30);
	m_L_X=new QLabel("X",this);
	m_L_X->setGeometry(83,0,16,17);
	m_L_Y=new QLabel("Y",this);
	m_L_Y->setGeometry(149,0,16,17);
	m_L_Z=new QLabel("Z",this);
	m_L_Z->setGeometry(218,0,16,17);
	m_L_Origin=new QLabel("Origin",this);
	m_L_Origin->setGeometry(0,20,38,21);
	m_L_Normal=new QLabel("Normal",this);
	m_L_Normal->setGeometry(0,45,45,21);
	m_LE_XOrigin=new QLineEdit(this);
	m_LE_XOrigin->setGeometry(50,20,66,23);
	m_LE_YOrigin=new QLineEdit(this);
	m_LE_YOrigin->setGeometry(119,20,66,23);
	m_LE_ZOrigin=new QLineEdit(this);
	m_LE_ZOrigin->setGeometry(188,20,66,23);
	m_LE_XNormal=new QLineEdit(this);
	m_LE_XNormal->setGeometry(50,45,66,23);
	m_LE_YNormal=new QLineEdit(this);
	m_LE_YNormal->setGeometry(119,45,66,23);
	m_LE_ZNormal=new QLineEdit(this);
	m_LE_ZNormal->setGeometry(188,45,66,23);
	setFixedSize(300,108);
	
	connect(m_PB_UpdatePlane, SIGNAL(clicked()), this, SLOT(UpdatePlan()));
	connect(m_PB_GetParam, SIGNAL(clicked()), this, SLOT(GetParam()));
}

void PlanSetting::UpdatePlan()
{
	m_Display->GetPlan()->SetNormal(m_LE_XNormal->text().toDouble(),m_LE_YNormal->text().toDouble(),m_LE_ZNormal->text().toDouble());
	m_Display->GetPlan()->SetOrigin(m_LE_XOrigin->text().toDouble(),m_LE_YOrigin->text().toDouble(),m_LE_ZOrigin->text().toDouble());
	m_Display->Render();
}

void PlanSetting::GetParam()
{
	double Param[3];
	std::ostringstream Value;
	
	m_Display->GetPlan()->GetOrigin(Param);
	Value<<Param[0];
	m_LE_XOrigin->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[1];
	m_LE_YOrigin->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[2];
	m_LE_ZOrigin->setText(Value.str().c_str());
	Value.str("");
	m_Display->GetPlan()->GetNormal(Param);
	Value<<Param[0];
	m_LE_XNormal->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[1];
	m_LE_YNormal->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[2];
	m_LE_ZNormal->setText(Value.str().c_str());
}

