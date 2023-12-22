mkdir build

cmake -B ./build -S ./
cd build
make

cp -a ./Lab3 ./../

cd ..
rm -r ./build
