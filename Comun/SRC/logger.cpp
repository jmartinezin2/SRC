#include "logger.h"
#include <windows.h>
#include <winldap.h>
#include <winber.h>
#include <rpc.h>
#include <rpcdce.h>
#include <stdio.h>

#include <QString>
#include <QDebug>
#include <QFile>
#include <QIODevice>



Logger::Logger(QObject *parent) :
    QObject(parent)
{
    m_hostName=QString("basalde.cemitec.local");
    m_pMyDN =QString("CN=users,DC=cemitec,DC=local");
    m_filter=QString("(memberOf=CN=txemauser,CN=Users,DC=cemitec,DC=local)");
    m_localHost="cemitec.local";
}
/** Esytablece el dist.name*/
void Logger::setDN(const QString &txt){
    m_pMyDN=txt;
}
/** Establece el filtro*/
void Logger::setFilter(const QString &txt){
    m_filter=txt;
}
/** Establece el hostname*/
void Logger::setHostName(const QString &txt){
    m_hostName=txt;
}
/** Establece el localHost*/
void Logger::setLocalHost(const QString &txt){
    m_localHost=txt;
}
/** Testea una conexion*/
int Logger::TryConnection(QString name, QString Password){

    QByteArray baName=name.toLocal8Bit();
    QByteArray baPassword=Password.toLocal8Bit();
    QByteArray baLocalHost=m_localHost.toLocal8Bit();

    const char* pUserName=baName.data();
    const char* pPassword=baPassword.data();
    const char *localHost=baLocalHost.data();
    LDAP* pLdapConnection = NULL;
    ULONG version = LDAP_VERSION3;
    INT returnCode = 0;
    int rtn=LOGGER_NO_INIT;

    pLdapConnection = ldap_init((WCHAR*)m_hostName.utf16(), LDAP_PORT);
    if (pLdapConnection == NULL)
       {
           ldap_unbind(pLdapConnection);
           return LOGGER_NO_INIT ;
       }
    ldap_set_option(pLdapConnection, LDAP_OPT_PROTOCOL_VERSION, (void*)&version);
    if (ldap_connect(pLdapConnection, NULL)!=LDAP_SUCCESS){
        return LOGGER_NO_CONNECTION ;
    }
    SEC_WINNT_AUTH_IDENTITY secIdent;
    secIdent.User = (unsigned short*)pUserName;
    secIdent.UserLength = strlen(pUserName);
    secIdent.Password = (unsigned short*)pPassword;
    secIdent.PasswordLength = strlen(pPassword);
    secIdent.Domain = (unsigned short*)localHost;
    secIdent.DomainLength = strlen(localHost);
    secIdent.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    returnCode = ldap_bind_s(pLdapConnection, (WCHAR*)m_pMyDN.utf16(), (PWCHAR)&secIdent,LDAP_AUTH_NEGOTIATE);
    QString filter= QString("(&(sAMAccountName=%1)%2)").arg(name).arg(m_filter);
    if(returnCode == LDAP_SUCCESS)
    {
        rtn=LOGGER_USER_GRANTED;
        PWCHAR Attrib[1];
        Attrib[0]=NULL;
        LDAPMessage *pMsg=NULL;
        returnCode=ldap_search_s(pLdapConnection,(WCHAR*)m_pMyDN.unicode(),LDAP_SCOPE_SUBTREE,(WCHAR*)filter.unicode(),Attrib,0,&pMsg);
        if (returnCode==LDAP_SUCCESS){
            ULONG numEntries=ldap_count_entries(pLdapConnection,pMsg);
            if (numEntries) rtn=LOGGER_USER_INCLUDED;
            else rtn=LOGGER_USER_NOT_INCLUDED;
        }
        ldap_unbind_s(pLdapConnection);
        ldap_msgfree(pMsg);
    }
    else rtn=LOGGER_USER_NOT_GRANTED;
    return rtn;
}


// Por si en algun momento queremos parsear el retorno*/


/*
                for (int i=0;i<numEntries;i++){
                LDAPMessage* pEntry = NULL;
                PCHAR pEntryDN = NULL;
                ULONG iCnt = 0;
                char* sMsg;
                BerElement* pBer = NULL;
                PWCHAR pAttribute = NULL;
                PWCHAR* ppValue = NULL;
                ULONG iValue = 0;
                if (!i)  pEntry=ldap_first_entry(pLdapConnection, pMsg);
                else pEntry=ldap_next_entry(pLdapConnection, pMsg);
                if( pEntry == NULL ){
                    ldap_unbind_s(pLdapConnection);
                    ldap_msgfree(pMsg);
                }
                else{
                    pAttribute = ldap_first_attribute(pLdapConnection,pEntry,&pBer);
                    if (!pAttribute) qDebug()<<"Sin atributos";
                    while(pAttribute != NULL){
                        QString rs=QString("Atributo:%1\n").arg(QString::fromStdWString(pAttribute));
                        fileResult.write(rs.toUtf8().data());
                        ppValue = ldap_get_values(pLdapConnection,pEntry,pAttribute);
                        iValue = ldap_count_values(ppValue);
                        for(int i=0; i<iValue; i++)
                        {
                            QString rs=QString("Valor:%1\n").arg(QString::fromStdWString(ppValue[i]));//ppValue[i];
                            fileResult.write(rs.toUtf8().data());
                        }
                        if(ppValue != NULL) ldap_value_free(ppValue);
                        ppValue = NULL;
                        ldap_memfree(pAttribute);
                        pAttribute = ldap_next_attribute(pLdapConnection,pEntry,pBer);             // Current BerElement
                    }
                    if( pBer != NULL )ber_free(pBer,0);
                            pBer = NULL;
                }
            }
            fileResult.close();
        }

    }
    else
    {
        qDebug()<<"Connection unsucessfull";

    }
}
*/
