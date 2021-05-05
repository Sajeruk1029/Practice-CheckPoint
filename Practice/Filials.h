#ifndef FILIALS_H
#define FILIALS_H

#include <QObject>

#include <QWidget>

#include <QBoxLayout>

#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>

#include <QMessageBox>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QTableWidgetItem>

class Filials : public QWidget
{
	Q_OBJECT

	public:
		explicit Filials(QSqlDatabase*);
		~Filials();

	private slots:
		void filialadd();
		void filialchange();
		void filialdelete();

	private:
		QBoxLayout *layout;

		QLineEdit *lineaddress;

		QTableWidget *table;

		QPushButton *butadd;
		QPushButton *butchange;
		QPushButton *butdelete;

		QSqlDatabase *db;
		QSqlQuery *query;
};

#endif
