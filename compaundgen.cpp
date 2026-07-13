#include "compaundgen.h"

CompaundGen::CompaundGen(double mean, double variance, double a, double b, QWidget *parent)
    : QWidget(parent), mean(mean), variance(variance), a(a), b(b)
{

}

CompaundGen::~CompaundGen()
{
}

void CompaundGen::GenerateCoordinates(double a, double b, double& E, double& x, double& y, double& x_prime, double& y_prime)
{

    double u = QRandomGenerator::global()->generateDouble();
    double r = sqrt(QRandomGenerator::global()->generateDouble()); // Радиус, равномерно внутри круга
    double beta = 2 * M_PI * u;
    x = r * a * cos(beta)/2; // Преобразуем координаты
    y = r * b * sin(beta)/2;


    x_prime = 0; // Берем, что пучки летят параллельно
    y_prime = 0;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(mean, mean * variance / 100);
    E = distribution(generator); // Энергия
//    qDebug()<<"E начальное - "<<E;
}

//    double CompaundGen::generateParticleEnergy()
//    {

//        std::normal_distribution<double> distribution(mean, mean * variance / 100);
//        return distribution(generator);
//    }

