// parser.h
#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QMap>
#include "separator.h"

class Parser
{
public:
    bool parse(const QString& filename, Separator& separator, Particle& refParticle);

private:
    QMap<QString, double> m_vars;
    void parseVariableLine(const QString& line);
    double getValue(const QString& token) const;
};

#endif // PARSER_H
