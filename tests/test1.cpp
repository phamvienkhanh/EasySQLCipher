#include <QTest>

#include "easysqlcipher.h"
#include "dbo/email.h"
#include "dbo/user.h"
#include "dbo/message.h"
#include "dbset.h"


class DBContext : public EasySQLCipher
{
public:
    DBContext() {
        users.setDbContext(this);
        messages.setDbContext(this);
        emails.setDbContext(this);
    }

    DbSet<User>    users;
    DbSet<Message> messages;
    DbSet<Email>   emails;
};
DBContext dbContext;
DBContext dbContext2;

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

    void open_db1() {

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

    void open_db2() {
        DBInitParam param;
        param.dbPath = "./test1.db";
        param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
        auto rs = dbContext2.init(param);

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
            });
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
            });
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
            });
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
            });
    }

    void remove_data_sync() {
        User user;
        user.m_id = 3;
        auto rs = dbContext.users.remove(user);

        QVERIFY(rs == DBCode::OK);
    }

    void remove_data_async() {
        User user;
        user.m_id = 3;
        dbContext.users.asyncRemove(user)
            .then([](){
                QVERIFY(true);
            })
            .onFailed([](const EZException& code){
                QVERIFY(false);
            });
    }

    void close_db() {
        dbContext.close();
    }

    void db2_fetch_sync1() {
        auto result = dbContext2.users.query("").select("*");
        QVERIFY(result.retCode == DBCode::OK);
        QVERIFY(result.value.size() == 4);

        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[i-1].m_id == i);
            QVERIFY(result.value[i-1].m_sip_id == QString("sip_id_%1").arg(i));
            QVERIFY(result.value[i-1].m_name == QString("name %1").arg(i));
            QVERIFY(result.value[i-1].m_data == QString("data %1").arg(i).toUtf8());
        }
    }

    void db2_fetch_async1() {
        dbContext2.users
            .query("")
            .asyncSelect("*")
            .then([](Result<QVector<User>, DBCode> result){
                QVERIFY(result.retCode == DBCode::OK);
                QVERIFY(result.value.size() == 4);

                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[i-1].m_id == i);
                    QVERIFY(result.value[i-1].m_sip_id == QString("sip_id_%1").arg(i));
                    QVERIFY(result.value[i-1].m_name == QString("name %1").arg(i));
                    QVERIFY(result.value[i-1].m_data == QString("data %1").arg(i).toUtf8());
                }
            })
            .onFailed([](const EZException& code){
                QVERIFY(false);
            });
    }

    void db2_fetch_sync2() {
        auto result = dbContext2.messages.query("").select("*");
        QVERIFY(result.retCode == DBCode::OK);
        QVERIFY(result.value.size() == 14);

        int offset = 0;
        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_1_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_1_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_1"));
        }

        offset += 4;
        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_2_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_2_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_2_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_2"));
        }

        offset += 4;
        for(int i = 1; i <= 6; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_3_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_3_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_3_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_3"));
        }
    }

    void db2_fetch_async2() {
        dbContext2.messages
            .query("")
            .asyncSelect("*")
            .then([](Result<QVector<Message>, DBCode> result){
                QVERIFY(result.retCode == DBCode::OK);
                QVERIFY(result.value.size() == 14);

                int offset = 0;
                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_1_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_1_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_1"));
                }

                offset += 4;
                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_2_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_2_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_2_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_2"));
                }

                offset += 4;
                for(int i = 1; i <= 6; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_3_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_3_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_3_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_3"));
                }
            })
            .onFailed([](const EZException& code){
                QVERIFY(false);
            });
    }

    void fetch_with_relation_sync1() {
        auto result = dbContext2.messages
                        .query("")
                        .with<User>("*")
                        .select("*");

        QVERIFY(result.retCode == DBCode::OK);
        QVERIFY(result.value.size() == 14);

        int offset = 0;
        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_1_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_1_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_1"));

            QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_1"));
            QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 1"));
            QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 1"));
        }

        offset += 4;
        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_2_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_2_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_2_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_2"));

            QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_2"));
            QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 2"));
            QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 2"));
        }

        offset += 4;
        for(int i = 1; i <= 6; i++) {
            QVERIFY(result.value[i-1+offset].m_id == i+offset);
            QVERIFY(result.value[i-1+offset].m_body == QString("message_body_3_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_3_%1").arg(i).toUtf8());
            QVERIFY(result.value[i-1+offset].m_identity == QString("identity_3_%1").arg(i));
            QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_3"));

            QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_3"));
            QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 3"));
            QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 3"));
        }

    }

    void fetch_with_relation_sync2() {
        auto result = dbContext2.users
                          .query("where #1.sip_id = 'sip_id_1'")
                          .with<Message>("*")
                          .with<Email>("*")
                          .select("*");

        QVERIFY(result.retCode == DBCode::OK);
        QVERIFY(result.value.size() == 1);

        QVERIFY(result.value[0].m_sip_id == "sip_id_1");
        QVERIFY(result.value[0].m_name == "name 1");
        QVERIFY(result.value[0].m_data == "data 1");

        QVERIFY(result.value[0].m_emails.size() == 2);
        for(int i = 1; i <= result.value[0].m_emails.size(); i++) {
            QVERIFY(result.value[0].m_emails[i-1].m_sip_id == "sip_id_1");
            QVERIFY(result.value[0].m_emails[i-1].m_address == QString("sip_id_1_%1@something.com").arg(i));
        }

        QVERIFY(result.value[0].m_messages.size() == 4);
        for(int i = 1; i <= 4; i++) {
            QVERIFY(result.value[0].m_messages[i-1].m_body == QString("message_body_1_%1").arg(i));
            QVERIFY(result.value[0].m_messages[i-1].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
            QVERIFY(result.value[0].m_messages[i-1].m_identity == QString("identity_1_%1").arg(i));
            QVERIFY(result.value[0].m_messages[i-1].m_author == QString("sip_id_1"));
        }
    }

    void fetch_with_relation_async1() {
        dbContext2.messages
            .query("")
            .with<User>("*")
            .asyncSelect("*")
            .then([](Result<QVector<Message>, DBCode> result){
                QVERIFY(result.retCode == DBCode::OK);
                QVERIFY(result.value.size() == 14);

                int offset = 0;
                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_1_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_1_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_1"));

                    QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_1"));
                    QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 1"));
                    QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 1"));
                }

                offset += 4;
                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_2_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_2_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_2_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_2"));

                    QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_2"));
                    QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 2"));
                    QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 2"));
                }

                offset += 4;
                for(int i = 1; i <= 6; i++) {
                    QVERIFY(result.value[i-1+offset].m_id == i+offset);
                    QVERIFY(result.value[i-1+offset].m_body == QString("message_body_3_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_md5 == QString("md5_3_%1").arg(i).toUtf8());
                    QVERIFY(result.value[i-1+offset].m_identity == QString("identity_3_%1").arg(i));
                    QVERIFY(result.value[i-1+offset].m_author == QString("sip_id_3"));

                    QVERIFY(result.value[i-1+offset].m_user->m_sip_id == QString("sip_id_3"));
                    QVERIFY(result.value[i-1+offset].m_user->m_name == QString("name 3"));
                    QVERIFY(result.value[i-1+offset].m_user->m_data == QString("data 3"));
                }
            })
            .onFailed([](const EZException& e){
                QVERIFY(false);
            });
    }

    void fetch_with_relation_async2() {
        dbContext2.users
            .query("where #1.sip_id = 'sip_id_1'")
            .with<Message>("*")
            .with<Email>("*")
            .asyncSelect("*")
            .then([](Result<QVector<User>, DBCode> result) {
                QVERIFY(result.retCode == DBCode::OK);
                QVERIFY(result.value.size() == 1);

                QVERIFY(result.value[0].m_sip_id == "sip_id_1");
                QVERIFY(result.value[0].m_name == "name 1");
                QVERIFY(result.value[0].m_data == "data 1");

                QVERIFY(result.value[0].m_emails.size() == 2);
                for(int i = 1; i <= result.value[0].m_emails.size(); i++) {
                    QVERIFY(result.value[0].m_emails[i-1].m_sip_id == "sip_id_1");
                    QVERIFY(result.value[0].m_emails[i-1].m_address == QString("sip_id_1_%1@something.com").arg(i));
                }

                QVERIFY(result.value[0].m_messages.size() == 4);
                for(int i = 1; i <= 4; i++) {
                    QVERIFY(result.value[0].m_messages[i-1].m_body == QString("message_body_1_%1").arg(i));
                    QVERIFY(result.value[0].m_messages[i-1].m_md5 == QString("md5_1_%1").arg(i).toUtf8());
                    QVERIFY(result.value[0].m_messages[i-1].m_identity == QString("identity_1_%1").arg(i));
                    QVERIFY(result.value[0].m_messages[i-1].m_author == QString("sip_id_1"));
                }
            })
            .onFailed([](const EZException& e){
                QVERIFY(false);
            });
    }

    void fetch_complex_sync() {
        DBHelper::ComplexQueryParams cplxParams;
        cplxParams.userData = new QVector<User>;
        cplxParams.query = "select * from User where id = :id;";
        cplxParams.dbContext = &dbContext2;
        cplxParams.cbFuncBind = [&](sqlite3_stmt* stmt, void*) -> bool {
            if(!DBHelper::stmtBindValue(stmt, 1, 3)) {
                return false;
            }

            return true;
        };
        cplxParams.cbFuncStep = [&](sqlite3_stmt* stmt, void* userData) -> bool {
            ColumnData id(0, stmt);
            ColumnData sipid(1, stmt);
            ColumnData name(2, stmt);
            ColumnData data(3, stmt);

            User user;
            user.m_id = id;
            user.m_sip_id = (QString)sipid;
            user.m_name = (QString)name;
            user.m_data = data;

            if(!QTest::qVerify(static_cast<bool>(user.m_id == 3), "user.m_id == 3", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_sip_id == "sip_id_3"), "user.m_sip_id == 'sip_id_3'", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_name == "name 3"), "user.m_name == 'name 3'", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_data == "data 3"), "user.m_data == 'data 3'", "", __FILE__, __LINE__))
                return false;

            if(userData) {
                ((QVector<User>*)userData)->push_back(user);
            }

            return true;
        };
        cplxParams.cbFuncFinished = [&](void* userData) {
            if(userData) {
                auto sz = ((QVector<User>*)userData)->size();
                QVERIFY(sz == 1);
            }
        };
        cplxParams.cbFuncError = [](DBCode code, void*) {
            QVERIFY(false);
        };

        DBHelper::execQuery(cplxParams);
    }

    void fetch_complex_async() {
        DBHelper::ComplexQueryParams cplxParams;
        cplxParams.userData = new QVector<User>;
        cplxParams.query = "select * from User where id = :id;";
        cplxParams.dbContext = &dbContext2;
        cplxParams.cbFuncBind = [&](sqlite3_stmt* stmt, void*) -> bool {
            if(!DBHelper::stmtBindValue(stmt, 1, 3)) {
                return false;
            }

            return true;
        };
        cplxParams.cbFuncStep = [&](sqlite3_stmt* stmt, void* userData) -> bool {
            ColumnData id(0, stmt);
            ColumnData sipid(1, stmt);
            ColumnData name(2, stmt);
            ColumnData data(3, stmt);

            User user;
            user.m_id = id;
            user.m_sip_id = (QString)sipid;
            user.m_name = (QString)name;
            user.m_data = data;

            if(!QTest::qVerify(static_cast<bool>(user.m_id == 3), "user.m_id == 3", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_sip_id == "sip_id_3"), "user.m_sip_id == 'sip_id_3'", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_name == "name 3"), "user.m_name == 'name 3'", "", __FILE__, __LINE__))
                return false;
            if(!QTest::qVerify(static_cast<bool>(user.m_data == "data 3"), "user.m_data == 'data 3'", "", __FILE__, __LINE__))
                return false;

            if(userData) {
                ((QVector<User>*)userData)->push_back(user);
            }

            return true;
        };
        cplxParams.cbFuncFinished = [&](void* userData) {
            if(userData) {
                auto sz = ((QVector<User>*)userData)->size();
                QVERIFY(sz == 1);
            }
        };
        cplxParams.cbFuncError = [](DBCode code, void*) {
            QVERIFY(false);
        };

        DBHelper::asyncExecQuery(cplxParams)
            .then([](void* userData){
                if(userData) {
                    auto sz = ((QVector<User>*)userData)->size();
                    QVERIFY(sz == 1);
                }
            })
            .onFailed(this, [](const EZException& e){
                QVERIFY(false);
            });
    }

    void close_db2() {
        dbContext2.close();
    }

    void cleanupTestCase()
    {

    }

private:

};

QTEST_MAIN(TestCase1)
#include "test1.moc"
