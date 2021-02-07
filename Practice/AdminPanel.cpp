#include "AdminPanel.h"

AdminPanel::AdminPanel(QSettings *conf) : layout(new QBoxLayout(QBoxLayout::Down)), linehost(new QLineEdit()), linelogin(new QLineEdit()),
linepassword(new QLineEdit()), linecontrollername(new QLineEdit()), butwriteconf(new QPushButton("Записать")),
settings(conf)
{
	//setWindowTitle("Панель администратора");
	setLayout(layout);
	//setFixedSize(300, 200);

	linehost->setPlaceholderText("Хост");
	linelogin->setPlaceholderText("Логин");
	linepassword->setPlaceholderText("Пароль");
	linecontrollername->setPlaceholderText("Имя контроллера");

	linepassword->setEchoMode(QLineEdit::EchoMode::Password);

	layout->addWidget(linehost);
	layout->addWidget(linelogin);
	layout->addWidget(linepassword);
	layout->addWidget(linecontrollername);
	layout->addWidget(butwriteconf);

	QObject::connect(butwriteconf, SIGNAL(clicked()), this, SLOT(writeConf()));
}

AdminPanel::~AdminPanel()
{
	if(settings){ delete settings; }

	delete linehost;
	delete linelogin;
	delete linepassword;
	delete linecontrollername;

	delete butwriteconf;

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

		settings->beginGroup("Controller");

		settings->setValue("NAME", linecontrollername->text());

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
