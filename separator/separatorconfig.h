#ifndef SEPARATORCONFIG_H
#define SEPARATORCONFIG_H

#include <QString>
#include <QVector>
#include "separator.h"

// Структура для хранения одной команды из конфигурационного файла
struct ConfigCommand {
    QString type;          // "DRIFT", "QUAD", "DEFLECTOR", "DIPOLE"
    QVector<double> params; // параметры команды (длины, поля, углы и т.д.)
};
struct OptimizableParam {
    QString elementName;
    QString paramName;
    double currentValue;
    double minValue = -1e6;
    double maxValue = 1e6;
    ElementType type;
    int commandIndex;
    int paramIndex;
};

class SeparatorConfig
{
public:
    SeparatorConfig();
    bool loadFromFile(const QString& filename);
    bool parseFromString(const QString& text);
    Separator buildSeparator(double energy, double mass, int charge) const;

    QVector<OptimizableParam> getOptimizableParams() const;

    void updateParam(int commandIndex, int paramIndex, double newValue);

private:
    QVector<ConfigCommand> m_commands;

};

#endif // SEPARATORCONFIG_H
