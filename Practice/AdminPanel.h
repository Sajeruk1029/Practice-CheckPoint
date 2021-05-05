#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

#include <QMessageBox>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>

#include <QSettings>

#include <QSerialPortInfo>

class AdminPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit AdminPanel(QSettings *);
		~AdminPanel();

	private slots:
		void writeConf();
		void portrefresh();

	private:
		QBoxLayout *layout;

		QLineEdit *linehost;
		QLineEdit *linelogin;
		QLineEdit *linepassword;
		QLineEdit *lineaccess;

		QComboBox *ports;

		QPushButton *butwriteconf;
		QPushButton *butportrefresh;

		QSettings *settings;

		QSqlDatabase *db;
};

#endif
