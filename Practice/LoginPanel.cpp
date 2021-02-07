#include "LoginPanel.h"

LoginPanel::LoginPanel() : layout(new QBoxLayout(QBoxLayout::Down)), linelogin(new QLineEdit()), linepassword(new QLineEdit()),
butlogin(new QPushButton("Войти")), db(new QSqlDatabase()), query(nullptr), settings(new QSettings("config.ini", QSettings::IniFormat))
{
	setWindowTitle("Панель входа");
	setLayout(layout);
	setFixedSize(300, 100);

	QObject::connect(butlogin, SIGNAL(clicked()), this, SLOT(clickButLogin()));
	qDebug() << "Debug!1";

	if(!QFile("config.ini").exists())
	{
		qDebug() << "Config not exist!";

		settings->beginGroup("BD");

		settings->setValue("HOST", "");
		settings->setValue("LOGIN", "");
		settings->setValue("PASSWORD", "");

		settings->endGroup();

		settings->beginGroup("Controller");

		settings->setValue("NAME", "");

		settings->endGroup();

		settings->sync();

		combp = new ComboPanel(1, settings, nullptr);

		combp->show();
	}
	else
	{
		qDebug() << "Config exist!";

		this->show();

		*db = QSqlDatabase::addDatabase("QMYSQL", "conn");

		linelogin->setPlaceholderText("Логин");
		linepassword->setPlaceholderText("Пароль");

		linepassword->setEchoMode(QLineEdit::EchoMode::Password);

		layout->addWidget(linelogin);
		layout->addWidget(linepassword);
		layout->addWidget(butlogin);

		settings->beginGroup("BD");

		db->setHostName(settings->value("HOST").toString());
		db->setUserName(settings->value("LOGIN").toString());
		db->setPassword(settings->value("PASSWORD").toString());
		db->setDatabaseName("practice");

		qDebug() << settings->value("HOST").toString();
		qDebug() << settings->value("LOGIN").toString();
		qDebug() << settings->value("PASSWORD").toString();
		qDebug() << settings->fileName();

		settings->endGroup();

		if(!db->open())
		{
			linelogin->setEnabled(false);
			linepassword->setEnabled(false);
			butlogin->setEnabled(false);

			QMessageBox::warning(this, "Ошибка", "Неверно введенные данные для авторизации в бд. Обратитесь к администратору.");
		}
		else
		{
			query = new QSqlQuery(*db);
			qDebug() << "Debug!2";
		}
	}


}

LoginPanel::~LoginPanel()
{
	if(query){ delete query; }

	delete linelogin;
	delete linepassword;

	delete butlogin;

	delete layout;
}

void LoginPanel::clickButLogin()
{
	QString type = "";

	qDebug() << "Debug!3";

	query->exec("select type from users where login='" + linelogin->text() + "' and password='" + linepassword->text() + "'");

	if(query->next())
	{
		type = query->value(0).toString();
		qDebug() << type;

		if(type == "Admin"){ combp = new ComboPanel(1, settings, nullptr); }
		else if(type == "Enginner"){ combp = new ComboPanel(2, settings, nullptr); }
		else if(type == "Security"){ combp = new ComboPanel(3, settings, db); }

		this->close();
		combp->show();
	}
	else
	{
		QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!");
	}
}
