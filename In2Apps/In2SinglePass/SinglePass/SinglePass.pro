
TEMPLATE = app
TARGET = SinglePass
QT += network sql xml webkit
QT-=CONSOLE
CONFIG += qaxcontainer
DEFINES += QT_XML_LIB QT_SQL_LIB QT_NETWORK_LIB
TRANSLATIONS += Languajes/Spanish.ts

INCLUDEPATH += $$_PRO_FILE_PWD_ \
        ./GeneratedFiles \
    ./SRC \
    ../../../Comun/qwt/windows/include \
    ../../../Comun/hydra/windows \
    ../../../Comun/xusb/windows \
    ../../../Comun/barcode \
    ../../../Comun/SRC \
    ../../../Comun/UDP \
    ../../../Comun/Logon \
    ../../../Comun/WenglorCP70QXVT80 \
    ../../../Comun/puertoserie/windows/inc \
    ../../../Comun/MultiThread \
    ../../../Comun/Designer \
    ../../../Comun/SMCChiller \
    ../../../Comun/Modbus \
    ../../../Comun/CMYK \
    ../../../Comun/Configurators \
    ../../../Comun/BBDD \
    ../../../Comun/Report \
    ../../../Comun/Report/In2 \
    ../../../Comun/Report/Esteve \
    ../../../Comun/Report/Cinfa \
    ../../../Comun/SAI \
    ../../../Comun/OPC \
    ../../../Comun/ImageTransformer \
    ../../../Comun/MagicPacket \
    ../../../Comun/ERP \
    ../../../Comun/ERP/Esteve \
    ../../../Comun/ERP/In2 \
    ../../../Comun/ERP/Gramar \
    ../../../Comun/Vision \
    ../../../Comun/Vision/AIS \
    ../../../Comun/Debug \
    ../../../Comun/Cripto \    
    ../../../Comun/PharmaLicense \
    ../../../Comun/MemoryMap


DEPENDPATH += $$_PRO_FILE_PWD_
#MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
RC_FILE = Recursos.rc
RESOURCES += shared.qrc \
    shared.qrc
#    ../../../Comun/In2Apps/In2SinglePass/SinglePass/shared.qrc


DEFINES+=PHARMA

DEFINES+= CON_TECLADOVIRTUAL

DEFINES+=  DEPURACION_IN2

DEFINES+=OPC

DEFINES-=NO_LOGON

DEFINES-=BALLUF

DEFINES+=HAPPA

DEFINES+=HONLE

DEFINES-=HIGH_SPEED

DEFINES-=TFT_1024x768

DEFINES-=MODULO_VARINFO

DEFINES-=MODULO_PLANTILLAS




 LIBS += -L../../../Comun/hydra/windows \
            -L../../../Comun/xusb/windows/ \
            -L../../../Comun/ftdi/windows \
            -L../../../Comun/libs/ \
            -L../../../Comun/barcode/libs \
            -L../../../Comun/qwt/windows/lib/ \
            -L../../../Comun/puertoserie/windows/lib/ \
#            -lAdvapi32 \
#            -lodbc32 \
#            -lodbccp32 \
#            -lwinmm \
#            -lwsock32 \
            -lXCL_Library \
            -lScorpionDLL \
            -lzint \
            -lqwt \
            -lwldap32\
            -lqextserialport1\
            -lqwtmathml


    CONFIG(debug,debug|release){
    LIBS+=  -lqwtd \
            -lqwtmathmld
    }

SOURCES += \
    ../../../Comun/UDP/ethmessage.cpp \
    ../../../Comun/MemoryMap/MemoryMap.cpp \
    ../../../Comun/SRC/usuarios.cpp \
    ../../../Comun/SRC/teclado.cpp \
    ../../../Comun/SRC/status.cpp \
    ../../../Comun/SRC/smtp.cpp \
    ../../../Comun/SRC/printparameters.cpp \
    ../../../Comun/SRC/maquinaAdv.cpp \
    ../../../Comun/SRC/mantenimiento.cpp \
    ../../../Comun/SRC/interfaz.cpp \
    ../../../Comun/SRC/inputdata.cpp \
    ../../../Comun/SRC/inkjetxusbctrl.cpp \
    ../../../Comun/SRC/inkjetwidgets.cpp \
    ../../../Comun/SRC/inkjetmultiview.cpp \
    ../../../Comun/SRC/inkjetcommon.cpp \
    ../../../Comun/SRC/globals.cpp \
    ../../../Comun/SRC/gestorethernet.cpp \
    ../../../Comun/SRC/gestorcontrol.cpp \
    ../../../Comun/SRC/eventos.cpp \
    ../../../Comun/SRC/confusers.cpp \
    ../../../Comun/SRC/configuracion.cpp \
    ../../../Comun/SRC/BotonFondo.cpp \
    ../../../Comun/barcode/qzint.cpp \
    ../../../Comun/barcode/In2Barcode.cpp \
    ../../../Comun/barcode/In2BarcodeClass.cpp \
    ./SRC/principal.cpp \
    ../../../Comun/SRC/curvas.cpp \
    ./SRC/maquina.cpp \
    ../../../Comun/SRC/compileversion.cpp \
    ../../../Comun/SRC/qlabelext.cpp \
    #../../../Comun/SRC/in2posmarker.cpp \
    ../../../Comun/Logon/logger.cpp \
    ../../../Comun/Logon/in2logon.cpp \
    ../../../Comun/hydra/windows/in2hydracontrol.cpp \
    ../../../Comun/SRC/in2gestorfluidica.cpp \
    ../../../Comun/SRC/viewini.cpp \
    ../../../Comun/SRC/inkjetinformer.cpp \
    ../../../Comun/MultiThread/imagevarxaarsender.cpp \
    ../../../Comun/MultiThread/imagevarxaarcreator.cpp \
    ../../../Comun/MultiThread/inkjetxusbvarctrl.cpp \
    ../../../Comun/SRC/in2bufferimages.cpp \
    ../../../Comun/SRC/gsrenderengine.cpp  \
    ../../../Comun/SRC/in2imagecreator.cpp \
    ../../../Comun/SRC/inkjetcommonext.cpp  \
    ./SRC/gestormaquina.cpp \
    ../../../Comun/SRC/Fluidica.cpp \
    ../../../Comun/SRC/in2message.cpp \
    ../../../Comun/SRC/in2historico.cpp \
    ./SRC/in2gestormaquina.cpp \
    ../../../Comun/SRC/gestorimpresion.cpp \
    ../../../Comun/WenglorCP70QXVT80/wenglormessage.cpp \
    ../../../Comun/WenglorCP70QXVT80/monitordiametro.cpp \
    ../../../Comun/WenglorCP70QXVT80/in2wenglorcontrol.cpp \
    ../../../Comun/WenglorCP70QXVT80/in2gestordiametro.cpp \
    ../../../Comun/WenglorCP70QXVT80/gestorserie.cpp \
    ../../../Comun/SRC/in2xmlparser.cpp \
    ../../../Comun/SRC/in2compofile.cpp \
    ../../../Comun/SRC/imgvargenerator.cpp \
    ../../../Comun/SMCChiller/in2smccontrol.cpp \
    ../../../Comun/SMCChiller/in2gestorchiller.cpp \
    ../../../Comun/Modbus/modbusmessage.cpp \
    ../../../Comun/Modbus/in2modbusmsg.cpp \
    ../../../Comun/Modbus/gestormodbus.cpp \
    ../../../Comun/SRC/in2char.cpp \
    ../../../Comun/puertoserie/windows/puertoserie.cpp \
    ../../../Comun/SRC/chiller.cpp \
    ./SRC/main.cpp \
    ../../../Comun/SRC/tecladoui.cpp \
    ../../../Comun/SRC/in2translator.cpp \
    ../../../Comun/CMYK/in2cmykgenerator.cpp \
    ../../../Comun/CMYK/cmykconfig.cpp \
    ../../../Comun/Configurators/in2configuratorapp.cpp \
    ../../../Comun/Configurators/in2configuratorprinter.cpp \
    ../../../Comun/Configurators/in2configuratorparams.cpp \
    ../../../Comun/BBDD/in2store.cpp \
    ../../../Comun/BBDD/In2GestorDB.cpp \
    ../../../Comun/BBDD/in2log.cpp \
    ../../../Comun/SRC/in2gestormantenimiento.cpp \
    ../../../Comun/SRC/in2previsualizator.cpp \
    ../../../Comun/ERP/in2gestorerp.cpp \
    ../../../Comun/Configurators/in2configuratorerp.cpp \
    ../../../Comun/ImageTransformer/in2sabre.cpp \
    ../../../Comun/Configurators/in2reportconfigurator.cpp \
    ../../../Comun/SRC/in2colaimpresion.cpp \
    ../../../Comun/ERP/in2gestorerpdaemon.cpp \
    ../../../Comun/ERP/in2managererp.cpp \
    ../../../Comun/Configurators/in2aisconfigurator.cpp \
    ../../../Comun/WenglorCP70QXVT80/in2managerdiametros.cpp \
    ../../../Comun/Configurators/in2configuratorvision.cpp \
    ../../../Comun/Vision/in2filtrovision.cpp \
    ./SRC/in2application.cpp \
    ../../../Comun/SRC/in2eventfilter.cpp \
    ../../../Comun/SRC/in2scrsaver.cpp \
    ../../../Comun/ERP/Esteve/in2estevepending.cpp \
    ../../../Comun/ERP/Esteve/in2esteveerp.cpp \
    ../../../Comun/ERP/Esteve/in2esteveend.cpp \
    ../../../Comun/ERP/In2/in2erp.cpp \
    ../../../Comun/Vision/AIS/in2gestorais.cpp \
    ../../../Comun/Vision/AIS/in2aisvisionselector.cpp \
    ../../../Comun/Vision/AIS/in2aisvisionobject.cpp \
    ../../../Comun/Vision/AIS/in2aisstringgen.cpp \
    ../../../Comun/Vision/in2gestorvision.cpp \
    ../../../Comun/Designer/in2sceneselection.cpp \
    ../../../Comun/Designer/in2random.cpp \
    ../../../Comun/Designer/in2qlabel.cpp \
    ../../../Comun/Designer/in2machineinfo.cpp \
    ../../../Comun/Designer/in2lineedit.cpp \
    ../../../Comun/Designer/in2inkcalclauncher.cpp \
    ../../../Comun/Designer/in2hash.cpp \
    ../../../Comun/Designer/in2graphicsview.cpp \
    ../../../Comun/Designer/in2graphicsscene.cpp \
    ../../../Comun/Designer/in2graphicspixmapitem.cpp \
    ../../../Comun/Designer/in2ghostscriptconfig.cpp \
    ../../../Comun/Designer/in2font.cpp \
    ../../../Comun/Designer/in2fieldpixmapitem.cpp \
    ../../../Comun/Designer/in2editcampos.cpp \
    ../../../Comun/Designer/in2counters.cpp \
    ../../../Comun/Designer/in2configprint.cpp \
    ../../../Comun/Designer/formatrandom.cpp \
    ../../../Comun/Designer/formatdatetime.cpp \
    ../../../Comun/Designer/formatcounter.cpp \
    ../../../Comun/Designer/formatbarcode.cpp \
    ../../../Comun/Designer/EditImagenFija.cpp \
    ../../../Comun/Designer/EditComposicion.cpp \
    ../../../Comun/Designer/EditCampos.cpp \
    ../../../Comun/Designer/counterStyle.cpp \
    ../../../Comun/Designer/corteimagen.cpp \
    ../../../Comun/Designer/composicion.cpp \
    ../../../Comun/Designer/in2filedialog.cpp \
    ../../../Comun/Designer/in2textedit.cpp \
    ../../../Comun/SAI/in2saicontrol.cpp \
    ../../../Comun/Report/In2/in2reportgenerator.cpp \
    ../../../Comun/Report/in2managerreport.cpp \
    ../../../Comun/Report/Esteve/in2Estevereportgenerator.cpp \
    ../../../Comun/OPC/opcautomation.cpp \
    ../../../Comun/OPC/clienteopc.cpp \
    ../../../Comun/OPC/cemitecproxyopc.cpp \
    ../../../Comun/MagicPacket/in2magicpacket.cpp \
    ../../../Comun/SRC/in2windowsdetector.cpp \
    ../../../Comun/Debug/inkjetdebug.cpp \
    ../../../Comun/Debug/in2debugcleaner.cpp \
    ../../../Comun/ERP/Esteve/in2colaimpresionmono.cpp \
    ../../../Comun/OPC/in2manageropc.cpp \
    ../../../Comun/ERP/Gramar/in2gramarerp.cpp \
    ../../../Comun/UDP/in2ping.cpp \
    ../../../Comun/ERP/Gramar/in2gramarcolaimpresion.cpp \
    ../../../Comun/OPC/in2opcserver.cpp \
    ../../../Comun/Configurators/in2configuratordesigner.cpp \
    ../../../Comun/Designer/in2selectpalette.cpp \
    ../../../Comun/CMYK/in2floydsteinberg.cpp \
    ../../../Comun/CMYK/in2whitecorrectionimage.cpp \
    ../../../Comun/CMYK/in2mediancut.cpp \
    ../../../Comun/CMYK/in2ditheralg.cpp \
    SRC/in2gestorlampara.cpp \
    ../../../Comun/Configurators/in2configuratorlampara.cpp \
    ../../../Comun/Configurators/in2configuratordiameter.cpp \
    ../../../Comun/SRC/in2configlampara.cpp \   
    ../../../Comun/Cripto/in2crypt.cpp \
    ../../../Comun/PharmaLicense/in2licenser.cpp \
    SRC/in2gestorcorte.cpp \
    ../../../Comun/Configurators/in2configuratorcorte.cpp \
    ../../../Comun/Configurators/in2configuratorpermisos.cpp \
    ../../../Comun/SRC/in2configcorte.cpp \
    ../../../Comun/SRC/in2editorpermisos.cpp \
    ../../../Comun/SRC/in2gsmonitor.cpp




FORMS += \
 #   ../../../Comun/Pantallas/WebCamUI.ui \
    ../../../Comun/Pantallas/UsuariosUI.ui \
    ../../../Comun/Pantallas/tecladoui.ui \
    ../../../Comun/Pantallas/Statusui.ui \
    ../../../Comun/Pantallas/status.ui \
    ../../../Comun/Pantallas/mantenimientoUI.ui \
    ../../../Comun/Pantallas/interfaz.ui \
    ../../../Comun/Pantallas/InputDataUI.ui \
    ../../../Comun/Pantallas/InkjetDebugUI.ui \
    ../../../Comun/Pantallas/GestionEventosUI.ui \
    ../../../Comun/Pantallas/EditValoresUI.ui \
    ../../../Comun/Pantallas/EditCamposUI.ui \
    ../../../Comun/Pantallas/ConfUsersUI.ui \
    ../../../Comun/Pantallas/ConfiguracionUI.ui \
    ../../../Comun/Pantallas/ComposicionUI.ui \
    ../../../Comun/Pantallas/ColaImpresionUI.ui \
    ../../../Comun/Pantallas/curvasUI.ui \
    ../../../Comun/Pantallas/compileversion.ui \
    ../../../Comun/Pantallas/corteimagen.ui \
    ../../../Comun/Pantallas/EditBarcodeUI.ui \
    ../../../Comun/Pantallas/EditCounterUI.ui  \
    ../../../Comun/Pantallas/EditDateTimeUI.ui \
    ../../../Comun/Pantallas/EditRandomUI.ui \
    ../../../Comun/Pantallas/ViewIniUI.ui \
    ../../../Comun/Pantallas/helpUI.ui \
    ../../../Comun/PantallasV2/PrincipalSPassUI.ui \
    ../../../Comun/PantallasV2/FluidicaUI.ui \
    ../../../Comun/PantallasV2/HistoricoUI.ui \
    ../../../Comun/PantallasV2/In2MessageUI.ui \
    ../../../Comun/PantallasV2/in2machineinfo.ui \
    ../../../Comun/PantallasV2/MaquinaAdvUI.ui \
    ../../../Comun/Pantallas/EditImagenFijaUI.ui \
    ../../../Comun/PantallasV2/MaquinaSPassUI.ui \
    ../../../Comun/PantallasV2/Chiller.ui \
    ../../../Comun/PantallasV2/DriverError.ui \
    ../../../Comun/PantallasV2/In2ConfigPrint.ui \
    ../../../Comun/PantallasV2/in2ghostscriptconfig.ui \
    ../../../Comun/PantallasV2/in2configappui.ui \
    ../../../Comun/PantallasV2/in2visualizator.ui \
    ../../../Comun/PantallasV2/in2colaimpresion.ui \
    ../../../Comun/PantallasV2/in2aisvisionselection.ui \
    ../../../Comun/PantallasV2/Esteve/In2SGLClose.ui \
    ../../../Comun/PantallasV2/Esteve/In2ColaImpresionMono.ui \
    ../../../Comun/PantallasV2/Gramar/In2GramarColaImpresion.ui \
    ../../../Comun/PantallasV2/in2selectpalette.ui \
    ../../../Comun/PantallasV2/in2ditheralgui.ui \
    ../../../Comun/PantallasV2/in2configcorte.ui \
    ../../../Comun/PantallasV2/in2configlampara.ui \
    ../../../Comun/PantallasV2/in2permisosui.ui

    #../../../Comun/Pantallas/MaquinaUI.ui




HEADERS += \
    ../../../Comun/UDP/In2UdpFramework.h \
    ../../../Comun/UDP/ethmessage.h \
    ../../../Comun/MemoryMap/MemoryMap.h \
    ../../../Comun/SRC/usuarios.h \
    ../../../Comun/SRC/teclado.h \
    ../../../Comun/SRC/status.h \
    ../../../Comun/SRC/smtp.h \
    ../../../Comun/SRC/Scorpionapi.h \
    ../../../Comun/SRC/printparameters.h \
    ../../../Comun/SRC/Messages.h \
    ../../../Comun/SRC/maquinaAdv.h \
    ../../../Comun/SRC/mantenimiento.h \
    ../../../Comun/SRC/interfaz.h \
    ../../../Comun/SRC/inputdata.h \
    ../../../Comun/SRC/inkjetxusbctrl.h \
    ../../../Comun/SRC/inkjetwidgets.h \
    ../../../Comun/SRC/inkjetstyle.h \
    ../../../Comun/SRC/inkjetmultiview.h \
    ../../../Comun/SRC/inkjetcommon.h \
    ../../../Comun/SRC/imgvargenerator.h \
    ../../../Comun/SRC/globals.h \
    ../../../Comun/SRC/gestorethernet.h \
    ../../../Comun/SRC/gestorcontrol.h \    
    ../../../Comun/SRC/eventos.h \
    ../../../Comun/SRC/defines.h \
    ../../../Comun/SRC/confusers.h \
    ../../../Comun/SRC/configuracion.h \
    ../../../Comun/SRC/BotonFondo.h \
    ../../../Comun/barcode/zint.h \
    ../../../Comun/barcode/qzint.h \
    ../../../Comun/barcode/In2Barcode.h \
    ../../../Comun/barcode/In2BarcodeClass.h \
    ./SRC/principal.h \
    ./SRC/maquina.h \
    ../../../Comun/SRC/compileversion.h \
    ../../../Comun/SRC/qlabelext.h \
    ../../../Comun/SRC/curvas.h \
    ../../../Comun/SRC/in2posmarker.h \
    ../../../Comun/Logon/logger.h \
    ../../../Comun/Logon/in2logon.h \
    ../../../Comun/SRC/inkjetinformer.h \
    ./SRC/In2Framework.h \
    ../../../Comun/hydra/windows/in2hydracontrol.h \
    ../../../Comun/SRC/in2gestorfluidica.h \
    ../../../Comun/SRC/viewini.h \
    ./SRC/In2Framework.h \
    ../../../Comun/MultiThread/imagevarxaarsender.h \
    ../../../Comun/MultiThread/imagevarxaarcreator.h \
    ../../../Comun/MultiThread/inkjetxusbvarctrl.h \
    ../../../Comun/SRC/in2bufferimages.h \
    ../../../Comun/SRC/gsrenderengine.h \
    ../../../Comun/SRC/in2imagecreator.h \
    ../../../Comun/SRC/inkjetcommonext.h \
    ./SRC/gestormaquina.h \
    ../../../Comun/SRC/Fluidica.h \
    ../../../Comun/SRC/in2message.h \
    ../../../Comun/SRC/in2historico.h \
    ./SRC/in2gestormaquina.h \
    ../../../Comun/SRC/gestorimpresion.h \
    ../../../Comun/WenglorCP70QXVT80/wenglormessage.h \
    ../../../Comun/WenglorCP70QXVT80/monitordiametro.h \
    ../../../Comun/WenglorCP70QXVT80/in2wenglorcontrol.h \
    ../../../Comun/WenglorCP70QXVT80/in2gestordiametro.h \
    ../../../Comun/WenglorCP70QXVT80/gestorserie.h \
    ../../../Comun/SRC/in2xmlparser.h \
    ../../../Comun/SRC/in2compofile.h \
    ../../../Comun/SMCChiller/in2smccontrol.h \
    ../../../Comun/SMCChiller/in2gestorchiller.h \
    ../../../Comun/Modbus/modbusmessage.h \
    ../../../Comun/Modbus/in2modbusmsg.h \
    ../../../Comun/Modbus/gestormodbus.h \
    ../../../Comun/SRC/in2char.h \
    ../../../Comun/puertoserie/windows/puertoserie.h \
    ../../../Comun/SRC/chiller.h \
    ../../../Comun/SRC/tecladoui.h \
    ../../../Comun/SRC/in2translator.h \
    ../../../Comun/CMYK/in2cmykgenerator.h \
    ../../../Comun/CMYK/cmykconfig.h \
    ../../../Comun/Configurators/in2configuratorapp.h \
    ../../../Comun/Configurators/In2ConfiguratorPrinter.h \
    ../../../Comun/Configurators/in2configuratorparams.h \
    ../../../Comun/BBDD/in2store.h \
    ../../../Comun/BBDD/In2GestorDB.h \
    ../../../Comun/BBDD/in2log.h \
    ../../../Comun/SRC/in2gestormantenimiento.h \
    ../../../Comun/SRC/in2previsualizator.h \
    ../../../Comun/ERP/in2gestorerp.h \
    ../../../Comun/Configurators/in2configuratorerp.h \
    ../../../Comun/SRC/in2defines.h \
    ../../../Comun/ImageTransformer/in2sabre.h \
    ../../../Comun/Configurators/in2reportconfigurator.h \
    ../../../Comun/SRC/in2colaimpresion.h \
    ../../../Comun/ERP/in2gestorerpdaemon.h \
    ../../../Comun/ERP/in2managererp.h \
    ../../../Comun/Configurators/in2aisconfigurator.h \
    ../../../Comun/WenglorCP70QXVT80/in2managerdiametros.h \
    ../../../Comun/Configurators/in2configuratorvision.h \
    ../../../Comun/Vision/in2filtrovision.h \
    ./SRC/in2application.h \
    ../../../Comun/SRC/in2eventfilter.h \
    ../../../Comun/SRC/in2scrsaver.h \
    ../../../Comun/Vision/in2gestorvision.h \
    ../../../Comun/ERP/Esteve/in2estevepending.h \
    ../../../Comun/ERP/Esteve/In2EsteveFramework.h \
    ../../../Comun/ERP/Esteve/in2esteveerp.h \
    ../../../Comun/ERP/Esteve/in2esteveend.h \
    ../../../Comun/ERP/In2/in2erp.h \
    ../../../Comun/Vision/AIS/in2gestorais.h \
    ../../../Comun/Vision/AIS/in2aisvisionselector.h \
    ../../../Comun/Vision/AIS/in2aisvisionobject.h \
    ../../../Comun/Vision/AIS/in2aisstringgen.h \
    ../../../Comun/Vision/in2gestorvision.h \
    ../../../Comun/Designer/in2sceneselection.h \
    ../../../Comun/Designer/in2random.h \
    ../../../Comun/Designer/in2qlabel.h \
    ../../../Comun/Designer/in2machineinfo.h \
    ../../../Comun/Designer/in2lineedit.h \
    ../../../Comun/Designer/in2inkcalclauncher.h \
    ../../../Comun/Designer/in2hash.h \
    ../../../Comun/Designer/in2graphicsview.h \
    ../../../Comun/Designer/in2graphicsscene.h \
    ../../../Comun/Designer/in2graphicspixmapitem.h \
    ../../../Comun/Designer/in2ghostscriptconfig.h \
    ../../../Comun/Designer/in2font.h \
    ../../../Comun/Designer/in2fieldpixmapitem.h \
    ../../../Comun/Designer/in2editcampos.h \
    ../../../Comun/Designer/in2counters.h \
    ../../../Comun/Designer/in2configprint.h \
    ../../../Comun/Designer/formatrandom.h \
    ../../../Comun/Designer/formatdatetime.h \
    ../../../Comun/Designer/formatcounter.h \
    ../../../Comun/Designer/formatBarcode.h \
    ../../../Comun/Designer/EditImagenFija.h \
    ../../../Comun/Designer/EditComposicion.h \
    ../../../Comun/Designer/EditCampos.h \
    ../../../Comun/Designer/counterStyle.h \
    ../../../Comun/Designer/corteimagen.h \
    ../../../Comun/Designer/Composicion.h \
    ../../../Comun/Designer/in2filedialog.h \
    ../../../Comun/Designer/in2textedit.h \
    ../../../Comun/SAI/in2saicontrol.h \
    ../../../Comun/Report/In2/in2reportgenerator.h \
    ../../../Comun/Report/in2managerreport.h \
    ../../../Comun/Report/Esteve/in2Estevereportgenerator.h \
    ../../../Comun/OPC/opcautomation.h \
    ../../../Comun/OPC/gbdaautomation.h \
    ../../../Comun/OPC/clienteopc.h \
    ../../../Comun/OPC/cemitecproxyopc.h \
    ../../../Comun/MagicPacket/in2magicpacket.h \
    ../../../Comun/SRC/in2windowsdetector.h \
    ../../../Comun/Debug/inkjetdebug.h \
    ../../../Comun/Debug/in2debugcleaner.h \
    ../../../Comun/ERP/Esteve/in2colaimpresionmono.h \
    ../../../Comun/OPC/in2manageropc.h \
    ../../../Comun/ERP/Gramar/in2gramarerp.h \
    ../../../Comun/UDP/in2ping.h \
    ../../../Comun/ERP/Gramar/in2gramarcolaimpresion.h \
    ../../../Comun/OPC/in2opcserver.h \
    ../../../Comun/Configurators/in2configuratordesigner.h \
    ../../../Comun/Designer/in2selectpalette.h \
    ../../../Comun/CMYK/in2floydsteinberg.h \
    ../../../Comun/CMYK/in2whitecorrectionimage.h \
    ../../../Comun/CMYK/in2mediancut.h \
    ../../../Comun/CMYK/in2ditheralg.h \
    SRC/in2gestorlampara.h \
    ../../../Comun/Configurators/in2configuratorlampara.h \
    ../../../Comun/Configurators/in2configuratordiameter.h \
    ../../../Comun/SRC/in2configlampara.h \
    ../../../Comun/Cripto/in2crypt.h \
    ../../../Comun/PharmaLicense/in2licenser.h \
    SRC/in2gestorcorte.h \
    ../../../Comun/Configurators/in2configuratorcorte.h \
    ../../../Comun/Configurators/in2configuratorpermisos.h \
    ../../../Comun/SRC/in2configcorte.h \
    ../../../Comun/SRC/in2editorpermisos.h \
    ../../../Comun/SRC/in2gsmonitor.h







