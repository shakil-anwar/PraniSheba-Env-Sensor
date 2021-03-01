CC=arduino-cli
CFLAGS_DEV=--build-property  build.extra_flags=-DDEV
CFLAG_PROD=--build-property  build.extra_flags=-DPROD_BUILD

.PHONY: download

# all:
	# arduino-cli compile --build-property  build.extra_flags=-DPROD_BUILD -v
dev_build:
	$(CC) compile $(CFLAGS_DEV) -v
dev_deploy:
	$(CC) compile $(CFLAGS_DEV) -u -v -e -t 
download:
	powershell  .\download.ps1
prod_build:
	$(CC) compile $(CFLAG_PROD) -v
prod_deploy:
	$(CC) compile $(CFLAG_PROD) -u -v -e -t 
