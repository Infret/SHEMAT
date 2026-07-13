#ifndef SEPARATORVIEW_H
#define SEPARATORVIEW_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QColor>
#include "separatorelement.h"

class SeparatorView : public QWidget
{
    Q_OBJECT
public:
    explicit SeparatorView(QWidget *parent = nullptr);

    void setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                            const QVector<double>& elementStartZ);
    void addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints);
    void clearTrajectories();
    void resetZoom();



    void setMaxTrajectories(int max) { m_maxTrajectories = max; update(); }
    void setShowAll(bool all) { m_showAll = all; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct ElementInfo {
        double zStart, zEnd;
        QString name;          // полное имя (опционально)
        QString shortName;     // сокращённое имя для отображения
        QColor color;
        ElementType type;
    };
    QVector<ElementInfo> m_elements;
    QVector<QVector<QPointF>> m_trajectoriesXZ;
    QVector<QVector<QPointF>> m_trajectoriesYZ;
    double m_totalLength;

    double m_zoom = 1.0;
    double m_offsetX = 0.0, m_offsetY = 0.0;
    QPoint m_lastMousePos;
    bool m_panning = false;


    int m_maxTrajectories = 250;
    bool m_showAll = false;
};

#endif // SEPARATORVIEW_H
