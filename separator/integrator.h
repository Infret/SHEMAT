// integrator.h — RK4 с адаптацией шага методом удвоения
#pragma once
#include "fieldcore.h"
#include <functional>
#include <algorithm>
#include <cmath>

namespace ft {

// Поле в ГЛОБАЛЬНОЙ точке (элемент сам делает пересчёт в локальные координаты)
using FieldFn = std::function<FieldSample(const Vec3& r)>;

struct IntegratorOpts {
    double h      = 0.002;   // базовый шаг, м
    double hMin   = 1e-5;
    double hMax   = 0.05;
    double tol    = 0.0;     // > 0 — включить адаптацию шага
    long   maxSteps = 200000;
    int    recordEvery = 0;  // каждые N принятых шагов вызывать recorder
};

// Один шаг RK4
inline void rk4Step(TrackState& st, const FieldFn& field, double h) {
    Vec3 k1r, k1p, k2r, k2p, k3r, k3p, k4r, k4p;

    equationsOfMotion(st, field(st.r), k1r, k1p);

    TrackState s2 = st; s2.r += k1r * (h * 0.5); s2.p += k1p * (h * 0.5);
    equationsOfMotion(s2, field(s2.r), k2r, k2p);

    TrackState s3 = st; s3.r += k2r * (h * 0.5); s3.p += k2p * (h * 0.5);
    equationsOfMotion(s3, field(s3.r), k3r, k3p);

    TrackState s4 = st; s4.r += k3r * h; s4.p += k3p * h;
    equationsOfMotion(s4, field(s4.r), k4r, k4p);

    st.r += (k1r + (k2r + k3r) * 2.0 + k4r) * (h / 6.0);
    st.p += (k1p + (k2p + k3p) * 2.0 + k4p) * (h / 6.0);
}

enum class TrackStatus { Ok = 0, ApertureLoss, Reversed, StepLimit };

// Интегрирование до плоскости z_local = zExit в рамке fr.
// Последний шаг укорачивается так, чтобы попасть в плоскость (точность RK4 сохраняется).
inline TrackStatus integrateToExit(TrackState& st, const FieldFn& field,
                                   const Frame& fr, double zExit,
                                   const IntegratorOpts& opt,
                                   const std::function<bool(const Vec3&)>& apertureOk,
                                   const std::function<void(const TrackState&, double)>& recorder)
{
    auto zLoc = [&](const TrackState& s) { return (s.r - fr.origin).dot(fr.ez); };

    double zl = zLoc(st);
    if (zl >= zExit) return TrackStatus::Ok;
    if (apertureOk && !apertureOk(toLocal(fr, st.r))) return TrackStatus::ApertureLoss;

    double h = opt.h;
    long accepted = 0;

    for (long i = 0; i < opt.maxSteps; ++i) {
        // не перешагиваем выходную плоскость: укорачиваем шаг
        double hStep = h;
        const double uzNow = st.p.dot(fr.ez) / st.p.length();
        if (uzNow > 1e-6) {
            const double hLand = (zExit - zl) / uzNow;
            if (hLand < hStep) hStep = hLand;
        }

        TrackState trial = st;
        rk4Step(trial, field, hStep);

        if (opt.tol > 0.0) {
            TrackState half = st;
            rk4Step(half, field, hStep * 0.5);
            rk4Step(half, field, hStep * 0.5);
            const double err = (trial.r - half.r).length() + (trial.p - half.p).length();
            if (err > opt.tol) {
                h *= std::max(0.2, 0.9 * std::pow(opt.tol / err, 0.2));
                if (h < opt.hMin) return TrackStatus::StepLimit;
                continue;
            }
            trial = half;
            const double grow = 0.9 * std::pow(opt.tol / std::max(err, 1e-300), 0.2);
            h = std::min(opt.hMax, std::max(opt.hMin, h * std::min(2.0, grow)));
        }

        // частица развернулась — стоп
        const double uz = trial.p.dot(fr.ez) / trial.p.length();
        if (uz < 1e-6) return TrackStatus::Reversed;

        const Vec3 prevR = st.r, prevP = st.p;
        const double prevZ = zl;
        st = trial;
        zl = zLoc(st);
        ++accepted;

        if (apertureOk && !apertureOk(toLocal(fr, st.r)))
            return TrackStatus::ApertureLoss;

        if (recorder && opt.recordEvery > 0 && (accepted % opt.recordEvery) == 0)
            recorder(st, zl);

        if (zl >= zExit - 1e-12) {
            if (zl > zExit) {   // микро-доводка (расстояние ничтожно)
                const double a = (zExit - prevZ) / (zl - prevZ);
                st.r = lerp(prevR, st.r, a);
                st.p = lerp(prevP, st.p, a);
            }
            if (recorder) recorder(st, zExit);
            return TrackStatus::Ok;
        }
    }
    return TrackStatus::StepLimit;
}

} // namespace ft
