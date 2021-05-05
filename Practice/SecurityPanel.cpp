#include "SecurityPanel.h"

SecurityPanel::SecurityPanel(QSettings *conf, QSqlDatabase *bd) : layout(new QBoxLayout(QBoxLayout::Down)), labelimage(new QLabel()), labeluid(new QLabel("UID:")),
labelname(new QLabel("Фамилия Имя Отчество: ")), labeltype(new QLabel("Тип: ")),labelcourse(new QLabel("Курс: ")),
labelgroup(new QLabel("Группа: ")),	db(bd), query(new QSqlQuery(*db)),
port(nullptr), settings(conf), timer(new QTimer())
{
	setLayout(layout);

	timer->setInterval(5000);

	settings->beginGroup("CONTROLLER");

	port = new QSerialPort(settings->value("NAME").toString());

	settings->endGroup();

	settings->beginGroup("ACCESS");

	accesslevel = settings->value("ACCESSLEVEL").toString();
	qDebug() << accesslevel;

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
	layout->addWidget(labelcourse);
	layout->addWidget(labelgroup);

	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(rec()));
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(send()));
}

SecurityPanel::~SecurityPanel()
{
	delete labelimage;
	delete labeluid;
	delete labelname;
	delete labeltype;
	delete labelcourse;
	delete labelgroup;

	delete layout;
}

void SecurityPanel::rec()
{
	QString buffer;

	QSqlQuery *query_two = nullptr;

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

				//query->exec("select * from accounts where uid='" + uid + "'");

				query->prepare("select * from accounts where uid = :UID");

				query->bindValue(":UID", uid);

				query->exec();

				if(query->next())
				{
					query_two = new QSqlQuery(*db);

					//query_two->exec("select number_course from courses where id = " + QString::number(query->value(9).toInt()));

					query_two->prepare("select access from filials_and_accounts join accounts on accounts.id = filials_and_accounts.id_account join filials on filials.id = filials_and_accounts.id_filial where accounts.uid = :UID and filials.address = :Address");

					query_two->bindValue(":Address", accesslevel);
					query_two->bindValue(":UID", uid);

					if(!query_two->exec())
					{
						qDebug() << "Error code: " << query->lastError().nativeErrorCode() << " Error: " << query->lastError().text();
					}

					if(query_two->next())
					{
						qDebug() << "ARBUZ: " << query_two->value(0).toBool();

						if(!(query_two->value(0).toBool()))
						{
							delete query_two;

							return;
						}
					}

					delete query_two;

					qDebug() << "YES!";

					pix.load("Imgs/" + query->value(6).toString());

					port->write("OPENDOOR");

					//qDebug() << uid;
					//qDebug() << type;
					//qDebug() << name;

					labelimage->setPixmap(pix);
					labeluid->setText("UID: " + query->value(1).toString());
					labelname->setText("Фамилия Имя Отчество: " + query->value(3).toString() + " " + query->value(2).toString() + " " +query->value(4).toString());
					labeltype->setText("Тип: " + query->value(5).toString());

					if(query->value(5).toString() == "Студент")
					{
						query_two = new QSqlQuery(*db);

						//query_two->exec("select number_course from courses where id = " + QString::number(query->value(9).toInt()));

						query_two->prepare("select number_course from courses where id = :Id");

						query_two->bindValue(":Id", query->value(9).toString());

						query_two->exec();

						if(query_two->next())
						{
							labelcourse->setText("Курс: " + query_two->value(0).toString());
						}
						//query_two->exec("select name from `groups` where id = " + QString::number(query->value(8).toInt()));

						query_two->prepare("select name from `groups` where id = :Id");

						query_two->bindValue(":Id", query->value(8).toString());

						query_two->exec();

						if(query_two->next())
						{
							labelgroup->setText("Группа:" + query_two->value(0).toString());
						}

						delete query_two;
				}
				else
				{
					labelgroup->setText("");
					labelcourse->setText("");
				}

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

	QMessageBox::warning(this, "Оповещение", "Контроллер переведен в режим чтения и готов к работе!");
}
