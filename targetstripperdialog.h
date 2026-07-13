#ifndef TARGETSTRIPPERDIALOG_H
#define TARGETSTRIPPERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class TargetStripperDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TargetStripperDialog(QWidget *parent = nullptr);

    // Получение введённых значений
    int getZ() const { return m_z; }
    int getA() const { return m_a; }
    double getTargetThickness() const { return m_targetThickness; }
    double getTargetDensity() const { return m_targetDensity; }
    double getTiltBeta() const { return m_tiltBeta; } // угол наклона мишени (градусы)
    bool isStripperEnabled() const { return m_stripperEnabled; }
    bool isTargetEnabled() const { return m_targetEnabled; }

private slots:
    void onAccept();

private:
    QLineEdit *m_zEdit, *m_aEdit;
    QLineEdit *m_targetThicknessEdit, *m_targetDensityEdit, *m_tiltBetaEdit;
    QCheckBox *m_targetCheckBox, *m_stripperCheckBox;

    int m_z, m_a;
    double m_targetThickness, m_targetDensity, m_tiltBeta;
    bool m_targetEnabled, m_stripperEnabled;
};

#endif // TARGETSTRIPPERDIALOG_H
