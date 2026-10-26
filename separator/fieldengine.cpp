// fieldengine.cpp — транспорт частицы через линию элементов (RK4 + аналитические дрейфы)
#include <algorithm>
#include <functional>
#include "fieldengine.h"

namespace ft {

TransportResult FieldEngine::transport(double x0, double y0, double xp0, double yp0,
                                       double eKinMeV, double massAmu, int charge,
                                       const IntegratorOpts& opts) const
{
    TransportResult res;

    TrackState st;
    st.massMeV = massAmu * AMU_MEV;
    st.charge  = charge;
    const double pmag = momentumFromEnergy(eKinMeV, st.massMeV);
    const Vec3 u0 = Vec3(std::tan(xp0), std::tan(yp0), 1.0).normalized();   // xp, yp — углы проекций
    st.r = Vec3(x0, y0, 0.0);
    st.p = u0 * pmag;

    Frame frame;
    frame.origin = Vec3(0, 0, 0);
    frame.ex = Vec3(1, 0, 0);
    frame.ey = Vec3(0, 1, 0);
    frame.ez = Vec3(0, 0, 1);

    double zAcc = 0.0;

    auto pushPoint = [&](const Frame& fr, const FieldElement* el,
                         const TrackState& s, double zDesign) {
        Vec3 l = toLocal(fr, s.r);
        if (el) l = el->designLocal(l);
        TrackPoint3 tp;
        tp.z = zDesign; tp.x = l.x; tp.y = l.y;
        tp.gx = s.r.x;  tp.gy = s.r.y;  tp.gz = s.r.z;
        res.track.push_back(tp);
    };
    pushPoint(frame, nullptr, st, 0.0);

    // прямой пролёт до плоскости (t может быть отрицательным — продолжение прямой назад)
    auto flyToPlane = [&](const Frame& plane) -> bool {
        const Vec3 u = st.p / st.p.length();
        const double uz = u.dot(plane.ez);
        if (std::fabs(uz) < 1e-6) return false;
        const double t = -(st.r - plane.origin).dot(plane.ez) / uz;
        st.r += u * t;
        return true;
    };

    for (size_t i = 0; i < m_elements.size(); ++i) {
        const FieldElement& el = *m_elements[i];
        const Frame entry = frame;
        const Frame exitF = el.exitFrame(entry);
        const double L = el.designLength();

        auto apertureGlobal = [&](const Vec3& rg) {
            return el.apertureLocal(el.designLocal(toLocal(entry, rg)));
        };
        auto fail = [&]() {
            res.alive = false;
            res.loss.elementIndex = static_cast<int>(i);
            res.loss.sDesign = zAcc;
            const Vec3 l = el.designLocal(toLocal(entry, st.r));
            res.loss.x = l.x; res.loss.y = l.y;
            res.x = l.x; res.y = l.y;
        };

        if (!apertureGlobal(st.r)) { fail(); return res; }

        if (el.isDrift()) {
            const Vec3 u = st.p / st.p.length();
            const double uz = u.dot(exitF.ez);
            if (uz < 1e-6) { fail(); return res; }
            double t = -(st.r - exitF.origin).dot(exitF.ez) / uz;
            if (t < 0.0) t = 0.0;
            st.r += u * t;
            if (!apertureGlobal(st.r)) { fail(); return res; }
            pushPoint(entry, &el, st, zAcc + L);
        } else {
            const FieldFn field = el.makeField(entry);
            auto recorder = [&](const TrackState& s, double sLocal) {
                pushPoint(entry, &el, s, zAcc + L + sLocal);
            };

            if (el.hasFaces()) {
                // --- вход: прямо до входной грани, краевой толчок, шаг внутрь поля ---
                if (!flyToPlane(el.entryFace(entry))) { fail(); return res; }
                st.r += (st.p / st.p.length()) * 1e-9;
                if (!apertureGlobal(st.r)) { fail(); return res; }
                el.faceKick(st, entry, true);

                // --- поле: до выходной грани ---
                const Frame f2 = el.exitFace(entry);
                auto apF2 = [&](const Vec3& v) { return apertureGlobal(toGlobalPoint(f2, v)); };
                const TrackStatus status =
                    integrateToExit(st, field, f2, 0.0, opts, apF2, recorder);
                if (status != TrackStatus::Ok) { fail(); return res; }

                // --- выход: толчок и прямо до выходной плоскости ---
                el.faceKick(st, entry, false);
                if (!flyToPlane(exitF)) { fail(); return res; }
                pushPoint(entry, &el, st, zAcc + L);
            } else {
                auto apExit = [&](const Vec3& vExit) {
                    return apertureGlobal(toGlobalPoint(exitF, vExit));
                };
                const TrackStatus status =
                    integrateToExit(st, field, exitF, 0.0, opts, apExit, recorder);
                if (status != TrackStatus::Ok) { fail(); return res; }
            }
        }

        zAcc += L;
        frame = exitF;

        {
            const Vec3 l = toLocal(frame, st.r);
            const Vec3 u = st.p / st.p.length();
            ElementExit ex;
            ex.z  = zAcc;
            ex.x  = l.x;  ex.y = l.y;
            ex.xp = std::atan2(u.dot(frame.ex), u.dot(frame.ez));
            ex.yp = std::atan2(u.dot(frame.ey), u.dot(frame.ez));
            const double pm = st.p.length();
            ex.eKin = std::sqrt(pm * pm + st.massMeV * st.massMeV) - st.massMeV;
            res.exits.push_back(ex);
        }
    }

    const Vec3 l = toLocal(frame, st.r);
    const Vec3 u = st.p / st.p.length();
    res.x = l.x; res.y = l.y;
    res.xp = std::atan2(u.dot(frame.ex), u.dot(frame.ez));
    res.yp = std::atan2(u.dot(frame.ey), u.dot(frame.ez));
    return res;
}

} // namespace ft
