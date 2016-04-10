#include "reportgenerator.h"
#include "globals.h"
#include <QFile>
//#include <QPrinter>
#include <QPainter>
#include <QSettings>
#include <QProcess>
#include <QDir>



#define DATA_HEIGHT 20
#define DEFAULT_NAME "Report"


#define DEFAULT_CONFIG_FILE "Config\\In2Report.ini"
//#define DEFAULT_PASSWORD "In2ps"
#define DEFAULT_COMMAND "pdf_sign.exe"
#define DEFAULT_PASSWORD ""
#define DEFAULT_SIGN_FILE "In2ps.pfx"
#define DEFAULT_LOGO_FILE "logo.png"
#define DEFAULT_OUTPATH "D:\\In2Output"

#define TXT_GROUP "CONFIG"
#define TXT_CERTIFICATE "Certificate"
#define TXT_PASSWORD "Password"
#define TXT_COMMAND "Command"
#define TXT_ACTIVE "Active"
#define TXT_LOGO "Logo"
#define TXT_OUTPATH "OutPath"

#define OUTPUT_FOLDER "PDF"
#define DEFAULT_REPORT_FONT_SIZE 12
#define LINE_HEIGHT DEFAULT_REPORT_FONT_SIZE+4
#define TITLE_FONT_SIZE 15


ReportGenerator::ReportGenerator(QObject *parent) :
    QObject(parent)
{
    LoadConfiguration();
    m_dateGeneration=QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    //m_Filename=QString("%1%2").arg(DEFAULT_NAME).arg(memoryMap->m_mapa.m_variables.NombreIni);
    m_Filename.remove(".ini");
    m_Filename.append(QString("%1.pdf").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
    m_report.append(QString("REPORT: %1\n").arg(m_Filename));
    m_report.append(QString("PRODUCTO: %1\n").arg(memoryMap->m_mapa.m_variables.NombreIni));
    m_report.append(QString("ORDEN FABRICACION: %1\n").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
    m_report.append(QString("RUTA: %1\n").arg(memoryMap->m_mapa.m_variables.RutaIni));
    //m_report.append(QString("BBDD original: %1/%2\n").arg(memoryMap->m_mapa.m_database.RutaDb).arg(memoryMap->m_mapa.m_database.NombreDb));
    //m_report.append(QString("Imagen: %1\n").arg(memoryMap->m_FilePath));
    //Cargamos la fecha y hora
    m_report.append(QString("Informe generado %1\n").arg(m_dateGeneration));
    m_report.append(QString("Usuario: %1\n").arg(memoryMap->getUser()));

    QString outPath=QString("%1\\%2").arg(m_Config.m_outputPath).arg(OUTPUT_FOLDER);
    QDir myDir(outPath);
    if (!myDir.exists()) myDir.mkpath(outPath);

    QString nameFile=QString("%1\\%2%3").arg(outPath).arg(DEFAULT_NAME).arg(memoryMap->m_mapa.m_variables.NombreIni);
    nameFile.remove(".ini");
    nameFile.append(QString("%1.pdf").arg(memoryMap->getOrdenAndProducto()));
    m_Config.m_inputPdfFile=nameFile;


    if (m_Config.m_outputCasualName.isEmpty()) {
        m_Config.m_outputPdfFile=QString("%1").arg(m_Config.m_inputPdfFile);
        m_Config.m_outputPdfFile.remove(".pdf");
        m_Config.m_outputPdfFile.append("Signed.pdf");
    }
    else{
        m_Config.m_outputPdfFile=m_Config.m_outputCasualName;
    }
    m_pages=1;

}

void ReportGenerator::AddSection(const QString &title, const QStringList &data){
    struct st_In2ReportSection section;
    section.title=title;
    section.arguments=data;
    this->m_sections.append(section);
}

/** Carga la configuracion*/

void ReportGenerator::LoadConfiguration(){
    QString strPath=QString("%1\\%2").arg(GetApplicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    QFile dummyFile(strPath);
    if (dummyFile.exists()){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(TXT_GROUP);
        m_Config.m_signCommand=FicheroIni.value(TXT_COMMAND,DEFAULT_COMMAND).toString();
        m_Config.m_signFile=FicheroIni.value(TXT_CERTIFICATE,DEFAULT_SIGN_FILE).toString();
        m_Config.m_signPassword=FicheroIni.value(TXT_PASSWORD,DEFAULT_PASSWORD).toString();
        m_Config.m_signActive=FicheroIni.value(TXT_ACTIVE,1).toInt();
        m_Config.m_Logo=FicheroIni.value(TXT_LOGO,DEFAULT_LOGO_FILE).toString();
        m_Config.m_outputPath=FicheroIni.value(TXT_OUTPATH,DEFAULT_OUTPATH).toString();
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,"ReportGenerator:No se encontró fichero de configuración");
        m_Config.m_signCommand=DEFAULT_COMMAND;
        m_Config.m_signFile=DEFAULT_SIGN_FILE;
        m_Config.m_signPassword=DEFAULT_PASSWORD;
        m_Config.m_Logo=DEFAULT_LOGO_FILE;
        m_Config.m_signActive=1;
        m_Config.m_outputPath=DEFAULT_OUTPATH;
    }
    m_Config.m_outputCasualName.clear();
}

/** Añade un string*/
void ReportGenerator::AddString(QString str){
    m_report.append(QString("%1\n").arg(str));
}

/** Devuelve el reporte*/
QString ReportGenerator::getReport(){
    return m_report;
}

/** Guarda en fichero*/
QString ReportGenerator::saveToFile(){
    QFile file(m_Filename);
    if (file.open(QIODevice::WriteOnly)){
        file.write((const char*)m_report.toAscii().data());
        file.close();
        GeneratePDF();
    }
    else m_Filename.clear();
    return m_Filename;
}
/** Genera el pdf*/
bool ReportGenerator::GeneratePDF(){
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    m_printer.setOutputFileName(m_Config.m_inputPdfFile);
    m_width = m_printer.width();
    QPainter painter; //The QPainter class performs painting on widgets and other paint devices (such as QPrinter)
    //here we start painting to the printer
    if (!painter.begin(&m_printer)){
        emit SendDebugInfo(LOGGER_WARNING, QString("ReportGenerator: No se pudo generar PDF"));
        return false;
    }

    QImage logo(m_Config.m_Logo);    
    QImage logoIn2(":res/images/in2ps_128x128.bmp");
    QImage logoIn2Print=logoIn2;
    QString title("PRINTING REPORT");
    QFont myFont;
    myFont.setPixelSize(TITLE_FONT_SIZE);
    myFont.setStyleStrategy(QFont::PreferAntialias);

    if (logo.width()){
        painter.drawImage(0,0,logo);
        logoIn2Print=logoIn2.scaledToHeight(logo.height(),Qt::SmoothTransformation);

    }
    painter.drawImage(m_width-logoIn2Print.width(),0,logoIn2Print);


    painter.drawLine(5,logoIn2Print.height()+5,m_width-5,logoIn2Print.height()+5);
    painter.setFont(myFont);
    QRect required = QRect(); //this represent the required rectangled size
    QRect r = QRect(logo.width(), 0, m_width-(logo.width()+logoIn2Print.width()), logo.height()); //this represent our calculated rectangle size
    painter.drawText(r, Qt::AlignCenter | Qt::TextWordWrap, title, &required);

    //Calcula el numero de hojas del documento
    CalculateNumPages();

    for (int i=0;i<m_sections.count();i++){
        PrintSection(&painter,i);
    }
    //Ultima pagina
    PrintNumPage(&painter);
    painter.end();

    /*QStringList DataList=m_report.split("\n");
    QRect required = QRect(); //this represent the required rectangled size
    QRect r = QRect(0, logo.height()+10, width, DataList.count()*DATA_HEIGHT); //this represent our calculated rectangle size
    QFont myfont;
    myfont.setPixelSize(DEFAULT_FONT_SIZE);
    myfont.setStyleStrategy(QFont::PreferAntialias);
    painter.setFont(myfont);
    painter.drawText(r, Qt::AlignLeft | Qt::TextWordWrap, m_report, &required);
    painter.end();*/
    if (m_Config.m_signActive) SignFile();
    return true;
}
/** Calcula el numero de hojas del report*/
void ReportGenerator::CalculateNumPages(){
    m_totalPages=1;
    int section=0;
    for (section=0;section<m_sections.count();section++){
        struct st_In2ReportSection localItem=this->m_sections.at(section);
        int y=getHeadHeight();

        for (int i=0;i<section;i++){
            y+=getSectionHeight(i);
        }
        y=y%m_printer.pageRect().height();

        //Si es la seccion de firmas, lo hacemnos en nueva hoja
        if (section==this->m_sections.count()-1){
            m_pages++;
            y=6*TITLE_FONT_SIZE;
        }

        y+=3*TITLE_FONT_SIZE;
        if (TestPage(y)) y=0;

        for (int i=0;i<localItem.arguments.count();i++){
            y+=LINE_HEIGHT+2;
            if (TestPage(y)) y=0;
        }
    }
    m_totalPages=m_pages;
    m_pages=1;
}


/** Imprime una seccion*/
void ReportGenerator::PrintSection(QPainter * paint,int section){
    struct st_In2ReportSection localItem=this->m_sections.at(section);
    int y=getHeadHeight();

    for (int i=0;i<section;i++){
        y+=getSectionHeight(i);
    }
    y=y%m_printer.pageRect().height();

    //Si es la seccion de firmas, lo hacemos en nueva hoja
    if (section==this->m_sections.count()-1){
        PrintNumPage(paint);
        m_pages++;
        m_printer.newPage();
        y=6*TITLE_FONT_SIZE;
    }
    //int newHeight=getSectionHeight(section);
    QFont myFont;
    myFont.setPixelSize(TITLE_FONT_SIZE);
    myFont.setStyleStrategy(QFont::PreferAntialias);
    paint->setFont(myFont);

    QRect required = QRect(); //this represent the required rectangled size
    QRect r = QRect(0, y,m_width , TITLE_FONT_SIZE); //this represent our calculated rectangle size
    QRect backRect(0, y-2,m_width , TITLE_FONT_SIZE+4); //this represent our calculated rectangle size
    paint->fillRect(backRect,Qt::lightGray);
    //if (m_printer.fullPage()) m_printer.newPage();
    paint->drawText(r, Qt::AlignLeft | Qt::TextWordWrap, localItem.title, &required);
    y+=3*TITLE_FONT_SIZE;
    if (TestPage(y,paint)) y=0;

    myFont.setPixelSize(DEFAULT_REPORT_FONT_SIZE);
    myFont.setStyleStrategy(QFont::PreferAntialias);
    paint->setFont(myFont);
    paint->setBackground(QBrush(Qt::white));
    for (int i=0;i<localItem.arguments.count();i++){
        QRect required = QRect(); //this represent the required rectangled size
        QRect r = QRect(0, y,m_width , LINE_HEIGHT); //this represent our calculated rectangle size
        //if (m_printer.fullPage()) m_printer.newPage();
        paint->drawText(r, Qt::AlignLeft | Qt::TextWordWrap, localItem.arguments.at(i), &required);
        y+=LINE_HEIGHT+2;
        if (TestPage(y,paint)) y=0;
    }
}
/** Pone el numero de pagina*/
void ReportGenerator::PrintNumPage(QPainter * paint){
    QRect required = QRect(); //this represent the required rectangled size
    QRect r = QRect(m_width-50, m_printer.pageRect().height()-LINE_HEIGHT,m_width , LINE_HEIGHT); //this represent our calculated rectangle size
    //if (m_printer.fullPage()) m_printer.newPage();
    QString txt=QString("%1 de %2").arg(this->m_pages).arg(this->m_totalPages);
    paint->drawText(r, Qt::AlignLeft | Qt::TextWordWrap, txt, &required);
}
/** Testea en qué pagina estoy*/
bool ReportGenerator::TestPage(int y,QPainter * paint){
    int height=m_printer.pageRect().height();
    if ((y/m_pages)>=height){
        PrintNumPage(paint);
        m_pages++;
        m_printer.newPage();
        return true;
    }
    else return false;
}
/** Testea en qué pagina estoy*/
bool ReportGenerator::TestPage(int y){
    int height=m_printer.pageRect().height();
    if ((y/m_pages)>=height){
        m_pages++;
        return true;
    }
    else return false;
}

/** Devuelve el tamaño de cabecera*/
int ReportGenerator::getHeadHeight(){
    QImage logo(m_Config.m_Logo);
    int rtn=logo.height()+3*TITLE_FONT_SIZE;
    return rtn;
}

/** Devuelve el ancho de una seccion
 */
int ReportGenerator::getSectionHeight(int i){
    int height=0;
    struct st_In2ReportSection localItem=this->m_sections.at(i);
    height=(localItem.arguments.count())*(LINE_HEIGHT+2)+6*TITLE_FONT_SIZE;
    return height;
}

/** Establece un nombre casual*/
void ReportGenerator::setCasualOutputFilename(const QString &name){
    m_Config.m_outputCasualName=name;
}

/** Firma el pdf*/
void ReportGenerator::SignFile(){
    QString command;
    if (m_Config.m_outputCasualName.isEmpty()) {
        m_Config.m_outputPdfFile=QString("%1").arg(m_Config.m_inputPdfFile);
        m_Config.m_outputPdfFile.remove(".pdf");
        m_Config.m_outputPdfFile.append("Signed.pdf");
    }
    else{
        m_Config.m_outputPdfFile=m_Config.m_outputCasualName;
    }
    //Borramos si hay fichero previo
    QFile outDummy(m_Config.m_outputPdfFile);
    if (outDummy.exists()) outDummy.remove();

    if (m_Config.m_signPassword.isEmpty())
        //command=QString("\"%1\" -i %2 -o %3 -p \"\" -c %4").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signFile);
        command=QString("%1 /INPUT %2 /OUTPUT %3 /SIGNCERTIFICATE %4 /SIGNPAGE %5  /DISPLAYSIGNATURE /SIGN").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signFile).arg(m_pages);
    else
        //command=QString("\"%1\" -i %2 -o %3 -p %4 -c %5").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signPassword).arg(m_Config.m_signFile);
        command=QString("%1 /INPUT %2 /OUTPUT %3 /SIGNCERTIFICATE %4 /SIGNPASSWORD %5 /SIGNPAGE %6  /DISPLAYSIGNATURE /SIGN").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signFile).arg(m_Config.m_signPassword).arg(m_pages);
    emit SendDebugInfo(LOGGER_WARNING, command);
    QFile myBat("tmp.bat");
    if (myBat.open(QIODevice::WriteOnly)){
        myBat.write(command.toAscii().data());
        myBat.close();

    }
    m_signProcess.start("cmd.exe", QStringList() << "/c" << "tmp.bat");
    if (m_signProcess.waitForStarted())
    {
        //Lo bloqueo o no?
        m_signProcess.waitForFinished();
    }
    else
        SendDebugInfo(LOGGER_WARNING,(LOGGER_WARNING,"Error de pdf_sign"));

    /*QFile inFile(m_Config.m_inputPdfFile);
    inFile.remove();*/
}

/** Devuelve el path de salida de reporte*/
QString ReportGenerator::OutputPath(){
    return m_Config.m_outputPath;
}
/** Devuelve el path y fichero firmado*/
QString ReportGenerator::outputPdfFile(){
    return m_Config.m_outputPdfFile;
}
/** Saca errors*/
void ReportGenerator::ProcessOutputError(){
    QByteArray dummy=m_signProcess.readAllStandardError();
    SendDebugInfo(LOGGER_WARNING, QString(dummy));
}
/** Saca output*/
void ReportGenerator::ProcessOutputOutput(){
    QByteArray dummy=m_signProcess.readAllStandardOutput();
    SendDebugInfo(LOGGER_WARNING, QString(dummy));
}

