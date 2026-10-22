#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QFileDialog>
#include <QStatusBar>
#include <QProgressBar>
#include <QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMainWindow>
#include <cmath>
#include <QDebug>
#include <QtCharts>
#include <QScatterSeries>
#include <QLineEdit>
#include <QCheckBox>
#include <QVector>
#include <QChart>
#include <QChartView>
#include <QScatterSeries>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>
#include <random>
#include <algorithm>
#include <QTableWidget>
#include <numeric>
#include <QProgressDialog>
#include <QRegularExpression>

#include "compaundgen.h"
#include "neutronevaporation.h"
#include "scattering.h"
#include "chargecalc.h"
#include "carbonenergyloss.h"

#include "elementselector.h"
#include "element.h"
#include "periodictable.h"

#include "angulardistributionmodel.h"

#include "targetstripperdialog.h"
#include "stripperdialog.h"

#include "optimizationdialog.h"

#include "separator/separator.h"
#include "separator/parser.h"
#include "separator/particle.h"
#include "separator/separatorconfig.h"
#include "separator/separatorview.h"
#include "separator/trajectorywindow.h"
#include "separator/fieldadapter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct TargetLayer {
        QString elementSymbol;
        int A;
        double stoich;
        double thickness;
        double density;
        bool Targetflag; // true = основной, false = подложка
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateClicked();
    void saveToFile();
    void writeToFile(const QString& data);
    void plotGraph(const QString &xParam, const QString &yParam);
    void SetAxis();
    void updateAxes();
    void Modeling();
    void debugShow(double x, double y, double x_prime, double y_prime, int Q, double BR);
    void plotHistogram(const QVector<double>& data, int bins, const QString& paramName = QString());
    void onTargetSelectionChanged();
    void updateParametersFromSelection();
    void applyReactionPreset(const QString &proj, const QString &prod, const QString &channel,
                             double thickness, double energy, double density,
                             const QVector<TargetLayer> &layers,
                             int compoundZ = -1, int compoundA = -1,
                             int neutrons = -1, int protons = -1, int alphas = -1);
    void clearLayersUI();
    void addLayerToUI(const QString &symbol, int A, double stoich, double thickness, double density);
    void addElementRowToLayer(QWidget *layerGroupWidget, const QString &symbol, int A, double stoich);
    void loadSeparatorConfig();
    void updateSeparator();
    void updateSeparatorScheme();
    void showTrajectories();

//    void onProductChanged(int index);



//    void onMultiProductToggled(bool checked);
//    void onApplyFilterClicked();

private:
    // Физические переменные
    double lr, Q, Q1, x_prime, y_prime, E, x, y, theta, phi, BR;
    double mean, variance, U, Z, A, Mass, density, mt, Zt, At, l, a, b;
    int N, numNeutrons, numProtons, numAlphas, brk;
    bool spAsked = false, snAsked = false, carbon_switch = false, targetflag = false;
    double tiltBeta = 0;
    bool l_is_along_beam = false;

    QString Data, infoData;
    QStringList modelingLogData;
    QFile* logFile = nullptr;

    Ui::MainWindow *ui; // пока оставлен для совместимости, позже удалим

    // График
    QChartView *m_chartView;
    QScatterSeries *m_series;
    QChart *m_chart;
    QProgressBar *m_progressBar;
    QBarSet *m_barSet;
    QBarSeries *m_barSeries;

    // Модели
    CompaundGen *compaundGen = nullptr;
    NeutronEvaporation *neutronEvaporation = nullptr;
    Scattering *scattering = nullptr;
    ChargeCalc *chargecalc = nullptr;
    CarbonEnergyLoss *carbonenergyloss = nullptr;
    angulardistributionmodel * m_angularDistributionModel = nullptr;

    // Таблица элементов
    PeriodicTable* m_periodicTable;
    ElementSelector* m_elementSelector;

    // Массивы данных
    QVector<double> xValues, yValues, xPrimeValues, yPrimeValues, energyValues;
    QVector<int> qValues;
    QVector<double> xData, yData;

    // Слои мишени
    QVector<TargetLayer> targetLayers;
    QVector<QWidget*> layerWidgets;
    void onAddLayerClicked();
    void onRemoveLayerClicked(QWidget* layerWidget);
    QVector<TargetLayer> getLayersFromUI();

    // Сепаратор
    SeparatorConfig m_separatorConfig;
    Separator m_separator;
    bool m_useOptics = false;
    bool m_configChanged = false;
    int m_passedCount = 0;
    SeparatorView *m_separatorView;
    TrajectoryWindow *m_trajWindow = nullptr;
    QVector<QVector<QPointF>> m_allXZ, m_allYZ;
    QPlainTextEdit *m_configEditor;
    void applyConfig();
    void loadDefaultConfig();
    void loadConfigFromFile();
    void saveConfigToFile();
    void applyConfigFromEditor();
    void loadDefaultConfigToEditor();

    void loadBeamFromFile();

    // Распределения по точкам
    struct PointData {
        QString pointName;
        QVector<double> x, y, xp, yp, energy, charge, br;
        QVector<int> Z, A;
    };
    QVector<PointData> m_distributions;
    QComboBox *m_pointCombo;
    void addTrackToDistributions(const Particle& p);
    void getDataForPoint(int pointIndex, const QString& param, QVector<double>& data);
    void printRMSDistribution();
    void printLossStatistics();
    QLabel *m_transmissionLabel;

    static int scaledFontSize(int);

    // Флаги и настройки
    bool m_applyTargetStripperToBeam;
    QCheckBox *m_applyTargetStripperCheck;
    double m_stripperThickness = 1.0;
    void showStripperDialog();
    void showOptimizationDialog();
    QVector<Particle> m_loadedBeam;
    QVector<Particle> generateFittingBeam(int numParticles);
    bool m_generatorsReady = false;
    void updateStatsUI();
    QTableWidget *m_statsTable;

    // Отладочные векторы для точки "До мишени"
    QVector<double> m_x_before_target, m_y_before_target, m_xp_before_target, m_yp_before_target, m_E_before_target;

    // Отладочные векторы для загруженного пучка
    QVector<double> m_loaded_x, m_loaded_y, m_loaded_xp, m_loaded_yp, m_loaded_E, m_loaded_Q;

    // --- ВИДЖЕТЫ ИНТЕРФЕЙСА (все с префиксом m_) ---
    // Вкладки
    QTabWidget *m_tabWidget;


    QGroupBox *m_stripperGroup;
    QCheckBox *m_stripperCheckBox;
    QLineEdit *m_stripperThicknessEdit;

    // Генерация
    QLineEdit *m_meanInput, *m_varianceInput, *m_UInput, *m_aInput, *m_bInput;
    QLineEdit *m_NInput, *m_neutrInput, *m_protInput, *m_alphaInput;
    QLineEdit *m_ZInput, *m_AInput, *m_angleInput;
    QPushButton *m_generateButton;

    // Мишень
    QScrollArea *m_targetScrollArea;
    QWidget *m_targetContainer;
    QVBoxLayout *m_targetLayout;
    QPushButton *m_addLayerButton;

    // Ионная оптика
    QPushButton *m_loadBtn, *m_saveBtn, *m_applyBtn;
    QCheckBox *m_useOpticsCheck;
    QPushButton *m_optimizeBtn, *m_showTrajBtn;

    // Графики
    QComboBox *m_xParamComboBox, *m_yParamComboBox;
    QPushButton *m_plotButton, *m_histogramButton;
    QWidget *m_chartContainer;
    QLineEdit *m_xMinInput, *m_xMaxInput, *m_yMinInput, *m_yMaxInput;
    QPushButton *m_updateAxesButton;

    // Метод настройки UI
    void setupUI();



    void applyOptimizedParams(const QVector<double>& values);


    // ---  элементы для мульти-продуктов ---
    QCheckBox* m_multiProductCheck;
    QComboBox* m_filterZCombo;
    QComboBox* m_filterACombo;
    QTableWidget* m_isotopeStatsTable;
    QMap<QPair<int, int>, int> m_isotopeStats; // Хранит {Z, A} -> количество прошедших

    void updateIsotopeStatsUI();
    void updateFilterCombos();

    QLineEdit *m_refQInput;
    int refCharge() const { return qMax(1, m_refQInput->text().toInt()); }

    // --- выбор алгоритма транспорта ---
    FieldLine m_fieldLine;
    QComboBox *m_transportCombo = nullptr;   // 0 матрица, 1 RK4, 2 сравнение
    QLineEdit *m_rkStepEdit = nullptr;
    void rebuildTransport();
    void transportField(Particle& p, bool recordLoss = true);
    double referenceMassAmu() const;





    double m_baseFontSize = 9.0;
};

#endif // MAINWINDOW_H
