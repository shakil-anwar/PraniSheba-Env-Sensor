# Before running this script please run following command in administrator mode in powershell
# set-executionpolicy unrestricted


cd  ./src
md lib
cd ./lib

# clone own repository
git clone https://github.com/shuvangkar/FlashMemory.git 
git clone https://github.com/shuvangkar/realTime.git 
git clone https://github.com/shuvangkar/RingEEPROM.git 
git clone https://github.com/shuvangkar/MQSensor.git
git clone https://github.com/shuvangkar/simple-json.git
git clone https://github.com/shuvangkar/asyncXfer.git
git clone https://github.com/shuvangkar/MemQ.git
git clone https://github.com/shuvangkar/ramQ.git
git clone https://github.com/shuvangkar/SerialPipe.git
git clone https://github.com/shuvangkar/nRF24.git
git clone https://github.com/shuvangkar/arduinoCwrapper.git
git clone https://github.com/shuvangkar/AVR_Watchdog.git
git clone https://github.com/shuvangkar/Timer1.git
git clone https://github.com/shuvangkar/tScheduler.git

# Clone arduino repository
# git clone https://github.com/adafruit/RTClib.git

# Remove all .git directory
Get-ChildItem -path .\  -Include '.git' -Recurse -force | Remove-Item -force -Recurse
cd ../..