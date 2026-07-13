#ifndef STRIPPERDIALOG_H
#define STRIPPERDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class StripperDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StripperDialog(QWidget *parent = nullptr);
    double getThickness() const;

private:
    QLineEdit *m_thicknessEdit;
};

#endif // STRIPPERDIALOG_H
