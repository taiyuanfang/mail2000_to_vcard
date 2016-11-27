#ifndef CONTACTREADER_H
#define CONTACTREADER_H
//--------------------------------------------------------------------------------
#include <QObject>
#include <QString>
#include <QList>
//--------------------------------------------------------------------------------
class Contact
{
public:
    Contact();
    Contact(const Contact &ref);

public:
    QString department;
    QString nickName;
    QString email;
    QString firstName;
    QString lastName;
    QString cellPhone;
    QString extNumber;
    QString note;
};
//--------------------------------------------------------------------------------
class ContactReader : public QObject
{
    Q_OBJECT

public:
    ContactReader(void);
    virtual ~ContactReader(void);

public slots:
    void open(const QString &filename);
    void save(const QString &filename);

private:
    bool parse(const QString &line);
    unsigned long getDelayMsec(int total);

private:
    QList<Contact> contactList;

signals:
    void clearContacts();
    void addContact(const Contact *contact);
    void updateProgress(int total, int pos, const QString &str);
};
//--------------------------------------------------------------------------------
#endif // CONTACTREADER_H
