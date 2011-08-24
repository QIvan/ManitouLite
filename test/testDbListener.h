#ifndef TESTDBLISTENER_H
#define TESTDBLISTENER_H

#include <QObject>
#include <QDebug>
#include "RegistryTest.h"
#include "db_listener.h"
#include "sqlstream.h"


class Notifer : public QObject
{
    Q_OBJECT
public:
    explicit Notifer(QObject *parent = 0);
public slots:
    void slotNotified()
    {
        qDebug() << "NOTIFER!";
    }
};


class test_db_listener :public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(test_db_listener);
    //CPPUNIT_TEST(addListener);
    CPPUNIT_TEST_SUITE_END();
public:
    void addListener()
    {
        /*db_cnx db;
        QString listener_name = "foo";
        db_listener listener(db, listener_name);
        Notifer notifer;
        QObject::connect(&listener, SIGNAL(notified()), &notifer, SLOT(slotNotified()));

        try{
            sql_stream insert2(QString("SELECT pg_notify(\'%1\', \'\')").arg(listener_name),db);
        }
        catch(db_excpt e)
        {
            qDebug() << e.query();
            qDebug() << e.errmsg();
            CPPUNIT_FAIL("Database Exept!");
        }*/
    }

};


#endif // TESTDBLISTENER_H
