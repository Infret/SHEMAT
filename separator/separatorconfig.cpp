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

bool SeparatorConfig::parseFromString(const QString& text)
{
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
            bool ok;
            double len = parts[1].toDouble(&ok);
            if (!ok) continue;
            cmd.params << len;
            if (parts.size() >= 3) {
                double aper = parts[2].toDouble(&ok);
                if (ok) cmd.params << aper;
                else cmd.params << 0.1;
            } else {
                cmd.params << 0.1; // апертура по умолчанию
            }
        }
        else if (type == "QUAD") {
            if (parts.size() < 4) continue;
            double len = parts[1].toDouble();
            double fieldTl = parts[2].toDouble();
            double aperture = parts[3].toDouble();
            cmd.params << len << fieldTl << aperture;
            if (parts.size() >= 5) {
                double fringe = parts[4].toDouble();
                cmd.params << fringe;
            } else {
                cmd.params << 0.2; // fringe по умолчанию
            }
        }
        else if (type == "DEFLECTOR") {
            if (parts.size() < 5) continue;
            double radius = parts[1].toDouble();
            double angleDeg = parts[2].toDouble();
            double gap = parts[3].toDouble();
            double voltage = parts[4].toDouble();
            cmd.params << radius << angleDeg << gap << voltage;
        }
        else if (type == "DIPOLE") {
            if (parts.size() < 8) continue;
            double radius = parts[1].toDouble();
            double angleDeg = parts[2].toDouble();
            double gap = parts[3].toDouble();
            double entranceDeg = parts[4].toDouble();
            double exitDeg = parts[5].toDouble();
            double renter = parts[6].toDouble();
            double rexit = parts[7].toDouble();
            cmd.params << radius << angleDeg << gap << entranceDeg << exitDeg << renter << rexit;
        }
        else {
            qDebug() << "Unknown command:" << type;
            continue;
        }
        m_commands.append(cmd);
    }
    return true;
}

Separator SeparatorConfig::buildSeparator(double energy, double mass, int charge) const
{

//    qDebug() << "=== buildSeparator: количество команд =" << m_commands.size();
    for (int i = 0; i < m_commands.size(); ++i) {
        qDebug() << "  команда" << i << "тип" << m_commands[i].type << "params" << m_commands[i].params;
    }


    Separator sep;
    const double degToRad = M_PI / 180.0;




    // Счётчики для уникальных имён (опционально)
    int driftCount = 0, quadCount = 0, deflCount = 0, dipoleCount = 0;

    for (const auto& cmd : m_commands) {
        if (cmd.type == "DRIFT") {
            double len = cmd.params[0];
            double aper = cmd.params[1];
            SeparatorElement elem(ElementType::Drift);
            elem.setParameters(len, aper);
            elem.setName(QString("Drift%1").arg(driftCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "QUAD") {
            qDebug() << "Создаём квадруполь" << cmd.params;
            double len = cmd.params[0];
            double fieldTl = cmd.params[1];
            double aperture = cmd.params[2];
            double fringe = cmd.params[3];
            double gradient = (fieldTl * 10.0) / aperture; // кГс/м
            SeparatorElement elem(ElementType::Quadrupole);
            elem.setParameters(len, gradient, aperture);
            // Если есть метод setFringe, можно вызвать
//            elem.setFringe(fringe);
            elem.setName(QString("Quad%1").arg(quadCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "DEFLECTOR") {

            double radius = cmd.params[0];
            double angleDeg = cmd.params[1];
            double gap = cmd.params[2];
            double voltage = cmd.params[3];
            double length = std::abs(radius) * angleDeg * degToRad;

//            qDebug() << "Создаём дефлектор: radius=" << radius << "angleDeg=" << angleDeg << "length=" << length;

            SeparatorElement elem(ElementType::ElectrostaticDeflector);
            elem.setParameters(length, gap, voltage, 0.15);
            elem.setName(QString("Deflector%1").arg(deflCount++));
            sep.addElement(elem);
        }
        else if (cmd.type == "DIPOLE") {
            double radius = cmd.params[0];
            double angleDeg = cmd.params[1];
            double gap = cmd.params[2];
            double entranceDeg = cmd.params[3];
            double exitDeg = cmd.params[4];
            double renter = cmd.params[5];
            double rexit = cmd.params[6];

            double angleRad = angleDeg * degToRad;
            double arc_length = std::abs(radius) * angleRad;
            double entranceRad = entranceDeg * degToRad;
            double exitRad = exitDeg * degToRad;


//            qDebug() << "Создаём диполь: radius=" << radius << "angleDeg=" << angleDeg << "arc_length=" << arc_length;

            // Входной край
            if (std::abs(entranceRad) > 1e-6) {
                SeparatorElement edge(ElementType::DipoleEdge);
                edge.setName(QString("DipoleEntranceEdge%1").arg(dipoleCount));
                edge.setParameters(entranceRad, radius, renter);
                sep.addElement(edge);
            }

            // Тело диполя
            SeparatorElement dipole(ElementType::Dipole);
            dipole.setName(QString("Dipole%1").arg(dipoleCount));
            double xAperture = 0.1;
            double yAperture = gap / 2.0;
            // Важно: третий параметр – радиус (для вычисления поля на лету)
            dipole.setParameters(arc_length, gap, radius, xAperture, yAperture);
            sep.addElement(dipole);

            // Выходной край
            if (std::abs(exitRad) > 1e-6) {
                SeparatorElement edge(ElementType::DipoleEdge);
                edge.setName(QString("DipoleExitEdge%1").arg(dipoleCount));
                edge.setParameters(-exitRad, radius, rexit);
                sep.addElement(edge);
            }
            dipoleCount++;
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
