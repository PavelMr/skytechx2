#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T18:46:06
#
#-------------------------------------------------

QT += core gui
QT += widgets opengl

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
               mapobject \
               debug \
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
    renderer/text2drenderer.cpp \
    mapobject/mapobject.cpp \
    renderer/painteroverlay.cpp \
    catalogue/gsc.cpp \
    renderer/layergscstars.cpp \
    renderer/starshader.cpp \
    debug/debugwidget.cpp \
    core/stringutils.cpp \
    catalogue/ucac4.cpp \
    renderer/layerucac4stars.cpp \
    catalogue/dso.cpp \
    renderer/layerbackground.cpp \
    core/skstring.cpp

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
    renderer/text2drenderer.h \
    mapobject/mapobject.h \
    renderer/painteroverlay.h \
    catalogue/gsc.h \
    renderer/layergscstars.h \
    renderer/starshader.h \
    debug/debugwidget.h \
    core/stringutils.h \
    catalogue/ucac4.h \
    renderer/layerucac4stars.h \
    catalogue/dso.h \
    catalogue/dso_def.h \
    renderer/layerbackground.h \
    core/skstring.h

FORMS    += mainwindow/mainwindow.ui \
    mainwindow/sidebar/sidebarquickinfo.ui \
    mainwindow/sidebar/sidebarmapcontrol.ui \
    debug/debugwidget.ui

RCC_DIR = res

RESOURCES += \
    resource.qrc

DISTFILES +=
