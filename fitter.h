#ifndef FITTER_H
#define FITTER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QThread>
#include <functional>
#include "separator/separator.h"
#include "separator/separatorconfig.h"
#include "separator/particle.h"

class Fitter : public QObject
{
    Q_OBJECT
public:
    // Структура для хранения информации об одной переменной оптимизации
    struct Variable {
        QString name;          // уникальное имя (например, "Quad1_field")
        double value;          // текущее значение
        double min;           // минимальное допустимое значение
        double max;           // максимальное допустимое значение
        double step;          // начальный шаг для симплекса
        int commandIndex;     // индекс команды в m_commands
        int paramIndex;       // индекс параметра внутри команды
    };

    // Настройки оптимизации
    struct Settings {
        double tolerance = 1e-6;      // точность остановки
        int maxIterations = 1000;     // максимум итераций
        int method = 1;               // 1 - симплекс, 2 - градиент
        QString targetFunction;       // тип целевой функции: "RMS X", "RMS Y", "RMS X+Y", "Transmission"
        double initialStep = 0.1;     // относительный размер начального симплекса
        int beamSize = 500;           // число частиц для фита (меньше, чем для финального расчёта)
    };

    Fitter(SeparatorConfig& config, Separator& separator, QObject *parent = nullptr);
    ~Fitter();

    // Установка параметров
    void setVariables(const QVector<Variable>& vars);
    void setSettings(const Settings& settings);
    void setBeam(const QVector<Particle>& beam);
    void setBeamGenerator(std::function<QVector<Particle>()> generator); // генератор пучка на лету


    void stop();

public slots:
    void run(); // основной метод, запускает оптимизацию (выполняется в отдельном потоке)
    void setVerbose(bool on);
signals:
    void progress(int iteration, double target, const QVector<double>& values);
    void finished(bool success, const QVector<double>& optimalValues, double finalTarget);
    void error(const QString& msg);


private:
    // Основные методы оптимизации
    bool simplexOptimization();
    bool gradientOptimization();

    // Вспомогательные методы
    double evaluateTargetFunction();
    void updateSeparator(const QVector<double>& values);
    void saveOptimalValues(const QVector<double>& values);

    // Целевые функции
    double computeRMSX(const QVector<Particle>& beam);
    double computeRMSY(const QVector<Particle>& beam);
    double computeRMSXY(const QVector<Particle>& beam);
    double computeTransmission(const QVector<Particle>& beam);

    // Генерация пучка (если не задан)
    QVector<Particle> generateBeam();

    // Данные
    SeparatorConfig& m_config;
    Separator& m_separator;
    QVector<Variable> m_variables;
    Settings m_settings;
    QVector<Particle> m_beam;
    std::function<QVector<Particle>()> m_beamGenerator; // опциональный генератор

    bool m_running = false;
    QVector<double> m_optimalValues;
    double m_finalTarget = 0.0;

    bool m_stopRequested = false;

    bool m_verbose = true;   // по умолчанию включено
    volatile bool m_stop = false;


    void logParams(const QString& label, const QVector<double>& values);


};

#endif // FITTER_H
