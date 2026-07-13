#include "simpleelementselector.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

SimpleElementSelector::SimpleElementSelector(PeriodicTable* table, QWidget* parent)
    : QWidget(parent), m_table(table) {

    // Создаем layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Верхняя панель для выбора элемента
    QHBoxLayout* selectionLayout = new QHBoxLayout();

    m_elementCombo = new QComboBox();
    m_elementCombo->setStyleSheet("QComboBox { backgorud-color: #fff; color: black; padding: 2px; }");
    selectionLayout->addWidget(m_elementCombo, 3);

    m_addButton = new QPushButton("Добавить элемент");
    m_addButton->setStyleSheet("QPushButton { background-color: #fff; color: black; padding: 4px; border-radius: 3px }");
    selectionLayout->addWidget(m_addButton, 1);

    mainLayout->addLayout(selectionLayout);

    // Список выбранных элементов
    m_selectedList = new QListWidget();
    m_selectedList->setStyleSheet("QListWidget { background-color: #fff; color: black; padding: 8px; border-radius: 3px }");
    mainLayout->addWidget(m_selectedList);

    // Кнопка удаления
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_removeButton = new QPushButton("Удалить выбранный");
    m_removeButton->setStyleSheet("QPushButton { background-color: #fff; color: black; padding: 4px; border-radius: 3px }");
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

//    // Статусная строка
//    m_statusLabel = new QLabel("Элементы не выбраны");
//    m_statusLabel->setStyleSheet("QLabel { background-color: #f5f5f5; padding: 5px; border: 1px solid #ddd; }");
//    mainLayout->addWidget(m_statusLabel);

    // Заполняем комбобокс
    populateElementCombo();

    // Подключаем сигналы
    connect(m_addButton, &QPushButton::clicked, this, &SimpleElementSelector::onAddElement);
    connect(m_removeButton, &QPushButton::clicked, this, &SimpleElementSelector::onRemoveElement);
    connect(m_selectedList, &QListWidget::itemSelectionChanged, this, [this]() {
        m_removeButton->setEnabled(m_selectedList->currentRow() >= 0);
    });
}

void SimpleElementSelector::populateElementCombo() {
    m_elementCombo->clear();

    QList<Element> elements = m_table->getAllElements();

    // Сортируем элементы по атомному номеру
    std::sort(elements.begin(), elements.end(),
        [](const Element& a, const Element& b) { return a.getZ() < b.getZ(); });

    for (const Element& element : elements) {
        QString text = QString("%1. %2 (%3) - %4")
            .arg(element.getZ(), 2)
            .arg(element.getSymbol())
            .arg(element.getName())
            .arg(element.getCommonDensity());
        m_elementCombo->addItem(text, element.getZ());
    }
}

void SimpleElementSelector::onAddElement() {
    int index = m_elementCombo->currentIndex();
    if (index < 0) return;

    int Z = m_elementCombo->itemData(index).toInt();
    const Element* element = m_table->getElementByZ(Z);

    if (element) {
        // Проверяем, не добавлен ли уже этот элемент
        for (int i = 0; i < m_selectedList->count(); ++i) {
            int existingZ = m_selectedList->item(i)->data(Qt::UserRole).toInt();
            if (existingZ == Z) {
                QMessageBox::information(this, "Элемент уже добавлен",
                    QString("Элемент %1 уже есть в списке").arg(element->getSymbol()));
                return;
            }
        }

        // Добавляем элемент в список
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString("%1 (%2) - Z: %3, плотность: %4 кг/м³")
                     .arg(element->getSymbol())
                     .arg(element->getName())
                     .arg(element->getZ())
                     .arg(element->getCommonDensity()));
        item->setData(Qt::UserRole, Z);
        m_selectedList->addItem(item);

//        // Обновляем статус
//        m_statusLabel->setText(QString("Выбрано элементов: %1").arg(m_selectedList->count()));
        emit selectionChanged();
    }
}

void SimpleElementSelector::onRemoveElement() {
    int row = m_selectedList->currentRow();
    if (row >= 0) {
        delete m_selectedList->takeItem(row);
//        m_statusLabel->setText(QString("Выбрано элементов: %1").arg(m_selectedList->count()));
        emit selectionChanged();
    }
}

QList<const Element*> SimpleElementSelector::getSelectedElements() const {
    QList<const Element*> elements;

    for (int i = 0; i < m_selectedList->count(); ++i) {
        int Z = m_selectedList->item(i)->data(Qt::UserRole).toInt();
        const Element* element = m_table->getElementByZ(Z);
        if (element) {
            elements.append(element);
        }
    }

    return elements;
}

QStringList SimpleElementSelector::getSelectedElementSymbols() const {
    QStringList symbols;

    for (int i = 0; i < m_selectedList->count(); ++i) {
        int Z = m_selectedList->item(i)->data(Qt::UserRole).toInt();
        const Element* element = m_table->getElementByZ(Z);
        if (element) {
            symbols.append(element->getSymbol());
        }
    }

    return symbols;
}
