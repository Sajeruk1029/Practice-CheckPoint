#ifndef COMBOPANEL
#define COMBOPANEL

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QTabWidget>

#include <QSettings>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>

#include "AdminPanel.h"
#include "MasterPanel.h"
#include "SecurityPanel.h"
#include "Filials.h"

class ComboPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit ComboPanel(unsigned char, QSettings*, QSqlDatabase*);
		~ComboPanel();

	private:
		QBoxLayout *layout;

		QTabWidget *tab;

		AdminPanel *adminPanel;
		MasterPanel *masterPanel;
		SecurityPanel *securityPanel;

		Filials *filials;
};

#endif
