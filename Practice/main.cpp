#include <QApplication>

#include <QSettings>
#include <QFile>
#include <QDebug>

#include "LoginPanel.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	LoginPanel loginPanel;

	//loginPanel.show();

	return app.exec();
}
