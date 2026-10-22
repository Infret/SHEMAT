#include "fieldengine.h"
#include <cmath>

namespace ft {

TransportResult FieldEngine::transport(double x0, double y0, double xp0, double yp0,
                                       double eKinMeV, double massAmu, int charge,
                                       const IntegratorOpts& opts) const
{
    TransportResult res;

    // --- Начальное состояние в глобальной системе ---
    TrackState st;
    st.massMeV = massAmu * AMU_MEV;
    st.charge  = charge;
    const double pMag = momentumFromEnergy(eKinMeV, st.massMeV);
    // Ваше соглашение: x' = dx/dz = tan(xp) (как в Drift: p.x += L*tan(p.xp))
    const Vec3 uLocal(std::tan(xp0), std::tan(yp0), 1.0);
    Frame frame = makeFrame(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(1, 0, 0));
    st.p = toGlobalDir(frame, uLocal).normalized() * pMag;
    st.r = toGlobalPoint(frame, Vec3(x0, y0, 0.0));

    double designZ = 0.0;

    for (size_t i = 0; i < m_elements.size(); ++i) {
        const FieldElement& el = *m_elements[i];
        const double zStart = designZ;

        // Апертура на ВХОДЕ в элемент
        if (!el.apertureLocal(toLocal(frame, st.r))) {
            res.alive = false;
            res.loss = { (int)i, designZ, 0, 0 };
            break;
        }

        // --- Запись точек трека ---
        const size_t elemIndex = i;
        auto recorder = [&](const TrackState& s, double sLocal) {
            const Vec3 rl = toLocal(frame, s.r);
            TrackPoint3 pt;
            pt.z  = zStart + sLocal;          // дизайн-координата (для графиков x(z))
            pt.x  = rl.x; pt.y = rl.y;
            pt.gx = s.r.x; pt.gy = s.r.y; pt.gz = s.r.z;
            res.track.push_back(pt);
        };
        (void)elemIndex;

        bool ok = true;

        if (el.isDrift()) {
            // --- Аналитический дрейф: перелёт до плоскости z_local = L ---
            const double L = el.designLength();
            const Vec3 u = st.p / st.p.length();
            const double denom = u.dot(frame.ez);
            if (denom <= 1e-9) { res.alive = false; res.loss = {(int)i, designZ, 0, 0}; break; }
            const double t = (L - (st.r - frame.origin).dot(frame.ez)) / denom;
            st.r += u * t;
            recorder(st, L);
            if (!el.apertureLocal(toLocal(frame, st.r))) {
                res.alive = false;
                const Vec3 rl = toLocal(frame, st.r);
                res.loss = { (int)i, designZ + L, rl.x, rl.y };
                break;
            }
        } else {
            // --- Интегрирование RK4 до выходной грани ---
            const FieldFn field = el.makeField(frame);
            auto apertureCb = [&el, &frame](const Vec3& rGlobal) {
                return el.apertureLocal(toLocal(frame, rGlobal));
            };
            const TrackStatus status = integrateToExit(
                st, field, frame, el.exitZ(), opts, apertureCb, recorder);

            if (status != TrackStatus::Ok) {
                res.alive = false;
                const Vec3 rl = toLocal(frame, st.r);
                res.loss = { (int)i, designZ + rl.z, rl.x, rl.y };
                break;
            }
            ok = true;
        }
        (void)ok;

        designZ += el.designLength();
        frame = el.exitFrame(frame);

        // Глобальная точка в конце элемента (как делает матричный Separator)
        {
            const Vec3 rl = toLocal(frame, st.r);
            const Vec3 u  = st.p / st.p.length();
            TrackPoint3 pt;
            pt.z = designZ; pt.x = rl.x; pt.y = rl.y;
            pt.gx = st.r.x; pt.gy = st.r.y; pt.gz = st.r.z;
            res.track.push_back(pt);
        }
    }

    // --- Перевод обратно в локальные координаты финальной рамки ---
    const Vec3 rl = toLocal(frame, st.r);
    const Vec3 u  = (st.p / st.p.length());
    res.x  = rl.x;
    res.y  = rl.y;
    res.xp = std::atan2(u.dot(frame.ex), u.dot(frame.ez));   // = угол x'
    res.yp = std::atan2(u.dot(frame.ey), u.dot(frame.ez));

    return res;
}

} // namespace ft
