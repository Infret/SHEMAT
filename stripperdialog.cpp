#include "stripperdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpressionValidator>

StripperDialog::StripperDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Настройка углеродной фольги");
    setModal(true);
    setMinimumWidth(350);

    // Стилизация диалога в соответствии с тёмной темой программы
    setStyleSheet(R"(
        QDialog {
            background-color: #0d2b3e;
            color: #e0f7fa;
        }
        QLabel {
            color: #e0f7fa;
            font-size: 11pt;
        }
        QLineEdit {
            background-color: #0a1e2c;
            color: #e0f7fa;
            border: 1px solid #1a4c6e;
            border-radius: 4px;
            padding: 5px;
            font-size: 10pt;
        }
        QPushButton {
            background-color: #6200ee;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3700b3;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Пояснительная надпись
    QLabel *infoLabel = new QLabel("Введите толщину углеродной фольги (стриппера):", this);
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    // Поле ввода толщины
    QHBoxLayout *thickLayout = new QHBoxLayout();
    QLabel *valueLabel = new QLabel("Толщина (мкг/см²):", this);
    m_thicknessEdit = new QLineEdit(this);
    m_thicknessEdit->setText("1.0");
    // Разрешаем только числа с точкой
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("^[0-9]*\\.?[0-9]*$"), this);
    m_thicknessEdit->setValidator(validator);
    thickLayout->addWidget(valueLabel);
    thickLayout->addWidget(m_thicknessEdit);
    mainLayout->addLayout(thickLayout);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Отмена", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

double StripperDialog::getThickness() const
{
    return m_thicknessEdit->text().toDouble();
}
