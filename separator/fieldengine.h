// fieldengine.h — транспорт частицы через линию элементов в глобальной 3D-системе
#pragma once
#include "fieldelement.h"
#include <vector>

namespace ft {

struct TrackPoint3 {
    double z, x, y;            // дизайн-координата; x, y — от дизайн-орбиты
    double gx, gy, gz;         // глобальные
};

// Состояние на выходе элемента (в выходной рамке, относительно дизайн-орбиты)
struct ElementExit {
    double z;
    double x, y;
    double xp, yp;             // УГЛЫ проекций, рад (xp = atan(dx/dz)) — как в матричной модели
    double eKin;               // МэВ
};

struct LossInfo {
    int    elementIndex = -1;
    double sDesign = 0.0;
    double x = 0, y = 0;
};

struct TransportResult {
    bool alive = true;
    double x = 0, y = 0, xp = 0, yp = 0;     // финал, рамка последнего элемента
    std::vector<TrackPoint3> track;
    std::vector<ElementExit> exits;
    LossInfo loss;
};

class FieldEngine {
public:
    void addElement(const FieldElementPtr& el) { m_elements.push_back(el); }
    void clear() { m_elements.clear(); }
    const std::vector<FieldElementPtr>& elements() const { return m_elements; }

    TransportResult transport(double x0, double y0, double xp0, double yp0,
                              double eKinMeV, double massAmu, int charge,
                              const IntegratorOpts& opts = IntegratorOpts()) const;

private:
    std::vector<FieldElementPtr> m_elements;
};

} // namespace ft
