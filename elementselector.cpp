#include "elementselector.h"
#include <QDebug>
#include <algorithm>

ElementSelector::ElementSelector(PeriodicTable* table,
                                 QComboBox* typeCombo,
                                 QComboBox* elementCombo,
                                 QComboBox* isotopeCombo,
                                 QObject* parent)
    : QObject(parent),
      m_table(table),
      m_typeCombo(typeCombo),
      m_elementCombo(elementCombo),
      m_isotopeCombo(isotopeCombo)
{
    Q_ASSERT(m_typeCombo != nullptr);
    Q_ASSERT(m_elementCombo != nullptr);
    Q_ASSERT(m_isotopeCombo != nullptr);
}

void ElementSelector::initialize()
{
    m_typeCombo->clear();
    m_typeCombo->addItem("Элемент");

    populateElementCombo();
    onTypeChanged(m_typeCombo->currentIndex());

    // Подключаем сигналы
    connect(m_elementCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ElementSelector::onElementSelected);

    connect(m_isotopeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ElementSelector::selectionChanged);

    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ElementSelector::onTypeChanged);
}

void ElementSelector::populateElementCombo()
{
    m_elementCombo->clear();
    m_isotopeCombo->clear();

    QList<Element> elements = m_table->getAllElements();
    std::sort(elements.begin(), elements.end(), [](const Element& a, const Element& b) { return a.getZ() < b.getZ(); });

    for (const Element& element : elements) {
        QString text = QString("%1. %2 (%3)")
            .arg(element.getZ(), 2)
            .arg(element.getSymbol())
            .arg(element.getName());
        m_elementCombo->addItem(text, element.getZ());
    }

    if (!elements.isEmpty()) {
        onElementSelected(0); // Обновляем комбобокс изотопов для первого элемента
    }
}

void ElementSelector::onElementSelected(int index)
{
    if (index < 0) return;

    int Z = m_elementCombo->itemData(index).toInt();
    const Element* element = m_table->getElementByZ(Z);
    if (element) {
        populateIsotopeCombo(element);
    }
}

void ElementSelector::populateIsotopeCombo(const Element* element)
{
    m_isotopeCombo->blockSignals(true);
    m_isotopeCombo->clear();

    // если есть изотопы — используем первый как "природный"
    if (!element->getAllIsotopes().isEmpty()) {
        const Isotope& first = element->getAllIsotopes().first();
        m_isotopeCombo->addItem("Природный", first.A);
    } else {
        // fallback, если нет изотопов вообще
        m_isotopeCombo->addItem("Природный", 0);
    }

    for (const Isotope& isotope : element->getAllIsotopes()) {
        QString text = QString("%1  (M=%2 g/mol, плотн=%3 кг/м³)")
            .arg(isotope.A)
            .arg(isotope.mass, 0, 'f', 5)
            .arg(isotope.density);
        m_isotopeCombo->addItem(text, isotope.A);
    }

    m_isotopeCombo->blockSignals(false);
    emit selectionChanged();
}

void ElementSelector::onTypeChanged(int index)
{
    Q_UNUSED(index)
    m_elementCombo->setVisible(true);
    m_isotopeCombo->setVisible(true);

//    emit selectionChanged();
}

QPair<const Element*, int> ElementSelector::getSelectedElement() const
{
    int elementIndex = m_elementCombo->currentIndex();
    if (elementIndex < 0) return qMakePair(nullptr, 0);

    int Z = m_elementCombo->itemData(elementIndex).toInt();
    const Element* element = m_table->getElementByZ(Z);
    if (!element) return qMakePair(nullptr, 0);

    int isotopeA = m_isotopeCombo->currentData().toInt();
    return qMakePair(element, isotopeA);
}

ElementSelector::SelectionType ElementSelector::getCurrentSelectionType() const
{
    return ElementType;
}
