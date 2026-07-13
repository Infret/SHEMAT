#include "fitter.h"
#include <cmath>
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

Fitter::Fitter(SeparatorConfig& config, Separator& separator, QObject *parent)
    : QObject(parent), m_config(config), m_separator(separator), m_verbose(true)
{
}

Fitter::~Fitter()
{
}

void Fitter::setVerbose(bool on)
{
    m_verbose = on;
}

void Fitter::setVariables(const QVector<Variable>& vars)
{
    m_variables = vars;
}

void Fitter::setSettings(const Settings& settings)
{
    m_settings = settings;
}

void Fitter::setBeam(const QVector<Particle>& beam)
{
    m_beam = beam;
    m_beamGenerator = nullptr;
}

void Fitter::setBeamGenerator(std::function<QVector<Particle>()> generator)
{
    m_beamGenerator = generator;
    m_beam.clear();
}

void Fitter::stop()
{
    m_stop = true;
}

void Fitter::run()
{
    qDebug() << "=== Fitter::run() START ===";
    qDebug() << "m_variables.size() =" << m_variables.size();
    qDebug() << "m_beam.size() =" << m_beam.size();
    qDebug() << "m_settings.method =" << m_settings.method;

    if (m_running) return;
    m_running = true;

    if (m_variables.isEmpty()) {
        emit error("Не задано ни одной переменной для оптимизации.");
        m_running = false;
        return;
    }

    if (m_beam.isEmpty() && !m_beamGenerator) {
        emit error("Не задан пучок для оптимизации.");
        m_running = false;
        return;
    }

    if (m_beamGenerator && m_beam.isEmpty()) {
        m_beam = m_beamGenerator();
        if (m_beam.isEmpty()) {
            emit error("Генератор пучка не вернул ни одной частицы.");
            m_running = false;
            return;
        }
    }

    qDebug() << "Запуск оптимизации, метод:" << m_settings.method;

    bool success = false;
    if (m_settings.method == 1) {
        qDebug() << "Вызываем симплекс";
        success = simplexOptimization();
    } else if (m_settings.method == 2) {
        qDebug() << "Вызываем градиентный метод";
        success = gradientOptimization();
    } else {
        emit error("Неизвестный метод оптимизации.");
        m_running = false;
        return;
    }

    emit finished(success, m_optimalValues, m_finalTarget);
    m_running = false;
}

// ----- Логирование параметров -----
void Fitter::logParams(const QString& label, const QVector<double>& values)
{
    if (!m_verbose) return;
    QStringList parts;
    for (int i = 0; i < values.size(); ++i) {
        parts << m_variables[i].name + "=" + QString::number(values[i], 'f', 4);
    }
    qDebug() << qPrintable(label) << " [" << parts.join(", ") << "]";
}

// ----- Симплекс-метод Нелдера–Мида -----
bool Fitter::simplexOptimization()
{
    qDebug() << "=== Simplex start ===";
    const int n = m_variables.size();
    const int m = n + 1; // число вершин симплекса
    QVector<QVector<double>> simplex(m, QVector<double>(n));
    QVector<double> targetValues(m, 0.0);

    // 1. Инициализация симплекса
    for (int i = 0; i < n; ++i) {
        simplex[0][i] = m_variables[i].value;
    }
    // Создаём остальные вершины со сдвигом
    for (int i = 1; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            simplex[i][j] = simplex[0][j];
        }
        double step = (m_variables[i-1].step > 0) ? m_variables[i-1].step : 0.1 * std::max(1e-6, std::abs(simplex[0][i-1]));
        simplex[i][i-1] += step;
        simplex[i][i-1] = std::max(m_variables[i-1].min, std::min(m_variables[i-1].max, simplex[i][i-1]));
    }

    // 2. Вычисление целевой функции для всех вершин
    for (int i = 0; i < m; ++i) {
        updateSeparator(simplex[i]);
        targetValues[i] = evaluateTargetFunction();
    }

    if (m_verbose) {
        qDebug() << "Initial simplex:";
        for (int i = 0; i < m; ++i) {
            logParams("  Vertex " + QString::number(i), simplex[i]);
            qDebug() << "    target =" << targetValues[i];
        }
    }

    // 3. Основной цикл
    for (int iter = 0; iter < m_settings.maxIterations && !m_stop; ++iter) {
        // Находим индексы лучшей (min), худшей (max) и второй худшей (second) точек
        int bestIdx = 0, worstIdx = 0, secondWorstIdx = 0;
        for (int i = 1; i < m; ++i) {
            if (targetValues[i] < targetValues[bestIdx]) bestIdx = i;
            if (targetValues[i] > targetValues[worstIdx]) worstIdx = i;
        }
        secondWorstIdx = (worstIdx == 0) ? 1 : 0;
        for (int i = 0; i < m; ++i) {
            if (i != worstIdx && targetValues[i] > targetValues[secondWorstIdx]) {
                secondWorstIdx = i;
            }
        }

        if (m_verbose) {
            qDebug() << "Iteration" << iter << ":";
            qDebug() << "  Best vertex:" << bestIdx << "target=" << targetValues[bestIdx];
            qDebug() << "  Worst vertex:" << worstIdx << "target=" << targetValues[worstIdx];
            qDebug() << "  Second worst:" << secondWorstIdx << "target=" << targetValues[secondWorstIdx];
        }

        // Вычисляем центроид
        QVector<double> centroid(n, 0.0);
        for (int i = 0; i < m; ++i) {
            if (i == worstIdx) continue;
            for (int j = 0; j < n; ++j) {
                centroid[j] += simplex[i][j];
            }
        }
        for (int j = 0; j < n; ++j) centroid[j] /= (m - 1);

        // Отражение
        QVector<double> reflected(n);
        for (int j = 0; j < n; ++j) {
            reflected[j] = centroid[j] + 1.0 * (centroid[j] - simplex[worstIdx][j]);
            reflected[j] = std::max(m_variables[j].min, std::min(m_variables[j].max, reflected[j]));
        }
        updateSeparator(reflected);
        double reflectedTarget = evaluateTargetFunction();

        if (m_verbose) {
            logParams("  Reflected", reflected);
            qDebug() << "    target =" << reflectedTarget;
        }

        // Сравнение
        if (reflectedTarget < targetValues[bestIdx]) {
            // Успешное отражение – пробуем растяжение
            QVector<double> expanded(n);
            for (int j = 0; j < n; ++j) {
                expanded[j] = centroid[j] + 2.0 * (centroid[j] - simplex[worstIdx][j]);
                expanded[j] = std::max(m_variables[j].min, std::min(m_variables[j].max, expanded[j]));
            }
            updateSeparator(expanded);
            double expandedTarget = evaluateTargetFunction();
            if (m_verbose) {
                logParams("  Expanded", expanded);
                qDebug() << "    target =" << expandedTarget;
            }
            if (expandedTarget < reflectedTarget) {
                simplex[worstIdx] = expanded;
                targetValues[worstIdx] = expandedTarget;
                if (m_verbose) qDebug() << "  Accept expansion";
            } else {
                simplex[worstIdx] = reflected;
                targetValues[worstIdx] = reflectedTarget;
                if (m_verbose) qDebug() << "  Accept reflection";
            }
        } else if (reflectedTarget < targetValues[secondWorstIdx]) {
            simplex[worstIdx] = reflected;
            targetValues[worstIdx] = reflectedTarget;
            if (m_verbose) qDebug() << "  Accept reflection (not best but better than second worst)";
        } else {
            // Сжатие
            QVector<double> contracted(n);
            double contractionFactor = (reflectedTarget < targetValues[worstIdx]) ? 0.5 : -0.5;
            for (int j = 0; j < n; ++j) {
                contracted[j] = centroid[j] + contractionFactor * (centroid[j] - simplex[worstIdx][j]);
                contracted[j] = std::max(m_variables[j].min, std::min(m_variables[j].max, contracted[j]));
            }
            updateSeparator(contracted);
            double contractedTarget = evaluateTargetFunction();
            if (m_verbose) {
                logParams("  Contracted", contracted);
                qDebug() << "    target =" << contractedTarget;
            }
            if (contractedTarget < targetValues[worstIdx]) {
                simplex[worstIdx] = contracted;
                targetValues[worstIdx] = contractedTarget;
                if (m_verbose) qDebug() << "  Accept contraction";
            } else {
                // Редукция
                if (m_verbose) qDebug() << "  Reduction (shrink)";
                for (int i = 0; i < m; ++i) {
                    if (i == bestIdx) continue;
                    for (int j = 0; j < n; ++j) {
                        simplex[i][j] = simplex[bestIdx][j] + 0.5 * (simplex[i][j] - simplex[bestIdx][j]);
                        simplex[i][j] = std::max(m_variables[j].min, std::min(m_variables[j].max, simplex[i][j]));
                    }
                    updateSeparator(simplex[i]);
                    targetValues[i] = evaluateTargetFunction();
                }
            }
        }

        // Отправляем сигнал о прогрессе
        emit progress(iter, targetValues[bestIdx], simplex[bestIdx]);

        // Проверка сходимости
        double stdDev = 0.0;
        for (int i = 0; i < m; ++i) {
            stdDev += (targetValues[i] - targetValues[bestIdx]) * (targetValues[i] - targetValues[bestIdx]);
        }
        stdDev = std::sqrt(stdDev / m);
        if (m_verbose) {
            qDebug() << "  stdDev =" << stdDev << " tolerance =" << m_settings.tolerance;
            logParams("  Best so far", simplex[bestIdx]);
            qDebug() << "  Best target =" << targetValues[bestIdx];
        }
        if (stdDev < m_settings.tolerance) {
            if (m_verbose) qDebug() << "Convergence reached!";
            m_optimalValues = simplex[bestIdx];
            m_finalTarget = targetValues[bestIdx];
            updateSeparator(m_optimalValues);
            return true;
        }
    }

    // Достигнут максимум итераций или остановлен
    int bestIdx = 0;
    for (int i = 1; i < m; ++i) {
        if (targetValues[i] < targetValues[bestIdx]) bestIdx = i;
    }
    m_optimalValues = simplex[bestIdx];
    m_finalTarget = targetValues[bestIdx];
    updateSeparator(m_optimalValues);

    if (m_stop) {
        qDebug() << "Optimization stopped by user.";
        return false;
    } else {
        qDebug() << "Max iterations reached without convergence.";
        return false;
    }
}

bool Fitter::gradientOptimization()
{
    emit error("Градиентный метод пока не реализован.");
    return false;
}

void Fitter::updateSeparator(const QVector<double>& values)
{
    if (values.size() != m_variables.size()) {
        emit error("Несоответствие числа переменных.");
        return;
    }

    if (m_verbose) {
        logParams("Updating separator with", values);
    }

    for (int i = 0; i < values.size(); ++i) {
        m_config.updateParam(m_variables[i].commandIndex,
                             m_variables[i].paramIndex,
                             values[i]);
    }

    if (m_beam.isEmpty()) {
        emit error("Нет пучка для перестроения сепаратора.");
        return;
    }
    const Particle& ref = m_beam[0];
    m_separator = m_config.buildSeparator(ref.energy, ref.mass, ref.charge);

    if (m_verbose) {
        qDebug() << "  New separator has" << m_separator.elementsCount() << "elements";
    }
}

double Fitter::evaluateTargetFunction()
{
    // Копируем пучок и прогоняем через сепаратор
    QVector<Particle> beamCopy = m_beam;
    for (auto& p : beamCopy) {
        p.alive = true;
        p.clearTrack();
        p.addTrackPoint(0.0, p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge, "Target");
        m_separator.transport(p);
    }

    // Подсчёт живых частиц для отладки
    int alive = 0;
    for (const auto& p : beamCopy) if (p.alive) alive++;
    double transmission = (alive > 0) ? (double)alive / beamCopy.size() : 0.0;

    double result = 0.0;
       if (m_settings.targetFunction.contains("RMS X")) {
           result = computeRMSX(beamCopy);
       } else if (m_settings.targetFunction.contains("RMS Y")) {
           result = computeRMSY(beamCopy);
       } else if (m_settings.targetFunction.contains("RMS X+Y") || m_settings.targetFunction.contains("RMS X + Y")) {
           result = computeRMSXY(beamCopy);
       } else if (m_settings.targetFunction.contains("Transmission") || m_settings.targetFunction.contains("Максимизировать трансмиссию")) {
           result = computeTransmission(beamCopy);
       } else {
           result = computeRMSXY(beamCopy); // по умолчанию
       }

    if (m_verbose) {
        qDebug() << "  evaluateTarget: alive=" << alive << "/" << beamCopy.size()
                 << "transmission=" << transmission
                 << "target function selected: " << m_settings.targetFunction
                 << " -> result=" << result;
    }

    return (std::isnan(result) || std::isinf(result)) ? 1e9 : result;
}

// ----- Вычисление целевых функций -----
double Fitter::computeRMSX(const QVector<Particle>& beam)
{
    double sum = 0.0, sumSq = 0.0;
    int count = 0;
    for (const auto& p : beam) {
        if (!p.alive || p.track.isEmpty()) continue;
        const auto& last = p.track.last();
        if (std::isnan(last.x) || std::isinf(last.x)) continue;
        sum += last.x;
        sumSq += last.x * last.x;
        count++;
    }
    if (count == 0) return 1e9;
    double mean = sum / count;
    double rms = std::sqrt(sumSq / count - mean * mean);
    if (std::isnan(rms) || std::isinf(rms)) return 1e9;
    return rms;
}

double Fitter::computeRMSY(const QVector<Particle>& beam)
{
    double sum = 0.0, sumSq = 0.0;
    int count = 0;
    for (const auto& p : beam) {
        if (!p.alive || p.track.isEmpty()) continue;
        const auto& last = p.track.last();
        if (std::isnan(last.y) || std::isinf(last.y)) continue;
        sum += last.y;
        sumSq += last.y * last.y;
        count++;
    }
    if (count == 0) return 1e9;
    double mean = sum / count;
    double rms = std::sqrt(sumSq / count - mean * mean);
    if (std::isnan(rms) || std::isinf(rms)) return 1e9;
    return rms;
}

double Fitter::computeRMSXY(const QVector<Particle>& beam)
{
    double rmsX = computeRMSX(beam);
    double rmsY = computeRMSY(beam);
    if (rmsX >= 1e9 || rmsY >= 1e9) return 1e9;
    double sum = rmsX + rmsY;
    return (std::isnan(sum) || std::isinf(sum)) ? 1e9 : sum;
}

double Fitter::computeTransmission(const QVector<Particle>& beam)
{
    if (beam.isEmpty()) return 1e9;
    int alive = 0;
    for (const auto& p : beam) if (p.alive) alive++;
    if (alive == 0) return 1e9;
    double transmission = static_cast<double>(alive) / beam.size();
    double value = -transmission;
    return (std::isnan(value) || std::isinf(value)) ? 1e9 : value;
}
