#include "finddialog.h"
#include<QLabel>
#include<QHBoxLayout>
#include<QLineEdit>
#include <QPushButton>
#include <QMessageBox>


FindDialog::FindDialog(QWidget *parent) : QDialog(parent)
{
    QLabel *findLabel = new QLabel(tr("Enter the name of a contact:"));
    lineEdit = new QLineEdit;

    findButton = new QPushButton(tr("&Find"));
    findText = "";

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(findLabel);
    layout->addWidget(lineEdit);
    layout->addWidget(findButton);

    setLayout(layout);
    setWindowTitle(tr("Find a Contact"));
    connect(findButton,&QPushButton::clicked,this,&FindDialog::findClicked);
    connect(findButton,&QPushButton::clicked,this,&FindDialog::accept);

}

QString FindDialog::gatFindText()
{
    return findText;
}

void FindDialog::findClicked()
{
    QString text = lineEdit->text();

    if(text.isEmpty())
    {
        QMessageBox::information(this,tr("Empty Field"),tr("Please enter a name. "));
        return;
    }
    else
    {
        findText = text;
        lineEdit->clear();
        hide();
    }
}
