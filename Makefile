
all: package

package: dynr-dns/usr/sbin/pbdnsd
	fakeroot dpkg-deb --build dynr-dns dynr-dns_`grep Version dynr-dns/DEBIAN/control |sed -e 's/.* //'`_`grep Architecture dynr-dns/DEBIAN/control |sed -e 's/.* //'`.deb

dynr-dns/usr/sbin/pbdnsd :
	$(MAKE) -C  src/pbdns
	cp src/pbdns/pbdnsd dynr-dns/usr/sbin/
