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

#include <QMessageBox>

#include <QSerialPort>

#include <QSettings>

#include <QString>
#include <QDebug>

#include <QTimer>
#include <QTime>

class MasterPanel : public QWidget
{
	Q_OBJECT

	public:
		explicit MasterPanel(QSettings*);
		~MasterPanel();

	private	slots:
		void clickButWrite();
		void rec();
		void send();
		void finish();

	private:
		QBoxLayout *layout;

		QLineEdit *linename;

		QRadioButton *modestudent;
		QRadioButton *modeteacher;

		QTextEdit *linelog;

		QPushButton *butwrite;

		QProgressBar *progress;

		QSerialPort *port;

		QSettings *settings;

		QTimer *timer;

		QString msg;

		bool click;
};

#endif

