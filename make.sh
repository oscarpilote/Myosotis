echo "Building DEBUG version."
echo "-----------------------"
make --no-print-directory -C build/debug

echo
echo "Building RELEASE version."
echo "-----------------------"
make --no-print-directory -C build/release

mkdir -p bin
echo
echo "Copying executables to bin idrectory."
echo "-------------------------------------"
echo "-> bin/myosotis_debug"
cp build/debug/src/main bin/myosotis_debug
echo "-> bin/myosotis"
cp build/release/src/main bin/myosotis
echo
echo "Build completed."
