#ifndef IN2GSMONITOR_H
#define IN2GSMONITOR_H

#include <QObject>
#include <QLibrary>

class In2GsMonitor
{

public:
    explicit In2GsMonitor(QObject *parent = 0);
    int In2gsapi_new_instance(void *caller_handle);
    void In2gsapi_delete_instance();
    int In2gsapi_run_string(const char *str, int user_errors, int *pexit_code);
    int In2gsapi_set_arg_encoding(int encoding);
    int In2gsapi_init_with_args(int argc, char **argv);
    int In2Render(QString foo);
    bool isLoaded();


private:
    QLibrary library;
    void *m_inst;
    bool m_loaded;
};

#endif // IN2GSMONITOR_H
