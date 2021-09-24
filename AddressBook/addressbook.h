#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>
#include <QMap>
#include <QString>

#include "finddialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AddressBook; }
QT_END_NAMESPACE

class AddressBook : public QWidget
{
    Q_OBJECT

public:
    AddressBook(QWidget *parent = nullptr);
    ~AddressBook();

    enum Mode{NavigationMode, AddingMode,EditingMode};
public slots:
    void addContact();
    void submitContact();
    void cancel();

    void next();
    void previous();

    void editContact();
    void removeContact();

    void findContact();

    void saveToFile();
    void loadFromFile();

private:
    Ui::AddressBook *ui;

    QMap<QString,QString> contacts;
    QString oldName;
    QString oldAddress;

    void updateInterface(Mode mode);
    Mode currentMode;

    QPushButton *findButton;
    FindDialog *dialog;

};
#endif // ADDRESSBOOK_H
