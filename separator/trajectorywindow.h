#ifndef TRAJECTORYWINDOW_H
#define TRAJECTORYWINDOW_H

#include <QDialog>
#include "trajectoryview.h"   // изменили с separatorview.h на trajectoryview.h
#include "separatorview.h"

class TrajectoryWindow : public QDialog
{
    Q_OBJECT
public:
    explicit TrajectoryWindow(QWidget *parent = nullptr);

    void setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                            const QVector<double>& elementStartZ);
    void addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints);
    void clearTrajectories();

private:
    SeparatorView *m_view;
};

#endif // TRAJECTORYWINDOW_H
