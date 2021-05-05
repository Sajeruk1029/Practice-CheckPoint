#include "AdminPanel.h"

AdminPanel::AdminPanel(QSettings *conf) : layout(new QBoxLayout(QBoxLayout::Down)), linehost(new QLineEdit()), linelogin(new QLineEdit()),
linepassword(new QLineEdit()), lineaccess(new QLineEdit()), ports(new QComboBox()),
butwriteconf(new QPushButton("Записать")), butportrefresh(new QPushButton("Обновить список портов")), settings(conf), db(nullptr)
{
	//setWindowTitle("Панель администратора");
	setLayout(layout);
	//setFixedSize(300, 200);

	linehost->setPlaceholderText("Хост");
	linelogin->setPlaceholderText("Логин");
	linepassword->setPlaceholderText("Пароль");
	lineaccess->setPlaceholderText("Адрес");

	linepassword->setEchoMode(QLineEdit::EchoMode::Password);

	butwriteconf->setEnabled(false);

	layout->addWidget(linehost);
	layout->addWidget(linelogin);
	layout->addWidget(linepassword);
	layout->addWidget(lineaccess);
	layout->addWidget(ports);
	layout->addWidget(butportrefresh);
	layout->addWidget(butwriteconf);

	QObject::connect(butwriteconf, SIGNAL(clicked()), this, SLOT(writeConf()));
	QObject::connect(butportrefresh, SIGNAL(clicked()), this, SLOT(portrefresh()));
}

AdminPanel::~AdminPanel()
{
	if(settings){ delete settings; }

	delete linehost;
	delete linelogin;
	delete linepassword;
	delete ports;

	delete butwriteconf;
	delete butportrefresh;

	delete layout;
}

void AdminPanel::writeConf()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "conn");

	db.setHostName(linehost->text());
	db.setUserName(linelogin->text());
	db.setPassword(linepassword->text());
	db.setDatabaseName("practice");

	if(db.open())
	{
		settings->beginGroup("BD");

		settings->setValue("HOST", linehost->text());
		settings->setValue("LOGIN", linelogin->text());
		settings->setValue("PASSWORD", linepassword->text());

		settings->endGroup();

		settings->beginGroup("CONTROLLER");

		settings->setValue("NAME", ports->currentText());

		settings->endGroup();

		settings->beginGroup("ACCESS");

		settings->setValue("ACCESSLEVEL", lineaccess->text());

		settings->endGroup();

		settings->sync();

		QMessageBox::warning(this, "Успех", "Данные верны.");

		db.close();
	}
	else
	{
		QMessageBox::warning(this, "Ошибка", "Данные неверны.");
	}

	QSqlDatabase::removeDatabase("conn");
}

void AdminPanel::portrefresh()
{
	ports->clear();

	for(unsigned char counter = 0; counter < QSerialPortInfo::availablePorts().count(); ++counter)
	{
		ports->addItem(QSerialPortInfo::availablePorts()[counter].portName());
	}

	if(QSerialPortInfo::availablePorts().count() != 0)
	{
		ports->setCurrentIndex(0);
		butwriteconf->setEnabled(true);
	}
	else
	{
		ports->addItem("Ничего");
		butwriteconf->setEnabled(false);
	}
}
