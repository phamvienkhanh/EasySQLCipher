## EasySQLCipher

**EasySQLCipher** được xây dựng nhằm mục đích hỗ trợ thao tác truy vấn trên sqlcipher. Cung cấp cơ chế truy vấn bất đồng bộ dựa trên QtConcurrent của Qt6. Giúp việc truy vấn được thực hiện dễ dàng hơn. Chi tiết về ý tưởng và cách xây dựng có thể xem [quá trình xây dựng mô hình](./doc/README.md).  

## Build
SQLCipher là submodule của project, sau khi clone source chạy git submodule để pull nó về.

```
git submodule init
git submodule update
```

Vào thư mục đã clone source, chạy file **build.sh** để build.  
Mở QtCreator để build **EasySQLCipher** và test.

## Sử dụng

Tạo DBO

```cpp
class User : public AbstractDBO
{
public:
    User(){}

    QString getTableName() const override {
        return "User";
    }

    void registerMember() override {
        AbstractDBO::registerMember();

        bind("sip_id", "text", &m_sip_id);
        bind("name", "text", &m_name);
        bind("data", "BLOB", &m_data);

        relation("sip_id=author", &m_messages, getTableName());
        relation("sip_id=sip_id", &m_emails, getTableName());
    }

public:
    QString m_name;
    QString m_sip_id;
    QByteArray m_data;

    QVector<Message> m_messages;
    QVector<Email>   m_emails;

};
```

- DBO là các class kế thừ từ *AbstractDBO*
- getTableName() là function trả về tên table.
- registerMember() phải được định nghĩa để đăng ký các member của DBO. Mỗi member tương ứng với mỗi cột trên sql table và liên kết với một trường trong class để lưu dữ liệu.
- Phải AbstractDBO::registerMember() từ lớp dẫn xuất.
- bind khai báo bind(\<tên cột\>, \<kiểu dữ liệu\>, \< trường liên kết\>)
- relation khai báo mỗi quan hệ giữa các bảng, hỗ trợ thông tin để thực hiện join khi truy vấn. Tương tự với bind, relation("sip_id=author", &m_messages, getTableName()) sẽ có ý nghĩa - table user quan hệ 1-n với table message với trường liên kết User.sip_id=Message.author.  
- Member không hỗ trợ pointer và shared pointer.
- Relation có hỗ trợ std::shared_ptr\<\> để tránh trường hợp circular dependencies giữa các DBO.
- Không hỗ trợ QVector<std::shared_ptr<>>

Tạo DBContext

```cpp
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
```

- DBContext là class kế thừa từ EasySQLCipher, nó có nhiệm vụ khởi tạo và giữ connection. Cung cấp DbSet để thực hiện truy vấn.
- DbSet là các template tạo ra từ DBO, cũng cấp phương thức truy vấn cho các DBO.
- Cần phải gọi hàm setFnConProvider trên từng DbSet để set callback connection provider. DbSet sẽ gọi hàm này để lấy connnection khi cần.

Sử dụng DBContext đã tạo

```cpp
TestDB testDB;
DBInitParam param;
param.dbPath = "./test.db";
param.openMode = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
qDebug() << (qint32)testDB.init(param);

testDB.users.createTable();
testDB.messages.createTable();
testDB.emails.createTable();
```

Insert dữ liệu:  

```cpp
User user;
user.m_id = 0;
user.m_data = QByteArray::fromRawData("testData", 8);
user.m_name = "my_name";
user.m_sip_id = "my_sip_id";

testDB.users.insert(user);

// hoặc insert danh sách dbo
QVector<User> listUser;
for(auto i = 0; i < 20; i++) {
    User user;
    user.m_id = 0;
    user.m_data = QByteArray::fromRawData("testData", 8);
    user.m_name = "my_name_" + QString::number(i);
    user.m_sip_id = "my_sip_" + QString::number(i);

    listUser.push_back(user);
}

testDB.users.insert(listUser);
```

Update dữ liệu

```cpp
User user;
user.m_id = 0;
user.m_name = "kakakak";
user.m_data = "daaaaaa";

testDB.users.update(user, {"name", "data"});
```

- Tương tự với insert, cũng có thể update danh sách dbo.

Truy vấn dữ liệu

```cpp

// truy vấn trên 1 bảng
auto result = testDB.users.query("where id = 3")
                           .select("sip_id, name");

if(result.retCode == DBCode::OK) {
    for(auto& iUser : result.value) {
        qDebug() << iUser.m_name;
        qDebug() << iUser.m_sip_id;
    }
}

// truy vấn trên nhiều bảng
auto result = testDB.users
                .query("where #1.id > 1")
                .with<Message>("body")
                .with<Email>("address")
                .select("id, name");
    
```

Xoá dữ liệu

```cpp
User user;
user.m_id = 0;
testDB.users.remove(user);
```

Các thao tác xoá và update đều dựa trên id của dbo.  
Để thực hiện các thao tác truy vấn phức tạp hơn thì dùng DBHelper::execQuery(const ComplexQueryParams& params). Chi tiết xem [ComplexQuery](./doc/README.md).  

Truy vấn bất đồng bộ

```cpp
testDB.users.asyncInsert(user)
    .then(&app, [](const User& users){
        qDebug() << "ok";
        qDebug() << users.m_id;
    })
    .onFailed(&app, [](const EZException& e){
        qDebug() << "error : " << (int)e.code();
    });

testDB.users
    .asyncUpdate("set sip_id = 'blalalalal' where id > 23;")
    .then(&app, [] () {
        qDebug() << "code : ";
    })
    .onFailed(&app, [](const EZException& e){
        qDebug() << (int)e.code();
    });

testDB.users.query("where #1.id > 7")
    .with<Email>("*")
    .asyncSelect("*")
    .then([](Result<QVector<User>, DBCode> result){
        qDebug() << (int)result.value.size();
    })
    .onFailed([](const EZException& e){
        qDebug() << (int)e.code();
    });
```

Cơ chế bất đồng được xây dựng dựa trên QtConcurrent của Qt6 (tìm hiểu thêm Qt6/QtConcurrent để có thêm thông tin) và tạo một threadpool riêng. Có thể set max thread count thông qua DBInitParam.  
Để an toàn, cần gọi hàm close của EasySQLCipher phải được gọi trên cùng thread mà nó được khỏi tạo. Các truy vấn đồng bộ cũng phải thực hiện hiện trên cùng thread mà nó được khởi tạo.  
Không khuyến khích việc truy vấn đồng bộ trên một thread nào khác ngoài threadpool của DBContext. Nên dùng các hàm asyncXXXX để thực hiện truy vấn bất đồng bộ.  

```cpp
// Không khuyến khích việc này, nhưng lib cũng không chặn việc truy vấn ngoài threadpool. Khi bắt buộc phải thực hiện phương thức truy vấn này thì người dùng phải tự quản lý các vấn đề liên quan đến multithread.
std::thread t([testDB](){
    auto result = testDB.users.query("where id = 3")
                           .select("sip_id, name");
});
```



