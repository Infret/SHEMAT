#ifndef BINDINGENERGYDATA_H
#define BINDINGENERGYDATA_H

#include <QString>
#include <QMap>
#include <QPair>

struct BindingEnergyEntry {
    int N;                     // Количество нейтронов
    int Z;                     // Количество протонов
    int A;                     // Массовое число (N + Z)
    QString element;           // Символ элемента
    double neutronBindingEnergy;  // Энергия связи нейтрона (кэВ?)
    double protonBindingEnergy;   // Энергия связи протона (кэВ?)
};

class BindingEnergyData {
public:
    BindingEnergyData();

    BindingEnergyEntry get(int A, int Z) const;
    double getNeutronBindingEnergy(int A, int Z) const;
    double getProtonBindingEnergy(int A, int Z) const;

private:
    QMap<QPair<int, int>, BindingEnergyEntry> bindingData;
};

#endif // BINDINGENERGYDATA_H
