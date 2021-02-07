#include "SecurityPanel.h"

SecurityPanel::SecurityPanel(QSettings *conf, QSqlDatabase* bd) : layout(new QBoxLayout(QBoxLayout::Down)), labelimage(new QLabel()), labeluid(new QLabel("UID:")),
labelname(new QLabel("Имя: ")), labeltype(new QLabel("Тип: ")), db(bd),
query(new QSqlQuery(*db)), port(nullptr), settings(conf), timer(new QTimer())
{
	setLayout(layout);

	timer->setInterval(5000);

	settings->beginGroup("Controller");

	port = new QSerialPort(settings->value("NAME").toString());

	settings->endGroup();

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

	layout->addWidget(labelimage);
	layout->addWidget(labeluid);
	layout->addWidget(labelname);
	layout->addWidget(labeltype);

	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(rec()));
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(send()));
}

SecurityPanel::~SecurityPanel()
{
	delete labelimage;
	delete labeluid;
	delete labelname;
	delete labeltype;

	delete layout;
}

void SecurityPanel::rec()
{
	QString buffer;
	QPixmap pix;

	if(port)
	{
		if(port->isOpen())
		{
			buffer = QString(port->readLine());
			//qDebug() << buffer;

			msg += buffer;

			if(buffer == "\n")
			{

				if(msg.indexOf("UID") >= 0)
				{
					uid = msg.split("UID: ")[1];
					qDebug() << "1000 uid: " << uid;
				}

				if(msg.indexOf("NAME") >= 0)
				{
					name = msg.split("NAME: ")[1];
					qDebug() << "2000 name: " << name;
				}

				if(msg.indexOf("GUEST TYPE") >= 0)
				{
					type = msg.split("GUEST TYPE: ")[1];
					qDebug() << "3000 type: " << type;
				}

				uid = uid.replace("\n", "").replace("\r", "");
				name = name.replace("\n", "").replace("\r", "");
				type =  type.replace("\n", "").replace("\r", "");

				if(uid[0] == " "){ uid = uid.replace(0, 1, ""); }

				query->exec("select pathimg from accounts where uid='" + uid + "' and name='" + name + "' and type='" + type + "'");

				if(query->next())
				{
					qDebug() << "YES!";

					pix.load(query->value(0).toString());

					port->write("OPENDOOR");

					//qDebug() << uid;
					//qDebug() << type;
					//qDebug() << name;

					labelimage->setPixmap(pix);
					labeluid->setText(labeluid->text() + uid);
					labelname->setText(labelname->text() + name);
					labeltype->setText(labeltype->text() + type);

					msg = "";

					uid = "";
					type = "";
					name = "";
				}
				else
				{
					qDebug() << "NO!";
				}

				msg = "";
			}
		}
	}
}

void SecurityPanel::send()
{
	timer->stop();

	port->write("READ");
}
