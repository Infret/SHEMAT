#pragma once
#include "separatorconfig.h"
#include "fieldengine.h"
#include <QStringList>

struct FieldLine {
    ft::FieldEngine engine;
    QStringList names;
};

// Убрали energyMeV, massAmu, charge
FieldLine buildFieldLine(const SeparatorConfig& cfg, bool hardEdge = true);
