rm -r ./build/ ./pnet*
cmake -B build -S p-net -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DUSE_SCHED_FIFO=ON
cmake --build build --target install
./build/pn_dev -i eth0


# TODO: prüfen wegen Lizenzen:
# * IPC verwenden (z.B.: dbus) um mit anderen Anwendungen (CoProLib) zu kommunizieren.
# * Dann nur p-net unter GPLv3 offenlegen und andere Bibs nicht!