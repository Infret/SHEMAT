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
    ui->setupUi(this);

//    // фон
//    QLabel *backgroundLabel = new QLabel(this);
//    backgroundLabel->setGeometry(this->rect());  // растягиваем на всё окно
//    backgroundLabel->setScaledContents(true);    // масштаб по размеру окна

//    // Загружаем GIF через QMovie
//    QMovie *movie = new QMovie(":/resourses/images/background_glow.gif");
//    backgroundLabel->setMovie(movie);
//    movie->start();

//    // Отправляем фон назад (под все виджеты)
//    backgroundLabel->lower();

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














    m_angularDistributionModel = new angulardistributionmodel(this);
    // Инициализируем таблицу элементов
     m_periodicTable = new PeriodicTable(this);
     if (m_periodicTable->loadFromTxt(":/data/elements_data.txt")) {
         qDebug() << "Periodic table loaded successfully";
     }

     m_elementSelector = new ElementSelector(
         m_periodicTable,
         ui->compoundCombo,
         ui->elementCombo,
         ui->isotopeCombo,
         static_cast<QObject*>(this)
     );

     m_elementSelector->initialize();

     connect(m_elementSelector, &ElementSelector::selectionChanged,
             this, &MainWindow::onTargetSelectionChanged);












     ////////////////////////////МЕНЮ+ПРЕСЕТЫ


     // ====== Меню и пресеты ======

     QMenuBar *menuBar = new QMenuBar(this);
     setMenuBar(menuBar);

     // --- Файл ---
     QMenu *fileMenu = new QMenu(tr("Файл"), this);
     menuBar->addMenu(fileMenu);

     QAction *saveAction = new QAction(tr("Сохранить..."), this);
     QAction *exitAction = new QAction(tr("Выход"), this);
     QAction *loadBeamAction = new QAction(tr("Загрузить пучок из файла..."), this);  // новый пункт



     fileMenu->addAction(loadBeamAction);
     fileMenu->addAction(saveAction);
     fileMenu->addSeparator();
     fileMenu->addAction(exitAction);

     connect(saveAction, &QAction::triggered, this, &MainWindow::saveToFile);
     connect(exitAction, &QAction::triggered, this, &QWidget::close);
     connect(loadBeamAction, &QAction::triggered, this, &MainWindow::loadBeamFromFile);

     // --- Реакции ---
     QMenu *reactionMenu = new QMenu(tr("Реакции"), this);
     menuBar->addMenu(reactionMenu);

     // (198Pt,5n) → 215Ra (мишень 300 мкг/см²)
     // compound Z=88 (Ra), A=215, neutrons=5
     QAction *reaction1 = new QAction(tr("(198Pt,5n) → 215Ra (300 мкг/см²)"), this);
     connect(reaction1, &QAction::triggered, this, [this]() {
         QVector<TargetLayer> layers;
         layers.append({"Pt", 198, 1});
         applyReactionPreset( QString("198Pt"),
                              QString("215Ra"),
                              QString("5n"),
                              300.0,     // толщина мкг/см2
                              10500.0,   // энергия (keV)
                              21.45,     // плотность Pt (пример, г/см3)
                              layers,
                              88, 215,   // compoundZ, compoundA
                              5, 0, 0);  // neutrons, protons, alphas
     });
     reactionMenu->addAction(reaction1);

     // 255No + 238U (5n, E=13 MeV, 350 мкг/см²)
     // compound Z(No)=102, A=255, neutrons=5
     QAction *reaction2 = new QAction(tr("22Ne + 238U → 255No (5n, E=13 MeV, 350 мкг/см²)"), this);
     connect(reaction2, &QAction::triggered, this, [this]() {
         QVector<TargetLayer> layers;
         layers.append({"U", 238, 1});
         applyReactionPreset(QString("22Ne"),
                             QString("255No"),
                             QString("5n"),
                             350.0,
                             13000.0,   // 13 MeV -> 13000 keV
                             19.05,     // плотность U ~19.05 g/cm3
                             layers,
                             102, 255,
                             5, 0, 0);
     });
     reactionMenu->addAction(reaction2);

     // 28Mg + 238U → 260Rf (4n, E=20 MeV, 290 мкг/см²)
     // compound Z(Rf)=104, A=260, neutrons=4
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


     //  No255 + U3O8
     QAction *reaction4 = new QAction(tr(" 255No + U3O8 (4n, E=10 MeV, 350 мкг/см²)"), this);
     connect(reaction4, &QAction::triggered, this, [this]() {
         QVector<TargetLayer> layers;
         layers.append({"U", 238, 3});
         layers.append({"O", 16, 8});


         applyReactionPreset(QString("proj?"),
                             QString("Compound"),
                             QString("4n"),
                             350.0,     // толщина мкг/см2 (пример)
                             10000.0,   // 15 MeV -> 15000 keV
                             8.05,
                             layers,
                             102, 255,
                             4,0,0);    // канал 4n
     });
     reactionMenu->addAction(reaction4);

     reactionMenu->addSeparator();
     QAction *resetPreset = new QAction(tr("Сбросить параметры мишени"), this);
     connect(resetPreset, &QAction::triggered, this, [this]() {
//         ui->TargetLenghtInput->setText("0");
//         ui->DensInput->setText("0");
         ui->meanInput->setText("0");
         // очистить слои
         clearLayersUI();
     });
     reactionMenu->addAction(resetPreset);



     // --- Настройки ---

     QMenu *SettingsMenu = new QMenu(tr("Настройки"), this);
     menuBar->addMenu(SettingsMenu);

     // Чекбокс для углеродной фольги (в меню "Настройки")
     QAction *CarbonLayer = new QAction(tr("Углеродная фольга"), this);
     CarbonLayer->setCheckable(true);
     CarbonLayer->setChecked(false); // по умолчанию выключена
     SettingsMenu->addAction(CarbonLayer);

     carbon_switch = false; // начальное состояние

     connect(CarbonLayer, &QAction::toggled, this, [this](bool checked) {
         if (checked) {
             // Показываем диалог ввода толщины
             StripperDialog dlg(this);
             if (dlg.exec() == QDialog::Accepted) {
                 m_stripperThickness = dlg.getThickness();
                 carbon_switch = true;
                 qDebug() << "Стриппер включён, толщина =" << m_stripperThickness << "мкг/см²";
             } else {
                 // Если пользователь отменил, снимаем галку
                 QAction *action = qobject_cast<QAction*>(sender());
                 if (action) action->setChecked(false);
                 carbon_switch = false;
             }
         } else {
             carbon_switch = false;
             qDebug() << "Стриппер выключен";
         }
     });


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
         textEdit->setFont(QFont("Courier New", 10));
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

//         QPushButton *closeBtn = new QPushButton("Закрыть", &dlg);
//         connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
//         dlgLayout->addWidget(closeBtn, 0, Qt::AlignRight);

         dlg.exec();
     });



//     m_applyTargetStripperCheck = new QCheckBox("Применить мишень/стриппер к загруженному пучку", this);
//     m_applyTargetStripperCheck->setChecked(false);
//     ui->horizontalLayout_19->addWidget(m_applyTargetStripperCheck); // поместите в подходящий layout
//     connect(m_applyTargetStripperCheck, &QCheckBox::toggled, this, [this](bool checked) {
//         m_applyTargetStripperToBeam = checked;
//     });
 ///////////////////////////////////////////////











    // Создаем график и связываем его с виджетом QChartView
    chart = new QChart;
    chartView = new QChartView(chart);

    QScreen *screen = QGuiApplication::primaryScreen();
    double dpi = screen->logicalDotsPerInch();
    double scale = dpi / 96.0;
    chartView->setTransform(QTransform::fromScale(scale, scale));

    chartView->setRenderHint(QPainter::Antialiasing);

    barSet = new QBarSet("Данные");
    barSeries = new QBarSeries();

    series = new QScatterSeries;

    series->setColor(Qt::red);
    series->setMarkerSize(5);


    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setBackgroundBrush(QBrush(Qt::white));

//    ui->MenuComboBox->setStyleSheet(R"(
//        QComboBox::drop-down {
//            width: 0;
//            height: 0;
//            border: none;  // Убирает границу стрелки
//        }
//    )");

    QValueAxis *xAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    QValueAxis *yAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    // Настройка основной сетки
    QPen gridPen;
    gridPen.setColor(QColor(0, 0, 0, 100));
    gridPen.setWidth(1);
    gridPen.setStyle(Qt::SolidLine);

    xAxis->setGridLinePen(gridPen);
    yAxis->setGridLinePen(gridPen);
    // Настройка внешнего вида осей
    QPen axisPen(Qt::black);
    axisPen.setWidth(2); // Толщина линии оси

    int fontSize = scaledFontSize(12);

    // Для X оси
    xAxis->setLinePen(axisPen);
    xAxis->setLabelsColor(Qt::black); // Цвет меток
    xAxis->setTitleBrush(QBrush(Qt::black)); // Цвет заголовка
    xAxis->setTitleFont(QFont("Arial", fontSize, QFont::Bold));

    // Для Y оси
    yAxis->setLinePen(axisPen);
    yAxis->setLabelsColor(Qt::black);
    yAxis->setTitleBrush(QBrush(Qt::black));
    yAxis->setTitleFont(QFont("Arial", fontSize, QFont::Bold));

    ui->xMinInput->setText("0");
    ui->xMaxInput->setText("0");
    ui->yMinInput->setText("0");
    ui->yMaxInput->setText("0");


    connect(ui->xMinInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(ui->xMaxInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(ui->yMinInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);
    connect(ui->yMaxInput, &QLineEdit::editingFinished, this, &MainWindow::updateAxes);


    ///////////////////////PHAZE SPACE/////////////////////////////////////////////////////////////////////
    // Находим verticalLayout_8
    QVBoxLayout *targetLayout = qobject_cast<QVBoxLayout*>(ui->verticalLayout_8);
    if (targetLayout) {
        // Горизонтальный layout для выбора точки
        QHBoxLayout *pointLayout = new QHBoxLayout();
        QLabel *pointLabel = new QLabel("Точка сепаратора:", this);
        m_pointCombo = new QComboBox(this);
        m_pointCombo->addItem("После мишени"); // по умолчанию
        pointLayout->addWidget(pointLabel);
        pointLayout->addWidget(m_pointCombo);
        pointLayout->addStretch();
        targetLayout->insertLayout(0, pointLayout); // вставляем в начало

        // Подключаем выбор точки к обновлению графика (опционально)
        connect(m_pointCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](){ /* можно перестроить график, если нужно */ });
    }


//    QPushButton *showTrajBtn = new QPushButton("Показать траектории", this);
//    connect(showTrajBtn, &QPushButton::clicked, this, &MainWindow::showTrajectories);
//    ui->verticalLayout->addWidget(showTrajBtn); // или в targetLayout

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////







    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
       layout->addWidget(chartView);
       ui->chartContainer->setLayout(layout);


       ui->xParamComboBox->addItems({"x", "y", "x'", "y'", "E", "Q", "N", "BR"});
       ui->yParamComboBox->addItems({"x", "y", "x'", "y'", "E", "Q", "N", "BR"});


    // Связываем кнопку построения графика с функцией
    connect(ui->plotButton, &QPushButton::clicked, this, [=]() {
            QString xParam = ui->xParamComboBox->currentText();
            QString yParam = ui->yParamComboBox->currentText();
            plotGraph(xParam, yParam);
        });

    // Установка начальных значений
    ui->meanInput->setText(QString::number(490000)); // Средняя энергия компаунда в КэВ
    ui->varianceInput->setText(QString::number(0)); // Дисперсия энергии компаунда
    ui->UInput->setText(QString::number(0)); // Энергия возбуждения МЭВЫ!!!!!!!!
//    ui->DensInput->setText(QString::number(0)); // Плотность мишени g/cm^3
//    ui->TargetLenghtInput->setText(QString::number(0)); // Длина мишени мкгр/см^2
    ui->aInput->setText(QString::number(8.0)); // Размеры эллипса a
    ui->bInput->setText(QString::number(8.0)); // Размеры эллипса b
    ui->NInput->setText(QString::number(1000)); // Количество компаундов
    ui->NeutrInput->setText(QString::number(0)); // Количество нейтронов
    ui->ProtInput->setText(QString::number(0)); // Количество протонов
    ui->AlphaInput->setText(QString::number(0)); // Количество альфа-частиц
    ui->ZInput->setText(QString::number(114));
    ui->AInput->setText(QString::number(298));
    ui->angleInput->setText(QString::number(0));






    ///ДЛЯ ИНТЕРФЭЙСА СО СЛОЯМИ///////////////////////////////////////////////////////////////////

    // В конструкторе MainWindow после инициализации других элементов:
    // Создаем контейнер для слоев
    QWidget *layersContainer = new QWidget(this);
//    QVBoxLayout *layersLayout = new QVBoxLayout(layersContainer);
    ui->scrollAreaWidgetContents->setLayout(new QVBoxLayout());
    ui->scrollAreaWidgetContents->layout()->addWidget(layersContainer);

    // Кнопка добавления слоя
    connect(ui->addLayerButton, &QPushButton::clicked, this, &MainWindow::onAddLayerClicked);

    // Добавляем первый слой по умолчанию
    onAddLayerClicked();

//    // Добавляем layout в основной интерфейс
//    ui->targetGroupBox->setLayout(layersLayout);




    ////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////ИОН ОПТИКА///////////////////////////////////





//        QPushButton *showTrajBtn = new QPushButton("Показать траектории", this);
//        connect(showTrajBtn, &QPushButton::clicked, this, &MainWindow::showTrajectories);
//        ui->verticalLayout->addWidget(showTrajBtn);



        // ====== Кнопка загрузки конфигурации сепаратора ======
//        QPushButton *loadConfigBtn = new QPushButton("Загрузить конфигурацию сепаратора", this);
//        ui->verticalLayout->addWidget(loadConfigBtn);
//        connect(loadConfigBtn, &QPushButton::clicked, this, &MainWindow::loadSeparatorConfig);

        // В конструкторе, после создания кнопок оптики:
        m_separatorView = new SeparatorView(this);
        m_separatorView->hide();


        QWidget *tab4 = ui->tabWidget->findChild<QWidget*>("tab_4");
        if (!tab4) {
            qDebug() << "tab_4 not found";
            return;
        }

        QVBoxLayout *mainLayout4 = ui->verticalLayout_17;
        if (!mainLayout4) {
            mainLayout4 = new QVBoxLayout(tab4);
            tab4->setLayout(mainLayout4);
        }

        // Очистка старых виджетов
        QLayoutItem *child;
        while ((child = mainLayout4->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        // Горизонтальный layout: левая часть (редактор + кнопки) и правая (справка)
        QHBoxLayout *mainHorizontal = new QHBoxLayout();

        // Левая часть: вертикальный layout с редактором и панелью кнопок
        QVBoxLayout *leftLayout = new QVBoxLayout();

        // Редактор конфигурации
        m_configEditor = new QPlainTextEdit(tab4);
        m_configEditor->setFont(QFont("Courier New", 10));
        m_configEditor->setStyleSheet("background-color: #1e1e1e; color: #ffffff; border: 1px solid #555;");
        leftLayout->addWidget(m_configEditor, 1); // растягивается по вертикали

        // Панель кнопок управления конфигурацией (под редактором)
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *loadBtn = new QPushButton("Загрузить из файла", tab4);
        QPushButton *saveBtn = new QPushButton("Сохранить в файл", tab4);
        QPushButton *applyBtn = new QPushButton("Применить конфигурацию", tab4);
//        QPushButton *defaultBtn = new QPushButton("Сбросить к примеру", tab4);
        btnLayout->addWidget(loadBtn);
        btnLayout->addWidget(saveBtn);
        btnLayout->addWidget(applyBtn);
//        btnLayout->addWidget(defaultBtn);
        leftLayout->addLayout(btnLayout);





        // В mainwindow.cpp, после создания mainLayout4
        QTabWidget *tabWidget = ui->tabWidget;
        QWidget *statsTab = new QWidget();
        QVBoxLayout *statsLayout = new QVBoxLayout(statsTab);
        m_statsTable = new QTableWidget(0, 4, this);
        m_statsTable->setHorizontalHeaderLabels({"Элемент", "Потеряно", "% от потерь", "RMS X, м"});
        m_statsTable->setStyleSheet(
            "QTableWidget { background-color: #0a1e2c; color: #e0f7fa; gridline-color: #1a4c6e; }"
            "QTableWidget::item { color: #e0f7fa; }"
            "QHeaderView::section { background-color: #1a4c6e; color: #e0f7fa; padding: 4px; }"
        );
        m_statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        statsLayout->addWidget(m_statsTable);
        tabWidget->addTab(statsTab, "Статистика");



        // Чекбокс "Применить мишень/стриппер к загруженному пучку" (добавляем под кнопками)
        m_applyTargetStripperCheck = new QCheckBox("Применить мишень/стриппер к загруженному пучку", tab4);
        m_applyTargetStripperCheck->setChecked(false);
        leftLayout->addWidget(m_applyTargetStripperCheck);





        // Чекбокс "Использовать ионную оптику" (можно добавить сюда же, если нужно)
        QCheckBox *useOpticsCheck = new QCheckBox("Использовать ионную оптику", tab4);
        useOpticsCheck->setChecked(m_useOptics);
        leftLayout->addWidget(useOpticsCheck);

        QPushButton *optimizeBtn = new QPushButton("Фитирование", this);
        optimizeBtn->setMinimumSize(80, 25);   // не меньше
        optimizeBtn->setMaximumSize(150, 40);
        connect(optimizeBtn, &QPushButton::clicked, this, &MainWindow::showOptimizationDialog);
        leftLayout->addWidget(optimizeBtn);

        // Кнопка "Показать траектории"
        QPushButton *showTrajBtn = new QPushButton("Показать траектории", tab4);
        showTrajBtn->setMinimumSize(80, 25);   // не меньше
        showTrajBtn->setMaximumSize(150, 40);

        leftLayout->addWidget(showTrajBtn);

        mainHorizontal->addLayout(leftLayout, 7); // 70% ширины

//        // Правая часть: справка (без ограничения высоты)
//        QTextEdit *helpText = new QTextEdit(tab4);
//        helpText->setReadOnly(true);
//        helpText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//        helpText->setFont(QFont("Courier New", 9));
//        helpText->setStyleSheet("background-color: #ffffff; color: #000000; border: 1px solid #aaa;");
//        helpText->setPlainText(
//            "Доступные команды конфигурации (единицы: метры, радианы, Тесла, кВ):\n"
//            "\n"
//            "  DRIFT длина_м [апертура_м] - дрейф (апертура по умолч. 0.1)\n"
//            "  QUAD длина_м поле_Тл апертура_м [fringe] - квадруполь (fringe по умолч. 0.2)\n"
//            "  DEFLECTOR радиус_м угол_град зазор_м напряжение_кВ - эл.стат. дефлектор\n"
//            "  DIPOLE радиус_м угол_град зазор_м угол_входа_град угол_выхода_град renter rexit\n"
//            "                                                   - магнитный диполь с краями\n"
//            "\n"
//            "Пример (сепаратор Q-Q-Q-E10-D37):\n"
//            "  DRIFT 0.6 0.1\n"
//            "  QUAD 0.375 0.5961 0.095 0.2\n"
//            "  DRIFT 0.2 0.1\n"
//            "  QUAD 0.375 -0.6221 0.095 0.2\n"
//            "  DRIFT 0.2 0.1\n"
//            "  QUAD 0.375 0.3891 0.095 0.2\n"
//            "  DRIFT 1.0 0.1\n"
//            "  DEFLECTOR 11.0 10.0 0.07 0.0\n"
//            "  DRIFT 0.8 0.1\n"
//            "  DIPOLE 1.055 37.0 0.057 46.0 -43.0 0.5 -0.5\n"
//            "  DRIFT 1.9 0.1\n"
//        );
//        mainHorizontal->addWidget(helpText, 3); // 30% ширины

        mainLayout4->addLayout(mainHorizontal);

        // --- Индикатор трансмиссии (внизу) ---
        m_transmissionLabel = new QLabel("Трансмиссия: -- %", tab4);
        m_transmissionLabel->setStyleSheet("QLabel { font-weight: bold; color: #00ff00; font-size: 12px; }");
        mainLayout4->addWidget(m_transmissionLabel);

        // Подключение сигналов
        connect(useOpticsCheck, &QCheckBox::toggled, this, [this](bool checked) {
            m_useOptics = checked;
        });
        connect(m_applyTargetStripperCheck, &QCheckBox::toggled, this, [this](bool checked) {
            m_applyTargetStripperToBeam = checked;
        });
        connect(showTrajBtn, &QPushButton::clicked, this, &MainWindow::showTrajectories);
        connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadConfigFromFile);
        connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveConfigToFile);
        connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyConfigFromEditor);
//        connect(defaultBtn, &QPushButton::clicked, this, &MainWindow::loadDefaultConfigToEditor);

        // Загружаем конфигурацию по умолчанию
//        loadDefaultConfigToEditor();
    ////////// ////////// ////////// ////////// ////////// ////////// ////////// ////////// //////////








    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(ui->histogramButton, &QPushButton::clicked, this, [=]() {
        QString selectedParam = ui->xParamComboBox->currentText();
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
            else if (selectedParam == "BR") data = pd.br;   // <-- добавлено
            else {
                QMessageBox::warning(this, "Ошибка", "Неизвестный параметр.");
                return;
            }
        } else {
            // Старый режим (оптика выключена)
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

     mean = ui->meanInput->text().toDouble();
     variance = ui->varianceInput->text().toDouble();
     U = ui->UInput->text().toDouble();
     Z = ui->ZInput->text().toDouble();
     A = ui->AInput->text().toDouble();
     Mass = (Z * 1.007276) + ((A - Z) * 1.008665) - 0.008 * A + (Z * 0.00054858);

//     mt = At * 1.6726219e-27 * 6.022e23;


     a = ui->aInput->text().toDouble();
     b = ui->bInput->text().toDouble();
     N = ui->NInput->text().toInt();
     numNeutrons = ui->NeutrInput->text().toInt();
     numProtons = ui->ProtInput->text().toInt();
     numAlphas = ui->AlphaInput->text().toInt();
    xValues.clear();
    yValues.clear();
    xPrimeValues.clear();
    yPrimeValues.clear();
    energyValues.clear();
    qValues.clear();

    ///
    ///
    ///
    ///
    ///
    ///
    ///
    compaundGen = new CompaundGen(mean, variance, a, b);
    neutronEvaporation = new NeutronEvaporation(Z, A, U, E);
    scattering = new Scattering(E, Z, A, Zt, At, density, mt, l, targetflag,tiltBeta, this);




//    m_angularDistributionModel = new angulardistributionmodel(this);


//   angulardistributionmodel model(E/1000, Z, A, Zt, At, this);

//    m_langevin = new LangevinModel(this);

    chargecalc = new ChargeCalc(Z, Mass, Q, E);
    if (carbon_switch) {
        carbonenergyloss = new CarbonEnergyLoss(E, Z, A, m_stripperThickness, this);
    } else {
        carbonenergyloss = nullptr;
    }
    ///
    ///
    ///
    ///
    ///
    ///
    ///

    // --- Здесь создаём и настраиваем QProgressDialog ---
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
       // Показываем окно сразу, даже при малых N иначе не появляется при быстрых расчетах
       progress.setMinimumDuration(0);

       // QSS
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




        /// /// /// /// /// /// /// /// /// /// /// /// ////// --- ПОДГОТОВКА СЕПАРАТОРА  --- /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
       if (m_useOptics) {
           double refEnergy_MeV = ui->meanInput->text().toDouble() / 1000.0;
           double refMass = ui->AInput->text().toDouble();
           int refCharge = ui->ZInput->text().toInt();
           double refMassAmu = (refCharge * 1.007276) + ((refMass - refCharge) * 1.008665) - 0.008 * refMass + (refCharge * 0.00054858);
           m_separator = m_separatorConfig.buildSeparator(refEnergy_MeV, refMassAmu, refCharge);
           updateSeparatorScheme();
       }

       m_passedCount = 0;
       m_allXZ.clear();
       m_allYZ.clear();

       m_separator.resetLossStats();

       for (int i = 0; i < N; ++i) {
           QCoreApplication::processEvents();
           if (progress.wasCanceled()) break;

           Modeling();   // генерация одной частицы

           // Конвертация координат из миллиметров в метры (если генератор выдаёт мм)
           x /= 1000.0;
           y /= 1000.0;

           if (m_useOptics && Q1 > 0) {
               Particle p;
               p.x = x; p.y = y; p.xp = x_prime; p.yp = y_prime;
               p.energy = E; p.mass = Mass; p.charge = Q1; p.alive = true;
               p.clearTrack();
               // Точка "После мишени" (начало сепаратора, Z=0)
               p.addTrackPoint(0.0, x, y, x_prime, y_prime, E, Mass, Q1, "После мишени");

               m_separator.transport(p);

               // Всегда добавляем распределения по точкам (для фазовых пространств)
               addTrackToDistributions(p);

               if (p.alive) {
                   // Траектории только для живых частиц
                   QVector<QPointF> xz, yz;
                   for (const auto& tp : p.track) {
                       xz.append(QPointF(tp.z, tp.x));
                       yz.append(QPointF(tp.z, tp.y));
                   }
                   m_allXZ.append(xz);
                   m_allYZ.append(yz);
                   m_passedCount++;
               }
           }

           // Обновление прогресса
           progress.setValue(i + 1);
           QApplication::processEvents();
       }

       // ======================== ПОСЛЕ ЦИКЛА ========================
       // Заполняем комбобокс точками сепаратора (один раз!)
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
//       qDebug() << "Количество траекторий:" << m_allXZ.size();
//       qDebug() << "Количество точек распределений:" << m_distributions.size();
       for (const auto& pd : m_distributions) {
            qDebug() << "  -" << pd.pointName;
           m_pointCombo->addItem(pd.pointName);
       }
       if (m_pointCombo->count() > 0) {
           m_pointCombo->setCurrentIndex(0);
       }




//       if (m_useOptics && !m_allXZ.isEmpty()) {
//           showTrajectories();  // или m_trajWindow->show(), если у вас есть отдельный метод
//       }

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
   }






void MainWindow::Modeling()
{

    mean = ui->meanInput->text().toDouble();
    variance = ui->varianceInput->text().toDouble();
    U = ui->UInput->text().toDouble();
    Z = ui->ZInput->text().toDouble();
    tiltBeta = ui->angleInput->text().toDouble(); //УГОЛ НАКЛОНА МИШЕНИ

    A = ui->AInput->text().toDouble();
//    density = ui->DensInput->text().toDouble() * 1000;
//    l = (ui->TargetLenghtInput->text().toDouble() * 1e-4) / density;
    a = ui->aInput->text().toDouble();
    b = ui->bInput->text().toDouble();
    N = ui->NInput->text().toInt();
    numNeutrons = ui->NeutrInput->text().toInt();
    numProtons = ui->ProtInput->text().toInt();
    numAlphas = ui->AlphaInput->text().toInt();
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
        //qDebug()<<Q1<<"Q1";

        //debugShow(x,y,x_prime, y_prime,Q1,BR);



        ///ПРОХОЖДЕНИЕ ЧЕРЕЗ ФОЛЬГУ
        ///
        ///
        ///
        if (!carbon_switch) {
            Q1 = 1; // или ничего, просто не применяем стриппер
        }


        if (carbon_switch && carbonenergyloss) {
            carbonenergyloss->CarbonCalc(E, Z, A, theta, phi, x_prime, y_prime);
        }

        E = E/1000;

//        Q1 = 19;////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//////УБРАТЬ ПОТОМ

        BR = (1.439) / (10 * Q1) * sqrt(Mass * E/1000);

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
    // Очищаем старые серии и оси
    chart->removeAllSeries();
    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis* axis : axes) {
        chart->removeAxis(axis);
        delete axis;
    }

    QVector<double> xData, yData;
    // Если оптика включена и есть распределения по точкам
    if (m_useOptics && !m_distributions.isEmpty()) {
        int pointIdx = m_pointCombo->currentIndex();
        if (pointIdx < 0 || pointIdx >= m_distributions.size()) {
            QMessageBox::warning(this, "Ошибка", "Выберите точку сепаратора.");
            return;
        }
        const PointData& pd = m_distributions[pointIdx];

        // Заполняем xData в зависимости от xParam
        if (xParam == "x") xData = pd.x;
        else if (xParam == "y") xData = pd.y;
        else if (xParam == "x'") xData = pd.xp;
        else if (xParam == "y'") xData = pd.yp;
        else if (xParam == "E") xData = pd.energy;
        else if (xParam == "Q") xData = pd.charge;
        else if (xParam == "BR") xData = pd.br;
        else if (xParam == "N") {
            xData.resize(pd.x.size());
            std::iota(xData.begin(), xData.end(), 1.0); // номера частиц 1..N
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Неизвестный параметр X.");
            return;
        }

        // Заполняем yData в зависимости от yParam
        if (yParam == "x") yData = pd.x;
        else if (yParam == "y") yData = pd.y;
        else if (yParam == "x'") yData = pd.xp;
        else if (yParam == "y'") yData = pd.yp;
        else if (yParam == "E") yData = pd.energy;
        else if (yParam == "Q") yData = pd.charge;
        else if (yParam == "BR") yData = pd.br;
        else if (yParam == "N") {
            yData.resize(pd.x.size());
            std::iota(yData.begin(), yData.end(), 1.0);
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Неизвестный параметр Y.");
            return;
        }
    }
    else {
        // Старый режим (оптика выключена) – используем глобальные массивы
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

    if (xData.isEmpty() || yData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно данных для построения графика.");
        return;
    }

    // Создаём scatter series
    QScatterSeries *series = new QScatterSeries();
    series->setColor(Qt::red);
    series->setMarkerSize(5);
    for (int i = 0; i < xData.size(); ++i) {
        series->append(xData[i], yData[i]);
    }
    chart->addSeries(series);

    // Создаём оси
    QValueAxis *xAxis = new QValueAxis();
    QValueAxis *yAxis = new QValueAxis();
    double xMin = *std::min_element(xData.begin(), xData.end());
    double xMax = *std::max_element(xData.begin(), xData.end());
    double yMin = *std::min_element(yData.begin(), yData.end());
    double yMax = *std::max_element(yData.begin(), yData.end());
    xAxis->setRange(xMin, xMax);
    yAxis->setRange(yMin, yMax);
    xAxis->setTitleText(xParam);
    yAxis->setTitleText(yParam);
    // Настройка сетки и внешнего вида (как в вашем старом коде)
    QPen gridPen(QColor(0,0,0,100), 1, Qt::SolidLine);
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

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    chart->setTitle("Scatter Plot");
    chartView->setChart(chart);
}
void MainWindow::plotHistogram(const QVector<double>& data, int bins, const QString& paramName)
{
    chart->removeAllSeries();
    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis* axis : axes) {
        chart->removeAxis(axis);
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
        chart->addSeries(barSeries);
        chart->setTitle("Histogram (Q)");

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        QFont font = axisX->labelsFont();
        font.setPointSize(8);
        axisX->setLabelsFont(font);
        axisX->setLabelsAngle(45);   // поворот, чтобы влезали
        chart->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, *std::max_element(frequencies.begin(), frequencies.end()) + 1);
        chart->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        chartView->setChart(chart);
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
    chart->addSeries(barSeries);
    chart->setTitle("Histogram");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    QFont font = axisX->labelsFont();
    font.setPointSize(8);
    axisX->setLabelsFont(font);
    axisX->setLabelsAngle(45);      // поворот, чтобы подписи влезали
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, *std::max_element(frequencies.begin(), frequencies.end()) + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    chartView->setChart(chart);
}
void MainWindow::updateAxes()
{
    qDebug() << "Обновление осей...";

       QValueAxis *xAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).value(0, nullptr));
       QValueAxis *yAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).value(0, nullptr));

       if (!xAxis || !yAxis) {
           qDebug() << "Ошибка: оси графика отсутствуют!";
           chart->createDefaultAxes();
           return;
       }

       bool xMinOk, xMaxOk, yMinOk, yMaxOk;
       double xMin = ui->xMinInput->text().toDouble(&xMinOk);
       double xMax = ui->xMaxInput->text().toDouble(&xMaxOk);
       double yMin = ui->yMinInput->text().toDouble(&yMinOk);
       double yMax = ui->yMaxInput->text().toDouble(&yMaxOk);

       if (xMinOk && xMaxOk) xAxis->setRange(xMin, xMax);
       if (yMinOk && yMaxOk) yAxis->setRange(yMin, yMax);

       qDebug() << "Оси обновлены: X(" << xMin << "," << xMax << "), Y(" << yMin << "," << yMax << ")";
}
void MainWindow::SetAxis()
{
    // Получаем указатели на оси
    //QChart *chart = chartView->chart();
    QValueAxis *xAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    QValueAxis *yAxis = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
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

    // Обновляем параметры на UI
    updateParametersFromSelection();
}
void MainWindow::updateParametersFromSelection()
{
    auto selection = m_elementSelector->getSelectedElement();
    const Element* element = selection.first;
    int isotopeA = selection.second;

    if (!element) return;

    // Всегда показываем поля Z и A
//    ui->TargetZInput->setVisible(true);


    // Устанавливаем Z и A
//    ui->TargetZInput->setText(QString::number(element->getZ()));
    int A = isotopeA > 0 ? isotopeA : element->getZ() * 2;


    // Плотность
    double density = (isotopeA > 0 && element->findIsotope(isotopeA)) ?
                     element->findIsotope(isotopeA)->density :
                     element->getCommonDensity();

//    ui->DensInput->setText(QString::number(density));

//    ui->outputText->append(
//        QString("Выбран элемент: %1, Z=%2, A=%3, плотность=%4")
//        .arg(element->getSymbol())
//        .arg(element->getZ())
//        .arg(A)
//        .arg(density)
//    );
}















//////////////для слоев мишени//////////////////////////
// Реализация функций управления слоями
void MainWindow::onRemoveLayerClicked(QWidget *layerWidget)
{
    if (!layerWidget) return;

    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    if (layout) layout->removeWidget(layerWidget);

    layerWidgets.removeAll(layerWidget);

    layerWidget->setParent(nullptr);
    layerWidget->deleteLater();
}

// Добавление слоя
void MainWindow::onAddLayerClicked()
{
    // group box — один слой
    QGroupBox *layerGroup = new QGroupBox(QString("Слой %1").arg(layerWidgets.size() + 1), this);

    layerGroup->setObjectName("layerGroup");
    QVBoxLayout *groupLayout = new QVBoxLayout(layerGroup);
      layerGroup->setStyleSheet("QGroupBox { font-weight: bold; margin-top: 6px; }");

    // верхняя строка: Толщина, Плотность, кнопки

    QHBoxLayout *topRow = new QHBoxLayout();

    QCheckBox *mainLayerCheck = new QCheckBox("Основной слой", layerGroup);
    mainLayerCheck->setObjectName("mainLayerCheck");
    mainLayerCheck->setChecked(true); // по умолчанию — подложка
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
//    densityEdit->setText(ui->DensInput->text(0.0)); // по умолчанию взять глобальную
    topRow->addWidget(densityEdit);


    QPushButton *addElemBtn = new QPushButton("Добавить элемент", layerGroup);
    topRow->addWidget(addElemBtn);

    topRow->addSpacing(20);   // 20 пикселей между кнопками

    QPushButton *removeLayerBtn = new QPushButton("Удалить слой", layerGroup);
    topRow->addWidget(removeLayerBtn);

    addElemBtn->setFixedSize(105, 28);
    removeLayerBtn->setFixedSize(80, 28);
    ///



    groupLayout->addLayout(topRow);

    // Контейнер для строк элементов в этом слое
    QWidget *elementsContainer = new QWidget(layerGroup);
    elementsContainer->setObjectName("elementsContainer");
    QVBoxLayout *elementsLayout = new QVBoxLayout(elementsContainer);
    elementsLayout->setContentsMargins(0,0,0,0);
    elementsContainer->setLayout(elementsLayout);
    groupLayout->addWidget(elementsContainer);

    // Стили (можешь настроить)
    layerGroup->setStyleSheet("QGroupBox { font-weight: bold; margin-top: 6px; }");

    // подключаем кнопки
    connect(removeLayerBtn, &QPushButton::clicked, this, [this, layerGroup]() {
        this->onRemoveLayerClicked(layerGroup);
    });

    connect(addElemBtn, &QPushButton::clicked, this, [this, layerGroup]() {
        // добавляет элемент в этот слой (по умолчанию первый элемент таблицы)
        addElementRowToLayer(layerGroup, QString(), 0, 1.0);
    });

    // добавить один элемент по умолчанию
    addElementRowToLayer(layerGroup, QString(), 0, 1.0);

    // вставляем в scroll area
    QVBoxLayout *containerLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!containerLayout) {
        containerLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
        ui->scrollAreaWidgetContents->setLayout(containerLayout);
    }
    containerLayout->addWidget(layerGroup);
    layerWidgets.append(layerGroup);
}
// Безопасный getLayersFromUI (искать по objectName)
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



        // Перебираем виджеты-строки внутри elementsContainer
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

    // Combo элементов
    QComboBox *elementCombo = new QComboBox(rowWidget);
    for (const Element &el : m_periodicTable->getAllElements()) {
        elementCombo->addItem(el.getSymbol());
    }
    elementCombo->setObjectName("elementCombo");

    // поле A
    QLineEdit *aEdit = new QLineEdit(rowWidget);
    aEdit->setObjectName("aEdit");
    aEdit->setValidator(new QIntValidator(1, 10000, aEdit));
    aEdit->setMinimumWidth(50);
    aEdit->setMaximumWidth(100);
    aEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // stoich
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

    // Инициализация значений, если переданы
    if (!symbol.isEmpty()) {
        int idx = elementCombo->findText(symbol);
        if (idx >= 0) elementCombo->setCurrentIndex(idx);
        else elementCombo->addItem(symbol);
    }

    if (A > 0) aEdit->setText(QString::number(A));
    else {
        // авто-подстановка первого изотопа элемента
        const Element *el = m_periodicTable->getElementBySymbol(elementCombo->currentText());
        if (el && !el->getAllIsotopes().isEmpty()) aEdit->setText(QString::number(el->getAllIsotopes().first().A));
    }
    stoichEdit->setText(QString::number(stoich));

    // при смене элемента — можно обновлять A автоматически
    connect(elementCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, elementCombo, aEdit](int){
        const Element* el = m_periodicTable->getElementBySymbol(elementCombo->currentText());
        if (el && !el->getAllIsotopes().isEmpty()) {
            aEdit->setText(QString::number(el->getAllIsotopes().first().A));
        }
    });

    // удалить строку
    connect(removeElemBtn, &QPushButton::clicked, this, [this, rowWidget](){
        QWidget *parent = rowWidget->parentWidget();
        if (parent) {
            QLayout *parentLayout = parent->layout();
            if (parentLayout) parentLayout->removeWidget(rowWidget);
        }
        rowWidget->setParent(nullptr);
        rowWidget->deleteLater();
    });

    // добавляем строку в контейнер
    QVBoxLayout *elementsLayout = qobject_cast<QVBoxLayout*>(elementsContainer->layout());
    elementsLayout->addWidget(rowWidget);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

// ====== Метод для подстановки значений ======

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
    // Заполняем основные поля
//    ui->TargetLenghtInput->setText(QString::number(thickness)); // мкг/см2
//    ui->DensInput->setText(QString::number(density));
    ui->meanInput->setText(QString::number(energy));

    // Обновляем Z/A компаунд-ядра, если переданы корректные значения
    if (compoundZ > 0) {
        ui->ZInput->setText(QString::number(compoundZ));
    }
    if (compoundA > 0) {
        ui->AInput->setText(QString::number(compoundA));
    }

    // Обновляем количество испарённых частиц (если переданы >=0)
    if (neutrons >= 0) ui->NeutrInput->setText(QString::number(neutrons));
    if (protons  >= 0) ui->ProtInput->setText(QString::number(protons));
    if (alphas   >= 0) ui->AlphaInput->setText(QString::number(alphas));

    // Очистим старые слои UI и внутренний список
    clearLayersUI();

    // Добавим слои из пресета
    for (const TargetLayer &layer : layers) {
        // layer.elementSymbol, layer.A, layer.stoich (и опционально thickness, если есть)
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
    // Удаляем виджеты, которые мы сами создавали и храним в layerWidgets
    for (QWidget* w : layerWidgets) {
        if (!w) continue;
        QLayout *parentLayout = ui->scrollAreaWidgetContents->layout();
        if (parentLayout)
            parentLayout->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
    }
    layerWidgets.clear();

    // На всякий случай очистим все оставшиеся элементы layout (если есть)
    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    if (!layout) return;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        QWidget *pw = child->widget();
        if (pw) {
            pw->setParent(nullptr);
            pw->deleteLater();
        }
        delete child;
    }
}


void MainWindow::addLayerToUI(const QString &symbol, int A, double stoich, double thickness = 0.0, double density = 0.0)
{
    // создаём слой (переиспользуем onAddLayerClicked, а потом исправляем поля)
    onAddLayerClicked();
    QLineEdit *thicknessEdit = new QLineEdit(QString::number(thickness), this);
    QLineEdit *densityEdit = new QLineEdit(QString::number(density), this);

    thicknessEdit->setValidator(new QDoubleValidator(0, 1e6, 6, this));
    densityEdit->setValidator(new QDoubleValidator(0, 1e6, 6, this));

    if (layerWidgets.isEmpty()) return;
    QGroupBox *lastLayer = qobject_cast<QGroupBox*>(layerWidgets.last());
    if (!lastLayer) return;

    // заполняем поля толщины/плотности
    QLineEdit *th = lastLayer->findChild<QLineEdit*>("thicknessEdit");
    QLineEdit *de = lastLayer->findChild<QLineEdit*>("densityEdit");
    if (th && thickness > 0.0) th->setText(QString::number(thickness));
    if (de && density > 0.0) de->setText(QString::number(density));

    // добавляем элемент-строку
    addElementRowToLayer(lastLayer, symbol, A, stoich);
}










//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////












void MainWindow::loadSeparatorConfig()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите конфигурацию сепаратора", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;
    if (m_separatorConfig.loadFromFile(filename)) {
        updateSeparator(); // строим сепаратор с текущими параметрами из GUI
        QMessageBox::information(this, "Успех", "Конфигурация загружена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
    }
}

void MainWindow::updateSeparator()
{
    double refEnergy_MeV = ui->meanInput->text().toDouble() / 1000.0;
    double refMass = ui->AInput->text().toDouble();
    int refCharge = ui->ZInput->text().toInt();
    double refMassAmu = (refCharge * 1.007276) + ((refMass - refCharge) * 1.008665) - 0.008 * refMass + (refCharge * 0.00054858);
    m_separator = m_separatorConfig.buildSeparator(refEnergy_MeV, refMassAmu, refCharge);
    updateSeparatorScheme();
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
    // Обновляем данные
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
    applyConfig(); // сразу применяем
}

void MainWindow::applyConfig()
{
    QString text = m_configEditor->toPlainText();
    if (m_separatorConfig.parseFromString(text)) {
        // Обновляем сепаратор при следующей генерации (или сразу, если нужен)
        // Можно вызвать updateSeparator() позже при нажатии Generate
        // Пока просто сохраним флаг, что конфиг изменился
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
        // Если оптика включена, можно сразу перестроить сепаратор, но лучше при следующей генерации
        // Для наглядности можно перестроить и обновить схему:
        if (m_useOptics) {
            double refEnergy_MeV = ui->meanInput->text().toDouble() / 1000.0;
            double refMass = ui->AInput->text().toDouble();
            int refCharge = ui->ZInput->text().toInt();
            double refMassAmu = (refCharge * 1.007276) + ((refMass - refCharge) * 1.008665) - 0.008 * refMass + (refCharge * 0.00054858);
            m_separator = m_separatorConfig.buildSeparator(refEnergy_MeV, refMassAmu, refCharge);
            updateSeparatorScheme();
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось разобрать конфигурацию. Проверьте синтаксис.");
    }
}

void MainWindow::addTrackToDistributions(const Particle& p)
{
    for (const auto& tp : p.track) {
        // Пропускаем промежуточные точки (содержат "_")
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
    }
}

void MainWindow::printRMSDistribution()
{
    qDebug() << "\n========== RMS DISTRIBUTIONS AT EACH POINT ==========";
    for (const auto& pd : m_distributions) {
        qDebug() << "\nPoint: " << pd.pointName;
        qDebug() << "  Number of particles: " << pd.x.size();

        if (pd.x.isEmpty()) continue;

        // Вычисление среднего и RMS
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

        // Средние значения (для справки)
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
int MainWindow::scaledFontSize(int baseSize = 10) {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return baseSize;
    double dpi = screen->logicalDotsPerInch();
    double scale = dpi / 96.0;
    return qRound(baseSize * scale);
}
void MainWindow::loadBeamFromFile()
{


    m_loaded_x.clear();
    m_loaded_y.clear();
    m_loaded_xp.clear();
    m_loaded_yp.clear();
    m_loaded_E.clear();
    m_loaded_Q.clear();



    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл с пучком", "", "Text files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }


    // Очистка старых данных
    m_allXZ.clear();
    m_allYZ.clear();
    m_distributions.clear();
    xValues.clear(); yValues.clear(); xPrimeValues.clear(); yPrimeValues.clear();
    energyValues.clear(); qValues.clear();
    if (m_trajWindow) m_trajWindow->clearTrajectories();

    QTextStream in(&file);
    QVector<QStringList> allParts;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 6) continue;
        allParts.append(parts);
    }
    file.close();

    if (allParts.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит корректных данных.");
        return;
    }

    // --- Параметры для обработки мишени/стриппера (берём из GUI) ---
    bool doTargetStripper = m_applyTargetStripperToBeam; // чекбокс на вкладке
    int partZ = ui->ZInput->text().toInt();
    int partA = ui->AInput->text().toInt();
    double tiltBetaDeg = ui->angleInput->text().toDouble(); // угол наклона мишени (град)
    double tiltBetaRad = tiltBetaDeg * M_PI / 180.0;
    bool applyStripper = carbon_switch; // чекбокс "Углеродная фольга" из меню "Настройки"
    QVector<TargetLayer> targetLayers = getLayersFromUI(); // слои мишени из интерфейса

    // Проверка: если включена обработка, но нет слоёв мишени, выдаём предупреждение
    if (doTargetStripper && targetLayers.isEmpty() && applyStripper == false) {
        QMessageBox::warning(this, "Ошибка", "Включена обработка мишени/стриппера, но не заданы слои мишени и не включена углеродная фольга.");
        return;
    }

    QProgressDialog progress("Загрузка пучка...", "Отмена", 0, allParts.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    m_passedCount = 0;
    const double refMass = ui->AInput->text().toDouble(); // масса для референсной частицы (если не используем partA)


    if (m_useOptics) {
        double refEnergy_MeV = ui->meanInput->text().toDouble() / 1000.0;
        double refMass = ui->AInput->text().toDouble();
        int refCharge = ui->ZInput->text().toInt();
        double refMassAmu = (refCharge * 1.007276) + ((refMass - refCharge) * 1.008665) - 0.008 * refMass + (refCharge * 0.00054858);
        m_separator = m_separatorConfig.buildSeparator(refEnergy_MeV, refMassAmu, refCharge);
        updateSeparatorScheme();
        m_separator.resetLossStats(); // <-- СБРОС СТАТИСТИКИ
    }



    for (int i = 0; i < allParts.size(); ++i) {
        if (progress.wasCanceled()) break;
        progress.setValue(i);
        QCoreApplication::processEvents();

        const QStringList& parts = allParts[i];
        double x_mm = parts[0].toDouble();
        double y_mm = parts[1].toDouble();
        double xp = parts[2].toDouble();        // рад
        double yp = parts[3].toDouble();        // рад
        double E_MeV = parts[4].toDouble();     // МэВ
        double Q_double = parts[5].toDouble();
        int Q = qRound(Q_double);
        qDebug() << "Read from file: xp=" << xp << "yp=" << yp;
        qDebug() << "i=" << i << "file xp=" << xp << "file yp=" << yp;

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


        if (doTargetStripper && Q >= 0) {
            // 1) Рассеяние в мишени (по слоям)
            if (!targetLayers.isEmpty()) {
                double theta = xp;
                double phi = yp;
                for (const auto& layer : targetLayers) {
                    const Element* elem = m_periodicTable->getElementBySymbol(layer.elementSymbol);
                    if (!elem) continue;
                    double Zt = elem->getZ();
                    double At = layer.A;
                    double layerDens = layer.density * 1000.0;      // г/см³ → кг/м³
                    double layerThickM = layer.thickness * 1e-5 / layerDens; // мкг/см² → м
                    double Mt_calc = At * 1.6726219e-27 * 6.022e23;

                    qDebug() << "Before scattering: xp=" << xp << "yp=" << yp;
                    Scattering scatter(E_keV, partZ, partA, Zt, At,
                                       layerDens, Mt_calc, layerThickM,
                                       layer.Targetflag, tiltBetaRad, this);


                    // Приводим целые к double для передачи по ссылке
                    double dZ = partZ, dA = partA, dZt = Zt, dAt = At;
                    scatter.ScatteringCalc(E_keV, dZ, dA, dZt, dAt,
                                           layerDens, Mt_calc, layerThickM,
                                           theta, phi, xp, yp,
                                           layer.Targetflag, tiltBetaRad);
                    qDebug() << "after scattering: xp=" << xp << "yp=" << yp;
                }
            }

            // 2) Стриппер
            if (applyStripper) {
                double theta = xp;
                double phi = yp;
                double dZ = partZ, dA = partA;

                qDebug() << "before stripper: xp=" << xp << "yp=" << yp;
                CarbonEnergyLoss carbonLoss(E_keV, partZ, partA, m_stripperThickness, this);
                carbonLoss.CarbonCalc(E_keV, dZ, dA, theta, phi, xp, yp);
                qDebug() << "after stripper: xp=" << xp << "yp=" << yp;

            }

            // 3) Пересчёт заряда после стриппера
            if (applyStripper) {
                double newQ = Q;
                double dZ = partZ;    // int -> double
                double dA = partA;    // int -> double
                ChargeCalc chargeCalc(dZ, dA, newQ, E_keV);
                chargeCalc.CalculateQ(dZ, dA, newQ, E_keV);
                Q = qRound(newQ);
                qDebug() << "Новый заряд после стриппера:" << Q;
            }

            E_MeV = E_keV / 1000.0;
            // === ЗАПИСЬ В ФАЙЛ (только ДО оптики, после стриппера) ===
            // Вычисляем BR для этой частицы
            double BR = (1.439) / (10.0 * Q) * sqrt(partA * E_MeV / 1000.0);
            QString dataLine = QString("%1 %2 %3 %4 %5 %6 %7")
                               .arg(x, 8, 'f', 6)
                               .arg(y, 8, 'f', 6)
                               .arg(xp, 8, 'f', 4)
                               .arg(yp, 8, 'f', 4)
                               .arg(E_MeV, 6, 'f', 2)
                               .arg(Q, 2)
                               .arg(BR, 6, 'f', 3);

            if (Q >= 0) {   // только частицы с положительным зарядом
                Particle pForFit;
                pForFit.x = x;
                pForFit.y = y;
                pForFit.xp = xp;
                pForFit.yp = yp;
                pForFit.energy = E_MeV;          // уже в МэВ
                pForFit.mass = partA;            // или используйте свою переменную массы
                pForFit.charge = Q;
                pForFit.alive = true;
                pForFit.clearTrack();
                pForFit.addTrackPoint(0.0, x, y, xp, yp, E_MeV, partA, Q, "После мишени");
                m_loadedBeam.append(pForFit);
            }

            writeToFile(dataLine);
        }

        // --- Транспорт через сепаратор ---
        if (m_useOptics && Q >= 0) {
            Particle p;
            p.x = x; p.y = y; p.xp = xp; p.yp = yp;
            p.energy = E_MeV;
            double mass_amu = (doTargetStripper && partA > 0) ? partA : refMass;
            p.mass = mass_amu;
            p.charge = Q;
            p.alive = true;
            p.clearTrack();
            p.addTrackPoint(0.0, x, y, xp, yp, E_MeV, mass_amu, Q, "После мишени");
            m_separator.transport(p);

            // Всегда добавляем распределения по точкам (для фазовых пространств)
            addTrackToDistributions(p);

            if (p.alive) {
                // Траектории только для живых частиц
                QVector<QPointF> xz, yz;
                for (const auto& tp : p.track) {
                    xz.append(QPointF(tp.z, tp.x));
                    yz.append(QPointF(tp.z, tp.y));
                }
                m_allXZ.append(xz);
                m_allYZ.append(yz);
                m_passedCount++;
            }



        } else if (m_useOptics && Q <= 0) {
            continue;
        } else {
            // Оптика выключена – для старых графиков
            m_passedCount++;
            xValues.append(x);
            yValues.append(y);
            xPrimeValues.append(xp);
            yPrimeValues.append(yp);
            energyValues.append(E_MeV);
            qValues.append(Q);
        }
    }

    // После цикла:
    progress.setValue(allParts.size());


    // Добавляем точку "Загруженный пучок" для всех частиц (до обработки)
    if (!m_loaded_x.isEmpty()) {
        PointData pd;
        pd.pointName = "Загруженный пучок";
        pd.x = m_loaded_x;
        pd.y = m_loaded_y;
        pd.xp = m_loaded_xp;
        pd.yp = m_loaded_yp;
        pd.energy = m_loaded_E;
        pd.charge = m_loaded_Q;  // если в PointData есть поле charge
        m_distributions.insert(0, pd);
    }



    // Обновление комбобокса и вывод трансмиссии
    if (m_useOptics && !m_distributions.isEmpty()) {
        m_pointCombo->clear();
        for (const auto& pd : m_distributions) {
            m_pointCombo->addItem(pd.pointName);
        }
        if (m_pointCombo->count() > 0) m_pointCombo->setCurrentIndex(0);
    }

    double transmission = (double)m_passedCount / allParts.size() * 100.0;
    qDebug() << QString("Загружено частиц: %1, прошло сепаратор: %2 (%3%)")
                    .arg(allParts.size()).arg(m_passedCount).arg(transmission, 0, 'f', 2);
    if (m_transmissionLabel) {
        m_transmissionLabel->setText(QString("Трансмиссия: %1% (%2 из %3)")
                                     .arg(transmission, 0, 'f', 2)
                                     .arg(m_passedCount)
                                     .arg(allParts.size()));
    }

    // --- ВЫВОД СТАТИСТИКИ ПОТЕРЬ ---
    if (m_useOptics && !m_distributions.isEmpty()) {
        printLossStatistics();
        updateStatsUI();
    }

    if (m_useOptics && !m_allXZ.isEmpty()) {
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

    // 1. Если есть загруженный пучок – используем его
    if (!m_loadedBeam.isEmpty()) {
        beam = m_loadedBeam;
        qDebug() << "Используется загруженный пучок, частиц:" << beam.size();
    }
    // 2. Иначе если генераторы готовы – генерируем новый
    else if (m_generatorsReady) {
        beam = generateFittingBeam(500);
        qDebug() << "Сгенерирован новый пучок для фиттера, частиц:" << beam.size();
    }
    // 3. Иначе – ошибка
    else {
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
    dlg.exec();

    // Можно добавить обработку результата (опционально)
}


QVector<Particle> MainWindow::generateFittingBeam(int numParticles)
{
    QVector<Particle> beam;

    // Сохраняем текущие значения
    int savedN = N;
    bool savedOptics = m_useOptics;
    bool savedStripper = carbon_switch;

    // Временно отключаем оптику (чтобы получить пучок после мишени/стриппера, но до сепаратора)
    m_useOptics = false;
    N = numParticles;

    // Очищаем старые данные
    xValues.clear(); yValues.clear(); xPrimeValues.clear(); yPrimeValues.clear();
    energyValues.clear(); qValues.clear();

    // Генерируем частицы
    for (int i = 0; i < numParticles; ++i) {
        Modeling(); // генерирует одну частицу, заполняет x, y, x_prime, y_prime, E, Q1
        // Конвертация координат из мм в м
        x /= 1000.0;
        y /= 1000.0;

        Particle p;
        p.x = x; p.y = y; p.xp = x_prime; p.yp = y_prime;
        p.energy = E; // уже в МэВ
        p.mass = Mass;
        p.charge = Q1;
        p.alive = true;
        p.clearTrack();
        p.addTrackPoint(0.0, p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge, "Target");
        beam.append(p);
//        qDebug()<<"parmetri PUCHKA dla FIT"<<p.x, p.y, p.xp, p.yp, p.energy, p.mass, p.charge;
    }

    // Восстанавливаем настройки
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
    int totalParticles = N; // общее число частиц

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

    // Сортируем по индексу элемента (порядок прохождения)
    std::sort(lossStats.begin(), lossStats.end(),
              [](const LossStat& a, const LossStat& b) {
                  return a.elementIndex < b.elementIndex;
              });

    int totalLost = m_separator.getTotalLost();
    int totalParticles = N;

    // Очищаем таблицу
    m_statsTable->setRowCount(0);
    m_statsTable->setRowCount(lossStats.size() + 1); // +1 для строки "Итого"

    int row = 0;
    for (const auto& stat : lossStats) {
        // --- Находим RMS X для точки, где произошла потеря ---
        double rmsX = 0.0;
        // Ищем точку по имени (stat.pointName)
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

        // Заполняем строку
        m_statsTable->setItem(row, 0, new QTableWidgetItem(stat.elementName));
        m_statsTable->setItem(row, 1, new QTableWidgetItem(QString::number(stat.lostCount)));

        double percentOfLosses = (totalLost > 0) ? (double)stat.lostCount / totalLost * 100.0 : 0.0;
        m_statsTable->setItem(row, 2, new QTableWidgetItem(QString::number(percentOfLosses, 'f', 1) + "%"));

        m_statsTable->setItem(row, 3, new QTableWidgetItem(QString::number(rmsX, 'f', 6)));

        row++;
    }

    // --- Итоговая строка ---
    m_statsTable->setItem(row, 0, new QTableWidgetItem("Итого"));
    m_statsTable->setItem(row, 1, new QTableWidgetItem(QString::number(totalLost)));

    double totalPercent = (totalParticles > 0) ? (double)totalLost / totalParticles * 100.0 : 0.0;
    m_statsTable->setItem(row, 2, new QTableWidgetItem(QString::number(totalPercent, 'f', 1) + "%"));

    // Для итоговой строки RMS X не имеет смысла, оставляем пустым или ставим прочерк
    m_statsTable->setItem(row, 3, new QTableWidgetItem("—"));

    // Растягиваем колонки
    m_statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
