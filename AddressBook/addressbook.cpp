#include "addressbook.h"
#include "ui_addressbook.h"
#include <QMessageBox>
#include <QMap>
#include <QFileDialog>

AddressBook::AddressBook(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddressBook)
{
    ui->setupUi(this);
    setWindowTitle(tr("Simple Address Book"));
    ui->nameLine->setReadOnly(true);
    ui->addressText->setReadOnly(true);

    ui->addButton->show();
    ui->submitButton->hide();
    ui->cancelButton->hide();

    connect(ui->addButton,&QPushButton::clicked,this,&AddressBook::addContact);
    connect(ui->submitButton,&QPushButton::clicked,this,&AddressBook::submitContact);
    connect(ui->cancelButton,&QPushButton::clicked,this,&AddressBook::cancel);
    ui->verticalLayout->addStretch();

    connect(ui->nextButton,&QPushButton::clicked,this,&AddressBook::next);
    connect(ui->previousButton,&QPushButton::clicked,this,&AddressBook::previous);
    ui->nextButton->setEnabled(false);
    ui->previousButton->setEnabled(false);

    ui->editButton->setEnabled(false);
    ui->removeButton->setEnabled(false);

    connect(ui->editButton,&QPushButton::clicked,this,&AddressBook::editContact);
    connect(ui->removeButton,&QPushButton::clicked,this,&AddressBook::removeContact);

    //findButton = new QPushButton(tr("&Find")); // ต้องแก้
    //findButton->setEnabled(false); // ต้องแก้
    ui->findButton->setEnabled(true);

    dialog = new FindDialog(this);

    connect(ui->findButton,&QPushButton::clicked,this,&AddressBook::findContact);

    ui->loadButton->setToolTip(tr("Load contacts from a file"));
    ui->saveButton->setToolTip(tr("Save contacts to a file"));

    connect(ui->saveButton,&QPushButton::clicked,this,&AddressBook::saveToFile);
    connect(ui->loadButton,&QPushButton::clicked,this,&AddressBook::loadFromFile);

}

AddressBook::~AddressBook()
{
    delete ui;
}

void AddressBook::addContact()
{

    oldName = ui->nameLine->text();
    oldAddress = ui->addressText->toPlainText();

    ui->nameLine->clear();
    ui->addressText->clear();

    ui->nameLine->setReadOnly(false);
    ui->nameLine->setFocus(Qt::OtherFocusReason); //setFocus คือไรไม่รู้
    ui->addressText->setReadOnly(false);

    ui->addButton->setEnabled(false);
    ui->submitButton->show();
    ui->cancelButton->show();

    ui->nextButton->setEnabled(true);
    ui->previousButton->setEnabled(true);
}

void AddressBook::submitContact()
{
    QString name = ui->nameLine->text();
    QString address = ui->addressText->toPlainText();

    if(name.isEmpty() || address.isEmpty())
    {
        QMessageBox::information(this,tr("Empty Field"),tr("Please enter a name and address. "));
        return;
    }
    if(!contacts.contains(name))
    {
        contacts.insert(name,address);
        QMessageBox::information(this,tr("Add Successful"),tr("\"%1\" has been added to your address book. ").arg(name));
    }
    else
    {
        QMessageBox::information(this,tr("Add Unsuccessful"),tr("Sory, \"%1\" is already in your address book. ").arg(name));
        return;
    }
    if(contacts.isEmpty())
    {
        ui->nameLine->clear();
        ui->addressText->clear();
    }

    ui->nameLine->setReadOnly(true);
    ui->addressText->setReadOnly(true);
    ui->addButton->setEnabled(true);
    ui->submitButton->hide();
    ui->cancelButton->hide();

    int number = contacts.size();
    ui->nextButton->setEnabled(number>1); //ถ้ามี สมาชิกมากกว่า 1 จะสามารถกดปุ่ม previousbutton ได้
    ui->previousButton->setEnabled(number>1); //ถ้ามี สมาชิกมากกว่า 1 จะสามารถกดปุ่ม nextbutton ได้

    if(currentMode == AddingMode)
    {
        if(!contacts.contains(name))
        {
            contacts.insert(name,address);
            QMessageBox::information(this,tr("Add Successful"),tr("\"%1\" has been added to your address book. ").arg(name));

        }
        else
        {
            QMessageBox::information(this,tr("Add Unsuccessful"),tr("Sory, \"%1\" is already in your address book. ").arg(name));
        }
    }
    else if(currentMode == EditingMode)
    {
        if(oldName != name)
        {
            if(!contacts.contains(name))
            {
                QMessageBox::information(this,tr("Edit Successful"),tr("\"%1\" has been edited in your address book.").arg(name));
                contacts.remove(oldName);
                contacts.insert(name,address);
            }
            else
            {
                QMessageBox::information(this, tr("Edit Unsuccessful"),tr("Sorry, \"%1\" is already in your address book.").arg(name));
            }
        }
        else if(oldAddress != address)
        {
            QMessageBox::information(this, tr("Edit Successful"),tr("\"%1\" has been edited in your address book.").arg(name));
            contacts[name] = address;
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::cancel()
{
    ui->nameLine->setText(oldName);
    ui->nameLine->setReadOnly(true);

    ui->addressText->setText(oldAddress);
    ui->addressText->setReadOnly(true);

    ui->addButton->setEnabled(true);
    ui->submitButton->hide();
    ui->cancelButton->hide();
}

void AddressBook::next()
{
    QString name = ui->nameLine->text();
    QMap<QString,QString>::iterator i = contacts.find(name);

    if(i != contacts.end())
        i++;
    if(i == contacts.end())
        i = contacts.begin();

    ui->nameLine->setText(i.key());
    ui->addressText->setText(i.value());
}

void AddressBook::previous()
{
    QString name = ui->nameLine->text();
    QMap<QString,QString>::iterator i = contacts.find(name);

    if(i != contacts.end())
    {
        ui->nameLine->clear();
        ui->addressText->clear();
        return;
    }
    if(i == contacts.begin())
        i = contacts.end();

    i--;
    ui->nameLine->setText(i.key());
    ui->addressText->setText(i.value());
}

void AddressBook::editContact()
{
    oldName = ui->nameLine->text();
    oldAddress = ui->addressText->toPlainText();

    updateInterface(EditingMode);
}

void AddressBook::removeContact()
{
    QString name = ui->nameLine->text();
    QString address = ui->addressText->toPlainText();

    if(contacts.contains(name))
    {
        int button = QMessageBox::question(this,tr("Confirm Remove"),tr("Are you sure you want to remove \"%1\"?").arg(name),QMessageBox::Yes| QMessageBox::No);

        if(button == QMessageBox::Yes)
        {
            previous();
            contacts.remove(name);

            QMessageBox::information(this,tr("Remove Successful"),tr("\"%1\" has been removed from your address book. ").arg(name));
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::findContact()
{
    dialog->show();

    if(dialog->exec()==QDialog::Accepted)
    {
        QString contactName = dialog->gatFindText();
        if(contacts.contains(contactName))
        {
            ui->nameLine->setText(contactName);
            ui->addressText->setText(contacts.value(contactName));
        }
        else
        {
            QMessageBox::information(this,tr("Contact Not Found"),tr("Sorry,\"%1\" is not in your address book. ").arg(contactName));
            return;
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::saveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Address Book"),"",tr("Address Book (*.kuay);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this,tr("Unable to open file"),file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_5);
        out << contacts;
    }
}

void AddressBook::loadFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Address Book"),"",tr("Address Book (*.kuay);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this,tr("Unable to open file"),file.errorString());
            return;
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_5);
        contacts.clear();
        in >> contacts;

        if(contacts.isEmpty())
        {
            QMessageBox::information(this,tr("No contacts in file"),tr("The file you are attempting to open contains no contacts. "));
        }
        else
        {
            QMap<QString,QString>::iterator i = contacts.begin();
            ui->nameLine->setText(i.key());
            ui->addressText->setText(i.value());
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::updateInterface(AddressBook::Mode mode)
{
    currentMode = mode;

    switch(currentMode)
    {
        case AddingMode:
        case EditingMode:

            ui->nameLine->setReadOnly(false);
            ui->nameLine->setFocus(Qt::OtherFocusReason);
            ui->addressText->setReadOnly(false);

            ui->addButton->setEnabled(false);
            ui->editButton->setEnabled(false);
            ui->removeButton->setEnabled(false);

            ui->nextButton->setEnabled(false);
            ui->previousButton->setEnabled(false);

            ui->submitButton->show();
            ui->cancelButton->show();
            break;
        case NavigationMode:

            if(contacts.isEmpty())
            {
                ui->nameLine->clear();
                ui->addressText->clear();
            }
            ui->nameLine->setReadOnly(true);
            ui->addressText->setReadOnly(true);
            ui->addButton->setEnabled(true);

            int number = contacts.size();
            ui->editButton->setEnabled(number >= 1);
            ui->removeButton->setEnabled(number >=1);
            ui->nextButton->setEnabled(number >1);
            ui->previousButton->setEnabled(number >1);

            ui->submitButton->hide();
            ui->cancelButton->hide();
            break;
    }
}

