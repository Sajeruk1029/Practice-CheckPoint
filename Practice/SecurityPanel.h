#ifndef SECURITYPANEL
#define SECURITYPANEL

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>

#include <QMessageBox>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QSerialPort>

#include <QSettings>

#include <QPixmap>

#include <QDebug>
#include <QString>

#include <QTimer>
#include <QTime>

class SecurityPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit SecurityPanel(QSettings*, QSqlDatabase*);
		~SecurityPanel();

	private slots:
		void rec();
		void send();

	private:
		QBoxLayout *layout;

		QLabel *labelimage;
		QLabel *labeluid;
		QLabel *labelname;
		QLabel *labeltype;
		QLabel *labelcourse;
		QLabel *labelgroup;

		QSqlDatabase *db;
		QSqlQuery *query;

		QSerialPort *port;

		QSettings *settings;

		QTimer *timer;

		QString uid;
		QString name;
		QString type;
		QString msg;
		QString accesslevel;

};

#endif
