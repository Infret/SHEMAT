#include "targetstripperdialog.h"
#include <QDoubleValidator>
#include <QIntValidator>

TargetStripperDialog::TargetStripperDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Параметры мишени и стриппера");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Частица
    QHBoxLayout *partLayout = new QHBoxLayout();
    partLayout->addWidget(new QLabel("Z частицы:"));
    m_zEdit = new QLineEdit; m_zEdit->setValidator(new QIntValidator(1, 120, this));
    partLayout->addWidget(m_zEdit);
    partLayout->addWidget(new QLabel("A частицы:"));
    m_aEdit = new QLineEdit; m_aEdit->setValidator(new QIntValidator(1, 300, this));
    partLayout->addWidget(m_aEdit);
    mainLayout->addLayout(partLayout);

    // Мишень
    m_targetCheckBox = new QCheckBox("Применить рассеяние в мишени");
    m_targetCheckBox->setChecked(true);
    mainLayout->addWidget(m_targetCheckBox);

    QGridLayout *targetLayout = new QGridLayout();
    targetLayout->addWidget(new QLabel("Толщина мишени (мкг/см²):"), 0, 0);
    m_targetThicknessEdit = new QLineEdit; m_targetThicknessEdit->setValidator(new QDoubleValidator(0, 1e9, 6, this));
    targetLayout->addWidget(m_targetThicknessEdit, 0, 1);
    targetLayout->addWidget(new QLabel("Плотность мишени (г/см³):"), 1, 0);
    m_targetDensityEdit = new QLineEdit; m_targetDensityEdit->setValidator(new QDoubleValidator(0, 100, 6, this));
    targetLayout->addWidget(m_targetDensityEdit, 1, 1);
    targetLayout->addWidget(new QLabel("Угол наклона мишени (градусы):"), 2, 0);
    m_tiltBetaEdit = new QLineEdit; m_tiltBetaEdit->setValidator(new QDoubleValidator(0, 90, 6, this));
    targetLayout->addWidget(m_tiltBetaEdit, 2, 1);
    mainLayout->addLayout(targetLayout);

    // Стриппер
    m_stripperCheckBox = new QCheckBox("Применить углеродную фольгу (стриппер)");
    m_stripperCheckBox->setChecked(true);
    mainLayout->addWidget(m_stripperCheckBox);

    // Кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("OK");
    QPushButton *cancelBtn = new QPushButton("Отмена");
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &TargetStripperDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    // Установка значений по умолчанию (можно извлечь из GUI)
    m_zEdit->setText("77");
    m_aEdit->setText("203");
    m_targetThicknessEdit->setText("300");
    m_targetDensityEdit->setText("21.45");
    m_tiltBetaEdit->setText("0");
}

void TargetStripperDialog::onAccept()
{
    m_z = m_zEdit->text().toInt();
    m_a = m_aEdit->text().toInt();
    m_targetThickness = m_targetThicknessEdit->text().toDouble();
    m_targetDensity = m_targetDensityEdit->text().toDouble();
    m_tiltBeta = m_tiltBetaEdit->text().toDouble();
    m_targetEnabled = m_targetCheckBox->isChecked();
    m_stripperEnabled = m_stripperCheckBox->isChecked();
    accept();
}
