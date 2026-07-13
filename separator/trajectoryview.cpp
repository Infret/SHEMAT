#include "trajectoryview.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <cmath>

TrajectoryView::TrajectoryView(QWidget *parent)
    : QGraphicsView(parent), m_panning(false)
{
    m_scene = new TrajectoryScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TrajectoryView::setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                                        const QVector<double>& elementStartZ)
{
    m_scene->setSeparatorScheme(elements, elementStartZ);
    // Автоматически подгоняем вид
    fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void TrajectoryView::addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints)
{
    m_scene->addTrajectory(xzPoints, yzPoints);
}

void TrajectoryView::clearTrajectories()
{
    m_scene->clearTrajectories();
}

void TrajectoryView::wheelEvent(QWheelEvent *event)
{
    double factor = 1.1;
    if (event->angleDelta().y() > 0)
        scale(factor, factor);
    else
        scale(1/factor, 1/factor);
}

void TrajectoryView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void TrajectoryView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPointF delta = event->pos() - m_lastPanPoint;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastPanPoint = event->pos();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TrajectoryView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning && event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void TrajectoryView::resetZoom()
{
    if (m_scene && !m_scene->items().isEmpty())
        fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
