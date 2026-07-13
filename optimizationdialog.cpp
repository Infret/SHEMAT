#include "optimizationdialog.h"
#include "fitter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QCloseEvent>

OptimizationDialog::OptimizationDialog(SeparatorConfig& config, Separator& separator,
                                       const QVector<Particle>& beam,
                                       QWidget *parent)
    : QDialog(parent)
    , m_config(config)
    , m_separator(separator)
    , m_beam(beam)
{
    setWindowTitle("Мастер оптимизации");
    setMinimumSize(700, 500);

    setStyleSheet(R"(
        QDialog, QWidget {
            background-color: #0d2b3e;
            color: #e0f7fa;
        }
        QTableWidget {
            background-color: #0a1e2c;
            color: #e0f7fa;
            alternate-background-color: #112a3e;
            gridline-color: #1a4c6e;
        }
        QHeaderView::section {
            background-color: #1a4c6e;
            color: #e0f7fa;
        }
        QLabel {
            color: #e0f7fa;
        }
        QComboBox, QLineEdit {
            background-color: #0a1e2c;
            color: #e0f7fa;
            border: 1px solid #1a4c6e;
            border-radius: 4px;
            padding: 4px;
        }
        QPushButton {
            background-color: #6200ee;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: #3700b3;
        }
        QProgressBar {
            background: #3c3f41;
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: #e0f7fa;
        }
        QProgressBar::chunk {
            background-color: #61afef;
            width: 20px;
            margin: 0.5px;
        }
        QCheckBox {
            color: #e0f7fa;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Таблица параметров ---
    m_paramsTable = new QTableWidget(this);
    m_paramsTable->setColumnCount(7);
    m_paramsTable->setHorizontalHeaderLabels({"Вкл.", "Элемент", "Параметр", "Текущее значение",
                                              "Минимум", "Максимум", "Шаг"});
    m_paramsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_paramsTable->setAlternatingRowColors(true);
    mainLayout->addWidget(new QLabel("Выберите параметры для оптимизации:"), 0, Qt::AlignLeft);
    mainLayout->addWidget(m_paramsTable);

    // --- Параметры оптимизации ---
    QGridLayout *settingsLayout = new QGridLayout();
    settingsLayout->addWidget(new QLabel("Целевая функция:"), 0, 0);
    m_targetCombo = new QComboBox(this);
    m_targetCombo->addItems({"Минимизировать RMS X", "Минимизировать RMS Y",
                             "Минимизировать RMS X + Y", "Максимизировать трансмиссию"});
    settingsLayout->addWidget(m_targetCombo, 0, 1);

    settingsLayout->addWidget(new QLabel("Метод:"), 1, 0);
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItems({"Симплекс (Nelder-Mead)", "Градиентный спуск"});
    settingsLayout->addWidget(m_methodCombo, 1, 1);

    settingsLayout->addWidget(new QLabel("Точность:"), 2, 0);
    m_toleranceEdit = new QLineEdit("1e-6", this);
    settingsLayout->addWidget(m_toleranceEdit, 2, 1);

    settingsLayout->addWidget(new QLabel("Макс. итераций:"), 3, 0);
    m_maxIterEdit = new QLineEdit("1000", this);
    settingsLayout->addWidget(m_maxIterEdit, 3, 1);

    mainLayout->addLayout(settingsLayout);

    // --- Прогресс и кнопки ---
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);

    m_statusLabel = new QLabel("Готов к запуску", this);
    mainLayout->addWidget(m_statusLabel);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_startBtn = new QPushButton("Запустить оптимизацию", this);
    m_cancelBtn = new QPushButton("Отмена", this);
    m_cancelBtn->setEnabled(false);
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);

    // Подключение сигналов
    connect(m_startBtn, &QPushButton::clicked, this, &OptimizationDialog::onStartOptimization);
    connect(m_cancelBtn, &QPushButton::clicked, this, &OptimizationDialog::onCancel);

    // Заполнение таблицы параметров
    populateParamsTable();
}

OptimizationDialog::~OptimizationDialog()
{
    if (m_fitter) {
        m_fitter->deleteLater();
        m_fitter = nullptr;
    }
}

void OptimizationDialog::populateParamsTable()
{
    QVector<OptimizableParam> params = m_config.getOptimizableParams();
    m_paramsTable->setRowCount(params.size());

    for (int i = 0; i < params.size(); ++i) {
        const auto& p = params[i];

        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(true);
        m_paramsTable->setCellWidget(i, 0, checkBox);

        // Основные данные
        m_paramsTable->setItem(i, 1, new QTableWidgetItem(p.elementName));
        m_paramsTable->item(i, 1)->setData(Qt::UserRole, p.commandIndex);
        m_paramsTable->item(i, 1)->setData(Qt::UserRole + 1, p.paramIndex);

        m_paramsTable->setItem(i, 2, new QTableWidgetItem(p.paramName));
        m_paramsTable->setItem(i, 3, new QTableWidgetItem(QString::number(p.currentValue, 'f', 6)));
        m_paramsTable->setItem(i, 4, new QTableWidgetItem(QString::number(p.minValue, 'f', 6)));
        m_paramsTable->setItem(i, 5, new QTableWidgetItem(QString::number(p.maxValue, 'f', 6)));
        m_paramsTable->setItem(i, 6, new QTableWidgetItem("0.001")); // шаг
    }
}

void OptimizationDialog::onStartOptimization()
{
    if (m_running) return;

    // --- 1. Сбор выбранных параметров из таблицы ---
    QVector<Fitter::Variable> vars;
    bool anySelected = false;

    for (int i = 0; i < m_paramsTable->rowCount(); ++i) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(m_paramsTable->cellWidget(i, 0));
        if (!cb || !cb->isChecked()) continue;
        anySelected = true;

        // Извлекаем реальные индексы команды и параметра из скрытых данных
        int commandIndex = m_paramsTable->item(i, 1)->data(Qt::UserRole).toInt();
        int paramIndex = m_paramsTable->item(i, 1)->data(Qt::UserRole + 1).toInt();

        Fitter::Variable var;
        var.name = m_paramsTable->item(i, 1)->text() + "_" + m_paramsTable->item(i, 2)->text();
        var.value = m_paramsTable->item(i, 3)->text().toDouble();
        var.min = m_paramsTable->item(i, 4)->text().toDouble();
        var.max = m_paramsTable->item(i, 5)->text().toDouble();
        var.step = m_paramsTable->item(i, 6)->text().toDouble();
        var.commandIndex = commandIndex;
        var.paramIndex = paramIndex;
        vars.append(var);
    }

    if (!anySelected) {
        QMessageBox::warning(this, "Ошибка", "Не выбрано ни одного параметра для оптимизации.");
        return;
    }

    // --- 2. Параметры оптимизации ---
    Fitter::Settings settings;
    settings.tolerance = m_toleranceEdit->text().toDouble();
    settings.maxIterations = m_maxIterEdit->text().toInt();
    settings.method = (m_methodCombo->currentIndex() == 0) ? 1 : 2; // 1 - симплекс, 2 - градиент
    settings.targetFunction = m_targetCombo->currentText();

    // --- 3. Создание и настройка Fitter ---
    if (m_fitter) {
        m_fitter->deleteLater();
        m_fitter = nullptr;
    }
    m_fitter = new Fitter(m_config, m_separator, this);
    m_fitter->setVariables(vars);
    m_fitter->setSettings(settings);
    m_fitter->setBeam(m_beam);

    // --- 4. Подключение сигналов ---
    connect(m_fitter, &Fitter::progress, this, &OptimizationDialog::onProgress);
    connect(m_fitter, &Fitter::finished, this, &OptimizationDialog::onFinished);
    connect(m_fitter, &Fitter::error, this, &OptimizationDialog::onError);

    // --- 5. Запуск в отдельном потоке ---
    m_running = true;
    updateUIState(true);

    QThread *thread = new QThread(this);
    m_fitter->moveToThread(thread);

    // Запуск
    connect(thread, &QThread::started, m_fitter, &Fitter::run);
    // Очистка после завершения
    connect(m_fitter, &Fitter::finished, thread, &QThread::quit);
    connect(m_fitter, &Fitter::finished, this, [this]() {
        updateUIState(false);
        m_running = false;
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

void OptimizationDialog::onCancel()
{
    if (m_fitter && m_running) {
        // Здесь можно реализовать остановку (например, установить флаг)
        // Для простоты просто закроем окно
        m_fitter->deleteLater();
        m_fitter = nullptr;
        m_running = false;
        updateUIState(false);
    }
    reject();
}

void OptimizationDialog::onProgress(int iteration, double target, const QVector<double>& values)
{
    m_progressBar->setValue(iteration % 100);
    m_statusLabel->setText(QString("Итерация %1, целевая функция = %2").arg(iteration).arg(target));
    // Можно также обновить таблицу текущими значениями
}

void OptimizationDialog::onFinished(bool success, const QVector<double>& optimalValues, double finalTarget)
{
    if (success) {
         QStringList valuesStr;
         for (double v : optimalValues) {
             valuesStr << QString::number(v, 'f', 6);
         }
         QMessageBox::information(this, "Успех",
                                  QString("Оптимизация завершена.\nЦелевая функция: %1\nОптимальные значения: %2")
                                  .arg(finalTarget)
                                  .arg(valuesStr.join(", ")));
     } else {
         QMessageBox::warning(this, "Ошибка", "Оптимизация не удалась.");
     }
     updateUIState(false);
     m_running = false;
}

void OptimizationDialog::onError(const QString& msg)
{
    qDebug() << "Ошибка фита:" << msg;
    QMessageBox::critical(this, "Ошибка", msg);
    updateUIState(false);
    m_running = false;
}

void OptimizationDialog::updateUIState(bool running)
{
    m_startBtn->setEnabled(!running);
    m_cancelBtn->setEnabled(running);
    m_paramsTable->setEnabled(!running);
    m_targetCombo->setEnabled(!running);
    m_methodCombo->setEnabled(!running);
    m_toleranceEdit->setEnabled(!running);
    m_maxIterEdit->setEnabled(!running);
}



void OptimizationDialog::closeEvent(QCloseEvent *event)
{
    if (m_running && m_fitter) {
        // Останавливаем фиттер (устанавливаем флаг, чтобы он завершился)
        m_fitter->stop(); // нужно добавить метод stop в Fitter
        // Ждём завершения потока (но не блокируем GUI)
        // Лучше просто отключить кнопку закрытия до завершения
        event->ignore();
        QMessageBox::information(this, "Идёт оптимизация", "Дождитесь завершения оптимизации.");
        return;
    }
    QDialog::closeEvent(event);
}
