QT += core gui charts quick qml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    angulardistributionmodel.cpp \
    bindingenergydata.cpp \
    carbonenergyloss.cpp \
    chargecalc.cpp \
    compaundgen.cpp \
    element.cpp \
    elementselector.cpp \
    fitter.cpp \
    main.cpp \
    mainwindow.cpp \
    neutronevaporation.cpp \
    newtonsolver.cpp \
    optimizationdialog.cpp \
    periodictable.cpp \
    scattering.cpp \
    separator/separatorconfig.cpp \
    separator/separatorview.cpp \
    separator/trajectoryscene.cpp \
    separator/trajectoryview.cpp \
    separator/trajectorywindow.cpp \
    stripperdialog.cpp \
    stylehelper.cpp \
    separator/parser.cpp \
    separator/separator.cpp \
    separator/separatorelement.cpp \
    targetstripperdialog.cpp

HEADERS += \
    angulardistributionmodel.h \
    bindingenergydata.h \
    carbonenergyloss.h \
    chargecalc.h \
    compaundgen.h \
    element.h \
    elementselector.h \
    fitter.h \
    mainwindow.h \
    neutronevaporation.h \
    newtonsolver.h \
    optimizationdialog.h \
    periodictable.h \
    scattering.h \
    separator/separatorconfig.h \
    separator/separatorview.h \
    separator/trajectoryscene.h \
    separator/trajectoryview.h \
    separator/trajectorywindow.h \
    stripperdialog.h \
    stylehelper.h \
    separator/parser.h \
    separator/particle.h \
    separator/separator.h \
    separator/separatorelement.h \
    targetstripperdialog.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    data.qrc \
    resourses.qrc

DISTFILES += \
    elements_data.json
