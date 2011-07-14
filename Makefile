
all: package

package: dynr-dns/usr/sbin/pbdnsd
	fakeroot dpkg-deb --build dynr-dns dynr-dns_`grep Version dynr-dns/DEBIAN/control |sed -e 's/.* //'`_amd64.deb

dynr-dns/usr/sbin/pbdnsd :
	$(MAKE) -C  src/pbdns
	cp src/pbdns/pbdnsd dynr-dns/usr/sbin/
