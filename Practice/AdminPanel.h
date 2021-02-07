#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include <QMessageBox>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>

#include <QSettings>

//#include <QCryptographicHash>

class AdminPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit AdminPanel(QSettings *);
		~AdminPanel();

	private slots:
		void writeConf();

	private:
		QBoxLayout *layout;

		QLineEdit *linehost;
		QLineEdit *linelogin;
		QLineEdit *linepassword;
		QLineEdit *linecontrollername;

		QPushButton *butwriteconf;

		QSettings *settings;
};

#endif
