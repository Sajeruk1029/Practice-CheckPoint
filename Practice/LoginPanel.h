#ifndef LOGINPANELH
#define LOGINPANELH

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <QMessageBox>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QSettings>
#include <QFile>

#include <QDebug>

#include "ComboPanel.h"

class LoginPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit LoginPanel();
		~LoginPanel();

	private slots:
		void clickButLogin();

	private:
		QBoxLayout *layout;

		QLineEdit *linelogin;
		QLineEdit *linepassword;

		QPushButton *butlogin;

		ComboPanel *combp;

		QSqlDatabase *db;
		QSqlQuery *query;

		QSettings *settings;
};

#endif
