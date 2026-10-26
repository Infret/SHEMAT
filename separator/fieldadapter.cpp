#include "fieldadapter.h"
#include <cmath>
#include <memory>
using namespace ft;

// Формат (как в вашем конфиге):
//   DEFLECTOR U_кВ R_м угол_град gap xmin xmax       U<0 — влево, U>0 — вправо, U=0 — выключен (дрейф)
//   DIPOLE    B_Тл R_м угол_град gap e1 e2 xmin xmax B>0 — влево, B<0 — вправо, B=0 — выключен (дрейф)
// Геометрия (R, угол) и поля заданы конфигом; параметры частицы берутся при транспорте.
FieldLine buildFieldLine(const SeparatorConfig& cfg, bool hardEdge) {
    FieldLine fl;
    const double deg = M_PI / 180.0;
    int nd = 0, nq = 0, nde = 0, ndi = 0;

    for (const auto& c : cfg.commands()) {
        if (c.type == "DRIFT") {
            const QString nm = QString("Drift%1").arg(nd++);
            fl.engine.addElement(std::make_shared<FieldDrift>(c.params[0], c.params[1], nm.toStdString()));
            fl.names << nm;
        }
        else if (c.type == "QUAD") {
            const double L = c.params[0], B = c.params[1], ap = c.params[2];
            const double G = B / ap;                       // Тл/м
            const QString nm = QString("Quad%1").arg(nq++);
            fl.engine.addElement(std::make_shared<FieldQuadrupole>(
                G, L, ap, hardEdge, engeCosyQuadrupole(), ap, nm.toStdString()));
            fl.names << nm;
        }
        else if (c.type == "DEFLECTOR") {
            const double U_kV = c.params[0], R = std::fabs(c.params[1]);
            const double angle = std::fabs(c.params[2]) * deg, gap = c.params[3];
            const double xmin = c.params[4], xmax = c.params[5];
            const QString nm = QString("Deflector%1").arg(nde++);
            if (std::fabs(U_kV) < 1e-9) {
                fl.engine.addElement(std::make_shared<FieldDrift>(R * angle, -1.0, nm.toStdString()));
            } else {
                const double E0 = std::fabs(U_kV) * 1000.0 / gap;                  // В/м
                const int bend = (U_kV < 0.0) ? +1 : -1;                           // U<0 — влево
                fl.engine.addElement(std::make_shared<FieldElectrostaticDeflector>(
                    E0, R, angle, bend, std::max(std::fabs(xmin), std::fabs(xmax)), 0.15, hardEdge,
                    engeCosyQuadrupole(), gap, nm.toStdString()));
            }
            fl.names << nm;
        }
        else if (c.type == "DIPOLE") {
            const double B = c.params[0], R = std::fabs(c.params[1]);
            const double angle = std::fabs(c.params[2]) * deg, gap = c.params[3];
            const double e1 = c.params[4] * deg, e2 = c.params[5] * deg;
            const double xmin = c.params[6], xmax = c.params[7];
            const QString nm = QString("Dipole%1").arg(ndi++);
            if (std::fabs(B) < 1e-12) {
                fl.engine.addElement(std::make_shared<FieldDrift>(R * angle, -1.0, nm.toStdString()));
            } else {
                const int bend = (B >= 0.0) ? +1 : -1;                              // B>0 — влево
                fl.engine.addElement(std::make_shared<FieldDipole>(
                    std::fabs(B), R, angle, bend, e1, e2, xmin, xmax, gap / 2.0, hardEdge,
                    engeCosyDipole(), 2.0 * gap, nm.toStdString()));
            }
            fl.names << nm;
        }
    }
    return fl;
}
