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

#define DELAY_MSEC             5
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
    , delayMsec(DELAY_MSEC)
{
}
//--------------------------------------------------------------------------------
ContactReader::~ContactReader(void)
{
}
//--------------------------------------------------------------------------------
void ContactReader::setDelay(int row)
{
    delayMsec = 1000 / row;
    if(delayMsec < 1)
        delayMsec = 1;
}
//--------------------------------------------------------------------------------
void ContactReader::open(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        emit updateProgress(0, 0, file.errorString());
        return;
    }

    QStringList slist;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        slist.append(line);
    }

    int row = slist.count();
    if(row > 0)
    {
        emit clearContacts();
        emit updateProgress(row, 0, "");
        setDelay(row);

        for(int i = 0; i < row; ++i)
        {
            if(!parse(slist[i]))
                qWarning() << i << slist[i];

            emit updateProgress(row, i + 1, "importing");
            QThread::msleep(delayMsec);
        }
    }

    file.close();
    emit updateProgress(row, row, "done");
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
    QFile file(filename);

    if(!file.open(QFile::ReadWrite))
    {
        emit updateProgress(0, 0, file.errorString());
        return;
    }

    QTextStream stream(&file);

    int row = contactList.count();
    if(row > 0)
    {
        emit updateProgress(row, 0, "");
        setDelay(row);

        for(int i = 0; i < row; ++i)
        {
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

            emit updateProgress(row, i + 1, "exporting");
            QThread::msleep(delayMsec);
        }
    }

    file.close();
    emit updateProgress(row, row, "done");
}
//--------------------------------------------------------------------------------
