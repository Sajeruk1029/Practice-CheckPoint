#include "Filials.h"

Filials::Filials(QSqlDatabase *bd) : layout(new QBoxLayout(QBoxLayout::Down)), lineaddress(new QLineEdit()), table(new QTableWidget(0, 2)),
butadd(new QPushButton("Добавить")), butchange(new QPushButton("Изменить")), butdelete(new QPushButton("Удалить")),
db(bd), query(new QSqlQuery(*db))
{
	int counter_data = 0;

	lineaddress->setPlaceholderText("Адрес");

	setLayout(layout);

	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);

	table->setHorizontalHeaderLabels({"Номер", "Адрес"});

	table->setEditTriggers(0);

	query->exec("select * from filials");

	while(query->next())
	{
		table->setRowCount(table->rowCount() + 1);

		for(int counter = 0; counter < 2; ++counter)
		{
			table->setItem(counter_data, counter, new QTableWidgetItem(query->value(counter).toString()));
		}

		++counter_data;
	}

	layout->addWidget(lineaddress);
	layout->addWidget(butadd);
	layout->addWidget(butchange);
	layout->addWidget(butdelete);
	layout->addWidget(table);

	QObject::connect(butadd, SIGNAL(clicked()), this, SLOT(filialadd()));
	QObject::connect(butchange, SIGNAL(clicked()), this, SLOT(filialchange()));
	QObject::connect(butdelete, SIGNAL(clicked()), this, SLOT(filialdelete()));
}

Filials::~Filials()
{
	if(query){ delete query; }

	if(db)
	{
		db->close();

		delete db;
	}

	delete lineaddress;

	delete butadd;
	delete butchange;
	delete butdelete;

	delete layout;
}

void Filials::filialadd()
{
	int counter_data = 0;

	if(lineaddress->text() == "")
	{
		QMessageBox::warning(this, "Ошибка", "Поле адреса не должно быть пустым!");

		return;
	}

	query->prepare("insert into filials values(null, :Address)");

	query->bindValue(":Address", lineaddress->text());

	if(!query->exec())
	{
		QMessageBox::warning(this, "Ошибка", "Такой адрес уже есть!");

		return;
	}

	table->clear();
	table->setRowCount(0);
	table->setEditTriggers(0);

	table->setHorizontalHeaderLabels({"Номер", "Адрес"});

	query->exec("select * from filials");

	while(query->next())
	{
		table->setRowCount(table->rowCount() + 1);

		for(int counter = 0; counter < 2; ++counter)
		{
			table->setItem(counter_data, counter, new QTableWidgetItem(query->value(counter).toString()));
		}

		++counter_data;
	}

}

void Filials::filialchange()
{
	int counter_data = 0;

	if(table->selectedItems().size() == 0)
	{
		QMessageBox::warning(this, "Ошибка", "Вы должны выбрать строку, которую хотите изменить!");

		return;
	}

	query->prepare("update filials set address = :Address where id = :Id");

	query->bindValue(":Address", lineaddress->text());
	query->bindValue(":Id", table->selectedItems()[0]->text().toInt());

	if(!query->exec())
	{
		QMessageBox::warning(this, "Ошибка", "Изменение не удалось!");

		return;
	}

	table->clear();
	table->setRowCount(0);
	table->setEditTriggers(0);

	table->setHorizontalHeaderLabels({"Номер", "Адрес"});

	query->exec("select * from filials");

	while(query->next())
	{
		table->setRowCount(table->rowCount() + 1);

		for(int counter = 0; counter < 2; ++counter)
		{
			table->setItem(counter_data, counter, new QTableWidgetItem(query->value(counter).toString()));
		}

		++counter_data;
	}
}

void Filials::filialdelete()
{
	int counter_data = 0;

	if(table->selectedItems().size() == 0)
	{
		QMessageBox::warning(this, "Ошибка", "Вы должны выбрать строку, которую хотите удалить!");

		return;
	}

	query->prepare("delete from filials where id = :Id");

	query->bindValue(":Id", table->selectedItems()[0]->text().toInt());

	if(!query->exec())
	{
		QMessageBox::warning(this, "Ошибка", "Удаление не удалось!");

		return;
	}

	table->clear();
	table->setRowCount(0);
	table->setEditTriggers(0);

	table->setHorizontalHeaderLabels({"Номер", "Адрес"});

	query->exec("select * from filials");

	while(query->next())
	{
		table->setRowCount(table->rowCount() + 1);

		for(int counter = 0; counter < 2; ++counter)
		{
			table->setItem(counter_data, counter, new QTableWidgetItem(query->value(counter).toString()));
		}

		++counter_data;
	}
}
