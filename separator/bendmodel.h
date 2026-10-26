// bendmodel.h — аналитический транспорт через изгибающий элемент (диполь / дефлектор).
// Без Qt. Модель: жёсткие края, плоская геометрия «прямо — дуга — прямо», точная в плоскости
// изгиба; вертикальная фокусировка на наклонных гранях — тонкая линза -tan(e)/rho * y.
// Никаких опорных частиц: на вход только геометрия элемента и радиус кривизны ЭТОЙ частицы.
#pragma once
#include <cmath>

struct BendGeom {
    double R0     = 1.0;     // радиус дизайн-орбиты, м
    double theta0 = 0.1;     // дизайн-угол, рад
    int    bend   = +1;      // +1: изгиб к -x, -1: к +x (для положительного заряда)
    double e1 = 0, e2 = 0;   // углы граней, рад (положительный => дефокусировка по x)
    double xMin = -0.1, xMax = 0.1;   // апертура по x относительно дизайн-орбиты (ориентация как у x частицы)
    double yHalf = 0.05;     // апертура по y, м
    bool   kicks = true;     // вертикальные краевые «толчки»
};

inline bool bendInAperture(const BendGeom& g, double x, double y)
{
    return x >= g.xMin && x <= g.xMax && std::fabs(y) <= g.yHalf;
}

// x, xp (угол проекции на плоскость изгиба, рад), y, yp — на входе -> на выходе
// (выходная плоскость — радиальная плоскость на дизайн-угле theta0).
// rho — радиус кривизны частицы в поле, м (>0). false — частица не выходит из элемента.
inline bool bendTransport(const BendGeom& g, double rho,
                          double& x, double& xp, double& y, double& yp)
{
    const double s = (g.bend > 0) ? 1.0 : -1.0;
    const double X0 = s * x;
    double a = s * xp;
    double ty = std::tan(yp) * std::cos(xp);              // vy / v_горизонтальная

    // --- свободный пролёт до входной грани ---
    const double cdn = std::cos(a + g.e1);
    if (std::fabs(cdn) < 1e-6) return false;
    const double t1 = X0 * std::sin(g.e1) / cdn;
    const double X1 = X0 + t1 * std::sin(a);
    const double Z1 = t1 * std::cos(a);
    double y1 = y + ty * t1;

    double rh = rho / std::sqrt(1.0 + ty * ty);
    if (g.kicks) { ty += -std::tan(g.e1) * y1 / rh; rh = rho / std::sqrt(1.0 + ty * ty); }

    // --- дуга до выходной грани ---
    const double Cx = X1 - rh * std::cos(a);
    const double Cz = Z1 + rh * std::sin(a);
    const double th = g.theta0, R0 = g.R0;
    const double P2x = -(1.0 - std::cos(th)) * R0, P2z = R0 * std::sin(th);
    const double gam = th - g.e2;
    const double A = (Cx - P2x) * (-std::sin(gam)) + (Cz - P2z) * std::cos(gam);
    const double ratio = A / rh;
    if (std::fabs(ratio) > 1.0) return false;
    const double phi = gam + a - std::asin(ratio);
    if (phi < -0.3 || phi > 3.0) return false;

    const double a2 = a - phi;
    const double r2x = Cx + rh * std::cos(a2);
    const double r2z = Cz - rh * std::sin(a2);
    double y2 = y1 + ty * rh * phi;
    if (g.kicks) { ty += -std::tan(g.e2) * y2 / rh; }

    // --- в выходную рамку и свободный пролёт до её плоскости ---
    const double wx = r2x + R0, wz = r2z;
    const double Xe = wx * std::cos(th) + wz * std::sin(th) - R0;
    const double Ze = -wx * std::sin(th) + wz * std::cos(th);
    const double ae = a2 + th;
    const double cae = std::cos(ae);
    if (cae < 1e-6) return false;
    const double t2 = -Ze / cae;
    const double Xo = Xe + t2 * std::sin(ae);
    const double yo = y2 + ty * t2;

    x  = s * Xo;
    xp = s * ae;
    y  = yo;
    yp = std::atan(ty / cae);
    return true;
}
