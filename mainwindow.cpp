#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stylehelper.h"
#include <QLabel>
#include <QMovie>
#include <QScreen>
#include <QGuiApplication>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_generatorsReady(false)
{
    // ---- Создаём интерфейс (полностью программно) ----
    setupUI();

    m_baseFontSize = qApp->font().pointSizeF();

    QIcon icon(":/resourses/images/free-icon-data-science-8201859.png"); // или .png
    if (!icon.isNull()) {
        setWindowIcon(icon);
    } else {
        qDebug() << "Icon not found, check resource path";
    }

    // ---- Применяем глобальный стиль (QSS) ----
    qApp->setStyleSheet(R"(
        QMainWindow, QDialog, QWidget {
            background-color: #0d2b3e;
            color: #e0f7fa;
        }
        QMenuBar {
            background-color: #1a4c6e;
            color: #e0f7fa;
        }
        QMenuBar::item {
            background-color: transparent;
            color: #e0f7fa;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #6200ee;
            color: #ffffff;
        }
        QMenu {
            background-color: #1a4c6e;
            color: #e0f7fa;
            border: 1px solid #0d2b3e;
        }
        QMenu::item {
            padding: 5px 20px;
            background-color: transparent;
        }
        QMenu::item:selected {
            background-color: #6200ee;
            color: #ffffff;
        }
        QTabWidget::pane {
            background-color: #0d2b3e;
            border: none;
        }
        QTabBar::tab {
            background-color: #1a4c6e;
            color: #e0f7fa;
            padding: 8px;
            border-radius: 4px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #6200ee;
            color: #ffffff;
        }
        QPlainTextEdit, QTextEdit {
            background-color: #0a1e2c;
            color: #e0f7fa;
            border: 1px solid #1a4c6e;
            border-radius: 4px;
        }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #0a1e2c;
            color: #e0f7fa;
            border: 1px solid #1a4c6e;
            border-radius: 4px;
            padding: 4px;
        }
        QPushButton {
            background-color: #6200ee;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: #3700b3;
        }
        QGroupBox {
            color: #e0f7fa;
            border: 1px solid #1a4c6e;
            border-radius: 4px;
            margin-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QLabel {
            color: #e0f7fa;
        }
        QScrollArea {
            background-color: #0d2b3e;
        }
        QProgressDialog {
            background-color: #0d2b3e;
        }
        QChartView {
            background-color: #0a1e2c;
        }
    )");

    // ---- Инициализация моделей и таблицы элементов ----
    m_angularDistributionModel = new angulardistributionmodel(this);

    m_periodicTable = new PeriodicTable(this);
    if (m_periodicTable->loadFromTxt(":/data/elements_data.txt")) {
        qDebug() << "Periodic table loaded successfully";
    }

//    m_elementSelector = new ElementSelector(
//        m_periodicTable,
//        nullptr, // compoundCombo не используется, можем передать nullptr
//        nullptr, // elementCombo
//        nullptr, // isotopeCombo
//        static_cast<QObject*>(this)
//    );
//    m_elementSelector->initialize();

//    connect(m_elementSelector, &ElementSelector::selectionChanged,
//            this, &MainWindow::onTargetSelectionChanged);

    // ---- Меню и пресеты ----
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // --- Файл ---
    QMenu *fileMenu = new QMenu(tr("Файл"), this);
    menuBar->addMenu(fileMenu);

    QAction *saveAction = new QAction(tr("Сохранить..."), this);
    QAction *exitAction = new QAction(tr("Выход"), this);
    QAction *loadBeamAction = new QAction(tr("Загрузить пучок из файла..."), this);

    fileMenu->addAction(loadBeamAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(m_stripperCheckBox, &QCheckBox::toggled, this, [this](bool on){ carbon_switch = on; });
    connect(m_stripperThicknessEdit, &QLineEdit::editingFinished, this, [this](){
        m_stripperThickness = m_stripperThicknessEdit->text().toDouble();
    });

    connect(saveAction, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(loadBeamAction, &QAction::triggered, this, &MainWindow::loadBeamFromFile);

    // --- Реакции ---
    QMenu *reactionMenu = new QMenu(tr("Реакции"), this);
    menuBar->addMenu(reactionMenu);

    QAction *reaction1 = new QAction(tr("(198Pt,5n) → 215Ra (300 мкг/см²)"), this);
    connect(reaction1, &QAction::triggered, this, [this]() {
        QVector<TargetLayer> layers;
        layers.append({"Pt", 198, 1});
        applyReactionPreset( QString("198Pt"),
                             QString("215Ra"),
                             QString("5n"),
                             300.0,
                             10500.0,
                             21.45,
                             layers,
                             88, 215,
                             5, 0, 0);
    });
    reactionMenu->addAction(reaction1);

    QAction *reaction2 = new QAction(tr("22Ne + 238U → 255No (5n, E=13 MeV, 350 мкг/см²)"), this);
    connect(reaction2, &QAction::triggered, this, [this]() {
        QVector<TargetLayer> layers;
        layers.append({"U", 238, 1});
        applyReactionPreset(QString("22Ne"),
                            QString("255No"),
                            QString("5n"),
                            350.0,
                            13000.0,
                            19.05,
                            layers,
                            102, 255,
                            5, 0, 0);
    });
    reactionMenu->addAction(reaction2);

    QAction *reaction3 = new QAction(tr("28Mg + 238U → 260Rf (4n, E=20 MeV, 290 мкг/см²)"), this);
    connect(reaction3, &QAction::triggered, this, [this]() {
        QVector<TargetLayer> layers;
        layers.append({"U", 238, 1});
        applyReactionPreset(QString("28Mg"),
                            QString("260Rf"),
                            QString("4n"),
                            290.0,
                            20000.0,
                            19.05,
                            layers,
                            104, 260,
                            4, 0, 0);
    });
    reactionMenu->addAction(reaction3);

    QAction *reaction4 = new QAction(tr("255No + U3O8 (4n, E=10 MeV, 350 мкг/см²)"), this);
    connect(reaction4, &QAction::triggered, this, [this]() {
        QVector<TargetLayer> layers;
        layers.append({"U", 238, 3});
        layers.append({"O", 16, 8});
        applyReactionPreset(QString("proj?"),
                            QString("Compound"),
                            QString("4n"),
                            350.0,
                            10000.0,
                            8.05,
                            layers,
                            102, 255,
                            4,0,0);
    });
    reactionMenu->addAction(reaction4);

    reactionMenu->addSeparator();
    QAction *resetPreset = new QAction(tr("Сбросить параметры мишени"), this);
    connect(resetPreset, &QAction::triggered, this, [this]() {
        m_meanInput->setText("0");
        clearLayersUI();
    });
    reactionMenu->addAction(resetPreset);

    // --- Настройки ---
    QMenu *SettingsMenu = new QMenu(tr("Настройки"), this);
    menuBar->addMenu(SettingsMenu);


    QMenu *scaleMenu = new QMenu("Масштаб шрифта", this);
    SettingsMenu->addMenu(scaleMenu);
    QActionGroup *scaleGroup = new QActionGroup(this);
    QVector<double> scales = {0.8, 1.0, 1.2, 1.5};
    for (double s : scales) {
        QAction *act = new QAction(QString("%1x").arg(s), this);
        act->setCheckable(true);
        if (qAbs(s - 1.0) < 0.01) act->setChecked(true);
        scaleGroup->addAction(act);
        scaleMenu->addAction(act);

        connect(act, &QAction::triggered, this, [this, s]() {
            QFont font = qApp->font();
            font.setPointSizeF(m_baseFontSize * s);
            qApp->setFont(font);

            // Переустанавливаем QSS (если в нём нет font-size, это не повлияет на размер)
            // Но если в QSS есть размеры, их нужно удалить.
            // Просто перезагружаем стиль, чтобы обновить всё.
            qApp->setStyleSheet(qApp->styleSheet());
        });
    }

    // --- Справка ---
    QMenu *helpMenu = new QMenu(tr("Справка"), this);
    menuBar->addMenu(helpMenu);

    QAction *helpAction = new QAction(tr("Руководство пользователя"), this);
    helpMenu->addAction(helpAction);

    connect(helpAction, &QAction::triggered, this, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle("Руководство пользователя");
        dlg.resize(700, 500);

        QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);
        QTextEdit *textEdit = new QTextEdit(&dlg);
        textEdit->setReadOnly(true);
        textEdit->setStyleSheet("font-family: Courier New;");
        textEdit->setStyleSheet("QTextEdit { background-color: #f5f5f5; color: #000000; }");
        textEdit->setPlainText(
            "Руководство пользователя программы SHEMAT\n"
            "================================================\n\n"
            "1. Основные вкладки\n"
            "   - Генерация: настройка параметров пучка (энергия, размеры, число частиц,\n"
            "     испаряемые частицы, Z, A компаунд-ядра).\n"
            "   - Мишень: управление слоями мишени (элемент, массовое число, стехиометрия,\n"
            "     толщина, плотность). При включенном основном слое генерация частицы происходит в случайной точке мишени.\n"
            "   - Ионная оптика: редактирование конфигурации сепаратора. Доступные команды:\n"
            "     DRIFT длина_м [апертура_м] — дрейф (апертура по умолч. 0.1).\n"
            "     QUAD длина_м поле_Тл апертура_м [fringe] — квадруполь (fringe по умолч. 0.2).\n"
            "     DEFLECTOR радиус_м угол_град зазор_м напряжение_кВ — эл.стат. дефлектор.\n"
            "     DIPOLE радиус_м угол_град зазор_м угол_входа_град угол_выхода_град renter rexit\n"
            "            — магнитный диполь с краевыми полями.\n"
            "   - Графики: выбор точки сепаратора, построение фазовых пространств и гистограмм.\n"
            "   - Статистика: таблица потерь по элементам.\n\n"
            "2. Управление пучком\n"
            "   - Кнопка 'Сгенерировать' — запуск моделирования с параметрами из вкладки 'Генерация'.\n"
            "   - Меню 'Файл' → 'Загрузить пучок из файла...' — загрузка данных из текстового файла\n"
            "     формата: x_мм y_мм x'_рад y'_рад E_МэВ Q.\n"
            "   - Меню 'Настройки' → 'Углеродная фольга' — включение стриппера с заданием толщины.\n\n"
            "3. Ионная оптика\n"
            "   - Редактор конфигурации: введите команды, затем нажмите 'Применить конфигурацию'.\n"
            "   - Кнопки 'Загрузить из файла' и 'Сохранить в файл' для работы с .txt.\n"
            "   - Чекбокс 'Использовать ионную оптику' включает/отключает транспорт.\n"
            "   - Кнопка 'Показать траектории' — отображение траекторий частиц в отдельном окне.\n"
            "   - Кнопка 'Фитирование' — автоматический подбор параметров.\n\n"
            "4. Оптимизация\n"
            "   - Выберите параметры для оптимизации в диалоге 'Мастер оптимизации'.\n"
            "   - Установите целевую функцию (RMS X, RMS Y, RMS X+Y, трансмиссия).\n"
            "   - Задайте точность и максимальное число итераций.\n\n"
            "5. Визуализация и анализ\n"
            "   - Траектории: окно с проекциями XZ и YZ, масштабирование колёсиком, панорамирование.\n"
            "   - Фазовые портреты: выберите параметры для осей и точку сепаратора.\n"
            "   - Гистограммы: распределения выбранного параметра.\n"
            "   - Таблица статистики показывает потери на каждом элементе.\n\n"
            "Пример конфигурации сепаратора (Q-Q-Q-E10-D37):\n"
            "  DRIFT 0.6 0.1\n"
            "  QUAD 0.375 0.5961 0.095 0.2\n"
            "  DRIFT 0.2 0.1\n"
            "  QUAD 0.375 -0.6221 0.095 0.2\n"
            "  DRIFT 0.2 0.1\n"
            "  QUAD 0.375 0.3891 0.095 0.2\n"
            "  DRIFT 1.0 0.1\n"
            "  DEFLECTOR 11.0 10.0 0.07 0.0\n"
            "  DRIFT 0.8 0.1\n"
            "  DIPOLE 1.055 37.0 0.057 46.0 -43.0 0.5 -0.5\n"
            "  DRIFT 1.9 0.1\n"
        );
        dlgLayout->addWidget(textEdit);
        dlg.exec();
    });

    // ---- Закомментированный фон (GIF) ----
    // QLabel *backgroundLabel = new QLabel(this);
    // backgroundLabel->setGeometry(this->rect());
    // backgroundLabel->setScaledContents(true);
    // QMovie *movie = new QMovie(":/resourses/images/background_glow.gif");
    // backgroundLabel->setMovie(movie);
    // movie->start();
    // backgroundLabel->lower();

    // ---- Настройка графика (chart) ----
    // chart и chartView уже созданы в setupUI, теперь настраиваем их
    m_chart = new QChart;
    m_chartView->setChart(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    m_barSet = new QBarSet("Данные");
    m_barSeries = new QBarSeries();

    m_series = new QScatterSeries;
    m_series->setColor(Qt::red);
    m_series->setMarkerSize(5);

    m_chart->addSeries(m_series);
    m_chart->createDefaultAxes();
    m_chart->setBackgroundBrush(QBrush(Qt::white));

    // Настройка осей
    QValueAxis *xAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).first());
    QValueAxis *yAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).first());

    QPen gridPen;
    gridPen.setColor(QColor(0, 0, 0, 100));
    gridPen.setWidth(1);
    gridPen.setStyle(Qt::SolidLine);
    xAxis->setGridLinePen(gridPen);
    yAxis->setGridLinePen(gridPen);

    QPen axisPen(Qt::black);
    axisPen.setWidth(2);
    int fontSize = scaledFontSize(12);

    xAxis->setLinePen(axisPen);
    xAxis->setLabelsColor(Qt::black);
    xAxis->setTitleBrush(QBrush(Qt::black));
    xAxis->setTitleFont(QFont("Arial", fontSize, QFont::Bold));

    yAxis->setLinePen(axisPen);
    yAxis->setLabelsColor(Qt::black);
    yAxis->setTitleBrush(QBrush(Qt::black));
    yAxis->setTitleFont(QFont("Arial", fontSize, QFont::Bold));

    // Установка начальных значений полей для осей
    m_xMinInput->setText("0");
    m_xMaxInput->setText("0");
    m_yMinInput->setText("0");
    m_yMaxInput->setText("0");

    connect(m_xMinInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(m_xMaxInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(m_yMinInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(m_yMaxInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);

    // ---- Вкладка "Мишень" – создание слоёв ----
    // targetContainer и targetLayout уже созданы в setupUI, теперь добавляем первый слой
    QWidget *layersContainer = new QWidget(this);
    m_targetLayout->addWidget(layersContainer);
    connect(m_addLayerButton, &QPushButton::clicked, this, &MainWindow::onAddLayerClicked);
    onAddLayerClicked(); // добавляем слой по умолчанию

    // ---- Ионная оптика: m_separatorView и прочее ----
    m_separatorView = new SeparatorView(this);
    m_separatorView->hide();

    // ---- Подключение кнопок и виджетов (новые члены) ----
    connect(m_useOpticsCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_useOptics = checked;
    });

    connect(m_applyTargetStripperCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_applyTargetStripperToBeam = checked;
    });

    connect(m_showTrajBtn, &QPushButton::clicked, this, &MainWindow::showTrajectories);
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::loadConfigFromFile);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::saveConfigToFile);
    connect(m_applyBtn, &QPushButton::clicked, this, &MainWindow::applyConfigFromEditor);
    connect(m_optimizeBtn, &QPushButton::clicked, this, &MainWindow::showOptimizationDialog);

    // ---- Кнопки генерации и графиков ----
    connect(m_generateButton, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);

    connect(m_histogramButton, &QPushButton::clicked, this, [=]() {
        QString selectedParam = m_xParamComboBox->currentText();
        QVector<double> data;

        if (m_useOptics && !m_distributions.isEmpty()) {
            int pointIdx = m_pointCombo->currentIndex();
            if (pointIdx < 0 || pointIdx >= m_distributions.size()) {
                QMessageBox::warning(this, "Ошибка", "Выберите точку сепаратора.");
                return;
            }
            const PointData& pd = m_distributions[pointIdx];
            if (selectedParam == "x") data = pd.x;
            else if (selectedParam == "y") data = pd.y;
            else if (selectedParam == "x'") data = pd.xp;
            else if (selectedParam == "y'") data = pd.yp;
            else if (selectedParam == "E") data = pd.energy;
            else if (selectedParam == "Q") data = pd.charge;
            else if (selectedParam == "BR") data = pd.br;
            else {
                QMessageBox::warning(this, "Ошибка", "Неизвестный параметр.");
                return;
            }
        } else {
            if (selectedParam == "BR") {
                QMessageBox::warning(this, "Ошибка", "BR доступен только при включённой оптике.");
                return;
            }
            if (selectedParam == "x") data = xValues;
            else if (selectedParam == "y") data = yValues;
            else if (selectedParam == "x'") data = xPrimeValues;
            else if (selectedParam == "y'") data = yPrimeValues;
            else if (selectedParam == "E") data = energyValues;
            else if (selectedParam == "Q") {
                data.resize(qValues.size());
                std::transform(qValues.begin(), qValues.end(), data.begin(), [](int v){ return double(v); });
            }
            else {
                QMessageBox::warning(this, "Ошибка", "Неизвестный параметр.");
                return;
            }
        }

        if (!data.isEmpty()) {
            plotHistogram(data, 12, selectedParam);
        } else {
            QMessageBox::warning(this, "Warning", "No data available for histogram.");
        }
    });

    connect(m_plotButton, &QPushButton::clicked, this, [=]() {
        QString xParam = m_xParamComboBox->currentText();
        QString yParam = m_yParamComboBox->currentText();
        plotGraph(xParam, yParam);
    });

    // ---- Закомментированный код (оставлен как есть) ----
    // ui->MenuComboBox->setStyleSheet(...)  // теперь не нужно
    // m_applyTargetStripperCheck и связанные настройки (были закомментированы)
    // QPushButton *showTrajBtn и т.п. (уже есть)

    // ---- Установка начальных значений в полях генерации ----
    m_meanInput->setText(QString::number(490000));
    m_varianceInput->setText(QString::number(0));
    m_UInput->setText(QString::number(0));
    m_aInput->setText(QString::number(8.0));
    m_bInput->setText(QString::number(8.0));
    m_NInput->setText(QString::number(1000));
    m_neutrInput->setText(QString::number(0));
    m_protInput->setText(QString::number(0));
    m_alphaInput->setText(QString::number(0));
    m_ZInput->setText(QString::number(114));
    m_AInput->setText(QString::number(298));
    m_angleInput->setText(QString::number(0));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete compaundGen;
    delete neutronEvaporation;
    delete scattering;
    delete chargecalc;
    delete carbonenergyloss;
    delete m_angularDistributionModel;
}
void MainWindow::onGenerateClicked()
{
    m_isotopeStats.clear();


    if (m_transmissionLabel) m_transmissionLabel->setText("Трансмиссия: -- %");
    m_allXZ.clear();
    m_allYZ.clear();
    m_distributions.clear();
    m_pointCombo->clear();
    m_pointCombo->addItem("Конечная (детектор)"); // для финальных параметров

    m_x_before_target.clear();
    m_y_before_target.clear();
    m_xp_before_target.clear();
    m_yp_before_target.clear();
    m_E_before_target.clear();

    // --- Очищаем файл с прошлой генерацией ---
    const QString filePath = "simulation_output.txt";
    {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qDebug() << "Не удалось очистить файл перед генерацией:" << filePath;
        }
    }

    mean =m_meanInput->text().toDouble();
    variance = m_varianceInput->text().toDouble();
    U = m_UInput->text().toDouble();
    Z = m_ZInput->text().toDouble();
    A = m_AInput->text().toDouble();
    Mass = (Z * 1.007276) + ((A - Z) * 1.008665) - 0.008 * A + (Z * 0.00054858);

    a = m_aInput->text().toDouble();
    b = m_bInput->text().toDouble();
    N = m_NInput->text().toInt();
    numNeutrons = m_neutrInput->text().toInt();
    numProtons = m_protInput->text().toInt();
    numAlphas = m_alphaInput->text().toInt();

    xValues.clear();
    yValues.clear();
    xPrimeValues.clear();
    yPrimeValues.clear();
    energyValues.clear();
    qValues.clear();

    // Создаём объекты генераторов
    delete compaundGen; delete neutronEvaporation; delete scattering;
    delete chargecalc;  delete carbonenergyloss;

    compaundGen = new CompaundGen(mean, variance, a, b);
    neutronEvaporation = new NeutronEvaporation(Z, A, U, E);
    scattering = new Scattering(E, Z, A, Zt, At, density, mt, l, targetflag, tiltBeta, this);

    // ... (закомментированный код для angular distribution и т.д. – оставляем как есть)

    chargecalc = new ChargeCalc(Z, Mass, Q, E);
    if (carbon_switch) {
        carbonenergyloss = new CarbonEnergyLoss(E, Z, A, m_stripperThickness, this);
    } else {
        carbonenergyloss = nullptr;
    }

    // --- Прогресс-бар ---
    QProgressDialog progress(this);
    progress.setWindowTitle(tr("Идёт расчёт"));
    progress.setLabelText(tr("Моделирование частиц…"));
    progress.setCancelButtonText(QString());
    progress.setMinimum(0);
    progress.setMaximum(N);
    progress.setModal(true);
    progress.setAutoClose(true);
    progress.setAutoReset(true);
    progress.setValue(0);
    progress.setMinimumWidth(400);
    progress.setMinimumDuration(0);

    progress.setStyleSheet(R"(
        QProgressDialog {
            background-color: #2b2b2b;
            color: #ffffff;
            font: 13px "Segoe UI";
            border: 1px solid #555;
            border-radius: 8px;
        }
        QProgressBar {
            background: #3c3f41;
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: #fff;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #61afef;
            width: 20px;
            margin: 0.5px;
        }
        QLabel {
            color: #ffffff;
        }
    )");

    spAsked = false;

    if (m_useOptics) rebuildTransport();

    m_passedCount = 0;
    m_allXZ.clear();
    m_allYZ.clear();
    m_separator.resetLossStats();

    struct CmpStats { int both = 0, onlyM = 0, onlyF = 0, none = 0;
                      double sdx = 0, sdy = 0, sdxp = 0, sdyp = 0; } cmp;

    for (int i = 0; i < N; ++i) {
        QCoreApplication::processEvents();
        if (progress.wasCanceled()) break;

        Modeling();

        x /= 1000.0;
        y /= 1000.0;

        if (m_useOptics && Q1 > 0) {
            Particle p;
            p.Z = qRound(Z); p.A = qRound(A);
            p.x = x; p.y = y; p.xp = x_prime; p.yp = y_prime;
            p.energy = E; p.mass = Mass; p.charge = Q1; p.alive = true;
            p.clearTrack();
            p.addTrackPoint(0.0, x, y, x_prime, y_prime, E, Mass, Q1, "После мишени");

            const int mode = m_transportCombo->currentIndex();
            if (mode == 0) {
                m_separator.transport(p);
            } else if (mode == 1) {
                transportField(p);
            } else {                                   // сравнение: считаем двумя способами
                Particle pm = p;
                m_separator.transport(pm);             // матрица (учитывает потери)
                transportField(p, false);              // RK4 (потери не дублируем)
                if (pm.alive && p.alive) {
                    ++cmp.both;
                    cmp.sdx  += (p.x  - pm.x)  * (p.x  - pm.x);
                    cmp.sdy  += (p.y  - pm.y)  * (p.y  - pm.y);
                    cmp.sdxp += (p.xp - pm.xp) * (p.xp - pm.xp);
                    cmp.sdyp += (p.yp - pm.yp) * (p.yp - pm.yp);
                } else if (pm.alive) ++cmp.onlyM;
                else if (p.alive)    ++cmp.onlyF;
                else                 ++cmp.none;
            }

            // Всегда добавляем распределения по точкам (для фазовых пространств)
            addTrackToDistributions(p);

            if (p.alive) {
                m_isotopeStats[{p.Z, p.A}]++;
                QVector<QPointF> xz, yz;
                for (const auto& tp : p.track) {
                    xz.append(QPointF(tp.z, tp.x));
                    yz.append(QPointF(tp.z, tp.y));
                }
                m_allXZ.append(xz);
                m_allYZ.append(yz);
                m_passedCount++;
            }
        } else if (m_useOptics && Q1 <= 0) {
            continue;
        } else {
            m_passedCount++;
        }

        progress.setValue(i + 1);
        QApplication::processEvents();
    }
    if (m_useOptics && m_transportCombo->currentIndex() == 2 && N > 0) {
        const int n = qMax(1, cmp.both);
        QString s = QString("Прошли в обоих: %1\nТолько матрица: %2\nТолько RK4: %3\nПотеряны в обоих: %4\n"
                            "Трансмиссия: матрица %5%, RK4 %6%\n"
                            "RMS разности (прошедшие в обоих): x=%7 мм, y=%8 мм, x'=%9 мрад, y'=%10 мрад")
            .arg(cmp.both).arg(cmp.onlyM).arg(cmp.onlyF).arg(cmp.none)
            .arg(100.0 * (cmp.both + cmp.onlyM) / N, 0, 'f', 2)
            .arg(100.0 * (cmp.both + cmp.onlyF) / N, 0, 'f', 2)
            .arg(std::sqrt(cmp.sdx / n) * 1e3, 0, 'g', 4)
            .arg(std::sqrt(cmp.sdy / n) * 1e3, 0, 'g', 4)
            .arg(std::sqrt(cmp.sdxp / n) * 1e3, 0, 'g', 4)
            .arg(std::sqrt(cmp.sdyp / n) * 1e3, 0, 'g', 4);
        qDebug().noquote() << s;
        QMessageBox::information(this, "Сравнение алгоритмов", s);
    }
    // ======================== ПОСЛЕ ЦИКЛА ========================
    m_pointCombo->clear();

    if (!m_x_before_target.isEmpty()) {
        PointData pd;
        pd.pointName = "До мишени";
        pd.x = m_x_before_target;
        pd.y = m_y_before_target;
        pd.xp = m_xp_before_target;
        pd.yp = m_yp_before_target;
        pd.energy = m_E_before_target;
        m_distributions.insert(0, pd);
    }

    for (const auto& pd : m_distributions) {
        qDebug() << "  -" << pd.pointName;
        m_pointCombo->addItem(pd.pointName);
    }
    if (m_pointCombo->count() > 0) {
        m_pointCombo->setCurrentIndex(0);
    }

    if (m_useOptics && !m_distributions.isEmpty()) {
        printRMSDistribution();
        printLossStatistics();
        updateStatsUI();
    }

    // Вывод трансмиссии
    double transmission = (double)m_passedCount / N * 100.0;
    if (m_transmissionLabel) {
        m_transmissionLabel->setText(QString("Трансмиссия: %1% (%2 из %3)")
                                   .arg(transmission, 0, 'f', 2)
                                   .arg(m_passedCount)
                                   .arg(N));
    }

    qDebug() << QString("Трансмиссия через сепаратор: %1% (%2 из %3)")
                    .arg(transmission, 0, 'f', 2)
                    .arg(m_passedCount)
                    .arg(N);
       //           ///генрация распределений углов МНТ
       //           ///
       //           ///
       //           m_angularDistributionModel->debugPrintAngularDistribution(1000);
       //           ///
       //           ///
       //           ///


       spAsked = false;
       m_generatorsReady = true;

       updateIsotopeStatsUI();
       updateFilterCombos();
   }






void MainWindow::Modeling()
{

    // ---- Чтение параметров из полей ввода ----
      mean = m_meanInput->text().toDouble();
      variance = m_varianceInput->text().toDouble();
      U = m_UInput->text().toDouble();
      Z = m_ZInput->text().toDouble();
      tiltBeta = m_angleInput->text().toDouble(); // УГОЛ НАКЛОНА МИШЕНИ

      A = m_AInput->text().toDouble();
      a = m_aInput->text().toDouble();
      b = m_bInput->text().toDouble();
      N = m_NInput->text().toInt();
      numNeutrons = m_neutrInput->text().toInt();
      numProtons = m_protInput->text().toInt();
      numAlphas = m_alphaInput->text().toInt();

      Q = 1;
      E = mean;
      x = 0;
      y = 0;
      theta = 0;
      phi = 0;
      x_prime = 0;
      y_prime = 0;

//    qDebug()<<"l:"<<l;


        ///ГЕНЕРАЦИЯ
        ///
        ///
        ///
        compaundGen->GenerateCoordinates(a, b, E, x, y, x_prime, y_prime);

        //qDebug()<<"ГЕНЕРАЦИЯ";
        //debugShow(x,y,x_prime, y_prime,Q,BR);
        // Передаем Z и A по ссылке, чтобы изменения были видны



        //запись параметров до моделирования
        m_x_before_target.append(x/1000);
        m_y_before_target.append(y/1000);
        m_xp_before_target.append(x_prime);
        m_yp_before_target.append(y_prime);
        m_E_before_target.append(E);




        ///ИСПАРЕНИЕ
        ///
        ///
        ///
        neutronEvaporation->Evaporate_particles(Z, A, U, E, numProtons, numNeutrons, numAlphas, x, y,
                                                theta, phi, x_prime, y_prime, brk);//, spAsked, snAsked);
        Mass = (Z * 1.007276) + ((A - Z) * 1.008665) - 0.008 * A + (Z * 0.00054858);
        //debugShow(x,y,x_prime, y_prime,Q,BR);




        /// УГОЛ ДЛЯ МНОГОНУКЛОННЫХ ПЕРЕДАЧ
        ///
        ///

        double e_mev = E / 1000.0;

        // Get target parameters from the first layer (or calculate weighted average)
        QVector<TargetLayer> layers = getLayersFromUI();
//        if (!layers.isEmpty()) {
//            // Use the first layer as the primary target for angular distribution
//            const Element* element = m_periodicTable->getElementBySymbol(layers.first().elementSymbol);
//            if (element) {
//                Zt = element->getZ();
//                At = layers.first().A;

////                qDebug() << "Setting angular distribution parameters:";
////                qDebug() << "Projectile: Z1 =" << Z << "A1 =" << A;
////                qDebug() << "Target: Z2 =" << Zt << "A2 =" << At;
////                qDebug() << "Energy:" << e_mev << "MeV";

//                m_angularDistributionModel->setparameters(e_mev, Z, A, Zt, At);

//                if (m_angularDistributionModel) {
//                    double scatteringAngle = m_angularDistributionModel->Generate_Scattering_Angle();
//                    double scatteringAngleDeg = angulardistributionmodel::radianstodegrees(scatteringAngle);

//                    // Apply the scattering angle to your particle trajectory
//                    // You'll need to integrate this with your existing theta/phi calculations

////                    qDebug() << "Generated scattering angle:" << scatteringAngle << "rad (" << scatteringAngleDeg << "deg)";
//                      qDebug() << scatteringAngleDeg;
//                }
//            } else {
//                qDebug() << "ERROR: Could not find element for angular distribution";
//            }
//        } else {
//            qDebug() << "ERROR: No target layers defined for angular distribution";
//        }

        /// РАССЕЯНИЕ ПО СЛОЯМ/////////////////////////////////////////////////////////////////////////////
        ///
        ///

        // 1) Суммарная «молярная масса состава» (в g/mol) с учётом stoich
        double Mass_sum = 0.0;
        for (const TargetLayer &layer : layers) {
            const Element* element = m_periodicTable->getElementBySymbol(layer.elementSymbol);
            if (!element) continue;
            double isoMass = element->getAtomicMassForIsotope(static_cast<int>(layer.A));
            if (isoMass <= 0.0) {
                qWarning() << "Missing atomic mass for" << layer.elementSymbol << "A=" << layer.A << " — skipping in Mass_sum";
                continue;
            }
            Mass_sum += isoMass * layer.stoich;
        }

        // If Mass_sum == 0 we can't compute mass fractions — handle gracefully
        if (Mass_sum <= 0.0) {
            qWarning() << "Mass_sum is zero — check isotopes' M values in elements_data.txt";
        }

        // 2) Расчёт по слоям
        for (const TargetLayer &layer : layers) {
            const Element* element = m_periodicTable->getElementBySymbol(layer.elementSymbol);
            if (!element) continue;

            double isoMass = element->getAtomicMassForIsotope(static_cast<int>(layer.A));
                if (isoMass <= 0.0) {
                    qWarning() << "Нет массы для" << layer.elementSymbol << "A=" << layer.A;
                    continue;
                }

                double layerZ = element->getZ();
                double layerA = layer.A;
                double layerDens = layer.density * 1000; // g/cm3 → kg/m3
                double layerThick = layer.thickness * 1e-5 / layerDens; // мкг/см² → г/см² → м
                double layerMt_calc = layerA * 1.6726219e-27 * 6.022e23;



//            qDebug() << "Layer" << layer.elementSymbol << "A="<<layerA
//                     << "M="<<isoMass << "stoich="<<layer.stoich
//                     << "layerDens="<< layerDens<<"layerThick"<<layerThick<< "density="<<layerDens;
//              qDebug() << "E-" << E <<"Z-"<<Z<<"layerZ-"<<layerZ<<"layerA-"<<layerA<<"layerDens-"<<layerDens<<"layerMt_calc-"<<layerMt_calc<<"layerThick"<<layerThick;

//            qDebug() << "Before layer" << layer.elementSymbol << "E=" << E << "theta="<<theta<<"phi="<<phi;
                qDebug() << "Before scattering: xp=" << x_prime << "yp=" << y_prime;
                scattering->ScatteringCalc(E, Z, A, layerZ, layerA,
                                           layerDens, layerMt_calc, layerThick,
                                           theta, phi, x_prime, y_prime, layer.Targetflag, tiltBeta);
                qDebug() << "Before scattering: xp=" << y_prime << "yp=" << y_prime;

//            qDebug() << "After layer" << layer.elementSymbol << "E=" << E << "theta="<<theta<<"phi="<<phi;
        }



        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //debugShow(x,y,x_prime, y_prime,Q,BR);




        ///Определение заряда рекойла
        ///
        ///
        ///

        chargecalc -> CalculateQ(Z, Mass, Q, E);
        Q1 = static_cast<int>(std::round(Q));



        if (Q1 < 0)
        {
            return;  // Прерываем текущую итерацию
        }
        qDebug()<<Q1<<"Q1";

        //debugShow(x,y,x_prime, y_prime,Q1,BR);



        ///ПРОХОЖДЕНИЕ ЧЕРЕЗ ФОЛЬГУ
        ///
        ///
        ///
//        if (!carbon_switch) {
//            Q1 = 1; // или ничего, просто не применяем стриппер
//        }
        qDebug()<<Q1<<"Q1";

        if (carbon_switch && carbonenergyloss) {
            carbonenergyloss->CarbonCalc(E, Z, A, theta, phi, x_prime, y_prime);
        }

        E = E/1000;

//        Q1 = 19;////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//////УБРАТЬ ПОТОМ

        BR = (1.439) / (10 * Q1) * sqrt(Mass * E);

        // Проверка на NaN и Inf
        bool isInvalid = std::isnan(x) || std::isinf(x) ||
                        std::isnan(y) || std::isinf(y)  ||
                        std::isnan(x_prime) || std::isinf(x_prime) ||
                        std::isnan(y_prime) || std::isinf(y_prime) ||
                        std::isnan(E)  || std::isinf(E)  ||
                        std::isnan(BR) || std::isinf(BR) ||
                        std::isnan(Q) || std::isinf(Q);
//        if (isInvalid){
//             ui->outputText->append(QString("Частица потеряла всю энергию"));
//        }
        // Если данные валидны, записываем их
        if (!isInvalid) {

//            ui->outputText->append(QString("%1 %2 %3 %4 %5 %6 %7")
//                                   .arg(x, 6, 'f', 2)
//                                   .arg(y, 6, 'f', 2)
//                                   .arg(x_prime, 8, 'f', 4)
//                                   .arg(y_prime, 8, 'f', 4)
//                                   .arg(E, 6, 'f', 2)
//                                   .arg(Q1, 2)
//                                   .arg(BR, 6, 'f', 3));
//            qDebug()<<E;

            //////////////////////////////////////////////////////////////////////////////////////////////////////УБРАТЬ ПОТОМ
//            double A0 = 0.07;   // рад
//            double B0 = 0.025;
//            x_prime = ((double)rand() / RAND_MAX) * 2 * A0 - A0;
//            y_prime = ((double)rand() / RAND_MAX) * 2 * B0 - B0;

//            x_prime = 0;
//            y_prime = 0;

           //////////////////////////////////////////////////////////////////////////////////////////////////////УБРАТЬ ПОТОМ

            Data = QString("%1 %2 %3 %4 %5 %6 %7")
                    .arg(x, 8, 'f', 6)
                    .arg(y, 8, 'f', 6)
                    .arg(x_prime, 8, 'f', 4)
                    .arg(y_prime, 8, 'f', 4)
                    .arg(E, 6, 'f', 2)
                    .arg(Q1, 2)
                    .arg(BR, 6, 'f', 3);
            writeToFile(Data);

            // Сохранение данных для графиков
            xValues.append(x);
            yValues.append(y);
            xPrimeValues.append(x_prime);
            yPrimeValues.append(y_prime);
            energyValues.append(E);
            qValues.append(Q1);
        }
}
void MainWindow::plotGraph(const QString &xParam, const QString &yParam)
{
    // 1. Очистка старых серий и осей
    m_chart->removeAllSeries();
    QList<QAbstractAxis*> axes = m_chart->axes();
    for (QAbstractAxis* axis : axes) {
        m_chart->removeAxis(axis);
        delete axis;
    }

    QVector<double> xData, yData;

    // 2. Считываем значения фильтров (с защитой от nullptr, если UI ещё не обновлён)
    int filterZ = -1;
    int filterA = -1;
    if (m_filterZCombo && m_filterZCombo->currentText() != "Все") {
        filterZ = m_filterZCombo->currentText().toInt();
    }
    if (m_filterACombo && m_filterACombo->currentText() != "Все") {
        filterA = m_filterACombo->currentText().toInt();
    }

    // 3. Извлечение данных в зависимости от режима
    if (m_useOptics && !m_distributions.isEmpty()) {
        int pointIdx = m_pointCombo->currentIndex();
        if (pointIdx < 0 || pointIdx >= m_distributions.size()) {
            QMessageBox::warning(this, "Ошибка", "Выберите точку сепаратора.");
            return;
        }
        const PointData& pd = m_distributions[pointIdx];

        // Лямбда-функция для проверки, проходит ли частица по фильтру Z и A
        auto passesFilter = [&](int i) {
            // Если массивы Z и A по какой-то причине короче, считаем, что фильтр не пройден (защита от сбоев)
            if (pd.Z.size() <= i || pd.A.size() <= i) return false;
            if (filterZ != -1 && pd.Z[i] != filterZ) return false;
            if (filterA != -1 && pd.A[i] != filterA) return false;
            return true;
        };

        // Проходим по всем частицам в точке и применяем фильтр
        for (int i = 0; i < pd.x.size(); ++i) {
            if (!passesFilter(i)) continue;

            // Заполняем X
            if (xParam == "x") xData.append(pd.x[i]);
            else if (xParam == "y") xData.append(pd.y[i]);
            else if (xParam == "x'") xData.append(pd.xp[i]);
            else if (xParam == "y'") xData.append(pd.yp[i]);
            else if (xParam == "E") xData.append(pd.energy[i]);
            else if (xParam == "Q") xData.append(pd.charge[i]);
            else if (xParam == "BR") xData.append(pd.br[i]);
            else if (xParam == "N") xData.append(xData.size() + 1.0); // Порядковый номер после фильтрации

            // Заполняем Y
            if (yParam == "x") yData.append(pd.x[i]);
            else if (yParam == "y") yData.append(pd.y[i]);
            else if (yParam == "x'") yData.append(pd.xp[i]);
            else if (yParam == "y'") yData.append(pd.yp[i]);
            else if (yParam == "E") yData.append(pd.energy[i]);
            else if (yParam == "Q") yData.append(pd.charge[i]);
            else if (yParam == "BR") yData.append(pd.br[i]);
            else if (yParam == "N") yData.append(yData.size() + 1.0);
        }
    }
    else {
        // Старый режим (оптика выключена).
        // Примечание: в этом режиме массивы Z и A не отслеживаются в глобальных переменных,
        // поэтому фильтр игнорируется, и выводятся все данные.
        if (xParam == "x") xData = xValues;
        else if (xParam == "y") xData = yValues;
        else if (xParam == "x'") xData = xPrimeValues;
        else if (xParam == "y'") xData = yPrimeValues;
        else if (xParam == "E") xData = energyValues;
        else if (xParam == "Q") {
            xData.resize(qValues.size());
            std::transform(qValues.begin(), qValues.end(), xData.begin(), [](int v){ return double(v); });
        }
        else if (xParam == "N") {
            xData.resize(xValues.size());
            std::iota(xData.begin(), xData.end(), 1.0);
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Неизвестный параметр X.");
            return;
        }

        if (yParam == "x") yData = xValues;
        else if (yParam == "y") yData = yValues;
        else if (yParam == "x'") yData = xPrimeValues;
        else if (yParam == "y'") yData = yPrimeValues;
        else if (yParam == "E") yData = energyValues;
        else if (yParam == "Q") {
            yData.resize(qValues.size());
            std::transform(qValues.begin(), qValues.end(), yData.begin(), [](int v){ return double(v); });
        }
        else if (yParam == "N") {
            yData.resize(xValues.size());
            std::iota(yData.begin(), yData.end(), 1.0);
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Неизвестный параметр Y.");
            return;
        }
    }

    // 4. Проверка на пустые данные (например, если фильтр слишком строгий)
    if (xData.isEmpty() || yData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно данных для построения графика.\nВозможно, ни одна частица не прошла через заданные фильтры Z и A.");
        return;
    }

    // 5. Создание и настройка серии точек
    QScatterSeries *series = new QScatterSeries();
    series->setColor(Qt::red);
    series->setMarkerSize(5);
    for (int i = 0; i < xData.size(); ++i) {
        series->append(xData[i], yData[i]);
    }
    m_chart->addSeries(series);

    // 6. Создание и настройка осей
    QValueAxis *xAxis = new QValueAxis();
    QValueAxis *yAxis = new QValueAxis();

    double xMin = *std::min_element(xData.begin(), xData.end());
    double xMax = *std::max_element(xData.begin(), xData.end());
    double yMin = *std::min_element(yData.begin(), yData.end());
    double yMax = *std::max_element(yData.begin(), yData.end());

    // Небольшой отступ, чтобы точки не прилипали к краям
    double xPadding = (xMax - xMin) * 0.05;
    double yPadding = (yMax - yMin) * 0.05;
    if (xPadding == 0) xPadding = 1.0;
    if (yPadding == 0) yPadding = 1.0;

    xAxis->setRange(xMin - xPadding, xMax + xPadding);
    yAxis->setRange(yMin - yPadding, yMax + yPadding);
    xAxis->setTitleText(xParam);
    yAxis->setTitleText(yParam);

    // Настройка сетки и внешнего вида (в вашем оригинальном стиле)
    QPen gridPen(QColor(0, 0, 0, 100), 1, Qt::SolidLine);
    xAxis->setGridLinePen(gridPen);
    yAxis->setGridLinePen(gridPen);

    QPen axisPen(Qt::black, 2);
    xAxis->setLinePen(axisPen);
    xAxis->setLabelsColor(Qt::black);
    xAxis->setTitleBrush(QBrush(Qt::black));
    xAxis->setTitleFont(QFont("Arial", 12, QFont::Bold));

    yAxis->setLinePen(axisPen);
    yAxis->setLabelsColor(Qt::black);
    yAxis->setTitleBrush(QBrush(Qt::black));
    yAxis->setTitleFont(QFont("Arial", 12, QFont::Bold));

    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    // 7. Формирование заголовка с указанием активного фильтра
    QString title = "Scatter Plot";
    if (filterZ != -1 || filterA != -1) {
        title += QString(" (Фильтр: Z=%1, A=%2)")
                    .arg(filterZ != -1 ? QString::number(filterZ) : "Любой")
                    .arg(filterA != -1 ? QString::number(filterA) : "Любой");
    }
    m_chart->setTitle(title);

    m_chartView->setChart(m_chart);
}
void MainWindow::plotHistogram(const QVector<double>& data, int bins, const QString& paramName)
{
    m_chart->removeAllSeries();
    QList<QAbstractAxis*> axes = m_chart->axes();
    for (QAbstractAxis* axis : axes) {
        m_chart->removeAxis(axis);
        delete axis;
    }

    if (data.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No data available for histogram.");
        return;
    }

    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());

    // --- Специальная обработка для заряда Q ---
    if (paramName == "Q") {
        // Бины должны быть целыми числами
        int minQ = qRound(minVal);
        int maxQ = qRound(maxVal);
        int numBins = maxQ - minQ + 1;
        if (numBins < 1) numBins = 1;
        // Ограничим число бинов, чтобы не было слишком много (максимум 50)
        if (numBins > 50) numBins = 50;

        QVector<int> frequencies(numBins, 0);
        for (double val : data) {
            int q = qRound(val);
            int idx = q - minQ;
            if (idx >= 0 && idx < numBins) frequencies[idx]++;
        }

        QBarSet *barSet = new QBarSet("Frequency");
        QBarSeries *barSeries = new QBarSeries();
        QStringList categories;
        for (int i = 0; i < numBins; ++i) {
            *barSet << frequencies[i];
            categories << QString::number(minQ + i);
        }

        barSeries->append(barSet);
        m_chart->addSeries(barSeries);
        m_chart->setTitle("Histogram (Q)");

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        QFont font = axisX->labelsFont();
        font.setPointSize(8);
        axisX->setLabelsFont(font);
        axisX->setLabelsAngle(45);   // поворот, чтобы влезали
        m_chart->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, *std::max_element(frequencies.begin(), frequencies.end()) + 1);
        m_chart->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        m_chartView->setChart(m_chart);
        return;
    }

    // --- Обычная гистограмма для других параметров ---
    if (minVal == maxVal) {
        minVal -= 1.0;
        maxVal += 1.0;
    }

    double binWidth = (maxVal - minVal) / bins;
    QVector<int> frequencies(bins, 0);

    for (double value : data) {
        int binIndex = qFloor((value - minVal) / binWidth);
        if (binIndex < 0) binIndex = 0;
        else if (binIndex >= bins) binIndex = bins - 1;
        frequencies[binIndex]++;
    }

    QBarSet *barSet = new QBarSet("Frequency");
    QBarSeries *barSeries = new QBarSeries();
    QStringList categories;

    for (int i = 0; i < bins; ++i) {
        *barSet << frequencies[i];
        double left = minVal + i * binWidth;
        double right = left + binWidth;
        categories << QString("%1-%2").arg(left, 0, 'f', 2).arg(right, 0, 'f', 2);
    }

    barSeries->append(barSet);
    m_chart->addSeries(barSeries);
    m_chart->setTitle("Histogram");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    QFont font = axisX->labelsFont();
    font.setPointSize(8);
    axisX->setLabelsFont(font);
    axisX->setLabelsAngle(45);      // поворот, чтобы подписи влезали
    m_chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, *std::max_element(frequencies.begin(), frequencies.end()) + 1);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    m_chartView->setChart(m_chart);
}
void MainWindow::updateAxes()
{
    qDebug() << "Обновление осей...";

       QValueAxis *xAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).value(0, nullptr));
       QValueAxis *yAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).value(0, nullptr));

       if (!xAxis || !yAxis) {
           qDebug() << "Ошибка: оси графика отсутствуют!";
           m_chart->createDefaultAxes();
           return;
       }

       bool xMinOk, xMaxOk, yMinOk, yMaxOk;
       double xMin = m_xMinInput->text().toDouble(&xMinOk);
       double xMax = m_xMaxInput->text().toDouble(&xMaxOk);
       double yMin = m_yMinInput->text().toDouble(&yMinOk);
       double yMax = m_yMaxInput->text().toDouble(&yMaxOk);

       if (xMinOk && xMaxOk) xAxis->setRange(xMin, xMax);
       if (yMinOk && yMaxOk) yAxis->setRange(yMin, yMax);

       qDebug() << "Оси обновлены: X(" << xMin << "," << xMax << "), Y(" << yMin << "," << yMax << ")";
}
void MainWindow::SetAxis()
{
    // Получаем указатели на оси
    //QChart *chart = chartView->chart();
    QValueAxis *xAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).first());
    QValueAxis *yAxis = qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).first());
        // Устанавливаем диапазон осей
        if (xAxis && yAxis) {
            double xMin = *std::min_element(xData.begin(), xData.end());
            double xMax = *std::max_element(xData.begin(), xData.end());
            double yMin = *std::min_element(yData.begin(), yData.end());
            double yMax = *std::max_element(yData.begin(), yData.end());
            xAxis->setRange(xMin, xMax);
            yAxis->setRange(yMin, yMax);
        }
}
void::MainWindow::debugShow(double x,double y, double x_prime,double y_prime,int Q, double BR)
{
    qDebug() << QString("x: %1, y: %2, x': %3, y': %4, E: %5, Q: %6, BR: %7 ")
                .arg(x, 0, 'f', 2)
                .arg(y, 0, 'f', 2)
                .arg(x_prime, 0, 'f', 4)
                .arg(y_prime, 0, 'f', 4)
                .arg(E, 0, 'f', 2)
                .arg(Q)
                .arg(QString::number(BR, 'g', 3));
}


void MainWindow::writeToFile(const QString& data) {
    QString filePath = "simulation_output.txt"; // Фиксированный путь, к файлу надо индекс добавить
       QFile file(filePath);

       // Открыть файл в режиме добавления
       if (file.open(QIODevice::Append | QIODevice::Text)) {
           QTextStream out(&file);
           out.setCodec("UTF-8"); // Установка кодировки UTF-8
           out << data << "\n"; // Записать данные в файл с переносом строки
           file.close();
       } else {
           qDebug() << "Ошибка открытия файла для записи:" << filePath;
       }

}

void MainWindow::saveToFile() {
    QString sourceFilePath = "simulation_output.txt"; // Файл с данными для сохранения
    QFile sourceFile(sourceFilePath);

    // Проверяем, существует ли исходный файл
    if (!sourceFile.exists()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Исходный файл отсутствует: %1").arg(sourceFilePath));
        return;
    }

    // Открываем исходный файл для чтения
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть исходный файл для чтения: %1").arg(sourceFilePath));
        return;
    }

    // Получаем путь для сохранения нового файла
    QString saveFilePath = QFileDialog::getSaveFileName(this,
                                                        tr("Сохранить файл"),
                                                        "",
                                                        tr("Text Files (*.txt);;All Files (*)"));

    if (saveFilePath.isEmpty()) {
        return; // Пользователь отменил выбор
    }

    QFile saveFile(saveFilePath);

    // Открываем новый файл для записи
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл для записи: %1").arg(saveFilePath));
        sourceFile.close();
        return;
    }

    QTextStream in(&sourceFile);
    QTextStream out(&saveFile);

    in.setCodec("UTF-8");
    out.setCodec("UTF-8");

    // Копируем данные из исходного файла в новый
    out << in.readAll();

    sourceFile.close();
    saveFile.close();
    // Очищаем исходный файл после успешного сохранения
        QFile clearFile(sourceFilePath);
        if (clearFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            clearFile.close(); // Пустое открытие в режиме записи очищает файл
        } else {
            QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось очистить файл: %1").arg(sourceFilePath));
        }

        QMessageBox::information(this, tr("Сохранение"), tr("Файл успешно сохранён!"));
    }

    //       // Разделитель
    //       out << ";\n";
    //       // Основные данные
    //       out << "Результаты моделирования\n";
    //       out << "Энергия пучка: " << mean << " МэВ\n";
    //       out << "Количество испарённых частиц:\n";
    //       out << "  Нейтроны: " << numNeutrons << "\n";
    //       out << "  Протоны: " << numProtons << "\n";
    //       out << "  Альфа-частицы: " << numAlphas << "\n";
    //       out << "Энергия возбуждения компаунд ядра: " << U << " МэВ\n";
    //       out << "Дата сохранения: " << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";
    //       // Лог данных
    //       for (const QString& logEntry : modelingLogData) {
    //           out << logEntry << "\n";
    //       }





//void MainWindow::onComboBoxIndexChanged(int index)
//{
//    if (ui->MenuComboBox->itemText(index) == "Сохранить в файл") {
//        saveToFile();
//        ui->MenuComboBox->setCurrentIndex(0); // Возврат на первый пункт, чтобы сохранить UX
//    }
//}

void MainWindow::onTargetSelectionChanged()
{
    auto selection = m_elementSelector->getSelectedElement();
    const Element* element = selection.first;
    int isotopeA = selection.second;

    if (!element) return;

    qDebug() << "Выбран элемент:" << element->getSymbol()
             << "Z=" << element->getZ()
             << "Изотоп A=" << isotopeA;

    updateParametersFromSelection();
}

void MainWindow::updateParametersFromSelection()
{
    auto selection = m_elementSelector->getSelectedElement();
    const Element* element = selection.first;
    int isotopeA = selection.second;

    if (!element) return;

    int A = isotopeA > 0 ? isotopeA : element->getZ() * 2;

    double density = (isotopeA > 0 && element->findIsotope(isotopeA)) ?
                     element->findIsotope(isotopeA)->density :
                     element->getCommonDensity();
}

//////////////для слоев мишени//////////////////////////
void MainWindow::onRemoveLayerClicked(QWidget *layerWidget)
{
    if (!layerWidget) return;

    m_targetLayout->removeWidget(layerWidget);
    layerWidgets.removeAll(layerWidget);
    layerWidget->setParent(nullptr);
    layerWidget->deleteLater();
}

void MainWindow::onAddLayerClicked()
{
    QGroupBox *layerGroup = new QGroupBox(QString("Слой %1").arg(layerWidgets.size() + 1), this);
    layerGroup->setObjectName("layerGroup");
    QVBoxLayout *groupLayout = new QVBoxLayout(layerGroup);
    layerGroup->setStyleSheet("QGroupBox { font-weight: bold; margin-top: 6px; }");

    QHBoxLayout *topRow = new QHBoxLayout();

    QCheckBox *mainLayerCheck = new QCheckBox("Основной слой", layerGroup);
    mainLayerCheck->setObjectName("mainLayerCheck");
    mainLayerCheck->setChecked(true);
    topRow->addWidget(mainLayerCheck);

    topRow->addWidget(new QLabel("Толщина (мкг/см²):"));
    QLineEdit *thicknessEdit = new QLineEdit(layerGroup);
    thicknessEdit->setObjectName("thicknessEdit");
    thicknessEdit->setValidator(new QDoubleValidator(0, 1e9, 6, thicknessEdit));
    thicknessEdit->setText("0");
    topRow->addWidget(thicknessEdit);

    topRow->addWidget(new QLabel("Плотность (г/см³):"));
    QLineEdit *densityEdit = new QLineEdit(layerGroup);
    densityEdit->setObjectName("densityEdit");
    densityEdit->setValidator(new QDoubleValidator(0, 1000, 6, densityEdit));
    topRow->addWidget(densityEdit);

    QPushButton *addElemBtn = new QPushButton("Добавить элемент", layerGroup);
    topRow->addWidget(addElemBtn);

    topRow->addSpacing(20);
    QPushButton *removeLayerBtn = new QPushButton("Удалить слой", layerGroup);
    topRow->addWidget(removeLayerBtn);

    addElemBtn->setFixedSize(105, 28);
    removeLayerBtn->setFixedSize(80, 28);

    groupLayout->addLayout(topRow);

    QWidget *elementsContainer = new QWidget(layerGroup);
    elementsContainer->setObjectName("elementsContainer");
    QVBoxLayout *elementsLayout = new QVBoxLayout(elementsContainer);
    elementsLayout->setContentsMargins(0,0,0,0);
    elementsContainer->setLayout(elementsLayout);
    groupLayout->addWidget(elementsContainer);

    connect(removeLayerBtn, &QPushButton::clicked, this, [this, layerGroup]() {
        this->onRemoveLayerClicked(layerGroup);
    });

    connect(addElemBtn, &QPushButton::clicked, this, [this, layerGroup]() {
        addElementRowToLayer(layerGroup, QString(), 0, 1.0);
    });

    addElementRowToLayer(layerGroup, QString(), 0, 1.0);

    m_targetLayout->addWidget(layerGroup);
    layerWidgets.append(layerGroup);
}

QVector<MainWindow::TargetLayer> MainWindow::getLayersFromUI()
{
    QVector<TargetLayer> layers;

    for (QWidget *w : layerWidgets) {
        QGroupBox *g = qobject_cast<QGroupBox*>(w);
        if (!g) continue;

        QLineEdit *thEdit = g->findChild<QLineEdit*>("thicknessEdit");
        QLineEdit *densEdit = g->findChild<QLineEdit*>("densityEdit");

        double layerThickness = thEdit ? thEdit->text().toDouble() : 0.0;
        double layerDensity = densEdit ? densEdit->text().toDouble() : 0.0;

        QWidget *elementsContainer = g->findChild<QWidget*>("elementsContainer");
        if (!elementsContainer) continue;

        QVBoxLayout *elementsLayout = qobject_cast<QVBoxLayout*>(elementsContainer->layout());
        if (!elementsLayout) continue;

        for (int i = 0; i < elementsLayout->count(); ++i) {
            QLayoutItem *item = elementsLayout->itemAt(i);
            QWidget *rowWidget = item ? item->widget() : nullptr;
            if (!rowWidget) continue;

            QComboBox *elementCombo = rowWidget->findChild<QComboBox*>("elementCombo");
            QLineEdit *aEdit = rowWidget->findChild<QLineEdit*>("aEdit");
            QLineEdit *stoichEdit = rowWidget->findChild<QLineEdit*>("stoichEdit");

            if (!elementCombo || !stoichEdit || !aEdit) continue;

            TargetLayer layer;

            QCheckBox *mainLayerCheck = g->findChild<QCheckBox*>("mainLayerCheck");
            bool isMain = mainLayerCheck ? mainLayerCheck->isChecked() : false;

            layer.elementSymbol = elementCombo->currentText();
            bool okA = false;
            int Aval = aEdit->text().toInt(&okA);
            if (okA && Aval > 0) layer.A = Aval;
            else {
                const Element* el = m_periodicTable->getElementBySymbol(layer.elementSymbol);
                if (el && !el->getAllIsotopes().isEmpty()) layer.A = el->getAllIsotopes().first().A;
                else layer.A = 0;
            }

            bool okSto = false;
            double sto = stoichEdit->text().toDouble(&okSto);
            layer.stoich = (okSto && sto > 0.0) ? sto : 1.0;
            layer.Targetflag = isMain;
            layer.thickness = layerThickness;
            layer.density = layerDensity;

            layers.append(layer);
        }
    }

    return layers;
}

void MainWindow::addElementRowToLayer(QWidget *layerGroupWidget, const QString &symbol, int A, double stoich)
{
    QGroupBox *layerGroup = qobject_cast<QGroupBox*>(layerGroupWidget);
    if (!layerGroup) return;

    QWidget *elementsContainer = layerGroup->findChild<QWidget*>("elementsContainer");
    if (!elementsContainer) return;

    QHBoxLayout *rowLayout = new QHBoxLayout();
    QWidget *rowWidget = new QWidget(elementsContainer);
    rowWidget->setLayout(rowLayout);

    QComboBox *elementCombo = new QComboBox(rowWidget);
    for (const Element &el : m_periodicTable->getAllElements()) {
        elementCombo->addItem(el.getSymbol());
    }
    elementCombo->setObjectName("elementCombo");

    QLineEdit *aEdit = new QLineEdit(rowWidget);
    aEdit->setObjectName("aEdit");
    aEdit->setValidator(new QIntValidator(1, 10000, aEdit));
    aEdit->setMinimumWidth(50);
    aEdit->setMaximumWidth(100);
    aEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QLineEdit *stoichEdit = new QLineEdit(rowWidget);
    stoichEdit->setObjectName("stoichEdit");
    stoichEdit->setValidator(new QDoubleValidator(0.0001, 10000.0, 6, stoichEdit));
    stoichEdit->setFixedWidth(70);

    QPushButton *removeElemBtn = new QPushButton("Убрать", rowWidget);
    removeElemBtn->setFixedWidth(70);

    rowLayout->addWidget(new QLabel("Элемент:"));
    rowLayout->addWidget(elementCombo);
    rowLayout->addWidget(new QLabel("A:"));
    rowLayout->addWidget(aEdit);
    rowLayout->addWidget(new QLabel("n:"));
    rowLayout->addWidget(stoichEdit);
    rowLayout->addWidget(removeElemBtn);

    if (!symbol.isEmpty()) {
        int idx = elementCombo->findText(symbol);
        if (idx >= 0) elementCombo->setCurrentIndex(idx);
        else elementCombo->addItem(symbol);
    }

    if (A > 0) aEdit->setText(QString::number(A));
    else {
        const Element *el = m_periodicTable->getElementBySymbol(elementCombo->currentText());
        if (el && !el->getAllIsotopes().isEmpty()) aEdit->setText(QString::number(el->getAllIsotopes().first().A));
    }
    stoichEdit->setText(QString::number(stoich));

    connect(elementCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, elementCombo, aEdit](int){
        const Element* el = m_periodicTable->getElementBySymbol(elementCombo->currentText());
        if (el && !el->getAllIsotopes().isEmpty()) {
            aEdit->setText(QString::number(el->getAllIsotopes().first().A));
        }
    });

    connect(removeElemBtn, &QPushButton::clicked, this, [this, rowWidget](){
        QWidget *parent = rowWidget->parentWidget();
        if (parent) {
            QLayout *parentLayout = parent->layout();
            if (parentLayout) parentLayout->removeWidget(rowWidget);
        }
        rowWidget->setParent(nullptr);
        rowWidget->deleteLater();
    });

    QVBoxLayout *elementsLayout = qobject_cast<QVBoxLayout*>(elementsContainer->layout());
    elementsLayout->addWidget(rowWidget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::applyReactionPreset(const QString &proj,
                                     const QString &prod,
                                     const QString &channel,
                                     double thickness,
                                     double energy,
                                     double density,
                                     const QVector<TargetLayer> &layers,
                                     int compoundZ,
                                     int compoundA,
                                     int neutrons,
                                     int protons,
                                     int alphas)
{
    clearLayersUI();
    m_meanInput->setText(QString::number(energy));

    if (compoundZ > 0) m_ZInput->setText(QString::number(compoundZ));
    if (compoundA > 0) m_AInput->setText(QString::number(compoundA));

    if (neutrons >= 0) m_neutrInput->setText(QString::number(neutrons));
    if (protons  >= 0) m_protInput->setText(QString::number(protons));
    if (alphas   >= 0) m_alphaInput->setText(QString::number(alphas));

    clearLayersUI();

    for (const TargetLayer &layer : layers) {
        addLayerToUI(layer.elementSymbol, layer.A, layer.stoich, thickness, density);
    }

    qDebug() << "Preset applied:" << proj << "→" << prod
             << "channel=" << channel
             << "thickness=" << thickness
             << "energy=" << energy
             << "density=" << density
             << "compound Z/A =" << compoundZ << "/" << compoundA
             << "neutrons/protons/alphas =" << neutrons << protons << alphas
             << "layers=" << layers.size();
}

void MainWindow::clearLayersUI()
{
    for (QWidget* w : layerWidgets) {
        if (!w) continue;
        m_targetLayout->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
    }
    layerWidgets.clear();

    QLayoutItem *child;
    while ((child = m_targetLayout->takeAt(0)) != nullptr) {
        QWidget *pw = child->widget();
        if (pw) {
            pw->setParent(nullptr);
            pw->deleteLater();
        }
        delete child;
    }
}

void MainWindow::addLayerToUI(const QString &symbol, int A, double stoich, double thickness, double density)
{
    onAddLayerClicked();
    if (layerWidgets.isEmpty()) return;
    QGroupBox *lastLayer = qobject_cast<QGroupBox*>(layerWidgets.last());
    if (!lastLayer) return;

    QLineEdit *th = lastLayer->findChild<QLineEdit*>("thicknessEdit");
    QLineEdit *de = lastLayer->findChild<QLineEdit*>("densityEdit");
    if (th && thickness > 0.0) th->setText(QString::number(thickness));
    if (de && density > 0.0) de->setText(QString::number(density));

    addElementRowToLayer(lastLayer, symbol, A, stoich);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::loadSeparatorConfig()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите конфигурацию сепаратора", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;
    if (m_separatorConfig.loadFromFile(filename)) {
        updateSeparator();
        QMessageBox::information(this, "Успех", "Конфигурация загружена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
    }
}

void MainWindow::updateSeparator()
{
    rebuildTransport();
}

void MainWindow::updateSeparatorScheme()
{
    const auto& elements = m_separator.getElements();
    QVector<double> startZ;
    double z = 0.0;
    for (const auto& elem : elements) {
        startZ.append(z);
        z += elem.getLength();
    }
    m_separatorView->setSeparatorScheme(elements, startZ);
}

void MainWindow::showTrajectories()
{
    if (!m_useOptics || m_allXZ.isEmpty()) {
        QMessageBox::information(this, "Информация", "Нет данных о траекториях.");
        return;
    }
    if (!m_trajWindow) {
        m_trajWindow = new TrajectoryWindow(this);
        connect(m_trajWindow, &QObject::destroyed, this, [this]() { m_trajWindow = nullptr; });
    }
    m_trajWindow->clearTrajectories();

    const auto& elements = m_separator.getElements();
    QVector<double> startZ;
    double z = 0.0;
    for (const auto& elem : elements) {
        startZ.append(z);
        z += elem.getLength();
    }
    m_trajWindow->setSeparatorScheme(elements, startZ);

    for (int i = 0; i < m_allXZ.size(); ++i) {
        m_trajWindow->addTrajectory(m_allXZ[i], m_allYZ[i]);
    }
    m_trajWindow->show();
    m_trajWindow->raise();
}

void MainWindow::loadDefaultConfig()
{
    QString defaultConfig = R"(
# Конфигурация сепаратора
DRIFT 0.6
QUAD 0.375 0.5961 0.095
DRIFT 0.2
QUAD 0.375 -0.6221 0.095
DRIFT 0.2
QUAD 0.375 0.3891 0.095
DRIFT 1.0
DEFLECTOR 11.0 10.0 0.07 0.0
DRIFT 0.8
DIPOLE 1.055 37.0 0.057 46.0 -43.0 0.5 -0.5
DRIFT 1.9
)";
    m_configEditor->setPlainText(defaultConfig);
    applyConfig();
}

void MainWindow::applyConfig()
{
    QString text = m_configEditor->toPlainText();
    if (m_separatorConfig.parseFromString(text)) {
        m_configChanged = true;
        QMessageBox::information(this, "Успех", "Конфигурация сепаратора успешно загружена.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось разобрать конфигурацию. Проверьте синтаксис.");
    }
}

void MainWindow::loadDefaultConfigToEditor()
{
    QString defaultConfig =
        "DRIFT 0.6\n"
        "QUAD 0.375 0.5961 0.095\n"
        "DRIFT 0.2\n"
        "QUAD 0.375 -0.6221 0.095\n"
        "DRIFT 0.2\n"
        "QUAD 0.375 0.3891 0.095\n"
        "DRIFT 1.0\n"
        "DEFLECTOR 11.0 10.0 0.07 0.0\n"
        "DRIFT 0.8\n"
        "DIPOLE 1.055 37.0 0.057 46.0 -43.0 0.5 -0.5\n"
        "DRIFT 1.9\n";
    m_configEditor->setPlainText(defaultConfig);
}

void MainWindow::loadConfigFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить конфигурацию сепаратора", "", "Text files (*.txt)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        m_configEditor->setPlainText(stream.readAll());
        file.close();
        QMessageBox::information(this, "Успех", "Файл загружен в редактор");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
    }
}

void MainWindow::saveConfigToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить конфигурацию сепаратора", "", "Text files (*.txt)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << m_configEditor->toPlainText();
        file.close();
        QMessageBox::information(this, "Успех", "Конфигурация сохранена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void MainWindow::applyConfigFromEditor()
{
    QString text = m_configEditor->toPlainText();
    if (m_separatorConfig.parseFromString(text)) {
        QMessageBox::information(this, "Успех", "Конфигурация сепаратора успешно применена");
       if (m_useOptics) rebuildTransport();
//            double refEnergy_MeV = m_meanInput->text().toDouble() / 1000.0;
//            double refMass = m_AInput->text().toDouble();
//            int refCharge = this->refCharge();
//            double refMassAmu = (refCharge * 1.007276) + ((refMass - refCharge) * 1.008665) - 0.008 * refMass + (refCharge * 0.00054858);
//            m_separator = m_separatorConfig.buildSeparator(refEnergy_MeV, refMassAmu, refCharge);
//            updateSeparatorScheme();
//        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось разобрать конфигурацию. Проверьте синтаксис.");
    }
}

void MainWindow::addTrackToDistributions(const Particle& p)
{
    for (const auto& tp : p.track) {
        if (tp.name.contains('_')) continue;
        int idx = -1;
        for (int j = 0; j < m_distributions.size(); ++j) {
            if (m_distributions[j].pointName == tp.name) {
                idx = j;
                break;
            }
        }
        if (idx == -1) {
            PointData pd;
            pd.pointName = tp.name;
            m_distributions.append(pd);
            idx = m_distributions.size() - 1;
        }
        auto& pd = m_distributions[idx];
        pd.x.append(tp.x);
        pd.y.append(tp.y);
        pd.xp.append(tp.xp);
        pd.yp.append(tp.yp);
        pd.energy.append(tp.energy);
        pd.charge.append(tp.charge);
        double br = 1.439 * std::sqrt(tp.mass * tp.energy) / std::abs(tp.charge);
        pd.br.append(br);


        pd.Z.append(p.Z);
        pd.A.append(p.A);
    }
}

void MainWindow::printRMSDistribution()
{
    qDebug() << "\n========== RMS DISTRIBUTIONS AT EACH POINT ==========";
    for (const auto& pd : m_distributions) {
        qDebug() << "\nPoint: " << pd.pointName;
        qDebug() << "  Number of particles: " << pd.x.size();

        if (pd.x.isEmpty()) continue;

        auto computeRMS = [](const QVector<double>& data) -> double {
            double sum = 0.0, sum2 = 0.0;
            for (double v : data) {
                sum += v;
                sum2 += v * v;
            }
            double mean = sum / data.size();
            double rms = std::sqrt(sum2 / data.size() - mean * mean);
            return rms;
        };

        double rms_x = computeRMS(pd.x);
        double rms_y = computeRMS(pd.y);
        double rms_xp = computeRMS(pd.xp);
        double rms_yp = computeRMS(pd.yp);
        double rms_E = computeRMS(pd.energy);
        double rms_Q = computeRMS(pd.charge);
        double rms_BR = computeRMS(pd.br);

        double mean_x = std::accumulate(pd.x.begin(), pd.x.end(), 0.0) / pd.x.size();
        double mean_y = std::accumulate(pd.y.begin(), pd.y.end(), 0.0) / pd.y.size();

        qDebug() << "  X: RMS = " << rms_x << " m, full size (2*RMS) = " << 2.0 * rms_x << " m";
        qDebug() << "  Y: RMS = " << rms_y << " m, full size (2*RMS) = " << 2.0 * rms_y << " m";
        qDebug() << "  XP: RMS = " << rms_xp << " rad";
        qDebug() << "  YP: RMS = " << rms_yp << " rad";
        qDebug() << "  Energy: RMS = " << rms_E << " MeV, mean = " << (pd.energy.isEmpty() ? 0.0 : std::accumulate(pd.energy.begin(), pd.energy.end(), 0.0)/pd.energy.size());
        qDebug() << "  Charge: RMS = " << rms_Q;
        qDebug() << "  BR: RMS = " << rms_BR << " kG·m";
        qDebug() << "  Mean X = " << mean_x << " m, Mean Y = " << mean_y << " m";
    }
}

int MainWindow::scaledFontSize(int baseSize)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return baseSize;
    double dpi = screen->logicalDotsPerInch();
    double scale = dpi / 96.0;
    return qRound(baseSize * scale);
}

void MainWindow::loadBeamFromFile()
{
    // --- 1. Очистка предыдущих данных ---
    m_loaded_x.clear(); m_loaded_y.clear(); m_loaded_xp.clear(); m_loaded_yp.clear();
    m_loaded_E.clear(); m_loaded_Q.clear();
    m_allXZ.clear(); m_allYZ.clear(); m_distributions.clear();
    xValues.clear(); yValues.clear(); xPrimeValues.clear(); yPrimeValues.clear();
    energyValues.clear(); qValues.clear();
    m_isotopeStats.clear(); // <--- ОЧИСТКА СТАТИСТИКИ ИЗОТОПОВ

    if (m_trajWindow) m_trajWindow->clearTrajectories();

    // --- 2. Диалог выбора файла ---
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл с пучком", "", "Text files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    // --- 3. Чтение файла ---
    QTextStream in(&file);
    QVector<QStringList> allParts;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#') || line.startsWith(';')) continue; // Игнорируем комментарии
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        // <--- ПРОВЕРКА КОЛИЧЕСТВА КОЛОНОК В ЗАВИСИМОСТИ ОТ РЕЖИМА
        int minCols = (m_multiProductCheck && m_multiProductCheck->isChecked()) ? 7 : 6;
        if (parts.size() >= minCols) {
            allParts.append(parts);
        }
    }
    file.close();

    if (allParts.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит корректных данных.");
        return;
    }

    // --- 4. Параметры обработки мишени/стриппера из GUI ---
    bool doTargetStripper = m_applyTargetStripperCheck && m_applyTargetStripperCheck->isChecked();
    int uiZ = m_ZInput->text().toInt();
    int uiA = m_AInput->text().toInt();
    double tiltBetaDeg = m_angleInput->text().toDouble();
    double tiltBetaRad = tiltBetaDeg * M_PI / 180.0;
    bool applyStripper = carbon_switch;
    QVector<TargetLayer> targetLayers = getLayersFromUI();

    if (doTargetStripper && targetLayers.isEmpty() && !applyStripper) {
        QMessageBox::warning(this, "Ошибка",
            "Включена обработка мишени/стриппера, но не заданы слои мишени и не включена углеродная фольга.");
        return;
    }

    QProgressDialog progress("Загрузка и моделирование пучка...", "Отмена", 0, allParts.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    m_passedCount = 0;

    // --- 5. Перестроение сепаратора (если оптика включена) ---
    if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked()) {
        rebuildTransport();
        m_separator.resetLossStats();
    }

    // --- 6. Цикл по всем частицам из файла ---
    N = allParts.size();
    for (int i = 0; i < allParts.size(); ++i) {
        if (progress.wasCanceled()) break;
        progress.setValue(i);
        QCoreApplication::processEvents();

        const QStringList& parts = allParts[i];
        double x_mm, y_mm, xp, yp, E_MeV;
        int Q = 1;
        int z = uiZ; // Значение по умолчанию из UI
        int a = uiA; // Значение по умолчанию из UI

        // <--- РАЗДЕЛЕНИЕ ЛОГИКИ ЧТЕНИЯ ПО РЕЖИМАМ
        if (m_multiProductCheck && m_multiProductCheck->isChecked()) {
            // Формат: Z, A, x_мм, y_мм, xp, yp, E_МэВ
            z = parts[0].toInt();
            a = parts[1].toInt();
            x_mm = parts[2].toDouble();
            y_mm = parts[3].toDouble();
            xp = parts[4].toDouble();
            yp = parts[5].toDouble();
            E_MeV = parts[6].toDouble();
        } else {
            // Старый формат: x_мм, y_мм, xp, yp, E_МэВ, Q
            x_mm = parts[0].toDouble();
            y_mm = parts[1].toDouble();
            xp = parts[2].toDouble();
            yp = parts[3].toDouble();
            E_MeV = parts[4].toDouble();
            Q = qRound(parts[5].toDouble());
        }

        double x = x_mm / 1000.0;
        double y = y_mm / 1000.0;
        double E_keV = E_MeV * 1000.0;

        // Сохраняем исходные данные (до обработки)
        m_loaded_x.append(x);
        m_loaded_y.append(y);
        m_loaded_xp.append(xp);
        m_loaded_yp.append(yp);
        m_loaded_E.append(E_MeV);
        m_loaded_Q.append(Q);

        int partZ = z;
        int partA = a;

        // --- Обработка мишени и стриппера (если включено) ---
        if (doTargetStripper && Q >= 0) {
            if (!targetLayers.isEmpty()) {
                double theta = xp;
                double phi = yp;
                for (const auto& layer : targetLayers) {
                    const Element* elem = m_periodicTable->getElementBySymbol(layer.elementSymbol);
                    if (!elem) continue;
                    double Zt = elem->getZ();
                    double At = layer.A;
                    double layerDens = layer.density * 1000.0;
                    double layerThickM = layer.thickness * 1e-5 / layerDens;
                    double Mt_calc = At * 1.6726219e-27 * 6.022e23;
                    Scattering scatter(E_keV, partZ, partA, Zt, At, layerDens, Mt_calc, layerThickM, layer.Targetflag, tiltBetaRad, this);
                    double dZ = partZ, dA = partA, dZt = Zt, dAt = At;
                    scatter.ScatteringCalc(E_keV, dZ, dA, dZt, dAt, layerDens, Mt_calc, layerThickM, theta, phi, xp, yp, layer.Targetflag, tiltBetaRad);
                }
            }
            if (applyStripper) {
                double theta = xp;
                double phi = yp;
                double dZ = partZ, dA = partA;
                CarbonEnergyLoss carbonLoss(E_keV, partZ, partA, m_stripperThickness, this);
                carbonLoss.CarbonCalc(E_keV, dZ, dA, theta, phi, xp, yp);

                double newQ = Q;
                ChargeCalc chargeCalc(dZ, dA, newQ, E_keV);
                chargeCalc.CalculateQ(dZ, dA, newQ, E_keV);
                Q = qRound(newQ);
            }
            E_MeV = E_keV / 1000.0;
        }

        // --- Транспорт через сепаратор ---
        if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked() && Q >= 0) {
            Particle p;
            p.x = x; p.y = y; p.xp = xp; p.yp = yp;
            p.energy = E_MeV;
            p.mass = partA;
            p.charge = Q;
            p.Z = z;      // <--- КЛЮЧЕВОЕ ДОБАВЛЕНИЕ: сохраняем Z
            p.A = partA;  // <--- КЛЮЧЕВОЕ ДОБАВЛЕНИЕ: сохраняем A
            p.alive = true;
            p.clearTrack();
            p.addTrackPoint(0.0, x, y, xp, yp, E_MeV, partA, Q, "После мишени");

            if (m_transportCombo->currentIndex() == 0) m_separator.transport(p);
            else transportField(p);

            addTrackToDistributions(p); // <--- Убедитесь, что эта функция обновлена (см. пункт 2 ниже)

            if (p.alive) {
                QVector<QPointF> xz, yz;
                for (const auto& tp : p.track) {
                    xz.append(QPointF(tp.z, tp.x));
                    yz.append(QPointF(tp.z, tp.y));
                }
                m_allXZ.append(xz);
                m_allYZ.append(yz);
                m_passedCount++;

                // <--- ПОДСЧЁТ СТАТИСТИКИ ПО ИЗОТОПАМ
                m_isotopeStats[{p.Z, p.A}]++;
            }
        } else if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked() && Q <= 0) {
            continue; // частицы с Q <= 0 не транспортируются
        } else {
            // Оптика выключена — просто считаем все частицы прошедшими
            m_passedCount++;
            xValues.append(x); yValues.append(y); xPrimeValues.append(xp); yPrimeValues.append(yp);
            energyValues.append(E_MeV); qValues.append(Q);
            m_isotopeStats[{z, partA}]++;
        }
    }
    if (!m_loaded_E.isEmpty()) {
           double sE = 0, sQ = 0, sEq = 0; int nq = 0;
           for (int i = 0; i < m_loaded_E.size(); ++i) {
               sE += m_loaded_E[i]; sQ += m_loaded_Q[i];
               if (m_loaded_Q[i] > 0) { sEq += m_loaded_E[i] / m_loaded_Q[i]; ++nq; }
           }
           const int n = m_loaded_E.size();
           qDebug() << "Пучок из файла: <E> =" << sE / n << "МэВ, <Q> =" << sQ / n
                    << ", <E/q> =" << (nq ? sEq / nq : 0.0) << "МэВ/заряд";
           qDebug() << "Опорная: E =" << m_meanInput->text().toDouble() / 1000.0
                    << "МэВ, Q =" << refCharge()
                    << ", m =" << referenceMassAmu() << "а.е.м.";
           qDebug() << "Для дефлектора нужно E/q = R*U/(2*gap) (кэВ), для вашего конфига ~7.86 МэВ/заряд";
       }

    progress.setValue(allParts.size());

    // --- 7. Добавление точки "Загруженный пучок" ---
    if (!m_loaded_x.isEmpty()) {
        PointData pd;
        pd.pointName = "Загруженный пучок";
        pd.x = m_loaded_x; pd.y = m_loaded_y; pd.xp = m_loaded_xp; pd.yp = m_loaded_yp;
        pd.energy = m_loaded_E; pd.charge = m_loaded_Q;

        // <--- Заполняем Z и A для сырых данных, чтобы фильтр не ломался при выборе этой точки
        int defaultZ = (m_multiProductCheck && m_multiProductCheck->isChecked()) ? uiZ : m_ZInput->text().toInt();
        int defaultA = (m_multiProductCheck && m_multiProductCheck->isChecked()) ? uiA : m_AInput->text().toInt();
        pd.Z.fill(defaultZ, pd.x.size());
        pd.A.fill(defaultA, pd.x.size());

        m_distributions.insert(0, pd);
    }

    // --- 8. Обновление комбобокса с точками сепаратора ---
    if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked() && !m_distributions.isEmpty()) {
        m_pointCombo->clear();
        for (const auto& pd : m_distributions) {
            m_pointCombo->addItem(pd.pointName);
        }
        if (m_pointCombo->count() > 0) m_pointCombo->setCurrentIndex(0);
    }

    // --- 9. Вывод трансмиссии ---
    double transmission = (double)m_passedCount / allParts.size() * 100.0;
    qDebug() << QString("Загружено частиц: %1, прошло сепаратор: %2 (%3%)")
                .arg(allParts.size()).arg(m_passedCount).arg(transmission, 0, 'f', 2);
    if (m_transmissionLabel) {
        m_transmissionLabel->setText(QString("Трансмиссия: %1% (%2 из %3)")
                                     .arg(transmission, 0, 'f', 2)
                                     .arg(m_passedCount)
                                     .arg(allParts.size()));
    }

    // --- 10. Вывод статистики потерь и обновление таблицы ---
    if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked() && !m_distributions.isEmpty()) {
        printLossStatistics();
        updateStatsUI();
    }

    // --- 11. Обновление новых UI элементов для мульти-продуктов ---
    updateIsotopeStatsUI();     // <--- ОБНОВЛЕНИЕ ТАБЛИЦЫ ИЗОТОПОВ
    updateFilterCombos();       // <--- ОБНОВЛЕНИЕ ВЫПАДАЮЩИХ СПИСКОВ ФИЛЬТРОВ

    // --- 12. Показать окно траекторий (если есть живые частицы) ---
    if (m_useOptics && m_useOpticsCheck && m_useOpticsCheck->isChecked() && !m_allXZ.isEmpty()) {
        showTrajectories();
    }
}
void MainWindow::showOptimizationDialog()
{
    if (!m_useOptics) {
        QMessageBox::warning(this, "Ошибка", "Включите ионную оптику для оптимизации.");
        return;
    }

    QVector<Particle> beam;

    if (!m_loadedBeam.isEmpty()) {
        beam = m_loadedBeam;
        qDebug() << "Используется загруженный пучок, частиц:" << beam.size();
    } else if (m_generatorsReady) {
        beam = generateFittingBeam(500);
        qDebug() << "Сгенерирован новый пучок для фиттера, частиц:" << beam.size();
    } else {
        QMessageBox::warning(this, "Ошибка",
            "Нет загруженного пучка и генераторы не инициализированы.\n"
            "Сначала сгенерируйте пучок (кнопка 'Generate') или загрузите файл.");
        return;
    }

    if (beam.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пучок пуст. Оптимизация невозможна.");
        return;
    }

    OptimizationDialog dlg(m_separatorConfig, m_separator, beam, this);
    connect(&dlg, &OptimizationDialog::optimizationFinished, this, &MainWindow::applyOptimizedParams);
    dlg.exec();
}

QVector<Particle> MainWindow::generateFittingBeam(int numParticles)
{
    QVector<Particle> beam;

    int savedN = N;
    bool savedOptics = m_useOptics;
    bool savedStripper = carbon_switch;

    m_useOptics = false;
    N = numParticles;

    xValues.clear(); yValues.clear(); xPrimeValues.clear(); yPrimeValues.clear();
    energyValues.clear(); qValues.clear();

    for (int i = 0; i < numParticles; ++i) {
        Modeling();
        x /= 1000.0;
        y /= 1000.0;

        Particle p;
        p.x = x; p.y = y; p.xp = x_prime; p.yp = y_prime;
        p.energy = E;
        p.mass = Mass;
        p.charge = Q1;
        p.alive = true;
        p.clearTrack();
        p.addTrackPoint(0.0, p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge, "Target");
        beam.append(p);
    }

    m_useOptics = savedOptics;
    carbon_switch = savedStripper;
    N = savedN;

    qDebug() << "Сгенерирован пучок для фита:" << beam.size() << "частиц";
    return beam;
}

void MainWindow::printLossStatistics()
{
    if (!m_useOptics) return;

    auto lossStats = m_separator.getLossStats();
    int totalLost = m_separator.getTotalLost();
    int totalParticles = N; // теперь N = allParts.size() после загрузки

    qDebug() << "\n========== LOSS STATISTICS ==========";
    qDebug() << "Total particles:" << totalParticles;
    qDebug() << "Lost particles:" << totalLost << "(" << (double)totalLost/totalParticles*100 << "%)";

    if (lossStats.isEmpty()) {
        qDebug() << "No losses (all particles passed).";
        return;
    }

    qDebug() << "\nLosses by element:";
    for (const auto& stat : lossStats) {
        qDebug() << "  " << stat.elementName << ":" << stat.lostCount
                 << "(" << (double)stat.lostCount/totalLost*100 << "% of losses)";
    }
}

void MainWindow::updateStatsUI()
{
    if (!m_statsTable) return;

    auto lossStats = m_separator.getLossStats();

    std::sort(lossStats.begin(), lossStats.end(),
              [](const LossStat& a, const LossStat& b) {
                  return a.elementIndex < b.elementIndex;
              });

    int totalLost = m_separator.getTotalLost();
    int totalParticles = N;

    m_statsTable->setRowCount(0);
    m_statsTable->setRowCount(lossStats.size() + 1);

    int row = 0;
    for (const auto& stat : lossStats) {
        double rmsX = 0.0;
        for (const auto& pd : m_distributions) {
            if (pd.pointName == stat.pointName) {
                if (!pd.x.isEmpty()) {
                    double sum = 0.0, sum2 = 0.0;
                    for (double v : pd.x) {
                        sum += v;
                        sum2 += v * v;
                    }
                    double mean = sum / pd.x.size();
                    double variance = sum2 / pd.x.size() - mean * mean;
                    if (variance > 0) rmsX = sqrt(variance);
                    else rmsX = 0.0;
                }
                break;
            }
        }

        m_statsTable->setItem(row, 0, new QTableWidgetItem(stat.elementName));
        m_statsTable->setItem(row, 1, new QTableWidgetItem(QString::number(stat.lostCount)));

        double percentOfLosses = (totalLost > 0) ? (double)stat.lostCount / totalLost * 100.0 : 0.0;
        m_statsTable->setItem(row, 2, new QTableWidgetItem(QString::number(percentOfLosses, 'f', 1) + "%"));

        m_statsTable->setItem(row, 3, new QTableWidgetItem(QString::number(rmsX, 'f', 6)));

        row++;
    }

    m_statsTable->setItem(row, 0, new QTableWidgetItem("Итого"));
    m_statsTable->setItem(row, 1, new QTableWidgetItem(QString::number(totalLost)));

    double totalPercent = (totalParticles > 0) ? (double)totalLost / totalParticles * 100.0 : 0.0;
    m_statsTable->setItem(row, 2, new QTableWidgetItem(QString::number(totalPercent, 'f', 1) + "%"));

    m_statsTable->setItem(row, 3, new QTableWidgetItem("—"));

    m_statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::applyOptimizedParams(const QVector<double>& values)
{
    auto params = m_separatorConfig.getOptimizableParams();
    if (params.size() != values.size()) {
        qWarning() << "Mismatch in parameter count";
        return;
    }

    for (int i = 0; i < values.size(); ++i) {
        m_separatorConfig.updateParam(params[i].commandIndex,
                                      params[i].paramIndex,
                                      values[i]);
    }

    rebuildTransport();

    QString text = m_configEditor->toPlainText();
    text += "\n# Оптимальные параметры:\n";
    for (int i = 0; i < values.size(); ++i) {
        text += QString("# %1 = %2\n").arg(params[i].elementName + "_" + params[i].paramName)
                                       .arg(values[i], 0, 'f', 6);
    }
    m_configEditor->setPlainText(text);

    QMessageBox::information(this, "Успех", "Оптимальные параметры применены к конфигурации и добавлены в редактор.");
}
void MainWindow::updateFilterCombos()
{
    if (!m_filterZCombo || !m_filterACombo) return;

    // Сохраняем текущий выбор, чтобы он не сбрасывался при обновлении
    QString currentZ = m_filterZCombo->currentText();
    QString currentA = m_filterACombo->currentText();

    m_filterZCombo->clear();
    m_filterZCombo->addItem("Все");
    m_filterACombo->clear();
    m_filterACombo->addItem("Все");

    QSet<int> uniqueZ, uniqueA;
    for (auto it = m_isotopeStats.keyBegin(); it != m_isotopeStats.keyEnd(); ++it) {
        uniqueZ.insert(it->first);
        uniqueA.insert(it->second);
    }

    QList<int> sortedZ = uniqueZ.values();
    std::sort(sortedZ.begin(), sortedZ.end());
    for (int z : sortedZ) m_filterZCombo->addItem(QString::number(z));

    QList<int> sortedA = uniqueA.values();
    std::sort(sortedA.begin(), sortedA.end());
    for (int a : sortedA) m_filterACombo->addItem(QString::number(a));

    // Восстанавливаем выбор, если он всё ещё актуален
    if (m_filterZCombo->findText(currentZ) != -1) m_filterZCombo->setCurrentText(currentZ);
    if (m_filterACombo->findText(currentA) != -1) m_filterACombo->setCurrentText(currentA);
}

void MainWindow::updateIsotopeStatsUI()
{
    if (!m_isotopeStatsTable) return;

    m_isotopeStatsTable->setRowCount(0);
    m_isotopeStatsTable->setRowCount(m_isotopeStats.size());

    int totalPassed = 0;
    for (int count : m_isotopeStats.values()) {
        totalPassed += count;
    }

    // Сортировка по Z, затем по A для аккуратного вида
    QList<QPair<int, int>> keys = m_isotopeStats.keys();
    std::sort(keys.begin(), keys.end(), [](const QPair<int, int>& a, const QPair<int, int>& b) {
        if (a.first != b.first) return a.first < b.first;
        return a.second < b.second;
    });

    int row = 0;
    for (const auto& key : keys) {
        int z = key.first;
        int a = key.second;
        int count = m_isotopeStats[key];
        double percent = totalPassed > 0 ? (double)count / totalPassed * 100.0 : 0.0;

        m_isotopeStatsTable->setItem(row, 0, new QTableWidgetItem(QString::number(z)));
        m_isotopeStatsTable->setItem(row, 1, new QTableWidgetItem(QString::number(a)));
        m_isotopeStatsTable->setItem(row, 2, new QTableWidgetItem(QString::number(count)));
        m_isotopeStatsTable->setItem(row, 3, new QTableWidgetItem(QString::number(percent, 'f', 2) + "%"));
        row++;
    }

    m_isotopeStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


// масса опорной частицы = остаток после испарения
double MainWindow::referenceMassAmu() const
{
    const double nP = m_protInput->text().toDouble();
    const double nN = m_neutrInput->text().toDouble();
    const double nA = m_alphaInput->text().toDouble();
    const double Zr = m_ZInput->text().toDouble() - nP - 2.0 * nA;
    const double Ar = m_AInput->text().toDouble() - nN - nP - 4.0 * nA;
    return Zr * 1.007276 + (Ar - Zr) * 1.008665 - 0.008 * Ar + Zr * 0.00054858;
}

// строит ОБА транспорта из одних и тех же опорных параметров
void MainWindow::rebuildTransport()
{
    const double refE = m_meanInput->text().toDouble() / 1000.0;   // МэВ
    const double refM = referenceMassAmu();
    const int    refQ = refCharge();
    m_separator = m_separatorConfig.buildSeparator(refE, refM, refQ);
    m_fieldLine = buildFieldLine(m_separatorConfig, refE, refM, refQ, true);
    updateSeparatorScheme();
}

// транспорт полевым движком; результат кладётся в Particle так же, как у матричного
void MainWindow::transportField(Particle& p, bool recordLoss)
{
    ft::IntegratorOpts opts;
    opts.h = qMax(0.0005, m_rkStepEdit->text().toDouble() / 1000.0);   // мм -> м
    opts.recordEvery = 10;

    const ft::TransportResult r = m_fieldLine.engine.transport(
        p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge, opts);

    const auto& els = m_fieldLine.engine.elements();
    size_t k = 0;
    double zEnd = 0.0;
    for (size_t i = 0; i < r.exits.size(); ++i) {
        zEnd += els[i]->designLength();
        // промежуточные точки (имя с '_' не попадает в распределения по точкам)
        while (k < r.track.size() && r.track[k].z < zEnd - 1e-9) {
            const auto& t = r.track[k++];
            p.addTrackPoint(t.z, t.x, t.y, 0, 0, p.energy, p.mass, p.charge, "fld_mid");
        }
        while (k < r.track.size() && r.track[k].z < zEnd + 1e-9) ++k;   // дубль конца
        const auto& e = r.exits[i];
        p.addTrackPoint(zEnd, e.x, e.y, e.xp, e.yp, e.eKin, p.mass, p.charge,
                        m_fieldLine.names.value(int(i)));
    }
    p.alive = r.alive;
    if (!r.alive) {
        while (k < r.track.size()) {
            const auto& t = r.track[k++];
            p.addTrackPoint(t.z, t.x, t.y, 0, 0, p.energy, p.mass, p.charge, "fld_mid");
        }
        if (recordLoss)
            m_separator.registerLoss(m_fieldLine.names.value(r.loss.elementIndex),
                                     r.loss.elementIndex);
    } else {
        p.x = r.x; p.y = r.y; p.xp = r.xp; p.yp = r.yp;
        if (!r.exits.empty()) p.energy = r.exits.back().eKin;
    }
}
void MainWindow::setupUI()
{
    // ------------------- Главное окно -------------------
    setWindowTitle("SHEMAT");
    resize(1000, 700);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    // ------------------- Вкладка "Генерация" -------------------
    QWidget *tabGen = new QWidget(this);
    QVBoxLayout *genLayout = new QVBoxLayout(tabGen);
    genLayout->setSpacing(10);

    QGroupBox *beamGroup = new QGroupBox("Параметры пучка", tabGen);
    QGridLayout *beamGrid = new QGridLayout(beamGroup);
    beamGrid->setColumnStretch(0, 1);
    beamGrid->setColumnStretch(1, 0);

    m_meanInput = new QLineEdit("490000");
    m_meanInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_meanInput->setMaximumWidth(120);
    m_varianceInput = new QLineEdit("0");
    m_varianceInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_varianceInput->setMaximumWidth(120);
    m_UInput = new QLineEdit("0");
    m_UInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_UInput->setMaximumWidth(120);
    m_aInput = new QLineEdit("8.0");
    m_aInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_aInput->setMaximumWidth(120);
    m_bInput = new QLineEdit("8.0");
    m_bInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_bInput->setMaximumWidth(120);
    m_ZInput = new QLineEdit("114");
    m_ZInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ZInput->setMaximumWidth(120);
    m_AInput = new QLineEdit("298");
    m_AInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_AInput->setMaximumWidth(120);
    m_NInput = new QLineEdit("1000");
    m_NInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_NInput->setMaximumWidth(120);

    int row = 0;
    beamGrid->addWidget(new QLabel("Средняя энергия, кэВ:"), row, 0);
    beamGrid->addWidget(m_meanInput, row, 1);
    row++;
    beamGrid->addWidget(new QLabel("Дисперсия энергии:"), row, 0);
    beamGrid->addWidget(m_varianceInput, row, 1);
    row++;
    m_refQInput = new QLineEdit("20");
    m_refQInput->setMaximumWidth(120);
    beamGrid->addWidget(new QLabel("Опорный заряд Q сепаратора:"), row, 0);
    beamGrid->addWidget(m_refQInput, row, 1);
    row++;
    beamGrid->addWidget(new QLabel("Энергия возбуждения U, МэВ:"), row, 0);
    beamGrid->addWidget(m_UInput, row, 1);
    row++;
    beamGrid->addWidget(new QLabel("Число частиц N:"), row, 0);
    beamGrid->addWidget(m_NInput, row, 1);
    row++;
    beamGrid->addWidget(new QLabel("Z ядра:"), row, 0);
    beamGrid->addWidget(m_ZInput, row, 1);
    row++;
    beamGrid->addWidget(new QLabel("A ядра:"), row, 0);
    beamGrid->addWidget(m_AInput, row, 1);
    row++;

    genLayout->addWidget(beamGroup);

    QGroupBox *targetGroup = new QGroupBox("Параметры мишени", tabGen);
    QGridLayout *targetGrid = new QGridLayout(targetGroup);
    targetGrid->setColumnStretch(0, 1);
    targetGrid->setColumnStretch(1, 0);


    m_angleInput = new QLineEdit("0");
    m_angleInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_angleInput->setMaximumWidth(120);

    row = 0;
    targetGrid->addWidget(new QLabel("Угол наклона мишени, град:"), row, 0);
    targetGrid->addWidget(m_angleInput, row, 1);
    row++;
    targetGrid->addWidget(new QLabel("Размер a, мм:"), row, 0);
    targetGrid->addWidget(m_aInput, row, 1);
    row++;
    targetGrid->addWidget(new QLabel("Размер b, мм:"), row, 0);
    targetGrid->addWidget(m_bInput, row, 1);

    genLayout->addWidget(targetGroup);

    QGroupBox *evapGroup = new QGroupBox("Испарение", tabGen);
    QGridLayout *evapGrid = new QGridLayout(evapGroup);
    evapGrid->setColumnStretch(0, 1);
    evapGrid->setColumnStretch(1, 0);

    m_neutrInput = new QLineEdit("0");
    m_neutrInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_neutrInput->setMaximumWidth(120);
    m_protInput = new QLineEdit("0");
    m_protInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_protInput->setMaximumWidth(120);
    m_alphaInput = new QLineEdit("0");
    m_alphaInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_alphaInput->setMaximumWidth(120);

    row = 0;
    evapGrid->addWidget(new QLabel("Нейтронов:"), row, 0);
    evapGrid->addWidget(m_neutrInput, row, 1);
    row++;
    evapGrid->addWidget(new QLabel("Протонов:"), row, 0);
    evapGrid->addWidget(m_protInput, row, 1);
    row++;
    evapGrid->addWidget(new QLabel("Альфа:"), row, 0);
    evapGrid->addWidget(m_alphaInput, row, 1);

    genLayout->addWidget(evapGroup);

    QHBoxLayout *genButtonLayout = new QHBoxLayout();
    m_generateButton = new QPushButton("Сгенерировать", tabGen);
    m_generateButton->setFixedWidth(200);
    m_generateButton->setMinimumHeight(32);
    genButtonLayout->addWidget(m_generateButton);
    genButtonLayout->addStretch();
    genLayout->addLayout(genButtonLayout);

    m_tabWidget->addTab(tabGen, "Генерация");

    // ------------------- Вкладка "Мишень" -------------------
    QWidget *tabTarget = new QWidget(this);
    QVBoxLayout *targetMainLayout = new QVBoxLayout(tabTarget);
    targetMainLayout->setSpacing(10);

    m_targetScrollArea = new QScrollArea(tabTarget);
    m_targetScrollArea->setWidgetResizable(true);
    m_targetContainer = new QWidget();
    m_targetLayout = new QVBoxLayout(m_targetContainer);
    m_targetLayout->setAlignment(Qt::AlignTop);
    m_targetScrollArea->setWidget(m_targetContainer);
    targetMainLayout->addWidget(m_targetScrollArea);

    QHBoxLayout *addButtonLayout = new QHBoxLayout();
    m_addLayerButton = new QPushButton("Добавить слой", tabTarget);
    m_addLayerButton->setFixedWidth(200);
    m_addLayerButton->setMinimumHeight(32);
    addButtonLayout->addWidget(m_addLayerButton);
    addButtonLayout->addStretch();
    targetMainLayout->addLayout(addButtonLayout);

    m_stripperGroup = new QGroupBox("Углеродная фольга (стриппер)", tabTarget);
    QHBoxLayout *stripperLayout = new QHBoxLayout(m_stripperGroup);
    m_stripperCheckBox = new QCheckBox("Включить стриппер", tabTarget);
    m_stripperCheckBox->setChecked(carbon_switch);
    stripperLayout->addWidget(m_stripperCheckBox);
    stripperLayout->addWidget(new QLabel("Толщина (мкг/см²):"));
    m_stripperThicknessEdit = new QLineEdit(QString::number(m_stripperThickness));
    m_stripperThicknessEdit->setMaximumWidth(80);
    stripperLayout->addWidget(m_stripperThicknessEdit);
    stripperLayout->addStretch();
    targetMainLayout->addWidget(m_stripperGroup);

    m_tabWidget->addTab(tabTarget, "Мишень");

    // ------------------- Вкладка "Ионная оптика" -------------------
    QWidget *tabOptics = new QWidget(this);
    QVBoxLayout *opticsMainLayout = new QVBoxLayout(tabOptics);

    m_configEditor = new QPlainTextEdit(tabOptics);
    m_configEditor->setStyleSheet("font-family: Courier New;");
    m_configEditor->setStyleSheet("background-color: #1e1e1e; color: #ffffff; border: 1px solid #555;");
    opticsMainLayout->addWidget(m_configEditor);

    QHBoxLayout *opticsBtnLayout = new QHBoxLayout();
    m_loadBtn = new QPushButton("Загрузить из файла", tabOptics);
    m_loadBtn->setFixedWidth(150);
    m_saveBtn = new QPushButton("Сохранить в файл", tabOptics);
    m_saveBtn->setFixedWidth(150);
    m_applyBtn = new QPushButton("Применить конфигурацию", tabOptics);
    m_applyBtn->setFixedWidth(180);
    opticsBtnLayout->addWidget(m_loadBtn);
    opticsBtnLayout->addWidget(m_saveBtn);
    opticsBtnLayout->addWidget(m_applyBtn);
    opticsBtnLayout->addStretch();
    opticsMainLayout->addLayout(opticsBtnLayout);

    m_applyTargetStripperCheck = new QCheckBox("Применить мишень/стриппер к загруженному пучку", tabOptics);
    m_useOpticsCheck = new QCheckBox("Использовать ионную оптику", tabOptics);
    opticsMainLayout->addWidget(m_applyTargetStripperCheck);
    opticsMainLayout->addWidget(m_useOpticsCheck);

    m_multiProductCheck = new QCheckBox("Режим мульти-продуктов", tabOptics);
    opticsMainLayout->insertWidget(opticsMainLayout->indexOf(m_useOpticsCheck), m_multiProductCheck);



    QHBoxLayout *trLayout = new QHBoxLayout();
    trLayout->addWidget(new QLabel("Алгоритм транспорта:", tabOptics));
    m_transportCombo = new QComboBox(tabOptics);
    m_transportCombo->addItems({"Матричный (быстро)", "Полевой RK4 (точно)",
                                "Сравнение: матрица и RK4"});
    trLayout->addWidget(m_transportCombo);
    trLayout->addWidget(new QLabel("Шаг RK4, мм:", tabOptics));
    m_rkStepEdit = new QLineEdit("5", tabOptics);
    m_rkStepEdit->setMaximumWidth(60);
    trLayout->addWidget(m_rkStepEdit);
    trLayout->addStretch();
    opticsMainLayout->addLayout(trLayout);

    QHBoxLayout *opticsActionLayout = new QHBoxLayout();
    m_optimizeBtn = new QPushButton("Фитирование", tabOptics);
    m_optimizeBtn->setFixedWidth(150);
    m_showTrajBtn = new QPushButton("Показать траектории", tabOptics);
    m_showTrajBtn->setFixedWidth(150);
    opticsActionLayout->addWidget(m_optimizeBtn);
    opticsActionLayout->addWidget(m_showTrajBtn);
    opticsActionLayout->addStretch();
    opticsMainLayout->addLayout(opticsActionLayout);

    m_transmissionLabel = new QLabel("Трансмиссия: -- %", tabOptics);
    m_transmissionLabel->setStyleSheet("QLabel { font-weight: bold; color: #00ff00;}");
    opticsMainLayout->addWidget(m_transmissionLabel);

    m_tabWidget->addTab(tabOptics, "Ионная оптика");

    // ------------------- Вкладка "Графики" -------------------
    QWidget *tabPlots = new QWidget(this);
    QVBoxLayout *plotsMainLayout = new QVBoxLayout(tabPlots);

    QHBoxLayout *plotsControl = new QHBoxLayout();
    plotsControl->addWidget(new QLabel("Точка сепаратора:"));
    m_pointCombo = new QComboBox(tabPlots);
    m_pointCombo->addItem("После мишени");
    plotsControl->addWidget(m_pointCombo);
    plotsControl->addStretch();
    plotsMainLayout->addLayout(plotsControl);

    QHBoxLayout *axesLayout = new QHBoxLayout();
    axesLayout->addWidget(new QLabel("X:"));
    m_xParamComboBox = new QComboBox(tabPlots);
    m_xParamComboBox->addItems({"x","y","x'","y'","E","Q","N","BR"});
    axesLayout->addWidget(m_xParamComboBox);
    axesLayout->addWidget(new QLabel("Y:"));
    m_yParamComboBox = new QComboBox(tabPlots);
    m_yParamComboBox->addItems({"x","y","x'","y'","E","Q","N","BR"});
    axesLayout->addWidget(m_yParamComboBox);
    axesLayout->addStretch();


    ///////////////////////
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Фильтр Z:"));
    m_filterZCombo = new QComboBox(tabPlots);
    m_filterZCombo->addItem("Все");
    m_filterZCombo->setMaximumWidth(80);
    filterLayout->addWidget(m_filterZCombo);

    filterLayout->addWidget(new QLabel("Фильтр A:"));
    m_filterACombo = new QComboBox(tabPlots);
    m_filterACombo->addItem("Все");
    m_filterACombo->setMaximumWidth(80);
    filterLayout->addWidget(m_filterACombo);
    filterLayout->addStretch();
    plotsMainLayout->insertLayout(1, filterLayout); // Вставляем после выбора точки сепаратора
    ///////////////////////


    plotsMainLayout->addLayout(axesLayout);

    QHBoxLayout *plotButtons = new QHBoxLayout();
    m_plotButton = new QPushButton("Построить график", tabPlots);
    m_histogramButton = new QPushButton("Гистограмма", tabPlots);
    plotButtons->addWidget(m_plotButton);
    plotButtons->addWidget(m_histogramButton);
    plotButtons->addStretch();
    plotsMainLayout->addLayout(plotButtons);

    m_chartContainer = new QWidget(tabPlots);
    QVBoxLayout *chartLayout = new QVBoxLayout(m_chartContainer);
    m_chartView = new QChartView(this);
    chartLayout->addWidget(m_chartView);
    plotsMainLayout->addWidget(m_chartContainer);

    QHBoxLayout *axisRangeLayout = new QHBoxLayout();
    axisRangeLayout->addWidget(new QLabel("X min:"));
    m_xMinInput = new QLineEdit("0");
    m_xMinInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_xMinInput->setMaximumWidth(80);
    axisRangeLayout->addWidget(m_xMinInput);
    axisRangeLayout->addWidget(new QLabel("X max:"));
    m_xMaxInput = new QLineEdit("0");
    m_xMaxInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_xMaxInput->setMaximumWidth(80);
    axisRangeLayout->addWidget(m_xMaxInput);
    axisRangeLayout->addWidget(new QLabel("Y min:"));
    m_yMinInput = new QLineEdit("0");
    m_yMinInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_yMinInput->setMaximumWidth(80);
    axisRangeLayout->addWidget(m_yMinInput);
    axisRangeLayout->addWidget(new QLabel("Y max:"));
    m_yMaxInput = new QLineEdit("0");
    m_yMaxInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_yMaxInput->setMaximumWidth(80);
    axisRangeLayout->addWidget(m_yMaxInput);
    m_updateAxesButton = new QPushButton("Обновить оси", tabPlots);
    m_updateAxesButton->setFixedWidth(120);
    axisRangeLayout->addWidget(m_updateAxesButton);
    axisRangeLayout->addStretch();
    plotsMainLayout->addLayout(axisRangeLayout);

    m_tabWidget->addTab(tabPlots, "Графики");

    // ------------------- Вкладка "Статистика" -------------------
    QWidget *tabStats = new QWidget(this);
    QVBoxLayout *statsLayout = new QVBoxLayout(tabStats);
    m_statsTable = new QTableWidget(0, 4, tabStats);
    m_statsTable->setHorizontalHeaderLabels({"Элемент","Потеряно","% от потерь","RMS X, м"});
    m_statsTable->setStyleSheet(
        "QTableWidget { background-color: #0a1e2c; color: #e0f7fa; gridline-color: #1a4c6e; }"
        "QTableWidget::item { color: #e0f7fa; }"
        "QHeaderView::section { background-color: #1a4c6e; color: #e0f7fa; padding: 4px; }"
    );
    m_statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statsLayout->addWidget(m_statsTable);

    statsLayout->addSpacing(10);
    statsLayout->addWidget(new QLabel("Статистика прошедших изотопов:", tabStats));
    m_isotopeStatsTable = new QTableWidget(0, 4, tabStats);
    m_isotopeStatsTable->setHorizontalHeaderLabels({"Z", "A", "Прошло частиц", "% от прошедших"});
    m_isotopeStatsTable->setStyleSheet(m_statsTable->styleSheet()); // Тот же стиль, что и у таблицы потерь
    m_isotopeStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statsLayout->addWidget(m_isotopeStatsTable);

    m_tabWidget->addTab(tabStats, "Статистика");
}
