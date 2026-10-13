#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector>
#include <QPointF>
#include <QString>

struct TrackPoint {
    double z;
    double x, y;
    double xp, yp;
    double energy;
    double mass;
    int charge;

    QString name;   // название точки (например, "Мишень", "После стрипера", "Quad1" и т.д.)
};

struct Particle
{
    double x; double y; double xp; double yp;
    double energy; double mass; int charge;
    bool alive;
    int Z, A;
    QVector<TrackPoint> track;

    Particle() : x(0), y(0), xp(0), yp(0), energy(0), mass(0), charge(0), alive(true) {}

    void clearTrack() { track.clear(); }

    void addTrackPoint(double z, double x, double y, double xp, double yp,
                       double energy, double mass, int charge, const QString& name) {
        track.append({z, x, y, xp, yp, energy, mass, charge, name});
    }
};

#endif // PARTICLE_H
