#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>
#include <QString>
#include <QDialog>

class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindDialog(QWidget *parent = nullptr);
    QString gatFindText();

public slots:
    void findClicked();


signals:

private:
    QPushButton *findButton;
    QLineEdit *lineEdit;
    QString findText;
    FindDialog *dialog;

};

#endif // FINDDIALOG_H
