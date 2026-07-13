#include "separator.h"
#include <QDebug>
#include <cmath>

void Separator::addElement(const SeparatorElement& elem)
{
    m_elements.push_back(elem);
}

void Separator::clear()
{
    m_elements.clear();
    resetLossStats();
}

void Separator::resetLossStats()
{
    m_lossStats.clear();
    m_totalLost = 0;
}

QVector<LossStat> Separator::getLossStats() const
{
    return m_lossStats;
}

//int Separator::getTotalLost() const
//{
//    return m_totalLost;
//}

void Separator::transport(Particle& p) const
{
    if (!p.alive) return;
    double currentZ = 0.0;

    for (size_t idx = 0; idx < m_elements.size(); ++idx) {
        const auto& elem = m_elements[idx];
        // Сохраняем начальные координаты для промежуточных точек
        double z_start = currentZ;
        double x_start = p.x, y_start = p.y;
        double xp_start = p.xp, yp_start = p.yp;

        elem.transport(p);
        currentZ += elem.getLength();
        QString name = elem.getName();

        // --- Промежуточные точки для диполей и дефлекторов (плавные дуги) ---
        ElementType type = elem.getType();
        if (type == ElementType::Dipole || type == ElementType::ElectrostaticDeflector) {
            int numSteps = 30;
            double len = elem.getLength();
            double absLen = std::abs(len);
            for (int i = 1; i <= numSteps; ++i) {
                double t = (double)i / numSteps;
                double z_interp = z_start + t * absLen;
                double x_interp = x_start + t * (p.x - x_start);
                double y_interp = y_start + t * (p.y - y_start);
                double xp_interp = xp_start + t * (p.xp - xp_start);
                double yp_interp = yp_start + t * (p.yp - yp_start);
                // Фильтр выбросов
                if (std::abs(x_interp) > 1.0 || std::abs(y_interp) > 1.0) continue;
                p.addTrackPoint(z_interp, x_interp, y_interp,
                                xp_interp, yp_interp,
                                p.energy, p.mass, p.charge,
                                name + QString("_%1").arg(i));
            }
        }

        // Основная точка (в конце элемента)
        p.addTrackPoint(currentZ, p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge, name);

        // Проверка апертуры
        if (!elem.apertureCheck(p)) {
            p.alive = false;
            QString name = elem.getName();

            // Логируем потерю с индексом
            bool found = false;
            for (auto& stat : m_lossStats) {
                if (stat.elementName == name) {
                    stat.lostCount++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                LossStat stat;
                stat.elementName = name;
                stat.pointName = name;   // или имя точки
                stat.lostCount = 1;
                stat.elementIndex = static_cast<int>(idx);   // <-- сохраняем индекс
                m_lossStats.append(stat);
            }
            m_totalLost++;
            break;
        }
    }
}
