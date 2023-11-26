mkdir _build && cd _build
cmake -S ../ -B ./
cmake --build .

rm -rf client*autogen
rm -rf host*autogen

mv client* ../
mv host* ../

cd ../
rm -r _build