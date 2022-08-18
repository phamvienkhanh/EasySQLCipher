#include <QTest>

#include "easysqlcipher.h"
#include "dbo/email.h"
#include "dbo/user.h"
#include "dbo/message.h"


class DBContext : public EasySQLCipher
{
public:
    DBContext() {
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
DBContext dbContext;

class TestCase1: public QObject
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

        QFile file("./test.db");
        if(file.exists()) {
            file.remove();
        }

        DBInitParam param;
        param.dbPath = "./test.db";
        param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
        auto rs = dbContext.init(param);

        QVERIFY(rs == DBCode::OK);
    }

    void create_table() {
        bool rs = false;
        rs = dbContext.users.createTable();
        QVERIFY2(rs == true, "create table user");

        rs = dbContext.emails.createTable();
        QVERIFY2(rs == true, "create table emails");

        rs = dbContext.messages.createTable();
        QVERIFY2(rs == true, "create table message");
    }

    void insert_data_sync() {
        User user;
        user.m_id = 0;
        user.m_sip_id = "test_sip_id_1";
        user.m_data = "some data 1";
        user.m_name = "my name is IS";

        auto rs = dbContext.users.insert(user);
        QVERIFY(rs == DBCode::OK);
        QVERIFY(user.m_id != 0);
        QVERIFY(user.m_sip_id == "test_sip_id_1");
        QVERIFY(user.m_data == "some data 1");
        QVERIFY(user.m_name == "my name is IS");
    }

    void insert_data_async() {
        User user;
        user.m_id = 0;
        user.m_sip_id = "test_sip_id_2";
        user.m_data = "some data 2";
        user.m_name = "my name is ISS2";

        dbContext.users.asyncInsert(user)
            .then([](const User& user){
                QVERIFY(user.m_id != 0);
                QVERIFY(user.m_sip_id == "test_sip_id_2");
                QVERIFY(user.m_data == "some data 2");
                QVERIFY(user.m_name == "my name is ISS2");
            })
            .onFailed([](const EZException& e){
                QVERIFY(false);
            })
            .waitForFinished();
    }

    void insert_list_data_sync() {
        QVector<User> listusers;
        for(auto i = 0; i < 10; i++) {
            User user;
            user.m_id = 0;
            user.m_sip_id = QString("test_sip_id_1_%1").arg(i);
            user.m_data = QString("some data 1 %1").arg(i).toUtf8();
            user.m_name = QString("my name 1 %1").arg(i);

            listusers.push_back(user);
        }

        auto result = dbContext.users.insert(listusers);
        QVERIFY(result == DBCode::OK);

        for(auto i = 0; i < 10; i++) {
            QVERIFY(listusers[i].m_id != 0);
            QVERIFY(listusers[i].m_sip_id == QString("test_sip_id_1_%1").arg(i));
            QVERIFY(listusers[i].m_data == QString("some data 1 %1").arg(i).toUtf8());
            QVERIFY(listusers[i].m_name == QString("my name 1 %1").arg(i));
        }
    }

    void insert_list_data_async() {
        QVector<User> listusers;
        for(auto i = 0; i < 10; i++) {
            User user;
            user.m_id = 0;
            user.m_sip_id = QString("test_sip_id_1_%1").arg(i);
            user.m_data = QString("some data 1 %1").arg(i).toUtf8();
            user.m_name = QString("my name 1 %1").arg(i);

            listusers.push_back(user);
        }

        dbContext.users.asyncInsert(listusers)
            .then([](const QVector<User>& users){
                for(auto i = 0; i < 10; i++) {
                    QVERIFY(users[i].m_id != 0);
                    QVERIFY(users[i].m_sip_id == QString("test_sip_id_1_%1").arg(i));
                    QVERIFY(users[i].m_data == QString("some data 1 %1").arg(i).toUtf8());
                    QVERIFY(users[i].m_name == QString("my name 1 %1").arg(i));
                }
            })
            .onFailed([](const EZException& e){
                QVERIFY(false);
            })
            .waitForFinished();
    }

    void update_data_sync() {
        User user;
        user.m_id = 2;
        user.m_name = "my name updated";

        auto rs = dbContext.users.update(user);
        QVERIFY(rs == DBCode::OK);
        QVERIFY(user.m_id == 2);
        QVERIFY(user.m_name == "my name updated");
    }

    void update_data_async() {
        User user;
        user.m_id = 2;
        user.m_name = "my name updated async";

        dbContext.users.asyncUpdate(user)
            .then([](const User& user){
              QVERIFY(user.m_id == 2);
              QVERIFY(user.m_name == "my name updated async");
            })
            .onFailed([](const EZException& e){
              QVERIFY(false);
            })
            .waitForFinished();
    }

    void update_list_data_sync() {
        QVector<User> listuser;
        for(auto i = 0; i < 7; i++) {
            User user;
            user.m_id = i + 1;
            user.m_name = QString("my name updated sync %1").arg(i);

            listuser.push_back(user);
        }

        auto rs = dbContext.users.update(listuser);
        QVERIFY(rs == DBCode::OK);

        for(auto i = 0; i < 7; i++) {
            QVERIFY(listuser[i].m_id == (i + 1));
            QVERIFY(listuser[i].m_name == QString("my name updated sync %1").arg(i));
        }
    }

    void update_list_data_async() {
        QVector<User> listuser;
        for(auto i = 0; i < 7; i++) {
            User user;
            user.m_id = i + 1;
            user.m_name = QString("my name updated async %1").arg(i);

            listuser.push_back(user);
        }

        dbContext.users.asyncUpdate(listuser)
            .then([](const QVector<User>& users){
                for(auto i = 0; i < 7; i++) {
                    QVERIFY(users[i].m_id == (i + 1));
                    QVERIFY(users[i].m_name == QString("my name updated async %1").arg(i));
                }
            })
            .onFailed([](const EZException& e){
                QVERIFY(false);
            })
            .waitForFinished();
    }

    void remove_data_sync() {
        User user;
        user.m_id = 3;
        auto rs = dbContext.users.remove(user);

        QVERIFY(rs == DBCode::OK);
    }

    void remove_data_ascync() {
        User user;
        user.m_id = 3;
        dbContext.users.asyncRemove(user)
            .then([](){
                QVERIFY(true);
            })
            .onFailed([](const EZException& code){
                QVERIFY(false);
            })
            .waitForFinished();
    }

    void close_db() {
        dbContext.close();
    }

    void cleanupTestCase()
    {
        qDebug("Called after myFirstTest and mySecondTest.");
    }

private:

};

QTEST_MAIN(TestCase1)
#include "test1.moc"
