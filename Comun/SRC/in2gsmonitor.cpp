/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "in2gsmonitor.h"
#include <QLibrary>
#include <QDebug>

typedef struct gsapi_revision_s {
    const char *product;
    const char *copyright;
    long revision;
    long revisiondate;
} gsapi_revision_t;

#define GS_ARG_ENCODING_LOCAL 0
#define GS_ARG_ENCODING_UTF8 1


typedef int (*gsapi_revision)(gsapi_revision_s * arg1, int arg2);
typedef int  (*gsapi_new_instance)(void **pinstance, void *caller_handle);
typedef void (*gsapi_delete_instance)(void *instance);
typedef int (*gsapi_set_stdio) (void *instance, int(*stdin_fn)(void *caller_handle, char *buf, int len), int(*stdout_fn)(void *caller_handle, const char *str, int len), int(*stderr_fn)(void *caller_handle, const char *str, int len));
typedef int (*gsapi_set_poll) (void *instance, int(*poll_fn)(void *caller_handle));
//typedef int (*gsapi_set_display_callback) (void *instance, display_callback *callback);
typedef int (*gsapi_set_arg_encoding) (void *instance, int encoding);
typedef int (*gsapi_init_with_args) (void *instance, int argc, char **argv);
typedef int (*gsapi_run_string_begin) (void *instance, int user_errors, int *pexit_code);
typedef int (*gsapi_run_string_continue) (void *instance, const char *str, unsigned int length, int user_errors, int *pexit_code);
typedef int (*gsapi_run_string_end) (void *instance, int user_errors, int *pexit_code);
typedef int (*gsapi_run_string_with_length) (void *instance, const char *str, unsigned int length, int user_errors, int *pexit_code);
typedef int (*gsapi_run_string) (void *instance, const char *str, int user_errors, int *pexit_code);
typedef int (*gsapi_run_file) (void *instance, const char *file_name, int user_errors, int *pexit_code);
typedef int (*gsapi_exit) (void *instance);
//typedef int (*gsapi_set_visual_tracer) (gstruct vd_trace_interface_s *I);

In2GsMonitor::In2GsMonitor(QObject *parent)
{
#ifdef V64
    library.setFileName("gsdll64");
#else
    library.setFileName("gsdll32");
#endif
    library.load();
    if( !library.isLoaded() ){
        qDebug() << "Cannot load library.";
        m_loaded=false;
        return;
    }
    m_loaded=true;
    gsapi_revision myFunction=(gsapi_revision)library.resolve("gsapi_revision");
    if (myFunction){
        m_inst=NULL;
        int code=In2gsapi_new_instance(NULL);
        if (code==0)
            code=In2gsapi_set_arg_encoding(GS_ARG_ENCODING_UTF8);
        /*if (code==0)
            code=In2gsapi_init_with_args(0,0);
        code=0;*/

    }
}
/** Indica si existe la libreria*/
bool In2GsMonitor::isLoaded(){
    return m_loaded;
}

/** Inicializa la instancia*/
int In2GsMonitor::In2gsapi_new_instance(void *caller_handle){
    int code=-1;
    if(library.isLoaded() ){
       gsapi_new_instance myFunction=(gsapi_new_instance)library.resolve("gsapi_new_instance");
       if (myFunction){
           code=myFunction(&m_inst,caller_handle);
       }
    }
    return code;
}

/** Destruye la instancia*/
void In2GsMonitor::In2gsapi_delete_instance(){
    if(library.isLoaded() ){
       gsapi_delete_instance myFunction=(gsapi_delete_instance)library.resolve("gsapi_delete_instance");
       if (myFunction){
           myFunction(m_inst);
       }
    }
}
/** Codificacion*/
int In2GsMonitor::In2gsapi_set_arg_encoding(int encoding){
    int code=-1;
    if(library.isLoaded() ){
       gsapi_set_arg_encoding myFunction=(gsapi_set_arg_encoding)library.resolve("gsapi_set_arg_encoding");
       if (myFunction){
           code=myFunction(m_inst, encoding);
       }
    }
    return code;
}

/** Ejecuta la sentencia*/
int In2GsMonitor::In2gsapi_run_string(const char *str, int user_errors, int *pexit_code){
    int code=-1;
    if(library.isLoaded() ){
       gsapi_run_string myFunction=(gsapi_run_string)library.resolve("gsapi_run_string");
       if (myFunction){
           code=myFunction(m_inst, str,user_errors,pexit_code);
       }
    }
    return code;
}

/** Inicializa el interprete*/
int In2GsMonitor::In2gsapi_init_with_args(int argc, char **argv){
    int code=-1;
    if(library.isLoaded() ){
       gsapi_init_with_args myFunction=(gsapi_init_with_args)library.resolve("gsapi_init_with_args");
       if (myFunction){
           code=myFunction(m_inst,argc,argv);
       }
    }
    return code;
}

/** Inicializa el interprete*/
int In2GsMonitor::In2Render(QString foo){
    int code=-1;
    //QStringList lst=foo.split(" ");

    char * argv[10];
    //argv[0]="gswin64c.exe";
    argv[0]="-sDEVICE=bmp16m";
    argv[1]="-sOutputFile=D:/dummy2.bmp";
    argv[2]="-r360x360";
    argv[3]="-dNOPAUSE";
    argv[4]="-dBATCH";
    argv[5]="-dGraphicsAlphaBits=1";
    argv[6]="-dNumRenderingThreads=4";
    argv[7]="-f";
    argv[8]="D:\\dummyPdf.pdf";

    if(library.isLoaded() ){
       gsapi_init_with_args myFunction=(gsapi_init_with_args)library.resolve("gsapi_init_with_args");
       if (myFunction){
           code=myFunction(m_inst,9,argv);
       }
    }
    return code;
}
