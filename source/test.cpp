#include "test.h"
#include "types.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    TestDB testDB;
    DBInitParam param;
    param.dbPath = "./test.db";
    param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
    qDebug() << (qint32)testDB.init(param);
    qDebug() << "thread safe : " << sqlite3_threadsafe();

    testDB.users.createTable();
    testDB.messages.createTable();
    testDB.emails.createTable();

    QVector<User> listUser;
    QVector<Message> listUser2;
    int a;

    User user;
//    user.m_id = 0;
//    user.m_data = QByteArray::fromRawData("testData", 8);
//    user.m_name = "keithpham";
//    user.registerMember();

//    User user2;
//    user.m_id = 0;
//    user.m_data = QByteArray::fromRawData("testData", 8);
//    user.m_name = "keithpham";

//    user2 = user;

//    for(auto i = 0; i < 20; i++) {
//        User user;
//        user.m_id = 0;
//        user.m_data = QByteArray::fromRawData("testData", 8);
//        user.m_name = "keithpham" + QString::number(i);
//        user.m_sip_id = "keithpham_sip_" + QString::number(i);

//        listUser.push_back(user);
//    }
    
//    for(auto i = 0; i < 20; i++) {
//        Message mesg;
//        mesg.m_id = 0;
//        mesg.m_author = "keithpham_sip_" + QString::number(std::rand() % 20);
//        mesg.m_body = "dadfgfggag" + QString::number(i);
//        listUser2.push_back(mesg);
//    }
    

//    testDB.users.insert(user);
//    testDB.users.insert(listUser);
//    testDB.messages.insert(listUser2);

//    auto result = testDB.users
//                  .query("where #1.id > 1")
//                  .with<Message>("body")
//                  .with<Email>("address")
//                  .select("id, name");
    
//    auto result2 = testDB.emails
//                  .query("where #1.id = 1")
//                  .with<User>("name")
//                  .select("*");
    
//    auto result3 = testDB.users.query("where id = 3").select("sip_id, name");
    
//    QVector<User> listUsers;
//    for(auto i = 3; i < 7; i++) {
//        User user;
//        user.m_id = i;
        
//        listUsers.push_back(user);
//    }
//    auto result4 = testDB.users.remove(listUsers);

//    user.m_id = 0;
//    user.m_name = "hoho";
//    listUsers.push_back(user);

//    user.m_id = 0;
//    user.m_name = "blalala";
//    listUsers.push_back(user);

//    user.m_id = 0;
//    user.m_name = "kakakak";
//    user.m_data = "daaaaaa";
//    listUsers.push_back(user);

//    auto r = testDB.users.update(listUsers, {"name", "data"});
    
//    if(result.retCode == DBCode::OK) {
//        for(auto& usr : result.value) {
//            qDebug() << usr.m_id;
//            qDebug() << usr.m_data;
//            qDebug() << usr.m_name;
//            qDebug() << " ================== ";
//        }
//    }

//    QVector<User> rsUsers;
//    DBHelper::ComplexQueryParams cplxParams;
//    cplxParams.query = "select * from User where id > :id;";
//    cplxParams.dbContext = &testDB;
//    cplxParams.cbFuncBind = [&](sqlite3_stmt* stmt, void*) -> bool {
//        if(!DBHelper::stmtBindValue(stmt, 1, 19)) {
//            return false;
//        }

//        return true;
//    };
//    cplxParams.cbFuncStep = [&](sqlite3_stmt* stmt, void*) -> bool {
//        User user;
//        user.registerMember();
//        ColumnData idData(0, stmt);
//        user.getRegister().setValue("id", idData);

//        ColumnData nameData(2, stmt);
//        user.m_name = (QString)nameData;

//        rsUsers.push_back(user);

//        return true;
//    };
//    cplxParams.cbFuncFinished = [&](void*) {
//        for(auto& user : rsUsers) {
//            qDebug() << user.m_id;
//            qDebug() << user.m_name;
//        }
//    };
//    cplxParams.cbFuncError = [](DBCode code, void*) {
//        qDebug() << "error";
//    };

//    DBHelper::execQuery(cplxParams);
    

//    user.m_id = 0;
//    user.m_name = "kakakak";
//    user.m_data = "daaaaaa";
//    listUsers.push_back(user);



//    testDB.users.asyncInsert(user)
//        .then(&app, [](const User& users){
//            qDebug() << "ok";
//            qDebug() << users.m_id;
//        })
//        .onFailed(&app, [](const EZException& e){
//            qDebug() << "error : " << (int)e.code();
//        });


//    testDB.users
//        .asyncUpdate("set sip_id = 'blalalalal' where id > 23;")
//        .then(&app, [] () {
//            qDebug() << "code : ";
//        })
//        .onFailed(&app, [](const EZException& e){
//            qDebug() << (int)e.code();
//        });


//    for(auto i = 0; i < 100; i++) {
//        testDB.users.query("where #1.id > 7")
//            .with<Email>("*")
//            .asyncSelect("*")
//            .then([](Result<QVector<User>, DBCode> result){
//                qDebug() << (int)result.value.size();
//            })
//            .onFailed([](const EZException& e){
//                qDebug() << (int)e.code();
//            });
//    }


    {
        for(auto i = 0; i < 100; i++) {
                      testDB.emails
                      .query("where id > 2")
                      .with<User>("*")
                      .asyncSelect("*")
                      .then([](Result<QVector<Email>, DBCode> result) {
                          if(result.retCode == DBCode::OK) {
                              qDebug() << result.value.size();
                          }
                      })
                      .onFailed([](EZException e) {
                          qDebug() << (int)e.code();
                      });

//            auto rs = testDB.emails.query("id > 1").asyncSelect("*")
        }
    }

    app.exec();

    testDB.close();

    return 0;
}
