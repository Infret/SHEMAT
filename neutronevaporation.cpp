#include "neutronevaporation.h"
#include "bindingenergydata.h"
#include <cmath>
#include <QDebug>
#include <random>
#include <numeric>
#include <QRandomGenerator>
#include <QMessageBox>
#include <chrono>
#include <QInputDialog>

NeutronEvaporation::NeutronEvaporation(double Z, double A, double U, double E, QWidget *parent)
    : QWidget(parent), Z(Z), A(A), U(U), E(E) // E в кэВ
{
}

NeutronEvaporation::~NeutronEvaporation()
{
}

void NeutronEvaporation::Evaporate_particles(double& Z, double& A, double U, double& E,
                                            int numProtons, int numNeutrons, int numAlphas,
                                            double& x, double& y, double& theta, double& phi,
                                            double& x_prime, double& y_prime, int& brk)//, bool &spAsked, bool &snAsked)
{
//    qDebug() << QString("x: %1, y: %2, x': %3, y': %4, E: %5, Q: %6, BR: %7 ")
//                .arg(x, 0, 'f', 2)
//                .arg(y, 0, 'f', 2)
//                .arg(x_prime, 0, 'f', 4)
//                .arg(y_prime, 0, 'f', 4)
//                .arg(E, 0, 'f', 2);
    BindingEnergyData bindingData;
//    bool ok;


//    BindingEnergyEntry entryTest = bindingData.get(A, Z);

//    //для протонов
//    if (!spAsked) {
//        if (entryTest.protonBindingEnergy <= 0.0 && numProtons != 0) {
//            double inputSp = QInputDialog::getDouble(
//                this,
//                tr("Введите энергию связи протона"),
//                tr("Sₚ для A=%1, Z=%2 (кэВ):").arg(A).arg(Z),
//                0.0,
//                0.0,
//                1e10,
//                2,
//                &ok
//            );
//            if (!ok) {
//                QMessageBox::warning(this, tr("Ввод отменён"),
//                                     tr("Без Sₚ расчёт невозможен"));
//                return;
//            }
//            entryTest.protonBindingEnergy = inputSp;
//        }
//        else {
//            entryTest.protonBindingEnergy = -1.123;
//        }
//        spAsked = true;
//    }

    //для нейтронов
//    if (!snAsked) {
//        if (entryTest.neutronBindingEnergy <= 0.0 && numNeutrons != 0) {
//            double inputSn = QInputDialog::getDouble(
//                this,
//                tr("Введите энергию связи нейтрона"),
//                tr("Sₙ для A=%1, Z=%2 (кэВ):").arg(A).arg(Z),
//                0.0,      // дефолтное значение
//                0.0,      // минимум
//                1e10,     // максимум
//                2,        // кол‑во знаков после запятой
//                &ok
//            );
//            if (!ok) {
//                QMessageBox::warning(this, tr("Ввод отменён"),
//                                     tr("Без Sₙ расчёт невозможен"));
//                return;
//            }
//            entryTest.neutronBindingEnergy = inputSn;
//        }
//        else {
//            entryTest.neutronBindingEnergy = -1.123;
//        }
//        snAsked = true;
//    }




    int EvCount = 0;
    brk = 0;
    if (U == 0) {
//        qDebug() << "Энергия возбуждения равна нулю. Пропуск испарения.";
        return;
    }
    U = U*1000; //в кэвы перевод
    // Константы (в кэВ)
    const double amu_to_keV = 931.5 * 1000;  // 1 а.е.м. = 931500 кэВ/c²
    const double c_sq = 931.5 * 1000;         // c² в [кэВ/а.е.м.]

    struct Particle {
        QString name;
        double Z;
        double A;
        double mass_keV;  // Масса в кэВ/c²
    };

    std::vector<Particle> particles = {
        {"Proton", 1, 1, 938.27  * 1000},   // 938270 кэВ/c²
        {"Neutron", 0, 1, 939.57 * 1000},  // 939570 кэВ/c²
        {"Alpha", 2, 4, 3727.38  * 1000}    // 3727380 кэВ/c²
    };
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);

    // Импульс в системе покоя ядра (E в кэВ)
    double M_compound = A * amu_to_keV;     // Масса ядра в кэВ/c²
    double P_initial = std::sqrt(2 * M_compound * E / c_sq);
    double P_x = P_initial * std::sin(theta) * std::cos(phi);
    double P_y = P_initial * std::sin(theta) * std::sin(phi);
    double P_z = P_initial * std::cos(theta);

    failureEvaporation = 1;

    while (true) {
        double Aev = A;
        double Zev = Z;
        double remainingU = U; // В мэвах
        int protonsEjected = 0;
        int neutronsEjected = 0;
        int alphasEjected = 0;

        double delta_Px = 0;
        double delta_Py = 0;
        double delta_Pz = 0;




        BindingEnergyEntry entry = bindingData.get(A, Z);
        //bool ok;
//        qDebug() << "Элемент:" << entry.element
//                 << "N =" << entry.N
//                 << "Z =" << entry.Z
//                 << "Sₙ =" << entry.neutronBindingEnergy
//                 << "Sₚ =" << entry.protonBindingEnergy;



        for (int i = 0; i < numProtons + numNeutrons + numAlphas; ++i) {
            Particle currentParticle;
            if (protonsEjected < numProtons) {
                currentParticle = particles[0];
                Zp = 1;
                Ap = 1;
                R0 = 1.222 * (std::pow(Ap, 1.0 / 3.0) + std::pow((Aev - Ap), 1.0 / 3.0)) + 2.0;
                C = 1.44 * Zp * (Zev - Zp) / R0;
                T = std::sqrt(8 * remainingU / Aev); // Температура ядра

                BindingEnergyEntry entry = bindingData.get(Aev, Zev);
                Sn = entry.protonBindingEnergy;

//                //колхоз для удобства в протоне
//                    if (entryTest.protonBindingEnergy == -1.123) {
//                    Sn = entry.protonBindingEnergy;
//                    }
//                    else {
//                        Sn = entryTest.protonBindingEnergy;
//                    }


                Zev -= 1;
                Aev -= 1;
                protonsEjected++;

            } else if (neutronsEjected < numNeutrons) {
                currentParticle = particles[1];
                Zp = 0;
                Ap = 1;
                R0 = 1.222 * (std::pow(Ap, 1.0 / 3.0) + std::pow((Aev - Ap), 1.0 / 3.0)) + 2.0;
                C = 1.44 * Zp * (Zev - Zp) / R0;
                T = std::sqrt(8 * remainingU / Aev); // Температура ядра
                BindingEnergyEntry entry = bindingData.get(Aev, Zev);
                Sn = entry.neutronBindingEnergy;

//                //колхоз для удобства в нейтроне
//                    if (entryTest.neutronBindingEnergy == -1.123) {
//                    Sn = entry.neutronBindingEnergy;
//                    }
//                    else {
//                        Sn = entryTest.neutronBindingEnergy;
//                    }

                Aev -= 1;
                neutronsEjected++;

            } else if (alphasEjected < numAlphas) {
                currentParticle = particles[2];
                Zp = 2;
                Ap = 4;
                R0 = 1.222 * (std::pow(Ap, 1.0 / 3.0) + std::pow((Aev - Ap), 1.0 / 3.0)) + 2.0;
                C = 1.44 * Zp * (Zev - Zp) / R0;
                T = std::sqrt(8 * remainingU / Aev); // Температура ядра
                Sn = (28.3 - 34.3 * std::pow(Zev - 2, 2.0/3.0) / Aev) * 1000; // Альфа  НЕ берется из таблицы!!!!!!!!!!
                Zev -= 2;
                Aev -= 4;
                alphasEjected++;
            } else {
                break;
            }

            // Проверка на отрицательные значения
            if (Zev < 0 || Aev < 0) {
                qDebug() << "Ошибка: отрицательный заряд или массовое число!";
                brk = 1;
                return;
            }





            //Расчет энергии связи (в кэВ)
//            double Sn = 15.7 * 1000 * (1 - 1.8 * std::pow(Zev/Aev, 2));  // Нейтрон
//            if (currentParticle.name == "Proton") {
//                Sn += 0.717 * 1000 * Zev / std::pow(Aev, 1.0/3.0);       // Протон (Нейтрон+кулоновский член)
//            } else if (currentParticle.name == "Alpha") {
//                Sn = (28.3 - 34.3 * std::pow(Zev - 2, 2.0/3.0) / Aev) * 1000; // Альфа
//            }
//            qDebug()<<"Sn-"<<Sn;
            // Генерация энергий
            std::vector<double> energies;
            std::vector<double> probabilities;
            int num_samples = 1000; // Количество выборок
            std::uniform_real_distribution<double> dist(C, remainingU);

            for (int j = 0; j < num_samples; ++j) {
                double E1 = dist(gen);
                double probability = probabilityDensity(E1, C, T);
                energies.push_back(E1);
                probabilities.push_back(probability);
            }

            // Нормализация вероятностей
            double sum_prob = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
            for (double &prob : probabilities) {
                prob /= sum_prob;
            }
            // Дискретное распределение
            std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());

            double selectedEnergy = 0.0;
            do {
                int index = distribution(gen);
                selectedEnergy = energies[index];
            } while (selectedEnergy > remainingU);

            // Обновление энергии возбуждения
            remainingU -= (Sn + selectedEnergy);
            ++EvCount;
            qDebug()<<"Частиц испарено"<<EvCount;
            qDebug()<<"selectedEnergy"<<selectedEnergy;
            qDebug()<<"remainingU"<<remainingU;

            if (remainingU < 0) {
                remainingU = 0;
                qDebug() << "Оставшаяся энергия возбуждения стала отрицательной!";
                break;
            }

            // Изотропное испарение (в системе покоя)
            double theta_part = M_PI * QRandomGenerator::global()->generateDouble();
            double phi_part = 2 * M_PI * QRandomGenerator::global()->generateDouble();

            // Импульс частицы (в кэВ/c)
            double p_part = sqrt(2 * currentParticle.mass_keV * selectedEnergy / c_sq);

            delta_Px -= p_part * sin(theta_part) * cos(phi_part);
            delta_Py -= p_part * sin(theta_part) * sin(phi_part);
            delta_Pz -= p_part * cos(theta_part);
//                          qDebug() << "Particle:" << currentParticle.name
//                                   << "Ejected Energy:" << selectedEnergy
//                                   << "Total Evaporation Energy:" << Sn + selectedEnergy
//                                   << "Remaining U:" << remainingU;

        }

        // Обновление импульса ядра
        P_x += delta_Px;
        P_y += delta_Py;
        P_z += delta_Pz;

        // Пересчёт энергии ядра (в кэВ)
        double M_new = Aev * amu_to_keV;
        E = (P_x*P_x + P_y*P_y + P_z*P_z) * c_sq / (2 * M_new);

        // Успешное испарение?
        if (remainingU > 0 && E > 0 &&
            protonsEjected == numProtons &&
            neutronsEjected == numNeutrons &&
            alphasEjected == numAlphas)
        {
            Z = Zev;
            A = Aev;

            U = remainingU;

            double P_total = sqrt(P_x*P_x + P_y*P_y + P_z*P_z);
            theta = acos(P_z / P_total);
            phi = atan2(P_y, P_x);

            x_prime = atan2(P_y, P_z);
            y_prime = atan2(P_x, P_z);

//            qDebug() << "Успех! Новая энергия ядра:" << E << "кэВ";
            break;
        }
        else
        {
            EvCount = 0;
            qDebug() << "fail";
            if (++failureEvaporation > 100) {
                QMessageBox::warning(nullptr, "Ошибка",
                    "Испарение крайне маловероятно для такого количества частиц с такой энергией возбуждения попробуйте изменить параметры");
                brk = 1;
                return;
            }
        }
    }
//    qDebug() << QString("x: %1, y: %2, x': %3, y': %4, E: %5, Q: %6, BR: %7 ")
//                .arg(x, 0, 'f', 2)
//                .arg(y, 0, 'f', 2)
//                .arg(x_prime, 0, 'f', 4)
//                .arg(y_prime, 0, 'f', 4)
//                .arg(E, 0, 'f', 2);
}

double NeutronEvaporation::probabilityDensity(double E, double C, double T) {
    return (E > C) ? (E - C) * exp(-(E - C)/T) : 0.0;
}
