#include "test.h"
#include "types.h"

int main(int argc, char *argv[])
{
    TestDB testDB;
    DBInitParam param;
    param.dbPath = "./test.db";
    param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
    qDebug() << (qint32)testDB.init(param);

    testDB.users.createTable();
    testDB.messages.createTable();

    QVector<User> listUser;
    std::vector<User> listUser2;

    User user;
    user.m_id = 0;
    user.m_data = QByteArray::fromRawData("testData", 8);
    user.m_name = "keithpham";
    user.registerMember();

    User user2;
    user.m_id = 0;
    user.m_data = QByteArray::fromRawData("testData", 8);
    user.m_name = "keithpham";

    user2 = user;

//    for(auto i = 0; i < 20; i++) {
//        User user;
//        user.m_id = 0;
//        user.m_data = QByteArray::fromRawData("testData", 8);
//        user.m_name = "keithpham" + QString::number(i);

//        listUser.push_back(user);
//    }
    

//    testDB.users.insert(listUser);

    auto result = testDB.users.query("where id > 7")
                              .select("id, name");
    
    if(result.retCode == DBCode::OK) {
        for(auto& usr : result.value) {
            qDebug() << usr.m_id;
            qDebug() << usr.m_data;
            qDebug() << usr.m_name;
            qDebug() << " ================== ";
        }
    }
    
            
    testDB.close();

    return 0;
}
