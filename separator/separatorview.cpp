#include "separatorview.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>
#include <QDebug>

SeparatorView::SeparatorView(QWidget *parent) : QWidget(parent), m_totalLength(0)
{
    setMinimumSize(800, 400);
    setAutoFillBackground(true);
    setMouseTracking(true);
}

void SeparatorView::setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                                       const QVector<double>& elementStartZ)
{
    m_elements.clear();
    m_totalLength = 0;
    int driftCount = 0, quadCount = 0, dipoleCount = 0, deflCount = 0, edgeCount = 0;

    for (size_t i = 0; i < elements.size(); ++i) {
        double zStart = elementStartZ[i];
        double len = elements[i].getLength();
        double zEnd = zStart + len;
        if (zEnd > m_totalLength) m_totalLength = zEnd;

        ElementInfo info;
        info.zStart = zStart;
        info.zEnd = zEnd;
        info.type = elements[i].getType();
        info.name = elements[i].getName();

        // Генерируем короткое имя
        switch (info.type) {
            case ElementType::Drift:
                info.shortName = QString("D%1").arg(++driftCount);
                break;
            case ElementType::Quadrupole:
                info.shortName = QString("Q%1").arg(++quadCount);
                break;
            case ElementType::Dipole:
                info.shortName = QString("Dip%1").arg(++dipoleCount);
                break;
            case ElementType::ElectrostaticDeflector:
                info.shortName = QString("ES%1").arg(++deflCount);
                break;
            case ElementType::DipoleEdge:
                info.shortName = QString("Edg%1").arg(++edgeCount);
                break;
            default:
                info.shortName = "?";
                break;
        }

        m_elements.append(info);
    }
    resetZoom();
    update();
}

void SeparatorView::addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints)
{
    m_trajectoriesXZ.append(xzPoints);
    m_trajectoriesYZ.append(yzPoints);
    update();
}

void SeparatorView::clearTrajectories()
{
    m_trajectoriesXZ.clear();
    m_trajectoriesYZ.clear();
    update();
}

void SeparatorView::resetZoom()
{
    m_zoom = 1.0;
    m_offsetX = 0.0;
    m_offsetY = 0.0;
    update();
}

void SeparatorView::wheelEvent(QWheelEvent *event)
{
    double factor = 1.1;
    if (event->angleDelta().y() > 0)
        m_zoom *= factor;
    else
        m_zoom /= factor;
    if (m_zoom < 0.1) m_zoom = 0.1;
    if (m_zoom > 10.0) m_zoom = 10.0;
    update();
    event->accept();
}

void SeparatorView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void SeparatorView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_offsetX += delta.x();
        m_offsetY += delta.y();
        m_lastMousePos = event->pos();
        update();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void SeparatorView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning && event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}


void SeparatorView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    // Панорамирование и масштабирование
    painter.translate(width()/2 + m_offsetX, height()/2 + m_offsetY);
    painter.scale(m_zoom, m_zoom);
    painter.translate(-width()/2, -height()/2);

    int w = width();
    int h = height();
    int margin = 80;
    int graphWidth = w - 2 * margin;
    int graphHeight = (h - 130) / 2; // делим на два графика

    // ---- Функция преобразования Z в X ----
    auto zToX = [&](double z) -> double {
        if (m_totalLength <= 0) return margin;
        return margin + z / m_totalLength * graphWidth;
    };

    // ---- Цвета для типов элементов (полупрозрачные) ----
    auto getColor = [](ElementType type) -> QColor {
        switch (type) {
            case ElementType::Drift:          return QColor(180, 180, 180, 200);
            case ElementType::Quadrupole:     return QColor(100, 220, 100, 200);
            case ElementType::Dipole:         return QColor(100, 100, 220, 200);
            case ElementType::DipoleEdge:     return QColor(255, 180, 100, 200);
            case ElementType::ElectrostaticDeflector: return QColor(220, 100, 220, 200);
            default: return QColor(200, 200, 200, 200);
        }
    };

    // ---- Получение кисти для типа ----
    auto getBrush = [](ElementType type, const QColor& color) -> QBrush {
        switch (type) {
            case ElementType::Drift:
                return QBrush(Qt::NoBrush); // только контур
            case ElementType::Quadrupole:
                return QBrush(color, Qt::SolidPattern);
            case ElementType::Dipole:
                return QBrush(color, Qt::BDiagPattern);
            case ElementType::ElectrostaticDeflector:
                return QBrush(color, Qt::Dense4Pattern);
            case ElementType::DipoleEdge:
                return QBrush(color, Qt::SolidPattern); // для краевых полей - сплошная, но они будут тонкими
            default:
                return QBrush(color, Qt::SolidPattern);
        }
    };

    // ---- Определяем масштаб для осей ----
    double maxX = 0.01, maxY = 0.01;
    for (const auto& traj : m_trajectoriesXZ)
        for (const auto& p : traj) if (std::abs(p.y()) > maxX) maxX = std::abs(p.y());
    for (const auto& traj : m_trajectoriesYZ)
        for (const auto& p : traj) if (std::abs(p.y()) > maxY) maxY = std::abs(p.y());
    maxX *= 1.1; if (maxX < 0.01) maxX = 0.1;
    maxY *= 1.1; if (maxY < 0.01) maxY = 0.1;

    // ---- ГРАФИК XZ ----
    int xzTop = 50;
    int xzBottom = xzTop + graphHeight;
    double blockHeightRatio = 1.00; // высота блоков 45% от графика
    double blockHeight = graphHeight * blockHeightRatio;

    auto xzToScreen = [&](double z, double x) -> QPointF {
        double sx = zToX(z);
        double sy = xzTop + graphHeight/2 - (x / maxX) * (graphHeight/2);
        return QPointF(sx, sy);
    };

    // ---- 1. РИСУЕМ ЭЛЕМЕНТЫ НА XZ (фон) ----
    painter.setPen(QPen(Qt::black, 1));
    for (const auto& elem : m_elements) {
        if (elem.type == ElementType::DipoleEdge) {
            // Краевые поля – рисуем вертикальную линию
            double xPos = zToX(elem.zStart);
            painter.setPen(QPen(Qt::darkGray, 2, Qt::DashLine));
            painter.drawLine(xPos, xzTop, xPos, xzBottom);
            painter.setPen(QPen(Qt::black, 1));
            continue;
        }
        double x1 = zToX(elem.zStart);
        double x2 = zToX(elem.zEnd);
        if (std::abs(x2 - x1) < 2) continue;
        double yCenter = xzTop + graphHeight/2;
        QRectF rect(x1, yCenter - blockHeight/2, x2 - x1, blockHeight);
        QColor color = getColor(elem.type);
        QBrush brush = getBrush(elem.type, color);
        painter.fillRect(rect, brush);
        painter.drawRect(rect);
        // Подпись СВЕРХУ от блока
        painter.drawText(rect.x(), rect.y() - 12, rect.width(), 12,
                         Qt::AlignHCenter | Qt::AlignBottom, elem.shortName);
    }

    // ---- 2. СЕТКА И ОСИ XZ (поверх элементов, под траекториями) ----
    painter.setPen(QPen(QColor(220,220,220), 1));
    for (int i = -4; i <= 4; ++i) {
        double x = i * (maxX / 4);
        double y = xzTop + graphHeight/2 - (x / maxX) * (graphHeight/2);
        painter.drawLine(margin, y, w - margin, y);
        painter.drawText(margin - 35, y + 3, QString::number(x, 'f', 2));
    }
    int numTicks = 6;
    for (int i = 0; i <= numTicks; ++i) {
        double z = m_totalLength * i / numTicks;
        double x = margin + (z / m_totalLength) * graphWidth;
        painter.drawLine(x, xzTop, x, xzBottom);
        painter.drawText(x - 10, xzBottom + 15, QString::number(z, 'f', 2));
    }
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(margin, xzTop + graphHeight/2, w - margin, xzTop + graphHeight/2);
    painter.drawLine(margin, xzTop, margin, xzBottom);
    painter.drawText(w - margin + 5, xzTop + graphHeight/2 + 5, "Z (м)");
    painter.drawText(margin - 45, xzTop - 5, "X (м)");

    // ---- 3. ТРАЕКТОРИИ XZ (поверх всего) ----
    painter.setPen(QPen(Qt::red, 1.2));
    int totalXZ = m_trajectoriesXZ.size();
    int drawCount = m_showAll ? totalXZ : qMin(totalXZ, m_maxTrajectories);

    for (int i = 0; i < drawCount; ++i) {
        const auto& traj = m_trajectoriesXZ[i];
        QPolygonF poly;
        for (const auto& p : traj) poly << xzToScreen(p.x(), p.y());
        painter.drawPolyline(poly);
    }

    // ---- ГРАФИК YZ ----
    int yzTop = xzBottom + 25;
    int yzBottom = yzTop + graphHeight;
    blockHeight = graphHeight * blockHeightRatio;

    auto yzToScreen = [&](double z, double y) -> QPointF {
        double sx = zToX(z);
        double sy = yzTop + graphHeight/2 - (y / maxY) * (graphHeight/2);
        return QPointF(sx, sy);
    };

    // ---- 4. ЭЛЕМЕНТЫ НА YZ ----
    painter.setPen(QPen(Qt::black, 1));
    for (const auto& elem : m_elements) {
        if (elem.type == ElementType::DipoleEdge) {
            double xPos = zToX(elem.zStart);
            painter.setPen(QPen(Qt::darkGray, 2, Qt::DashLine));
            painter.drawLine(xPos, yzTop, xPos, yzBottom);
            painter.setPen(QPen(Qt::black, 1));
            continue;
        }
        double x1 = zToX(elem.zStart);
        double x2 = zToX(elem.zEnd);
        if (std::abs(x2 - x1) < 2) continue;
        double yCenter = yzTop + graphHeight/2;
        QRectF rect(x1, yCenter - blockHeight/2, x2 - x1, blockHeight);
        QColor color = getColor(elem.type);
        QBrush brush = getBrush(elem.type, color);
        painter.fillRect(rect, brush);
        painter.drawRect(rect);
        painter.drawText(rect.x(), rect.y() - 12, rect.width(), 12,
                         Qt::AlignHCenter | Qt::AlignBottom, elem.shortName);
    }

    // ---- 5. СЕТКА И ОСИ YZ ----
    painter.setPen(QPen(QColor(220,220,220), 1));
    for (int i = -4; i <= 4; ++i) {
        double y = i * (maxY / 4);
        double sy = yzTop + graphHeight/2 - (y / maxY) * (graphHeight/2);
        painter.drawLine(margin, sy, w - margin, sy);
        painter.drawText(margin - 35, sy + 3, QString::number(y, 'f', 2));
    }
    for (int i = 0; i <= numTicks; ++i) {
        double z = m_totalLength * i / numTicks;
        double x = margin + (z / m_totalLength) * graphWidth;
        painter.drawLine(x, yzTop, x, yzBottom);
        painter.drawText(x - 10, yzBottom + 15, QString::number(z, 'f', 2));
    }
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(margin, yzTop + graphHeight/2, w - margin, yzTop + graphHeight/2);
    painter.drawLine(margin, yzTop, margin, yzBottom);
    painter.drawText(w - margin + 5, yzTop + graphHeight/2 + 5, "Z (м)");
    painter.drawText(margin - 45, yzTop - 5, "Y (м)");

    // ---- 6. ТРАЕКТОРИИ YZ ----
    painter.setPen(QPen(Qt::blue, 1.2));
    int totalYZ = m_trajectoriesYZ.size();
    int drawCountYZ = m_showAll ? totalYZ : qMin(totalYZ, m_maxTrajectories);
    for (int i = 0; i < drawCountYZ; ++i) {
        const auto& traj = m_trajectoriesYZ[i];
        QPolygonF poly;
        for (const auto& p : traj) poly << yzToScreen(p.x(), p.y());
        painter.drawPolyline(poly);
    }

//    // ---- 7. ЛЕГЕНДА (с образцами стилей) ----
//    int legendX = w - 180;
//    int legendY = 10;
//    painter.setPen(Qt::black);
//    painter.setBrush(Qt::white);
//    painter.drawRect(legendX, legendY, 160, 150);
//    int yOff = 10;
//    struct LegendItem { QString label; ElementType type; };
//    QList<LegendItem> items = {
//        {"Дрейф", ElementType::Drift},
//        {"Квадруполь", ElementType::Quadrupole},
//        {"Диполь", ElementType::Dipole},
//        {"Дефлектор", ElementType::ElectrostaticDeflector},
//        {"Край", ElementType::DipoleEdge}
//    };
//    painter.setPen(Qt::black);
//    for (const auto& item : items) {
//        // Рисуем образец (маленький прямоугольник)
//        int x = legendX + 5;
//        int y = legendY + 5 + yOff;
//        QRect sampleRect(x, y, 20, 16);
//        QColor color = getColor(item.type);
//        QBrush brush = getBrush(item.type, color);
//        painter.fillRect(sampleRect, brush);
//        painter.drawRect(sampleRect);
//        // Подпись
//        painter.drawText(x + 25, y + 13, item.label);
//        yOff += 22;
//    }
}
