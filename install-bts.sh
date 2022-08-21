#!/bin/bash
mkdir /heartbreaker
cd /heartbreaker
apt install autoconf-archive libdbd-sqlite3 gcc-9 g++-9 gcc-10 g++-10 git autoconf pkg-config libtool build-essential libtalloc-dev libpcsclite-dev gnutls-dev python2 python2-dev fftw3-dev libsctp-dev libdbi-dev -y
cp /usr/bin/python2 /usr/bin/python
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 --slave /usr/bin/g++ g++ /usr/bin/g++-10
update-alternatives --set gcc /usr/bin/gcc-9
git clone git://git.osmocom.org/libosmocore.git
cd  libosmocore
git checkout 1.1.0
autoreconf -fi
./configure
make
make install
ldconfig
cd ..
git clone git://git.osmocom.org/libosmo-dsp.git
cd libosmo-dsp
libtoolize && autoreconf -fi
autoreconf -fi
./configure
make
make install
ldconfig
apt install -y libortp-dev
cd ..

git clone https://github.com/osmocom/osmocom-bb
cd osmocom-bb/src
git checkout fixeria/trxcon
make nofirmware

cd ../..
git clone https://github.com/osmocom/libosmo-abis
cd libosmo-abis
git checkout 0.8.1
autoreconf -fi && ./configure --disable-dahdi && make -j4 && make install && ldconfig

cd ..
git clone https://github.com/osmocom/libosmo-netif
cd libosmo-netif
git checkout 0.6.0
autoreconf -fi && ./configure && make -j4 && make install && ldconfig


cd bsc-2rfa/openbsc
autoreconf -fi && ./configure && make -j4
cd ../..
git clone https://github.com/osmocom/osmo-bts
cd osmo-bts
git checkout 0.8.1
autoreconf -fi && ./configure --enable-trx && make -j4 && make install && ldconfig

