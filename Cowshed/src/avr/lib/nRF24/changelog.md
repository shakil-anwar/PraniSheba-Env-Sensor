# Changelog Needed

1. no function to check isChipConnected or not. Typically "reading address width" do this work. or reading CRC bit would work.

2. no function to check rxFifoFull or not. read FIFO_STATUS register for this.

3. A powerDown function is must to operate nrf in low power mode. set PWR_UP bit to 0 in NRF_CONFIG register.

4. similarly a powerUp funtion is also needed. set PWR_UP bit to 1 in NRF_CONFIG register.

5. need a function to writeAckPayload. 

6. a function to check ackPayloadAvailable or not.

7. a funtion to getDynamicPayloadSize 

8. a function to get status. typically sends a RF24_NOP.

9. switch between RX and TX mode easily for base station and bolus to sync time schedule.