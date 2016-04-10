#-------------------------------------------------
#
# Project created by QtCreator 2014-12-17T17:33:18
#
#-------------------------------------------------

DEFINES+=TARGET_64
TARGET = In2Designer

LIBS+= -L../../../Comun/barcode/libs/x64 \
       -lzint





QT       += core gui sql xml printsupport network

CONFIG -= CONSOLE

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app


TRANSLATIONS += Languajes/SinglePass_en.ts


DEFINES+=__WINDOWS__ NO_OPC IN2DESIGNER
DEFINES+=PHARMA
DEFINES+= X1001

SOURCES += main.cpp\
    ../../../Comun/SRC/teclado.cpp \
    ../../../Comun/SRC/tecladoui.cpp \
    ../../../Comun/SRC/globals.cpp \
    Principal.cpp \
    ../../../Comun/SRC/gsrenderengine.cpp \
    ../../../Comun/MemoryMap/MemoryMap.cpp \
    ../../../Comun/SRC/in2message.cpp \
    ../../../Comun/SRC/in2imagecreator.cpp \
    ../../../Comun/SRC/inputdata.cpp \
    ../../../Comun/SRC/in2compofile.cpp \
    ../../../Comun/barcode/In2BarcodeClass.cpp \
    ../../../Comun/SRC/viewini.cpp \
    ../../../Comun/barcode/qzint.cpp \
    ../../../Comun/SRC/qlabelext.cpp \
    ../../../Comun/SRC/colaimpresion.cpp \    
    ../../../Comun/CMYK/cmykconfig.cpp \
    ../../../Comun/CMYK/in2cmykgenerator.cpp \
    ../../../Comun/Configurators/in2configuratorparams.cpp \
    ../../../Comun/Configurators/in2configuratorprinter.cpp \
    ../../../Comun/Configurators/in2configuratorapp.cpp \
    ../../../Comun/BBDD/in2log.cpp \
    ../../../Comun/BBDD/GestorDB_ODBC.cpp \
    ../../../Comun/BBDD/GestorDB_SQLITE.cpp \
    ../../../Comun/BBDD/In2GestorDB.cpp \
    ../../../Comun/SRC/in2previsualizator.cpp \
    ../../../Comun/ImageTransformer/in2sabre.cpp \
    ../../../Comun/BBDD/in2store.cpp \
    ../../../Comun/ERP/in2gestorerp.cpp \
    ../../../Comun/ERP/in2gestorerpdaemon.cpp \
    ../../../Comun/ERP/in2managererp.cpp \
    ../../../Comun/SRC/in2colaimpresion.cpp \
    ../../../Comun/SRC/in2colaimpresionmono.cpp \
    ../../../Comun/Configurators/in2configuratorerp.cpp \
    ../../../Comun/Configurators/in2reportconfigurator.cpp \
    ../../../Comun/Vision/in2filtrovision.cpp \
    ../../../Comun/Vision/in2gestorvision.cpp \
    ../../../Comun/Configurators/in2aisconfigurator.cpp \
    ../../../Comun/Configurators/in2configuratorvision.cpp \
    ../../../Comun/Designer/composicion.cpp \
    ../../../Comun/Designer/corteimagen.cpp \
    ../../../Comun/Designer/counterStyle.cpp \
    ../../../Comun/Designer/EditCampos.cpp \
    ../../../Comun/Designer/EditComposicion.cpp \
    ../../../Comun/Designer/EditImagenFija.cpp \
    ../../../Comun/Designer/formatbarcode.cpp \
    ../../../Comun/Designer/formatcounter.cpp \
    ../../../Comun/Designer/formatdatetime.cpp \
    ../../../Comun/Designer/formatrandom.cpp \
    ../../../Comun/Designer/in2counters.cpp \
    ../../../Comun/Designer/in2editcampos.cpp \
    ../../../Comun/Designer/in2fieldpixmapitem.cpp \
    ../../../Comun/Designer/in2filedialog.cpp \
    ../../../Comun/Designer/in2font.cpp \
    ../../../Comun/Designer/in2ghostscriptconfig.cpp \
    ../../../Comun/Designer/in2graphicspixmapitem.cpp \
    ../../../Comun/Designer/in2graphicsscene.cpp \
    ../../../Comun/Designer/in2graphicsview.cpp \
    ../../../Comun/Designer/in2hash.cpp \
    ../../../Comun/Designer/in2inkcalclauncher.cpp \
    ../../../Comun/Designer/in2lineedit.cpp \
    ../../../Comun/Designer/in2machineinfo.cpp \
    ../../../Comun/Designer/in2pixmapcomposer.cpp \
    ../../../Comun/Designer/in2pixmapcomposeritem.cpp \
    ../../../Comun/Designer/in2qlabel.cpp \
    ../../../Comun/Designer/in2random.cpp \
    ../../../Comun/Designer/in2sceneselection.cpp \
    ../../../Comun/Designer/in2textedit.cpp \
    ../../../Comun/ERP/In2/in2erp.cpp \
    ../../../Comun/ERP/Esteve/in2esteveend.cpp \
    ../../../Comun/ERP/Esteve/in2esteveerp.cpp \
    ../../../Comun/ERP/Esteve/in2estevepending.cpp \
    ../../../Comun/Vision/AIS/in2aisstringgen.cpp \
    ../../../Comun/Vision/AIS/in2aisvisionobject.cpp \
    ../../../Comun/Vision/AIS/in2aisvisionselector.cpp \
    ../../../Comun/Vision/AIS/in2gestorais.cpp \
    ../../../Comun/Report/in2managerreport.cpp \
    ../../../Comun/Report/Esteve/in2Estevereportgenerator.cpp \
    ../../../Comun/Report/In2/in2reportgenerator.cpp \
    ../../../Comun/MagicPacket/in2magicpacket.cpp \
    ../../../Comun/Debug/in2debugcleaner.cpp \
    ../../../Comun/Debug/inkjetdebug.cpp \
    ../../../Comun/SRC/compileversion.cpp

INCLUDEPATH += ../../../Comun/SRC \
               ../../../Comun/MemoryMap \
               ../../../Comun/xusb/windows \
               ../../../Comun/barcode \
               ../../../Comun/cmyk \
               ../../../Comun/Configurators \
               ../../../Comun/BBDD \
               ../../../Comun/ERP \
               ../../../Comun/ERP/In2 \
               ../../../Comun/ERP/Esteve \
               ../../../Comun/ImageTransformer \
               ../../../Comun/Vision \
               ../../../Comun/Report \
               ../../../Comun/MagicPacket \
               ../../../Comun/Report/Esteve \
               ../../../Comun/Report/In2 \
               ../../../Comun/Vision/AIS \
               ../../../Comun/Designer \
               ../../../Comun/Debug \
               ../../../Comun/WenglorCP70QXVT80 \
               ../../../Comun/puertoserie/windows/inc \
               ../../../Comun/hydra/windows


HEADERS  += \
    ../../../Comun/SRC/teclado.h \
    ../../../Comun/SRC/tecladoui.h \
    ../../../Comun/SRC/globals.h \
    Principal.h \
    ../../../Comun/SRC/gsrenderengine.h \
    ../../../Comun/MemoryMap/MemoryMap.h \
    ../../../Comun/SRC/in2message.h \
    ../../../Comun/SRC/in2imagecreator.h \
    ../../../Comun/SRC/inputdata.h \
    ../../../Comun/SRC/in2compofile.h \
    ../../../Comun/barcode/In2BarcodeClass.h \
    ../../../Comun/SRC/viewini.h \
    ../../../Comun/barcode/zint.h \
    ../../../Comun/barcode/qzint.h \
    ../../../Comun/SRC/qlabelext.h \
    ../../../Comun/SRC/in2defines.h \
    ../../../Comun/SRC/colaimpresion.h \
    ../../../Comun/CMYK/cmykconfig.h \
    ../../../Comun/CMYK/in2cmykgenerator.h \
    ../../../Comun/Configurators/in2configuratorparams.h \
    ../../../Comun/Configurators/in2configuratorprinter.h \
    ../../../Comun/Configurators/in2configuratorapp.h \
    ../../../Comun/BBDD/in2log.h \
    ../../../Comun/BBDD/GestorDB_ODBC.h \
    ../../../Comun/BBDD/GestorDB_SQLITE.h \
    ../../../Comun/BBDD/In2GestorDB.h \
    ../../../Comun/SRC/in2previsualizator.h \
    ../../../Comun/ImageTransformer/in2sabre.h \
    ../../../Comun/BBDD/in2store.h \
    ../../../Comun/ERP/in2gestorerp.h \
    ../../../Comun/ERP/in2gestorerpdaemon.h \
    ../../../Comun/ERP/in2managererp.h \
    ../../../Comun/SRC/in2colaimpresion.h \
    ../../../Comun/SRC/in2colaimpresionmono.h \
    ../../../Comun/Configurators/in2configuratorerp.h \
    ../../../Comun/Configurators/in2reportconfigurator.h \
    ../../../Comun/Vision/in2filtrovision.h \
    ../../../Comun/Vision/in2gestorvision.h \
    ../../../Comun/Configurators/in2aisconfigurator.h \
    ../../../Comun/Configurators/in2configuratorvision.h \
    ../../../Comun/Designer/Composicion.h \
    ../../../Comun/Designer/corteimagen.h \
    ../../../Comun/Designer/counterStyle.h \
    ../../../Comun/Designer/EditCampos.h \
    ../../../Comun/Designer/EditComposicion.h \
    ../../../Comun/Designer/EditImagenFija.h \
    ../../../Comun/Designer/formatBarcode.h \
    ../../../Comun/Designer/formatcounter.h \
    ../../../Comun/Designer/formatdatetime.h \
    ../../../Comun/Designer/formatrandom.h \
    ../../../Comun/Designer/in2counters.h \
    ../../../Comun/Designer/in2editcampos.h \
    ../../../Comun/Designer/in2fieldpixmapitem.h \
    ../../../Comun/Designer/in2filedialog.h \
    ../../../Comun/Designer/in2font.h \
    ../../../Comun/Designer/in2ghostscriptconfig.h \
    ../../../Comun/Designer/in2graphicspixmapitem.h \
    ../../../Comun/Designer/in2graphicsscene.h \
    ../../../Comun/Designer/in2graphicsview.h \
    ../../../Comun/Designer/in2hash.h \
    ../../../Comun/Designer/in2inkcalclauncher.h \
    ../../../Comun/Designer/in2lineedit.h \
    ../../../Comun/Designer/in2machineinfo.h \
    ../../../Comun/Designer/in2pixmapcomposer.h \
    ../../../Comun/Designer/in2pixmapcomposeritem.h \
    ../../../Comun/Designer/in2qlabel.h \
    ../../../Comun/Designer/in2random.h \
    ../../../Comun/Designer/in2sceneselection.h \
    ../../../Comun/Designer/in2textedit.h \
    ../../../Comun/ERP/In2/in2erp.h \
    ../../../Comun/ERP/Esteve/in2esteveend.h \
    ../../../Comun/ERP/Esteve/in2esteveerp.h \
    ../../../Comun/ERP/Esteve/In2EsteveFramework.h \
    ../../../Comun/ERP/Esteve/in2estevepending.h \
    ../../../Comun/Vision/AIS/in2aisstringgen.h \
    ../../../Comun/Vision/AIS/in2aisvisionobject.h \
    ../../../Comun/Vision/AIS/in2aisvisionselector.h \
    ../../../Comun/Vision/AIS/in2gestorais.h \
    ../../../Comun/Report/in2managerreport.h \
    ../../../Comun/Report/Esteve/in2Estevereportgenerator.h \
    ../../../Comun/Report/In2/in2reportgenerator.h \
    ../../../Comun/MagicPacket/in2magicpacket.h \
    ../../../Comun/Debug/in2debugcleaner.h \
    ../../../Comun/Debug/inkjetdebug.h \
    ../../../Comun/SRC/compileversion.h \
    in2framework.h

FORMS    += mainwindow.ui \
    ../../../Comun/Pantallas/ComposicionUI.ui \
    ../../../Comun/Pantallas/EditValoresUI.ui \
    ../../../Comun/Pantallas/EditRandomUI.ui \
    ../../../Comun/Pantallas/EditImagenFijaUI.ui \
    ../../../Comun/Pantallas/EditDateTimeUI.ui \
    ../../../Comun/Pantallas/EditCounterUI.ui \
    ../../../Comun/Pantallas/EditBarcodeUI.ui \
    ../../../Comun/Pantallas/corteimagen.ui \
    ../../../Comun/Pantallas/tecladoui.ui \
    ../../../Comun/Pantallas/InputDataUI.ui \
    ../../../Comun/PantallasV2/In2MessageUI.ui \
    ../../../Comun/Pantallas/ViewIniUI.ui \
    ../../../Comun/Pantallas/InkjetDebugUI.ui \
    ../../../Comun/Pantallas/ColaImpresionUI.ui \
    ../../../Comun/PantallasV2/in2renderparams.ui \
    ../../../Comun/PantallasV2/In2ConfigPrint.ui \
    ../../../Comun/PantallasV2/in2ghostscriptconfig.ui \
    ../../../Comun/PantallasV2/in2configappui.ui \
    ../../../Comun/PantallasV2/in2visualizator.ui \
    ../../../Comun/PantallasV2/In2EditCamposUI.ui \
    ../../../Comun/Pantallas/EditCamposUI.ui \
    ../../../Comun/PantallasV2/in2colaimpresion.ui \
    ../../../Comun/PantallasV2/In2ColaImpresionMono.ui \
    ../../../Comun/PantallasV2/in2aisvisionselection.ui \
    ../../../Comun/PantallasV2/in2machineinfo.ui \
    ../../../Comun/PantallasV2/In2SGLClose.ui \
    ../../../Comun/Pantallas/compileversion.ui

RESOURCES += \
    shared.qrc

DISTFILES += \
    Recursos.rc

RC_FILE = Recursos.rc

