#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T18:46:06
#
#-------------------------------------------------

QT += core gui opengl

QT += widgets

TARGET = skytechx2
TEMPLATE = app

INCLUDEPATH += mainwindow \
               mainwindow/sidebar \
               dataresource \
               catalogue \
               renderer \
               transform \
               gscregions \
               core \
               define

SOURCES += main.cpp\
           mainwindow/mainwindow.cpp \
    mainwindow/sidebar/sidebarquickinfo.cpp \
    mainwindow/sidebar/sidebarmapcontrol.cpp \
    mainwindow/mapwidget.cpp \
    dataresource/dataresources.cpp \
    catalogue/tycho.cpp \
    core/skfile.cpp \
    renderer/layertychostars.cpp \
    renderer/renderer.cpp \
    transform/transform.cpp \
    catalogue/constellations.cpp \
    renderer/layerconstellations.cpp \
    transform/mapparam.cpp \
    core/skmath.cpp \
    gscregions/gscregions.cpp \
    core/plane.cpp \
    core/vecmath.cpp \
    renderer/textrenderer.cpp \
    renderer/layergrid.cpp \
    core/skdebug.cpp \
    renderer/text2drenderer.cpp

HEADERS  += mainwindow/mainwindow.h \
    mainwindow/sidebar/sidebarquickinfo.h \
    mainwindow/sidebar/sidebarmapcontrol.h \
    mainwindow/mapwidget.h \
    dataresource/dataresources.h \
    catalogue/tycho.h \
    core/skfile.h \
    define/define.h \
    renderer/layertychostars.h \
    renderer/starvertices.h \
    renderer/renderer.h \
    transform/transform.h \
    catalogue/constellations.h \
    renderer/layerconstellations.h \
    transform/mapparam.h \
    core/skmath.h \
    gscregions/gscregions.h \
    core/bbox.h \
    core/plane.h \
    core/vecmath.h \
    renderer/textrenderer.h \
    renderer/layergrid.h \
    core/skdebug.h \
    renderer/text2drenderer.h

FORMS    += mainwindow/mainwindow.ui \
    mainwindow/sidebar/sidebarquickinfo.ui \
    mainwindow/sidebar/sidebarmapcontrol.ui

RCC_DIR = res

RESOURCES += \
    resource.qrc
