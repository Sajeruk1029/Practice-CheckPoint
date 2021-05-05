#ifndef MASTERPANEL
#define MASTERPANEL

#include <QObject>

#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QListWidget>

#include <QListWidgetItem>

#include <QMessageBox>
#include <QFileDialog>

#include <QSerialPort>

#include <QSettings>

#include <QString>
#include <QDebug>

#include <QTimer>
#include <QTime>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QFile>

class MasterPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit MasterPanel(QSettings*, QSqlDatabase*);
		~MasterPanel();

	private	slots:
		void clickButWrite();
		void rec();
		void send();
		void finish();
		void choose_course();
		void set_filial_one();
		void set_filial_two();
		void set_filial_three();
		void clickButChoosePhoto();
		void timeout_timer();

	private:
		QBoxLayout *layout;

		QLineEdit *linename;
		QLineEdit *linelastname;
		QLineEdit *linepatronymic;
		//QLineEdit *lineaccesslevel;

		QComboBox *groups;
		QComboBox *courses;

		QRadioButton *modestudent;
		QRadioButton *modeteacher;

		QCheckBox *filial_one;
		QCheckBox *filial_two;
		QCheckBox *filial_three;

		QTextEdit *linelog;

		QPushButton *butwrite;
		QPushButton *butchoosephoto;

		QProgressBar *progress;

		QLabel *labelhintpathphoto;

		QListWidget *listbox;

		QSerialPort *port;

		QSettings *settings;

		QTimer *timer;
		QTimer *timer_timeout;

		QSqlDatabase *db;
		QSqlQuery *query;

		QListWidgetItem *item;

		unsigned char access_level;

		QString msg;
		QString filePhotoName;
		QString uid;

		bool click;
};

#endif

