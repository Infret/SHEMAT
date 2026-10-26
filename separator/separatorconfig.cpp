#include "separatorconfig.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <cmath>

SeparatorConfig::SeparatorConfig()
{}

bool SeparatorConfig::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;
        lines.append(line);
    }
    file.close();

    return parseFromString(lines.join('\n'));
}

bool SeparatorConfig::parseFromString(const QString& text) {
    m_commands.clear();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#')) continue;

        QStringList parts = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        QString type = parts[0].toUpper();
        ConfigCommand cmd;
        cmd.type = type;

        if (type == "DRIFT") {
            if (parts.size() < 2) continue;
            double len = parts[1].toDouble();
            double aper = (parts.size() >= 3) ? parts[2].toDouble() : 0.1;
            cmd.params << len << aper;
        }
        else if (type == "QUAD") {
            if (parts.size() < 4) continue;
            double len = parts[1].toDouble();
            double fieldTl = parts[2].toDouble();
            double aperture = parts[3].toDouble();
            double fringe = (parts.size() >= 5) ? parts[4].toDouble() : 0.2;
            cmd.params << len << fieldTl << aperture << fringe;
        }
        else if (type == "DEFLECTOR") {
            // Новый формат: DEFLECTOR U_кВ R_m угол_град gap x_min x_max
            if (parts.size() < 7) continue;
            double U = parts[1].toDouble();
            double R = parts[2].toDouble();
            double angle = parts[3].toDouble();
            double gap = parts[4].toDouble();
            double xmin = parts[5].toDouble();
            double xmax = parts[6].toDouble();
            cmd.params << U << R << angle << gap << xmin << xmax;
        }
        else if (type == "DIPOLE") {
            // Новый формат: DIPOLE B_Тл R_m угол_град gap e1_град e2_град x_min x_max
            if (parts.size() < 9) continue;
            double B = parts[1].toDouble();
            double R = parts[2].toDouble();
            double angle = parts[3].toDouble();
            double gap = parts[4].toDouble();
            double e1 = parts[5].toDouble();
            double e2 = parts[6].toDouble();
            double xmin = parts[7].toDouble();
            double xmax = parts[8].toDouble();
            cmd.params << B << R << angle << gap << e1 << e2 << xmin << xmax;
        }
        else {
            qDebug() << "Unknown command:" << type;
            continue;
        }
        m_commands.append(cmd);
    }
    return true;
}

Separator SeparatorConfig::buildSeparator() const {
    Separator sep;
    int driftCount = 0, quadCount = 0, deflCount = 0, dipoleCount = 0;

    for (const auto& cmd : m_commands) {
        if (cmd.type == "DRIFT") {
            SeparatorElement elem(ElementType::Drift);
            elem.setParameters(cmd.params[0], cmd.params[1]);
            elem.setName(QString("Drift%1").arg(driftCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "QUAD") {
            double gradient = (cmd.params[1] * 10.0) / cmd.params[2]; // кГс/м
            SeparatorElement elem(ElementType::Quadrupole);
            elem.setParameters(cmd.params[0], gradient, cmd.params[2], cmd.params[3]);
            elem.setName(QString("Quad%1").arg(quadCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "DEFLECTOR") {
            SeparatorElement elem(ElementType::ElectrostaticDeflector);
            // U, R, angle, gap, xmin, xmax
            elem.setParameters(cmd.params[0], cmd.params[1], cmd.params[2],
                               cmd.params[3], cmd.params[4], cmd.params[5]);
            elem.setName(QString("Deflector%1").arg(deflCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "DIPOLE") {
            SeparatorElement dipole(ElementType::Dipole);
            // B, R, angle, gap, e1, e2, xmin, xmax
            dipole.setParameters(cmd.params[0], cmd.params[1], cmd.params[2],
                                 cmd.params[3], cmd.params[4], cmd.params[5],
                                 cmd.params[6], cmd.params[7]);
            dipole.setName(QString("Dipole%1").arg(dipoleCount++));
            sep.addElement(dipole);
        }
    }
    return sep;
}

QVector<OptimizableParam> SeparatorConfig::getOptimizableParams() const
{
    QVector<OptimizableParam> result;
    int quadCount = 0;
    int deflCount = 0;

    for (int i = 0; i < m_commands.size(); ++i) {
        const auto& cmd = m_commands[i];
        if (cmd.type == "QUAD") {
            OptimizableParam p;
            p.elementName = QString("Quad%1").arg(quadCount++);
            p.paramName = "field_Tl";
            p.currentValue = cmd.params[1];   // поле (индекс 1)
            p.type = ElementType::Quadrupole;
            p.commandIndex = i;                // правильный индекс команды
            p.paramIndex = 1;                  // правильный индекс параметра (поле)
            p.minValue = -5.0;
            p.maxValue = 5.0;
            result.append(p);
        }
        else if (cmd.type == "DEFLECTOR") {
            OptimizableParam p;
            p.elementName = QString("Deflector%1").arg(deflCount++);
            p.paramName = "voltage_kV";
            p.currentValue = cmd.params[3];    // напряжение (индекс 3)
            p.type = ElementType::ElectrostaticDeflector;
            p.commandIndex = i;
            p.paramIndex = 3;                  // правильный индекс параметра (напряжение)
            p.minValue = -100.0;
            p.maxValue = 100.0;
            result.append(p);
        }
    }
    return result;
}
void SeparatorConfig::updateParam(int commandIndex, int paramIndex, double newValue)
{
    if (commandIndex >= 0 && commandIndex < m_commands.size()) {
        if (paramIndex >= 0 && paramIndex < m_commands[commandIndex].params.size()) {
            m_commands[commandIndex].params[paramIndex] = newValue;
        }
    }
//    qDebug() << "updateParam: commandIndex=" << commandIndex << "paramIndex=" << paramIndex << "newValue=" << newValue;
}
