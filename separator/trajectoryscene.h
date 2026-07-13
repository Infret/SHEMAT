#ifndef TRAJECTORYSCENE_H
#define TRAJECTORYSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QPointF>
#include "separatorelement.h"

class TrajectoryScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit TrajectoryScene(QObject *parent = nullptr);

    void setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                            const QVector<double>& elementStartZ);
    void addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints);
    void clearTrajectories();
    void updateAxes();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    struct ElementInfo {
        double zStart, zEnd;
        QString name;
        QColor color;
    };
    QVector<ElementInfo> m_elements;
    QVector<QGraphicsPathItem*> m_trajectoryItemsXZ;
    QVector<QGraphicsPathItem*> m_trajectoryItemsYZ;
    double m_totalLength;
    double m_maxX, m_minX, m_maxY, m_minY; // для осей
    QGraphicsLineItem *m_xAxisLine, *m_yAxisLine;
    QGraphicsTextItem *m_xLabel, *m_yLabel;
    QVector<QGraphicsLineItem*> m_xTicks;
    QVector<QGraphicsTextItem*> m_xTickLabels;
};

#endif // TRAJECTORYSCENE_H
