#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QObject>
#include <QComboBox>
#include <QPair>
#include "periodictable.h"
#include "element.h"

class ElementSelector : public QObject
{
    Q_OBJECT
public:
    explicit ElementSelector(PeriodicTable* table,
                             QComboBox* typeCombo,
                             QComboBox* elementCombo,
                             QComboBox* isotopeCombo,
                             QObject* parent = nullptr);

    void initialize();

    enum SelectionType { ElementType };

    QPair<const Element*, int> getSelectedElement() const;
    SelectionType getCurrentSelectionType() const;

signals:
    void selectionChanged();

private slots:
    void onElementSelected(int index);
    void onTypeChanged(int index);

private:
    void populateElementCombo();
    void populateIsotopeCombo(const Element* element);

    PeriodicTable* m_table;
    QComboBox* m_typeCombo;
    QComboBox* m_elementCombo;
    QComboBox* m_isotopeCombo;
};

#endif // ELEMENTSELECTOR_H
