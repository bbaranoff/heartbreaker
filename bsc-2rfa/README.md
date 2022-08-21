compile like osmocom-bb original... 
need old libosmocore 0.9.0

git clone https://github.com/osmocom/libosmocore
cd libosmocore
git checkout 0.9.0
autoreconf -fi
./configure
make -j$(nproc)
make install

git clone https://github.com/bbaranoff/bb-2rFA
cd bb-2rFA/src
make HOST_layer23_CONFARGS=--enable-transceiver

cd libosmocore
git checkout master
autoreconf -fi
./configure
make -j$(nproc)
make install

git clone https://github.com/bbaranoff/bsc-2rFA
cd bsc-2rFA
autoreconf -fi
./configure
make -j$(nproc)
make install

