#include "trajectoryscene.h"
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>

TrajectoryScene::TrajectoryScene(QObject *parent)
    : QGraphicsScene(parent), m_totalLength(0), m_minX(0), m_maxX(0), m_minY(0), m_maxY(0)
{
    setBackgroundBrush(Qt::white);
}

void TrajectoryScene::setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                                         const QVector<double>& elementStartZ)
{
    // Удаляем старые элементы схемы, но сохраняем траектории
    QList<QGraphicsItem*> allItems = items();
    for (QGraphicsItem* item : allItems) {
        bool isTrajectory = false;
        for (auto traj : m_trajectoryItemsXZ) {
            if (item == traj) { isTrajectory = true; break; }
        }
        if (!isTrajectory) {
            for (auto traj : m_trajectoryItemsYZ) {
                if (item == traj) { isTrajectory = true; break; }
            }
        }
        if (!isTrajectory) {
            removeItem(item);
            delete item;
        }
    }

    m_elements.clear();
    m_totalLength = 0;
    for (size_t i = 0; i < elements.size(); ++i) {
        double zStart = elementStartZ[i];
        double len = elements[i].getLength();
        double zEnd = zStart + len;
        if (zEnd > m_totalLength) m_totalLength = zEnd;

        ElementInfo info;
        info.zStart = zStart;
        info.zEnd = zEnd;

        switch (elements[i].getType()) {
        case ElementType::Drift: info.name = "Drift"; info.color = QColor(200,200,200); break;
        case ElementType::Quadrupole: info.name = "Quad"; info.color = QColor(150,200,150); break;
        case ElementType::Dipole: info.name = "Dipole"; info.color = QColor(150,150,200); break;
        case ElementType::DipoleEdge: info.name = "Edge"; info.color = QColor(255,200,150); break;
        case ElementType::ElectrostaticDeflector: info.name = "Defl"; info.color = QColor(200,150,200); break;
        default: info.name = "?"; info.color = Qt::gray;
        }
        m_elements.append(info);
    }

    // Рисуем прямоугольники элементов
    double elementHeight = 0.4;
    double axisY = 0;
    for (const auto& elem : m_elements) {
        QRectF rect(elem.zStart, axisY - elementHeight/2,
                    elem.zEnd - elem.zStart, elementHeight);
        QGraphicsRectItem *rectItem = addRect(rect, QPen(Qt::black), QBrush(elem.color));
        rectItem->setZValue(-10); // схема на заднем плане

        QGraphicsTextItem *text = addText(elem.name);
        text->setPos(elem.zStart + (elem.zEnd - elem.zStart)/2 - text->boundingRect().width()/2,
                     axisY - text->boundingRect().height()/2);
        text->setDefaultTextColor(Qt::black);
        text->setZValue(-5); // текст поверх схемы, но позади траекторий
    }

    updateAxes();
}

void TrajectoryScene::addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints)
{
    // XZ траектория (красная)
    QPainterPath pathXZ;
    if (!xzPoints.isEmpty()) {
        pathXZ.moveTo(xzPoints.first());
        for (int i = 1; i < xzPoints.size(); ++i)
            pathXZ.lineTo(xzPoints[i]);
    }
    QGraphicsPathItem *pathItemXZ = addPath(pathXZ, QPen(Qt::red, 2));
    pathItemXZ->setZValue(10); // траектории поверх всего
    m_trajectoryItemsXZ.append(pathItemXZ);

    // YZ траектория (синяя) – смещаем по Y, чтобы не перекрывать XZ
    QPainterPath pathYZ;
    if (!yzPoints.isEmpty()) {
        QVector<QPointF> shifted;
        for (auto p : yzPoints)
            shifted.append(QPointF(p.x(), p.y() - 0.5)); // смещение вниз
        pathYZ.moveTo(shifted.first());
        for (int i = 1; i < shifted.size(); ++i)
            pathYZ.lineTo(shifted[i]);
    }
    QGraphicsPathItem *pathItemYZ = addPath(pathYZ, QPen(Qt::blue, 2));
    pathItemYZ->setZValue(10);
    m_trajectoryItemsYZ.append(pathItemYZ);
}

void TrajectoryScene::clearTrajectories()
{
    for (auto item : m_trajectoryItemsXZ) {
        removeItem(item);
        delete item;
    }
    for (auto item : m_trajectoryItemsYZ) {
        removeItem(item);
        delete item;
    }
    m_trajectoryItemsXZ.clear();
    m_trajectoryItemsYZ.clear();
}

void TrajectoryScene::updateAxes()
{
    // Удаляем старые оси
    if (m_xAxisLine) { removeItem(m_xAxisLine); delete m_xAxisLine; m_xAxisLine = nullptr; }
    if (m_yAxisLine) { removeItem(m_yAxisLine); delete m_yAxisLine; m_yAxisLine = nullptr; }
    if (m_xLabel) { removeItem(m_xLabel); delete m_xLabel; m_xLabel = nullptr; }
    if (m_yLabel) { removeItem(m_yLabel); delete m_yLabel; m_yLabel = nullptr; }
    for (auto tick : m_xTicks) { removeItem(tick); delete tick; }
    for (auto label : m_xTickLabels) { removeItem(label); delete label; }
    m_xTicks.clear();
    m_xTickLabels.clear();

    double zMin = 0;
    double zMax = m_totalLength;

    // Определяем диапазоны по траекториям
    m_minX = -0.2; m_maxX = 0.2;
    m_minY = -0.2; m_maxY = 0.2;
    for (auto item : m_trajectoryItemsXZ) {
        QRectF rect = item->boundingRect();
        if (rect.bottom() < m_minX) m_minX = rect.bottom();
        if (rect.top() > m_maxX) m_maxX = rect.top();
    }
    for (auto item : m_trajectoryItemsYZ) {
        QRectF rect = item->boundingRect();
        if (rect.bottom() < m_minY) m_minY = rect.bottom();
        if (rect.top() > m_maxY) m_maxY = rect.top();
    }

    QPen axisPen(Qt::black, 2);
    // Горизонтальная ось Z
    m_xAxisLine = addLine(zMin, 0, zMax, 0, axisPen);
    m_xAxisLine->setZValue(-2);
    m_xLabel = addText("Z (м)");
    m_xLabel->setPos(zMax + 0.05, -0.05);
    m_xLabel->setZValue(-2);

    // Вертикальная ось X
    m_yAxisLine = addLine(0, m_minX, 0, m_maxX, axisPen);
    m_yAxisLine->setZValue(-2);
    m_yLabel = addText("X (м)");
    m_yLabel->setPos(-0.1, m_maxX + 0.05);
    m_yLabel->setZValue(-2);

    // Деления на оси Z
    int numTicks = 5;
    for (int i = 0; i <= numTicks; ++i) {
        double z = zMin + (zMax - zMin) * i / numTicks;
        QGraphicsLineItem *tick = addLine(z, -0.02, z, 0.02, QPen(Qt::black, 1));
        tick->setZValue(-2);
        m_xTicks.append(tick);
        QGraphicsTextItem *label = addText(QString::number(z, 'f', 2));
        label->setPos(z - 0.03, -0.1);
        label->setZValue(-2);
        m_xTickLabels.append(label);
    }
}

void TrajectoryScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    // Сетка
    painter->setPen(QPen(QColor(220,220,220), 1));
    double step = 0.1;
    double left = rect.left();
    double right = rect.right();
    double top = rect.top();
    double bottom = rect.bottom();
    for (double x = std::ceil(left / step) * step; x < right; x += step)
        painter->drawLine(x, top, x, bottom);
    for (double y = std::ceil(top / step) * step; y < bottom; y += step)
        painter->drawLine(left, y, right, y);
}
