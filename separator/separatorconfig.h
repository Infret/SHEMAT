#ifndef SEPARATORCONFIG_H
#define SEPARATORCONFIG_H

#include <QString>
#include <QVector>
#include "separator.h"

struct ConfigCommand {
    QString type;          // "DRIFT", "QUAD", "DEFLECTOR", "DIPOLE"
    QVector<double> params;
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

    Separator buildSeparator() const;

    const QVector<ConfigCommand>& commands() const { return m_commands; }
    QVector<OptimizableParam> getOptimizableParams() const;
    void updateParam(int commandIndex, int paramIndex, double newValue);

private:
    QVector<ConfigCommand> m_commands;
};

#endif // SEPARATORCONFIG_H
