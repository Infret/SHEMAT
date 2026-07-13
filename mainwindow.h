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
    void onGenerateClicked(); // слот для обработки нажатия кнопки
//    void onComboBoxIndexChanged(int index);
    void saveToFile();
    void writeToFile(const QString& data);


    void plotGraph(const QString &xParam, const QString &yParam);
    void SetAxis();
    void updateAxes();

    void Modeling();
    void debugShow(double x,double y, double x_prime,double y_prime,int Q, double BR);
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
//     void carbon_layer_switcher(bool carbon_switch);
     void loadSeparatorConfig();
     void updateSeparator();
     void updateSeparatorScheme(); // обновляет отображение схемы
     void showTrajectories();








private:

    double lr;
    double Q;
    int Q1;
    double x_prime;
    double y_prime;
    double E;
    double x;
    double y;
    double theta;
    double phi;
    double BR;
    double mean;
    double variance;
    double U;
    double Z ;
    double A;
    double Mass;
    double density;
    double mt;
    double Zt;
    double At;
    double l;
    double a;
    double b;
    int N;
    int numNeutrons;
    int numProtons;
    int numAlphas;
    int brk;
    bool spAsked;
    bool snAsked;
    bool carbon_switch;
    bool targetflag;
    double tiltBeta = 0;
    bool l_is_along_beam = false;










    QString Data;
    QString infoData;
    QStringList modelingLogData;
    QFile* logFile = nullptr;

    Ui::MainWindow *ui;

    QChartView *chartView;
    QScatterSeries *series;
    QChart *chart;
    QProgressBar *progressBar;

    QBarSet *barSet;
    QBarSeries *barSeries;

    CompaundGen *compaundGen;
    NeutronEvaporation *neutronEvaporation;
    Scattering *scattering;
    ChargeCalc *chargecalc;

    angulardistributionmodel *m_angularDistributionModel;

    CarbonEnergyLoss *carbonenergyloss;

    PeriodicTable* m_periodicTable;
    ElementSelector* m_elementSelector;


    // Массивы для хранения данных
       QVector<double> xValues;        // Координаты x
       QVector<double> yValues;        // Координаты y
       QVector<double> xPrimeValues;   // Угловые координаты x'
       QVector<double> yPrimeValues;   // Угловые координаты y'
       QVector<double> energyValues;   // Энергии
       QVector<int> qValues;           // Заряды Q

       QVector<double> xData, yData;





       QVector<TargetLayer> targetLayers;
       QVector<QWidget*> layerWidgets;
       void onAddLayerClicked();
       void onRemoveLayerClicked(QWidget* layerWidget);
       QVector<TargetLayer> getLayersFromUI();


       ///кал для отладки МНТ
       double degreestoradians(double degrees) const {
               return degrees * M_PI / 180.0;
           }




       SeparatorConfig m_separatorConfig;
       Separator m_separator;
       bool m_useOptics = false;
       bool m_configChanged = false;
       int m_passedCount = 0;
       SeparatorView *m_separatorView;
       TrajectoryWindow *m_trajWindow = nullptr;
       QVector<QVector<QPointF>> m_allXZ, m_allYZ; // для накопления траекторий
       QPlainTextEdit *m_configEditor;  // текстовый редактор для конфигурации
       void applyConfig();              // слот для применения конфигурации из редактора
       void loadDefaultConfig();        // загрузить пример конфигурации по умолчанию
       void loadConfigFromFile();
       void saveConfigToFile();
       void applyConfigFromEditor();
       void loadDefaultConfigToEditor();



       void loadBeamFromFile();





       // Структура для хранения распределений по точкам
       struct PointData {
           QString pointName;
           QVector<double> x, y, xp, yp, energy, charge, br;
       };
       QVector<PointData> m_distributions;   // все точки

       // Виджет для выбора точки
       QComboBox *m_pointCombo;

       // Метод для обновления данных в m_distributions из трека частицы
       void addTrackToDistributions(const Particle& p);

       // Метод для получения данных для графика/гистограммы по выбранной точке
       void getDataForPoint(int pointIndex, const QString& param, QVector<double>& data);

       void printRMSDistribution();
       void printLossStatistics();

       QLabel *m_transmissionLabel;


       static int scaledFontSize(int);


       bool m_applyTargetStripperToBeam;
       QCheckBox *m_applyTargetStripperCheck;

       double m_stripperThickness = 1.0; // значение по умолчанию
       void showStripperDialog();


       void showOptimizationDialog();

       QVector<Particle> m_loadedBeam;

       QVector<Particle> generateFittingBeam(int numParticles);
       bool m_generatorsReady = false;

       void updateStatsUI();

       QTableWidget *m_statsTable = nullptr;



       QVector<double> m_x_before_target;
       QVector<double> m_y_before_target;
       QVector<double> m_xp_before_target;
       QVector<double> m_yp_before_target;
       QVector<double> m_E_before_target;


       QVector<double> m_loaded_x;
       QVector<double> m_loaded_y;
       QVector<double> m_loaded_xp;
       QVector<double> m_loaded_yp;
       QVector<double> m_loaded_E;
       QVector<double> m_loaded_Q;

};

#endif // MAINWINDOW_H
