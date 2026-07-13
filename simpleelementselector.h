#ifndef SIMPLEELEMENTSELECTOR_H
#define SIMPLEELEMENTSELECTOR_H

#include "periodictable.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>

class SimpleElementSelector : public QWidget {
    Q_OBJECT

public:
    SimpleElementSelector(PeriodicTable* table, QWidget* parent = nullptr);

    QList<const Element*> getSelectedElements() const;
    QStringList getSelectedElementSymbols() const;

signals:
    void selectionChanged();

private slots:
    void onAddElement();
    void onRemoveElement();

private:
    PeriodicTable* m_table;

    QComboBox* m_elementCombo;
    QPushButton* m_addButton;
    QListWidget* m_selectedList;
    QPushButton* m_removeButton;
    QLabel* m_statusLabel;

    void populateElementCombo();
};

#endif // SIMPLEELEMENTSELECTOR_H
