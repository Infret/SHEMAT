#include "parser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <cmath>

bool Parser::parse(const QString& filename, Separator& separator, Particle& refParticle)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        int commentPos = line.indexOf(';');
        if (commentPos != -1)
            line = line.left(commentPos).trimmed();
        if (!line.isEmpty())
            lines.append(line);
    }

    // Сбор переменных
    m_vars.clear();
    for (const QString& line : lines) {
        if (line.contains('='))
            parseVariableLine(line);
    }

    separator.clear();
    bool insideDipole = false;
    double entranceAngle = 0.0, exitAngle = 0.0;
    double rentrer = 0.0, rexit = 0.0;

    for (const QString& line : lines) {
        if (line.contains('=')) continue;

        if (line.startsWith("ENERGY")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) refParticle.energy = getValue(parts[1]);
        }
        else if (line.startsWith("MASS")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) refParticle.mass = getValue(parts[1]);
        }
        else if (line.startsWith("CHARGE")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) refParticle.charge = static_cast<int>(getValue(parts[1]));
        }
        else if (line.startsWith("Drift Length")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                SeparatorElement elem(ElementType::Drift);
                double len = getValue(parts[2]);
                elem.setParameters(len);
                separator.addElement(elem);
            }
        }
        else if (line.startsWith("M Q")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 5) {
                double len = getValue(parts[2]);
                double B = getValue(parts[3]);   // поле на полюсе (Тл)
                double aperture = getValue(parts[4]);
                double gradient = (B * 10.0) / aperture; // кГс/м
                SeparatorElement elem(ElementType::Quadrupole);
                elem.setParameters(len, gradient, aperture);
                separator.addElement(elem);
            }
        }
        else if (line.startsWith("D L")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                SeparatorElement elem(ElementType::Drift);
                double len = getValue(parts[2]);
                elem.setParameters(len);
                separator.addElement(elem);
            }
        }
        else if (line.startsWith("E S")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 4) {
                double R = getValue(parts[1]);
                double angleDeg = getValue(parts[2]);
                double gap = getValue(parts[3]);
                double length = R * angleDeg * M_PI / 180.0;
                double voltage = (parts.size() > 4) ? getValue(parts[4]) : 0.0;
                double yAperture = 0.15;
                SeparatorElement elem(ElementType::ElectrostaticDeflector);
                elem.setParameters(length, gap, voltage, yAperture);
                separator.addElement(elem);
            }
        }
        else if (line.startsWith("C B")) {
            insideDipole = !insideDipole;
        }
        else if (line.startsWith("F F")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 4 && insideDipole) {
                if (parts[2] == "ENTRANG") {
                    entranceAngle = getValue(parts[3]) * M_PI / 180.0;
                    if (parts.size() > 4) rentrer = getValue(parts[4]);
                } else if (parts[2] == "EXITANG") {
                    exitAngle = getValue(parts[3]) * M_PI / 180.0;
                    if (parts.size() > 4) rexit = getValue(parts[4]);
                }
            }
        }
        else if (line.startsWith("M S")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 4) {
                double radius = getValue(parts[1]);
                double angleDeg = getValue(parts[2]);
                double gap = getValue(parts[3]);
                double length = radius * angleDeg * M_PI / 180.0;
                // Вычисляем поле B (кГс) из референсной частицы
                double B = 1.439 * std::sqrt(refParticle.mass * refParticle.energy) /
                           (radius * std::abs(refParticle.charge));
                double xAperture = 0.1;
                double yAperture = gap / 2.0;
                SeparatorElement elem(ElementType::Dipole);
                elem.setParameters(length, gap, B, xAperture, yAperture);
                separator.addElement(elem);

                // Добавляем краевые поля, если заданы углы
                if (entranceAngle != 0.0) {
                    SeparatorElement edge(ElementType::DipoleEdge);
                    edge.setParameters(entranceAngle, radius, rentrer);
                    separator.addElement(edge);
                }
                if (exitAngle != 0.0) {
                    SeparatorElement edge(ElementType::DipoleEdge);
                    edge.setParameters(-exitAngle, radius, rexit);
                    separator.addElement(edge);
                }
                entranceAngle = 0.0; exitAngle = 0.0;
            }
        }
        // Остальные команды (P E, P M, P N, Fit Begin/End, D H S, D B, D S, END) игнорируются
    }
    return true;
}

void Parser::parseVariableLine(const QString& line)
{
    QStringList parts = line.split('=', Qt::SkipEmptyParts);
    if (parts.size() == 2) {
        QString name = parts[0].trimmed();
        bool ok;
        double val = parts[1].trimmed().toDouble(&ok);
        if (ok) m_vars[name] = val;
    }
}

double Parser::getValue(const QString& token) const
{
    bool ok;
    double val = token.toDouble(&ok);
    if (ok) return val;
    if (m_vars.contains(token)) return m_vars[token];
    return 0.0;
}
