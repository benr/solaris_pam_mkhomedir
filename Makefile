#
#	pam_mkhomedir.so.1 Makefile
#	benr@cuddletech.com 6/9/11
#
CC=	/opt/SUNWspro/bin/cc

all: pam_mkhomedir.so

pam_mkhomedir.so: pam_mkhomedir.c
	$(CC) -fPIC -c homedir.c
	$(CC) -fPIC -I . -lpam -c pam_mkhomedir.c 
	$(CC) -shared -Wl,-soname,pam_mkhomedir.so.1 -o pam_mkhomedir.so.1 pam_mkhomedir.o homedir.o
	

install: pam_mkhomedir.so.1
	cp pam_mkhomedir.so.1 /usr/lib/security
	#cd /usr/lib/security && ln -s ./pam_mkhomedir.so.1 pam_mkhomedir.so

clean:
	rm -f *.o pam_mkhomedir.so.1
