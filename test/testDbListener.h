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


class test_db_listener :public CppUnit::TestFixture, public QThread
{
    CPPUNIT_TEST_SUITE(test_db_listener);
    CPPUNIT_TEST(run);
    CPPUNIT_TEST_SUITE_END();
public:
    void run()
    {
        emit started();
        db_cnx db;
        db_listener listener(db, "new_message");
        Notifer notifer;
        QObject::connect(&listener, SIGNAL(notified()), &notifer, SLOT(slotNotified()));

        sql_write_fields fiedls2(db);
        fiedls2.add_if_not_empty("test_text", "Insert Text 2", 9);//Insert Te
        fiedls2.add("test_char200", "Insert Text 2");
        fiedls2.add_no_quote("test_date", "now()");
        fiedls2.add_if_not_zero("test_int_not_null", 0);
        fiedls2.add_if_not_zero("test_int_not_null", 2);
        QString str2 =  QString("INSERT INTO test_table(%1) VALUES(%2)")
                .arg(fiedls2.fields())
                .arg(fiedls2.values());
        sql_stream insert2(str2,db);
        listener.process_notification();
        emit finished();
    }

};


#endif // TESTDBLISTENER_H
