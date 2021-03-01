SERIAL_PORT=COM14


FQBN_PCB_V1=arduino:avr:uno
FQBN_PCB_v2=arduino:avr:pro:cpu=8MHzatmega328

PCB_FLAG_v1 = build.extra_flags=-DPCB_V_0_1_0
PCB_FLAG_v2 = build.extra_flags=-DPCB_V_0_2_0

CC=arduino-cli
CFLAG_PCB1_DEV = --build-property  build.extra_flags=-DDEV  build.extra_flags=-DPCB_V_0_1_0
CFLAG_PCB2_DEV = --build-property  build.extra_flags=-DDEV  build.extra_flags=-DPCB_V_0_2_0

CFLAGS_DEV=--build-property  build.extra_flags=-DDEV
CFLAG_PROD=--build-property  build.extra_flags=-DPROD_BUILD

.PHONY: download

# all:
	# arduino-cli compile --build-property  build.extra_flags=-DPROD_BUILD -v
build_v1:
	$(CC) compile -b $(FQBN_PCB_V1) $(CFLAG_PCB1_DEV) -v
build_v2:
	$(CC) compile -b $(FQBN_PCB_v2) $(CFLAG_PCB2_DEV) -v
deploy_v1: 
	$(CC) compile -b $(FQBN_PCB_V1)  $(CFLAG_PCB1_DEV) -p $(SERIAL_PORT) -u -v -e -t 
deploy_v2:
	$(CC) compile -b $(FQBN_PCB_v2)  $(CFLAG_PCB2_DEV) -p $(SERIAL_PORT)   -u -v -e -t 
download:
	powershell  .\download.ps1
prod_build:
	$(CC) compile $(CFLAG_PROD) -v
prod_deploy:
	$(CC) compile $(CFLAG_PROD) -u -v -e -t 
