SERIAL_PORT_v2=COM6
SERIAL_PORT_v3=COM9


FQBN_PCB_V1=arduino:avr:uno
FQBN_PCB_v2=asrduino:avr:pro:cpu=8MHzatmega328
#FQBN_PCB_v3=esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none
#FQBN_PCB_v3=esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none
FQBN_PCB_v3=esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=160,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none


# PCB_FLAG_v1 = build.extra_flags=-DPCB_V_0_1_0
# PCB_FLAG_v2 = build.extra_flags=-DPCB_V_0_2_0

CC=arduino-cli
CFLAG_PCB1_DEV = --build-property  "build.extra_flags=-DDEV \"-DPCB_V_0_1_0 \""
CFLAG_PCB2_DEV = --build-property  "build.extra_flags=-DDEV \"-DPCB_V_0_2_0 \""


CFLAGS_PCB1_PROD =--build-property  "build.extra_flags=-DPROD_BUILD \"-DPCB_V_0_1_0 \""
CFLAGS_PCB2_PROD =--build-property  "build.extra_flags=-DPROD_BUILD \"-DPCB_V_0_2_0 \""
CFLAGS_PCB3_PROD =--build-property build.extra_flags="-DPROD_BUILD -DESP32_V010 -DSOFTWARE_I2C -DSTAND_ALONE"
CFLAGS_PCB32_PROD =--build-property build.extra_flags="-DPROD_BUILD -DESP32_V010 -DFRAM_V010 -DSTAND_ALONE"
CFLAGS_PCB32_SMOKE =--build-property build.extra_flags="-DPROD_BUILD -DESP32_V010 -DFRAM_V010 -DSTAND_ALONE -DDEVICE_HAS_SMOKE_SENSOR -DSUBCRIPTION_ON -DGSM_V010"

build_v1:
	$(CC) compile -b $(FQBN_PCB_V1) $(CFLAG_PCB1_DEV)
build_v2: 
	$(CC) compile -b $(FQBN_PCB_v2) $(CFLAG_PCB2_DEV)

build_prod_v1: download
	$(CC) compile -b $(FQBN_PCB_V1)  $(CFLAGS_PCB1_PROD)
build_prod_v2: #download
	$(CC) compile -b $(FQBN_PCB_v2)  $(CFLAGS_PCB2_PROD) -v -t
build_prod_v3: 
	$(CC) compile -b $(FQBN_PCB_v3)  $(CFLAGS_PCB3_PROD) 
build_prod_v32: 
	$(CC) compile -b $(FQBN_PCB_v3)  $(CFLAGS_PCB32_PROD) --warnings all
build_prod_v32_ota:
	$(CC) compile -b $(FQBN_PCB_v3)  $(CFLAGS_PCB32_PROD) --warnings all --output-dir ./bin/
build_prod_v32_smoke: 
	$(CC) compile -b $(FQBN_PCB_v3)  $(CFLAGS_PCB32_SMOKE) --warnings all


upload_v1:
	$(CC) upload -b $(FQBN_PCB_V1)   -p $(SERIAL_PORT)  -v -t
upload_v2:
	$(CC) upload -b $(FQBN_PCB_v2)   -p $(SERIAL_PORT_v2)  -v -t
upload_v3:
	$(CC) upload -b $(FQBN_PCB_v3)   -p $(SERIAL_PORT_v3)  -v -t


delete_lib:
	rm -rf ./src/lib
download:
	powershell  .\download.ps1

.PHONY: download delete_lib

# deploy_v1: 
# 	$(CC) compile -b $(FQBN_PCB_V1)  $(CFLAG_PCB1_DEV) -p $(SERIAL_PORT) -u -v -e -t 
# deploy_v2:
# 	$(CC) compile -b $(FQBN_PCB_v2)  $(CFLAG_PCB2_DEV) -p $(SERIAL_PORT)   -u -v -e -t 