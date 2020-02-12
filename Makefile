all:
	cd Serwer && make
	cd Arbiter && make
	cd Klient && make
	mkdir -p ODP
	ln -f -s ../Serwer/Serwer ODP/Serwer
	ln -f -s ../Arbiter/Arbiter ODP/Arbiter
	ln -f -s ../Klient/Klient ODP/Klient
