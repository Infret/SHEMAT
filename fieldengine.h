// fieldengine.h — транспорт частицы через линию элементов в глобальной 3D-системе
#pragma once
#include "fieldelement.h"
#include <vector>

namespace ft {

// Точка трека. z — накопленная дизайн-координата (для совместимости с TrackPoint),
// x, y — локальные координаты в рамке элемента, gx/gy/gz — глобальные.
struct TrackPoint3 {
    double z, x, y;
    double gx, gy, gz;
};

struct LossInfo {
    int    elementIndex = -1;
    double sDesign = 0.0;      // дизайн-координата потери, м
    double x = 0, y = 0;       // локальные координаты в момент потери
};

struct TransportResult {
    bool alive = true;
    // Финальные локальные координаты в рамке ПОСЛЕДНЕГО элемента
    // (так же, как матричный движок отдаёт Particle: x, y, xp, yp)
    double x = 0, y = 0, xp = 0, yp = 0;  std::vector<TrackPoint3> track;
    LossInfo loss;
};

class FieldEngine {
public:
    void addElement(const FieldElementPtr& el) { m_elements.push_back(el); }
    void clear() { m_elements.clear(); }
    const std::vector<FieldElementPtr>& elements() const { return m_elements; }

    // Вход: локальные координаты в стартовой рамке линии (ось z — начальное
    // направление пучка), xp/yp — углы в радианах (x' = dx/dz, как в вашем Drift).
    // eKinMeV — полная кинетическая энергия, МэВ; massAmu — масса в а.е.м.;
    // charge — зарядовое состояние z.
    TransportResult transport(double x0, double y0, double xp0, double yp0,
                              double eKinMeV, double massAmu, int charge,
                              const IntegratorOpts& opts = IntegratorOpts()) const;

private:
    std::vector<FieldElementPtr> m_elements;
};

} // namespace ft
