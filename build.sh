
cd ./vendors/sqlcipher
./configure --enable-tempstore=yes --prefix=$(pwd)/dist CFLAGS="-DSQLITE_HAS_CODEC" LDFLAGS="-lcrypto" && make && make install
cd ../..

if [ $? -eq 0 ]; then
	mkdir -p build
	cmake --build $(pwd)/build --target all
else
    echo "build sqlcipher error"
fi
