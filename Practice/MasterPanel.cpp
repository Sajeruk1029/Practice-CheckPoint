#include "MasterPanel.h"

MasterPanel::MasterPanel(QSettings *conf, QSqlDatabase *bd) : layout(new QBoxLayout(QBoxLayout::Down)), linename(new QLineEdit()), linelastname(new QLineEdit()),
linepatronymic(new QLineEdit()), groups(new QComboBox()), courses(new QComboBox()),
modestudent(new QRadioButton("Студент")), modeteacher(new QRadioButton("Преподаватель")), filial_one(new QCheckBox("Филиал первый")),
filial_two(new QCheckBox("Филиал второй")), filial_three(new QCheckBox("Филиал третий")), linelog(new QTextEdit()),
butwrite(new QPushButton("Запись")), butchoosephoto(new QPushButton("Выбрать фото")), progress(new QProgressBar()),
labelhintpathphoto(new QLabel("Путь до фото: ")), listbox(new QListWidget()),
port(nullptr), settings(conf), timer(new QTimer()),
timer_timeout(new QTimer()), db(bd), query(new QSqlQuery(*db)),
access_level(0)
{
	setLayout(layout);

	timer->setInterval(5000);
	timer_timeout->setInterval(300000);

	settings->beginGroup("CONTROLLER");

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

		butwrite->setEnabled(false);
	}
	else
	{
		timer->start();
	}

	linename->setPlaceholderText("Имя");
	linelastname->setPlaceholderText("Фамилия");
	linepatronymic->setPlaceholderText("Отчество");

	linelog->setPlaceholderText("Лог");
	linelog->setReadOnly(true);

	progress->setMaximum(4);
	progress->setMinimum(0);
	progress->setValue(progress->value() + 1);

	linename->setEnabled(false);
	linelastname->setEnabled(false);
	linepatronymic->setEnabled(false);
	courses->setEnabled(false);
	groups->setEnabled(false);
	butchoosephoto->setEnabled(false);
	modeteacher->setEnabled(false);
	modestudent->setEnabled(false);
	listbox->setEnabled(false);

	query->exec("select address from filials");

	while(query->next())
	{
		item = new QListWidgetItem();

		item->setData(Qt::DisplayRole, query->value(0).toString());
		item->setData(Qt::CheckStateRole, Qt::Unchecked);

		listbox->addItem(item);
	}

	qDebug() << listbox->count() << '\n' << '\n';

	for(int counter = 0; counter < listbox->count(); ++counter)
	{
		qDebug() << listbox->item(counter)->text() << '\t' << listbox->item(counter)->checkState();
	}

	layout->addWidget(linename);
	layout->addWidget(linelastname);
	layout->addWidget(linepatronymic);
	layout->addWidget(courses);
	layout->addWidget(groups);
	layout->addWidget(butchoosephoto);
	layout->addWidget(labelhintpathphoto);
	layout->addWidget(modeteacher);
	layout->addWidget(modestudent);
	layout->addWidget(listbox);
//	layout->addWidget(filial_one);
//	layout->addWidget(filial_two);
//	layout->addWidget(filial_three);
	layout->addWidget(linelog);
	layout->addWidget(progress);
	layout->addWidget(butwrite);

	query->exec("select number_course from courses");

	while(query->next())
	{
		courses->addItem(query->value(0).toString());
	}

	courses->setCurrentIndex(0);

	query->exec("select name from `groups` where number_course = 1");

	while(query->next())
	{
		groups->addItem(query->value(0).toString());
	}

	groups->setCurrentIndex(0);

	QObject::connect(butwrite, SIGNAL(clicked()), this, SLOT(clickButWrite()));
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(send()));
	QObject::connect(progress, SIGNAL(valueChanged(int)), this, SLOT(finish()));
	QObject::connect(courses, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_course()));
	QObject::connect(filial_one, SIGNAL(stateChanged(int)), this, SLOT(set_filial_one()));
	QObject::connect(filial_two, SIGNAL(stateChanged(int)), this, SLOT(set_filial_two()));
	QObject::connect(filial_three, SIGNAL(stateChanged(int)), this, SLOT(set_filial_three()));
	QObject::connect(butchoosephoto, SIGNAL(clicked()), this, SLOT(clickButChoosePhoto()));
	QObject::connect(timer_timeout, SIGNAL(timeout()), this, SLOT(timeout_timer()));
}

MasterPanel::~MasterPanel()
{
	if(query){ delete query; }

	if(port)
	{
		port->close();

		delete port;
	}

	delete linename;
	delete linelastname;
	delete linepatronymic;

	delete groups;
	delete courses;

	delete modestudent;
	delete modeteacher;

	delete filial_one;
	delete filial_two;
	delete filial_three;

	delete butwrite;

	delete progress;

	delete listbox;

	delete layout;
}

void MasterPanel::clickButWrite()
{
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(rec()));
	click = true;

	timer_timeout->start();

	linename->setEnabled(false);
	linelastname->setEnabled(false);
	linepatronymic->setEnabled(false);

	modestudent->setEnabled(false);
	modeteacher->setEnabled(false);

	courses->setEnabled(false);
	groups->setEnabled(false);

	filial_one->setEnabled(false);
	filial_two->setEnabled(false);
	filial_three->setEnabled(false);

	butchoosephoto->setEnabled(false);
	butwrite->setEnabled(false);

	listbox->setEnabled(false);

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
					linelog->setText(linelog->toPlainText() + msg);

					if((msg.indexOf("Enter the name") >= 0) && (click))
					{
						qDebug() << "Yes1";
						port->write((linename->text() + " " + linelastname->text() + " " + linepatronymic->text()).toUtf8());
						progress->setValue(progress->value() + 1);
						linelog->setText(linelog->toPlainText() + (linename->text() + " " + linelastname->text() + " " + linepatronymic->text()));
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
						linelog->setText(linelog->toPlainText() + ((modeteacher->isChecked())? "Teacher" : "Student"));
					}

					if((msg.indexOf("Type writing successful") >= 0) && (click))
					{
						qDebug() << "Yes4";
						progress->setValue(progress->value() + 1);
					}

					if(msg.indexOf("UID") >= 0)
					{
						uid = msg.split("UID: ")[1].replace("\n", "").replace("\r", "").replace(0, 1, "");
						qDebug() << uid;
					}

					msg = "";
				}

			}
		}
}

void MasterPanel::send()
{
	timer->stop();

	port->write("WRITE");

	linename->setEnabled(true);
	linelastname->setEnabled(true);
	linepatronymic->setEnabled(true);
	courses->setEnabled(true);
	groups->setEnabled(true);
	butchoosephoto->setEnabled(true);
	modeteacher->setEnabled(true);
	modestudent->setEnabled(true);
	listbox->setEnabled(true);

	QMessageBox::warning(this, "Оповещение", "Контроллер переведен в режим записи и готов к работе!");
}

void MasterPanel::finish()
{
	//int cur = 0;
	//int grp = 0;

	if(progress->value() == 4)
	{
		progress->setValue(0);

		/*

		query->exec("select id from courses where number_course = " + courses->currentText());

		if(query->next())
		{
			cur = query->value(0).toInt();
		}

		qDebug() << "Course: " << cur;

		query->exec("select id from `groups` where name = '" + groups->currentText() + "'");

		if(query->next())
		{
			grp = query->value(0).toInt();
		}

		qDebug() << "Group: " << grp;

		*/

		//query->exec("insert into accounts values(null, '" + uid + "', '" + linename->text() + "', '" + linelastname->text() + "', '" + linepatronymic->text() + "', " + ((modeteacher->isChecked())? "'Преподаватель'" : "'Студент'") + ", '" + filePhotoName  + "', " + QString::number(access_level) + ", " + QString::number(grp) + ", " + QString::number(cur) + ")");

		query->prepare("insert into accounts values(null, :UID, :Name, :LastName, :Patronymic, :Type, :Photo, :AccessLevel, (select id from `groups` where name = :Group), (select id from courses where number_course = :Course))");

		//qDebug() << "insert into accounts values(null, '" + uid + "', '" + linename->text() + "', '" + linelastname->text() + "', '" + linepatronymic->text() + "', " + ((modeteacher->isChecked())? "'Преподаватель'" : "'Студент'") + ", '" + filePhotoName  + "', " + QString::number(access_level) + ", " + QString::number(grp) + ", " + QString::number(cur) + ")";

		query->bindValue(":UID", uid);
		query->bindValue(":Name", linename->text());
		query->bindValue(":LastName", linelastname->text());
		query->bindValue(":Patronymic", linepatronymic->text());
		query->bindValue(":Type", ((modeteacher->isChecked())? "Преподаватель" : "Студент"));
		query->bindValue(":Photo", filePhotoName);
		query->bindValue(":AccessLevel", QString::number(access_level));
		query->bindValue(":Group", ((modeteacher->isChecked())? "null" : groups->currentText()));
		query->bindValue(":Course", ((modeteacher->isChecked())? 0 : courses->currentText()));

		qDebug() << "insert into accounts values(null," + query->boundValue(":UID").toString() + "," + query->boundValue("Name").toString() + "," + query->boundValue(":LastName").toString() + "," + query->boundValue(":Patronymic").toString() + "," + query->boundValue(":Type").toString() + "," + query->boundValue(":Photo").toString() + "," + query->boundValue(":AccessLevel").toString() + "(select id from `groups` where name = " + query->boundValue(":Group").toString() +"), (select id from courses where number_course = " + query->boundValue(":Course").toString() +"))";

		if(!query->exec())
		{
			qDebug() << "Error code: " << query->lastError().nativeErrorCode() << " Error: " << query->lastError().text();
		}

		for(int counter = 0; counter < listbox->count(); ++counter)
		{
			query->prepare("insert into filials_and_accounts values(null, (select id from accounts where firstname = :Name and lastname = :LastName and patronymic = :Patronymic), (select id from filials where address = :Address), :Access)");

			query->bindValue(":Name", linename->text());
			query->bindValue(":LastName", linelastname->text());
			query->bindValue(":Patronymic", linepatronymic->text());
			query->bindValue(":Address", listbox->item(counter)->text());
			query->bindValue(":Access", (listbox->item(counter)->checkState() == Qt::Checked)? true : false);

			qDebug() << "insert into filials_and_accounts values(null, (select id from accounts where firstname = " + query->boundValue(":Name").toString() + " and lastname = " + query->boundValue(":LastName").toString() + " and patronymic = " + query->boundValue(":Patronymic").toString() + "), (select id from filials where address = " + query->boundValue(":Address").toString() + "), " + query->boundValue(":Access").toString() + ")";

			if(!query->exec())
			{
				qDebug() << "Error code: " << query->lastError().nativeErrorCode() << " Error: " << query->lastError().text();
			}
		}

		linename->setEnabled(true);

		modestudent->setEnabled(true);
		modeteacher->setEnabled(true);

		linelastname->setEnabled(true);
		linepatronymic->setEnabled(true);

		courses->setEnabled(true);
		groups->setEnabled(true);

		//filial_one->setEnabled(true);
		//filial_two->setEnabled(true);
		//filial_three->setEnabled(true);

		butchoosephoto->setEnabled(true);
		butwrite->setEnabled(true);

		listbox->setEnabled(true);

		linelog->setText("");

		QObject::disconnect(port, SIGNAL(readyRead()), this, SLOT(rec()));

		click = false;

		timer_timeout->stop();
	}
}

void MasterPanel::choose_course()
{
	groups->clear();

	query->prepare("select name from `groups` where number_course = :Course");

	query->bindValue(":Course", courses->currentText());

	//query->exec("select name from `groups` where number_course = " + courses->currentText());

	query->exec();

	while(query->next())
	{
		groups->addItem(query->value(0).toString());
	}

	groups->setCurrentIndex(0);
}

void MasterPanel::set_filial_one()
{
	if(filial_one->checkState() == Qt::Checked)
	{
		access_level += 1;
	}
	else if(filial_one->checkState() == Qt::Unchecked)
	{
		access_level -= 1;
	}

	qDebug() << access_level;
}

void MasterPanel::set_filial_two()
{
	if(filial_two->checkState() == Qt::Checked)
	{
		access_level += 2;
	}
	else if(filial_two->checkState() == Qt::Unchecked)
	{
		access_level -= 2;
	}

	qDebug() << access_level;
}

void MasterPanel::set_filial_three()
{
	if(filial_three->checkState() == Qt::Checked)
	{
		access_level += 4;
	}
	else if(filial_three->checkState() == Qt::Unchecked)
	{
		access_level -= 4;
	}

	qDebug() << access_level;
}

void MasterPanel::clickButChoosePhoto()
{
	filePhotoName = QFileDialog::getOpenFileUrl(this, "Открыть изображение", QUrl("~/"), "Файлы изображений (*.jpeg)").fileName();

	qDebug() << filePhotoName;

	labelhintpathphoto->setText("Путь до фото:" + filePhotoName);
}

void MasterPanel::timeout_timer()
{
	progress->setValue(0);

	QMessageBox::warning(this, "Ошибка", "Тайм-аут ввода вывода с контроллером истек!");

	linename->setEnabled(true);

	modestudent->setEnabled(true);
	modeteacher->setEnabled(true);

	linelastname->setEnabled(true);
	linepatronymic->setEnabled(true);

	courses->setEnabled(true);
	groups->setEnabled(true);

	filial_one->setEnabled(true);
	filial_two->setEnabled(true);
	filial_three->setEnabled(true);

	butchoosephoto->setEnabled(true);
	butwrite->setEnabled(true);

	listbox->setEnabled(true);

	linelog->setText("");

	click = false;

	QObject::disconnect(port, SIGNAL(readyRead()), this, SLOT(rec()));

	timer_timeout->stop();
}
