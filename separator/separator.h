// separator.h
#ifndef SEPARATOR_H
#define SEPARATOR_H

#include "particle.h"
#include "separatorelement.h"
#include <QVector>
#include <QString>

struct LossStat {
    QString elementName;
    QString pointName;   // имя точки (элемента)
    int lostCount;
    int elementIndex;
};

class Separator
{
public:
    Separator() = default;
    void addElement(const SeparatorElement& elem);
    void clear();
    void transport(Particle& p) const;

    // Новые методы для статистики потерь
    void resetLossStats();                           // обнулить счётчики
    QVector<LossStat> getLossStats() const;          // получить статистику
    int getTotalLost() const { return m_totalLost; } // общее число потерянных

    const std::vector<SeparatorElement>& getElements() const { return m_elements; }
    int elementsCount() const { return static_cast<int>(m_elements.size()); }

    void registerLoss(const QString& name, int elementIndex) const;

private:
    std::vector<SeparatorElement> m_elements;
    mutable QVector<LossStat> m_lossStats;   // статистика потерь (mutable, чтобы менять в const transport)
    mutable int m_totalLost = 0;             // общее число потерянных
};

#endif // SEPARATOR_H
