#include <QTest>

#include "easysqlcipher.h"
#include "dbo/email.h"
#include "dbo/user.h"
#include "dbo/message.h"


class TestDB : public EasySQLCipher
{
public:
    TestDB() {
        auto getConnection = [this](){
            return m_connection;
        };

        users.setFnConProvider(getConnection);
        messages.setFnConProvider(getConnection);
        emails.setFnConProvider(getConnection);
    }

    DbSet<User>    users;
    DbSet<Message> messages;
    DbSet<Email>   emails;
};
TestDB testDB;

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {

    }

    void check_threadsafe() {
        int threadSafe = sqlite3_threadsafe();
        QVERIFY(threadSafe == 1);
    }

    void open_db() {
        DBInitParam param;
        param.dbPath = "./test.db";
        param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
        auto rs = testDB.init(param);

        QVERIFY(rs == DBCode::OK);
    }

    void close_db() {
        testDB.close();
    }

    void cleanupTestCase()
    {
        qDebug("Called after myFirstTest and mySecondTest.");
    }

private:

};

QTEST_MAIN(TestQString)
#include "test1.moc"
