#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QThread>
#include "ContactReader.h"
//--------------------------------------------------------------------------------
#define COL_CONTACT_DEPARTMENT 0
#define COL_CONTACT_NICKNAME   2
#define COL_CONTACT_EMAIL      4
#define COL_CONTACT_FIRSTNAME  5
#define COL_CONTACT_LASTNAME   6
#define COL_CONTACT_NOTE       9
#define COL_CONTACT_CELLPHONE  30
#define COL_CONTACT_EXTNUMBER  50
//--------------------------------------------------------------------------------
Contact::Contact()
{

}
//--------------------------------------------------------------------------------
Contact::Contact(const Contact &ref)
:department(ref.department)
,nickName(ref.nickName)
,email(ref.email)
,firstName(ref.firstName)
,lastName(ref.lastName)
,cellPhone(ref.cellPhone)
,extNumber(ref.extNumber)
,note(ref.note)
{

}
//--------------------------------------------------------------------------------
ContactReader::ContactReader(void)
    : QObject()
{
}
//--------------------------------------------------------------------------------
ContactReader::~ContactReader(void)
{
}
//--------------------------------------------------------------------------------
unsigned long ContactReader::getDelayMsec(int total)
{
    unsigned long delayMsec = 1000 / total;

    if(delayMsec < 1)
        delayMsec = 1;

    return delayMsec;
}
//--------------------------------------------------------------------------------
void ContactReader::open(const QString &filename)
{
    qDebug() << "ContactReader open:" << filename;

    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        emit updateProgress(0, 0, file.errorString());
        return;
    }

    int total = 0;
    int pos = 0;
    while(!file.atEnd())
    {
        if(QThread::currentThread()->isInterruptionRequested())
        {
            emit updateProgress(0, 0, "interrupt");
            return;
        }

        file.readLine();
        ++total;
    }

    if(total > 0)
    {
        emit clearContacts();
        emit updateProgress(total, 0, "");
        QList<Contact>().swap(contactList);
        unsigned long delayMsec = getDelayMsec(total);
        QString line;

        file.seek(0);
        while (!file.atEnd())
        {
            if(QThread::currentThread()->isInterruptionRequested())
            {
                emit updateProgress(pos, pos, "interrupt");
                return;
            }

            line = file.readLine();
            if(!parse(line))
                qWarning() << "parse fail:" << line;

            emit updateProgress(total, ++pos, "importing");
            QThread::msleep(delayMsec);
        }
    }

    file.close();
    emit updateProgress(total, total, "done");
}
//--------------------------------------------------------------------------------
bool ContactReader::parse(const QString &line)
{
    Contact contact;
    QString str;
    QStringList slist = line.split(",");

    int col = slist.count();
    for(int i = 0; i < col; ++i)
    {
        str = slist[i].remove("\"");

        if(str.contains(QRegExp("^[0-9]{3}$")))
            contact.extNumber = str;
        else if(str.contains(QRegExp("^09[0-9]{8}$")))
            contact.cellPhone = str;
    }

    contact.department = slist[COL_CONTACT_DEPARTMENT].remove("/").replace('-', ';');
    contact.nickName = slist[COL_CONTACT_NICKNAME];
    contact.email = slist[COL_CONTACT_EMAIL];
    contact.firstName = slist[COL_CONTACT_FIRSTNAME];
    contact.lastName = slist[COL_CONTACT_LASTNAME];
    contact.note = slist[COL_CONTACT_NOTE];

    if(contact.department.isEmpty() || contact.department == "UNCATEGORIZED" ||
            contact.nickName.isEmpty() ||
            contact.email.isEmpty() ||
            (contact.firstName.isEmpty() && contact.lastName.isEmpty()))
        return false;

    contactList.append(contact);
    emit addContact(&contactList.last());

    return true;
}
//--------------------------------------------------------------------------------
void ContactReader::save(const QString &filename)
{
    qDebug() << "ContactReader save:" << filename;

    QFile file(filename);
    if(!file.open(QFile::ReadWrite))
    {
        emit updateProgress(0, 0, file.errorString());
        return;
    }

    int total = contactList.count();
    if(total > 0)
    {
        emit updateProgress(total, 0, "");
        unsigned long delayMsec = getDelayMsec(total);
        QTextStream stream(&file);

        for(int i = 0; i < total; ++i)
        {
            if(QThread::currentThread()->isInterruptionRequested())
            {
                emit updateProgress(i, i, "interrupt");
                return;
            }

            Contact& c = contactList[i];
            stream << "BEGIN:VCARD" << endl;
            stream << "VERSION:3.0" << endl;
            stream << "N:" << c.lastName << ";" << c.firstName << ";;;" << endl;
            stream << "NICKNAME:" << c.nickName << endl;
            stream << "ORG:" << c.department << endl;
            stream << "EMAIL;type=WORK:" << c.email << endl;
            stream << "TEL;type=WORK:" << c.extNumber << endl;
            stream << "TEL;type=CELL:" << c.cellPhone << endl;
            stream << "END:VCARD" << endl;

            emit updateProgress(total, i + 1, "exporting");
            QThread::msleep(delayMsec);
        }
    }

    file.close();
    emit updateProgress(total, total, "done");
}
//--------------------------------------------------------------------------------
