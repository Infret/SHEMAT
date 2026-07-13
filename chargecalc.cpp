#include "chargecalc.h"

ChargeCalc::ChargeCalc(double Z, double Mass, double Q, double E, QWidget *parent) :
    QWidget(parent), Z(Z), Mass(Mass), Q(Q), E(E)
{
}

ChargeCalc::~ChargeCalc()
{
}

void ChargeCalc::CalculateQ(double& Z, double& Mass, double& Q, double& E)
{


    // Задание постоянных параметров
    const double c = 3.0e10;// Скорость света в см/с
    //qDebug()<<"E"<<E;
    // Расчет скорости частицы
    double V = c * std::sqrt(1 - std::pow((Mass * 931.5 / (E/1000 + Mass * 931.5)), 2));
    //qDebug() << "V:" << V;
    double X = V / (3.6e8 * std::pow(Z, 0.45));
    // Формула для расчета I по Николаеву-Дмитриеву
    double I = Z * std::pow((1 + std::pow(X, -5.0 / 3.0)), -3.0 / 5.0);
//    qDebug() << "I:" << I;
    //qDebug() << "Z:" << Z;
    //qDebug() << "I/Z:" << I / Z;

//        // Проверка диапазона I/Z
//        if (I / Z < 0.3 || I / Z > 0.7) {
//            qDebug() << "Значение I/Z для формулы Николаева-Дмитриева находится вне рекомендуемого диапазона (0.3 - 0.7). Результаты вычислений могут быть неточными.";
//        }
//    double mean = I;
    // Стандартная формула для D
    double D = 0.5 * std::pow(I, 0.5) * std::pow((1 - I / Z), 5.0 / 6.0);
//    qDebug() << "D:" << D;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(I, D);


    Q = distribution(generator);  // Вызов функции для генерации Q
    if (Q < 0)
    {
        Q = 0;//потом ПОМЕНЯТЬ!!!!!!!!!!!!!!
    }
//    qDebug() << "Сгенерированное значение Q:" << Q;
}


