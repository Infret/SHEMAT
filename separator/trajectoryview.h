#ifndef TRAJECTORYVIEW_H
#define TRAJECTORYVIEW_H

#include <QGraphicsView>
#include "trajectoryscene.h"

class TrajectoryView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TrajectoryView(QWidget *parent = nullptr);

    void setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                            const QVector<double>& elementStartZ);
    void addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints);
    void clearTrajectories();
    void resetZoom();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    TrajectoryScene *m_scene;
    QPointF m_lastPanPoint;
    bool m_panning;
};

#endif // TRAJECTORYVIEW_H
