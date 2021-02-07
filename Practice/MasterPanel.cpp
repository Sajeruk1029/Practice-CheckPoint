#include "MasterPanel.h"

MasterPanel::MasterPanel(QSettings *conf) : layout(new QBoxLayout(QBoxLayout::Down)), linename(new QLineEdit()), modestudent(new QRadioButton("Студент")),
modeteacher(new QRadioButton("Преподаватель")), linelog(new QTextEdit()), butwrite(new QPushButton("Запись")),
progress(new QProgressBar()), port(nullptr), settings(conf), timer(new QTimer())
{
	setLayout(layout);

	timer->setInterval(5000);

	settings->beginGroup("Controller");

	port = new QSerialPort(settings->value("NAME").toString());

	settings->endGroup();

	modeteacher->setChecked(true);

	port->setBaudRate(QSerialPort::Baud1200);
	port->setDataBits(QSerialPort::Data8);
	port->setParity(QSerialPort::NoParity);
	port->setStopBits(QSerialPort::OneStop);

		if(!port->open(QIODevice::ReadWrite))
	{
		QMessageBox::warning(this, "Ошибка", port->errorString());

		delete port;
		port = nullptr;
	}
	else
	{
		timer->start();
	}

	linename->setPlaceholderText("Имя");

	linelog->setPlaceholderText("Лог");
	linelog->setReadOnly(true);

	progress->setMaximum(4);
	progress->setMinimum(0);
	progress->setValue(progress->value() + 1);

	layout->addWidget(linename);
	layout->addWidget(modeteacher);
	layout->addWidget(modestudent);
	layout->addWidget(linelog);
	layout->addWidget(progress);
	layout->addWidget(butwrite);

	QObject::connect(butwrite, SIGNAL(clicked()), this, SLOT(clickButWrite()));
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(send()));
	QObject::connect(progress, SIGNAL(valueChanged(int)), this, SLOT(finish()));

}

MasterPanel::~MasterPanel()
{
	if(port)
	{
		port->close();

		delete port;
	}

	delete linename;

	delete modestudent;
	delete modeteacher;

	delete butwrite;

	delete progress;

	delete layout;
}

void MasterPanel::clickButWrite()
{
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(rec()));
	click = true;

		linename->setEnabled(false);

		modestudent->setEnabled(false);
		modeteacher->setEnabled(false);

		butwrite->setEnabled(false);
}

void MasterPanel::rec()
{
	QString buffer;

		if(port)
		{
			if(port->isOpen())
			{
				buffer = QString(port->readLine());
				qDebug() << buffer;

				msg += buffer;

				if(buffer == "\n")
				{
					if((msg.indexOf("Enter the name") >= 0) && (click))
					{
						qDebug() << "Yes1";
						port->write(linename->text().toUtf8());
						progress->setValue(progress->value() + 1);
					}

					if((msg.indexOf("Name writing successful") >= 0) && (click))
					{
						qDebug() << "Yes2";
						progress->setValue(progress->value() + 1);
					}

					if((msg.indexOf("Type the name") >= 0) && (click))
					{
						qDebug() << "Yes3";
						port->write((modeteacher->isChecked())? "Teacher" : "Student");
						progress->setValue(progress->value() + 1);
					}

					if((msg.indexOf("Type writing successful") >= 0) && (click))
					{
						qDebug() << "Yes4";
						progress->setValue(progress->value() + 1);
					}

					linelog->setText(linelog->toPlainText() + msg);

					msg = "";
				}

			}
		}
}

void MasterPanel::send()
{
	timer->stop();

	port->write("WRITE");
}

void MasterPanel::finish()
{
	if(progress->value() == 4)
	{
		progress->setValue(0);

		linename->setEnabled(true);

		modestudent->setEnabled(true);
		modeteacher->setEnabled(true);

		butwrite->setEnabled(true);

		linelog->setText("");

		QObject::disconnect(port, SIGNAL(readyRead()), this, SLOT(rec()));

		click = false;
	}
}
