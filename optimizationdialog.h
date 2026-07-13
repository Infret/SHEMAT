#ifndef OPTIMIZATIONDIALOG_H
#define OPTIMIZATIONDIALOG_H

#include <QDialog>
#include <QVector>
#include <QString>
#include "separator/separatorconfig.h"

class QTableWidget;
class QComboBox;
class QLineEdit;
class QPushButton;
class QProgressBar;
class QLabel;
class QCheckBox;
class Fitter;

class OptimizationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptimizationDialog(SeparatorConfig& config, Separator& separator,
                                 const QVector<Particle>& beam,
                                 QWidget *parent = nullptr);
    ~OptimizationDialog();

private slots:
    void onStartOptimization();
    void onCancel();
    void onProgress(int iteration, double target, const QVector<double>& values);
    void onFinished(bool success, const QVector<double>& optimalValues, double finalTarget);
    void onError(const QString& msg);

private:
    void populateParamsTable();
    void updateUIState(bool running);


    void closeEvent(QCloseEvent *event);

    SeparatorConfig& m_config;
    Separator& m_separator;
    QVector<Particle> m_beam;

    QTableWidget *m_paramsTable;
    QComboBox *m_targetCombo;
    QComboBox *m_methodCombo;
    QLineEdit *m_toleranceEdit;
    QLineEdit *m_maxIterEdit;
    QPushButton *m_startBtn;
    QPushButton *m_cancelBtn;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;

    Fitter *m_fitter = nullptr;
    bool m_running = false;
};

#endif // OPTIMIZATIONDIALOG_H
