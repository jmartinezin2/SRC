/****************************************************************************
**
** Namespace CemitecProxyOPC generated by dumpcpp from type library
** OPC_DLL_Puente.dll
**
****************************************************************************/

#ifndef QAX_DUMPCPP_CEMITECPROXYOPC_H
#define QAX_DUMPCPP_CEMITECPROXYOPC_H

// Define this symbol to __declspec(dllexport) or __declspec(dllimport)
#ifndef CEMITECPROXYOPC_EXPORT
#define CEMITECPROXYOPC_EXPORT
#endif

#include <qaxobject.h>
#include <qaxwidget.h>
#include <qdatetime.h>
#include <qpixmap.h>

struct IDispatch;


// Referenced namespace

namespace CemitecProxyOPC {


class CEMITECPROXYOPC_EXPORT _CemitecObjectOPC : public QAxObject
{
public:
    _CemitecObjectOPC(IDispatch *subobject = 0, QAxObject *parent = 0)
    : QAxObject((IUnknown*)subobject, parent)
    {
        internalRelease();
    }

    /*
    Method adicionaGrupo
    */
    inline QVariant adicionaGrupo(const QString& ConnectedGroupName);

    /*
    Method adicionaItem
    */
    inline int adicionaItem(const QString& itemName);

    /*
    Method conectaServidor
    */
    inline QVariant conectaServidor(const QString& ConnectedServerName);

    /*
    Method desconectaServidor
    */
    inline QVariant desconectaServidor();

    /*
    Method escribeDato
    */
    inline QVariant escribeDato(int& clientHandle, QVariant& value);

    /*
    Method getItemServerHandle
    */
    inline int getItemServerHandle(int& clientHandle);

    /*
    Method leeDato
    */
    inline QVariant leeDato(int& clientHandle);

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

class CEMITECPROXYOPC_EXPORT __CemitecObjectOPC : public QAxObject
{
public:
    __CemitecObjectOPC(IDispatch *subobject = 0, QAxObject *parent = 0)
    : QAxObject((IUnknown*)subobject, parent)
    {
        internalRelease();
    }

    /*
    Method EvDataChange
    */
    inline void EvDataChange(int& NumItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities);

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

// Actual coclasses
class CEMITECPROXYOPC_EXPORT CemitecObjectOPC : public QAxObject
{
public:
    CemitecObjectOPC(QObject *parent = 0)
    : QAxObject(parent)
    {
        setControl("{62a555db-ed30-4df9-91c5-9725cf84e35f}");
        //setControl("{B6EACB30-42D5-11D0-9517-0020AFAA4B3C}");


    }

    /*
    Method adicionaGrupo
    */
    inline QVariant adicionaGrupo(const QString& ConnectedGroupName);

    /*
    Method adicionaItem
    */
    inline int adicionaItem(const QString& itemName);

    /*
    Method conectaServidor
    */
    inline QVariant conectaServidor(const QString& ConnectedServerName);

    /*
    Method desconectaServidor
    */
    inline QVariant desconectaServidor();

    /*
    Method escribeDato
    */
    inline QVariant escribeDato(int& clientHandle, QVariant& value);

    /*
    Method getItemServerHandle
    */
    inline int getItemServerHandle(int& clientHandle);

    /*
    Method leeDato
    */
    inline QVariant leeDato(int& clientHandle);

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

// member function implementation
#ifndef QAX_DUMPCPP_CEMITECPROXYOPC_NOINLINES
inline QVariant _CemitecObjectOPC::adicionaGrupo(const QString& ConnectedGroupName)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&ConnectedGroupName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 7, _a);
    return qax_result;
}

inline int _CemitecObjectOPC::adicionaItem(const QString& itemName)
{
    int qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&itemName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 8, _a);
    return qax_result;
}

inline QVariant _CemitecObjectOPC::conectaServidor(const QString& ConnectedServerName)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&ConnectedServerName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 9, _a);
    return qax_result;
}

inline QVariant _CemitecObjectOPC::desconectaServidor()
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result};
    qt_metacall(QMetaObject::InvokeMetaMethod, 10, _a);
    return qax_result;
}

inline QVariant _CemitecObjectOPC::escribeDato(int& clientHandle, QVariant& value)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle, (void*)&value};
    qt_metacall(QMetaObject::InvokeMetaMethod, 11, _a);
    return qax_result;
}

inline int _CemitecObjectOPC::getItemServerHandle(int& clientHandle)
{
    int qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle};
    qt_metacall(QMetaObject::InvokeMetaMethod, 12, _a);
    return qax_result;
}

inline QVariant _CemitecObjectOPC::leeDato(int& clientHandle)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle};
    qt_metacall(QMetaObject::InvokeMetaMethod, 13, _a);
    return qax_result;
}


inline QVariant CemitecObjectOPC::adicionaGrupo(const QString& ConnectedGroupName)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&ConnectedGroupName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 8, _a);
    return qax_result;
}

inline int CemitecObjectOPC::adicionaItem(const QString& itemName)
{
    int qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&itemName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 9, _a);
    return qax_result;
}

inline QVariant CemitecObjectOPC::conectaServidor(const QString& ConnectedServerName)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&ConnectedServerName};
    qt_metacall(QMetaObject::InvokeMetaMethod, 10, _a);
    return qax_result;
}

inline QVariant CemitecObjectOPC::desconectaServidor()
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result};
    qt_metacall(QMetaObject::InvokeMetaMethod, 11, _a);
    return qax_result;
}

inline QVariant CemitecObjectOPC::escribeDato(int& clientHandle, QVariant& value)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle, (void*)&value};
    qt_metacall(QMetaObject::InvokeMetaMethod, 12, _a);
    return qax_result;
}

inline int CemitecObjectOPC::getItemServerHandle(int& clientHandle)
{
    int qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle};
    qt_metacall(QMetaObject::InvokeMetaMethod, 13, _a);
    return qax_result;
}

inline QVariant CemitecObjectOPC::leeDato(int& clientHandle)
{
    QVariant qax_result;
    void *_a[] = {(void*)&qax_result, (void*)&clientHandle};
    qt_metacall(QMetaObject::InvokeMetaMethod, 14, _a);
    return qax_result;
}


inline void __CemitecObjectOPC::EvDataChange(int& NumItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities)
{
    void *_a[] = {0, (void*)&NumItems, (void*)&ClientHandles, (void*)&ItemValues, (void*)&Qualities};
    qt_metacall(QMetaObject::InvokeMetaMethod, 7, _a);
}



#endif

}

template<>
inline void *qMetaTypeConstructHelper(const CemitecProxyOPC::_CemitecObjectOPC *t)
{ Q_ASSERT(!t); return new CemitecProxyOPC::_CemitecObjectOPC; }

template<>
inline void *qMetaTypeConstructHelper(const CemitecProxyOPC::CemitecObjectOPC *t)
{ Q_ASSERT(!t); return new CemitecProxyOPC::CemitecObjectOPC; }

template<>
inline void *qMetaTypeConstructHelper(const CemitecProxyOPC::__CemitecObjectOPC *t)
{ Q_ASSERT(!t); return new CemitecProxyOPC::__CemitecObjectOPC; }

#endif

