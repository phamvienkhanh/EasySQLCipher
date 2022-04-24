#include "test.h"

int main(int argc, char *argv[])
{
    TestDB testDB;
    DBInitParam param;
    param.dbPath = "/home/keith/programs/test.db";
    param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
    qDebug() << (qint32)testDB.init(param);

    testDB.users.createTable();
    testDB.messages.createTable();

    for(auto i = 0; i < 10; i++) {
        User user;
        user.m_id = 0;
        user.m_data = QByteArray::fromRawData("testData", 8);
        user.m_name = "keithpham" + QString::number(i);

        testDB.users.insert(user);
    }

    testDB.close();

    return 0;
}
