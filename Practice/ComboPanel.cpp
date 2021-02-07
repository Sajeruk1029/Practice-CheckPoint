#include "ComboPanel.h"

ComboPanel::ComboPanel(unsigned char version, QSettings *conf, QSqlDatabase *db) : layout(new QBoxLayout(QBoxLayout::Down)), tab(new QTabWidget()), adminPanel(nullptr),
masterPanel(nullptr), securityPanel(nullptr)
{
	setWindowTitle("Контрольно-пропускной пункт");
	setLayout(layout);

	switch(version)
	{
		case 1:
		{
			adminPanel = new AdminPanel(conf);

			setFixedSize(300, 200);

			tab->addTab(adminPanel, "Панель администратора");
			break;
		}

		case 2:
		{
			masterPanel = new MasterPanel(conf);

			setFixedSize(500, 500);

			tab->addTab(masterPanel, "Панель инженера");
			break;
		}

		case 3:
		{
			securityPanel = new SecurityPanel(conf, db);

			setFixedSize(500, 500);

			tab->addTab(securityPanel, "Панель охраны");
			break;
		}
	}

	layout->addWidget(tab);
}

ComboPanel::~ComboPanel()
{
	delete tab;

	if(adminPanel){ delete adminPanel; }
	if(masterPanel){ delete masterPanel; }
	if(securityPanel){ delete securityPanel; }

	delete layout;
}
