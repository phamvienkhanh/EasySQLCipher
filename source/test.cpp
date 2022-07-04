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
    QVector<Message> listUser2;

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
    

//    testDB.users.insert(listUser);
//    testDB.messages.insert(listUser2);

    auto result = testDB.users
                  .query("where Message.id > 7")                  
                  .with<Message>()
                  .select("*");
    
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
