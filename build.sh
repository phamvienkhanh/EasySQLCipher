
cd ./vendors/sqlcipher

# export LDFLAGS="-L/opt/homebrew/opt/openssl/lib"
# export CXXFLAGS="-I/opt/homebrew/opt/openssl/include"

if [[ $OSTYPE == 'darwin'* ]];
then 
	./configure --enable-tempstore=yes --with-crypto-lib=commoncrypto \
		--prefix=$(pwd)/dist \
		CFLAGS="-DSQLITE_HAS_CODEC" LDFLAGS="-framework Security -framework Foundation" \
	&& make \
	&& make install
	
else
./configure --enable-tempstore=yes --prefix=$(pwd)/dist CFLAGS="-DSQLITE_HAS_CODEC" LDFLAGS="-lcrypto" && make && make install
fi
cd ../..

if [ $? -eq 0 ]; then
	mkdir -p build
	cmake --build $(pwd)/build --target all
else
    echo "build sqlcipher error"
fi
