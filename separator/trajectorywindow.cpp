#include "trajectorywindow.h"
#include "trajectoryview.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>

TrajectoryWindow::TrajectoryWindow(QWidget *parent) : QDialog(parent)
{



    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Траектории частиц");
    resize(900, 600);
    m_view = new SeparatorView(this);
//    QPushButton *resetZoomBtn = new QPushButton("Сбросить масштаб");
//    connect(resetZoomBtn, &QPushButton::clicked, m_view, &SeparatorView::resetZoom);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    QHBoxLayout *controlLayout = new QHBoxLayout();
    QCheckBox *showAllCheck = new QCheckBox("Показать все траектории");
    controlLayout->addWidget(showAllCheck);
    controlLayout->addStretch();
    layout->addLayout(controlLayout);

    connect(showAllCheck, &QCheckBox::toggled, m_view, &SeparatorView::setShowAll);

    // При необходимости установить начальное состояние
    m_view->setShowAll(false);
//    layout->addWidget(resetZoomBtn);
    setLayout(layout);
}

void TrajectoryWindow::setSeparatorScheme(const std::vector<SeparatorElement>& elements,
                                          const QVector<double>& elementStartZ)
{
    m_view->setSeparatorScheme(elements, elementStartZ);
}

void TrajectoryWindow::addTrajectory(const QVector<QPointF>& xzPoints, const QVector<QPointF>& yzPoints)
{
    m_view->addTrajectory(xzPoints, yzPoints);
}

void TrajectoryWindow::clearTrajectories()
{
    m_view->clearTrajectories();
}
