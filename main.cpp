#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    QApplication a(argc, argv);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");

//    QFont defaultFont = a.font();
//    defaultFont.setPointSize(10); // или 11, подберите
//    a.setFont(defaultFont);

    MainWindow w;
    w.show();
    return a.exec();
}
