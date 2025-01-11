#include "w25q32jv.h"

/**
Prepare the flash chip for use.
*/
void Flash::initFlash(int cs) {
  flashCSPin = cs;
  pinMode(flashCSPin, OUTPUT);
  digitalWrite(flashCSPin, HIGH);
}

/**
 * Check if the flash chip is busy.
 * @return True if busy, false otherwise.
 */
bool Flash::isBusy() {
  uint8_t status;
  readStatusRegister1(&status);
  return status & 0x01;
}
/**
 * Check if write is enabled (WEL bit).
 * @return True if write is enabled, false otherwise.
 */
bool Flash::isWriteEnabled() {
  uint8_t status;
  readStatusRegister1(&status);
  return (status & 0x02) != 0;
}

/**
 * Read block protect bits.
 * @param output Array to store block protect bits.
 */
void Flash::readBlockProtectBits(uint8_t *output) {
  uint8_t status;
  readStatusRegister1(&status);
  for (int i = 0; i < 3; ++i) {
      output[i] = (status >> (2 + i)) & 0x01;
  }
}

/**
 * Wait for the flash chip to complete its write.
 */
void Flash::waitForFlash() {
  uint8_t reg1[1];
  uint8_t busy = 1;
  while(busy) {
    writeCommand(0x05, reg1, 1);
    busy = ((reg1[0]) & 0x01);
    delay(1);
  }
}

/**
 * Send a command to the flash chip.
 * @param cmd Command byte.
 * @return STATUS_OK if successful.
 */
uint8_t Flash::writeCommand(uint8_t cmd) {
  digitalWrite(flashCSPin, LOW);
  uint8_t status = SPI.transfer(cmd);
  digitalWrite(flashCSPin, HIGH);
  return status;
}

/**
 * Send a command to the flash chip and read data into a buffer.
 * @param cmd Command byte.
 * @param regdata Buffer to store data.
 * @param size Number of bytes to read.
 */
uint8_t Flash::writeCommand(uint8_t cmd, uint8_t *regdata, uint8_t size) {
  digitalWrite(flashCSPin, LOW);
  SPI.transfer(cmd);
  for(int i = 0; i < size; i++) {
    regdata[i] = SPI.transfer(0x00);
  }
  digitalWrite(flashCSPin, HIGH);
}

/**
 * Write a 24-bit address to the flash chip.
 * @param cmd Command byte.
 * @param address 24-bit address.
 */
void Flash::writeAddress(uint8_t cmd, uint32_t address) {
SPI.transfer(cmd);
    for (int8_t shift = 16; shift >= 0; shift -= 8) {
        SPI.transfer((address >> shift) & 0xFF);
    }
}

/**
 * Enable the write enable bit to allow chip to accept write/erase commands
 */
void Flash::writeEnable() {
  writeCommand(0x06);
}

/**
 * The non-volatile Status Register bits described in section 7.1 can also be written to as volatile bits.
 */
void Flash::volatileSRWriteEnable() {
  writeCommand(0x50);
}

/**
 * The Write Disable instruction (Figure 7) resets the Write Enable Latch (WEL) bit in the Status Register to a 0.
 */
void Flash::writeDisable() {
  writeCommand(0x04);
}



/**
 * The Read Manufacturer/Device ID instruction is an alternative to the Release from Power-down / Device
 * ID instruction that provides both the JEDEC assigned manufacturer ID and the specific device ID.
*/
void Flash::manufacturerDeviceId(uint8_t *result) {
  writeCommand(0x90, result, 5);
}

/**
 * For compatibility reasons, the W25Q32JV provides several instructions to electronically determine the
 * identity of the device.
*/
void Flash::jedecid(uint8_t *result) {
  writeCommand(0x9F, result, 3);
}

/**
 * The Read Unique ID Number instruction accesses a factory-set read-only 64-bit number that is unique to
 * each W25Q32JV device.
*/
void Flash::readUniqueId(uint8_t *result) {
  writeCommand(0x48, result, 5);
}

/**
 * Read data from memory.
 * @param address Starting address.
 * @param result Buffer to store data.
 * @param size Number of bytes to read.
 */
void Flash::readData(uint32_t address, uint8_t *result, uint32_t size) {
  uint8_t status = STATUS_OK;
  digitalWrite(flashCSPin, LOW);
  writeAddress(0x03, address);
  for(uint32_t i = 0; i < size; ++i) {
    result[i] = SPI.transfer(0x00);
  }
  digitalWrite(flashCSPin, HIGH);
}

/**
 * The Fast Read instruction is similar to the Read Data instruction except that it can operate at the highest
 * possible frequency of FR (see AC Electrical Characteristics).
*/
void Flash::fastRead(uint32_t address, uint8_t *result, uint32_t size) {
  uint8_t status = STATUS_OK;
  digitalWrite(flashCSPin, LOW);
  writeAddress(0x0B, address);
  for(uint32_t i = 0; i < size; i++) {
    result[i] = SPI.transfer(0x00);
  }
  digitalWrite(flashCSPin, HIGH);
}

/**
 * Program a page (up to 256 bytes).
 * @param address Starting address.
 * @param data Data to write.
 * @param size Number of bytes to write.
 */
void Flash::pageProgram(uint32_t address, uint8_t *data, uint32_t size) {
  writeEnable();
  digitalWrite(flashCSPin, LOW);
  writeAddress(0x02, address);
  for(int i = 0; i < size; ++i) {
    SPI.transfer(data[i]);
  }
  digitalWrite(flashCSPin, HIGH);
  waitForFlash();
}

/**
 * Erase a sector (4KB).
 * @param address Sector address.
 */
void Flash::sectorErase(uint32_t address) { //4kb
  eraseMemory(0x20, address);
} 

/**
 * Erase a block (32KB or 64KB).
 * @param address Block address.
 * @param cmd Command byte (0x52 for 32KB, 0xD8 for 64KB).
 */
void Flash::blockErase(uint32_t address, uint8_t cmd) {
    eraseMemory(cmd, address);
}

/**
 * Erase memory using a specific command and address.
 * @param cmd Command byte.
 * @param address Memory address.
 */
void Flash::eraseMemory(uint8_t cmd, uint32_t address) {
    writeEnable();
    digitalWrite(flashCSPin, LOW);
    writeAddress(cmd, address);
    digitalWrite(flashCSPin, HIGH);
    waitForFlash();
}

/**
 * The Chip Erase instruction sets all memory within the device to the erased state of all 1s (FFh).
 */
void Flash::chipErase() {
  writeEnable();
  writeCommand(0x60);
  waitForFlash();
}

/**
 * Write to the given register
 */
void Flash::writeRegister(uint8_t cmd, uint8_t *regData, uint8_t size) {
  waitForFlash();
  digitalWrite(flashCSPin, LOW);
  SPI.transfer(cmd);
  for(uint8_t i = 0; i < size; i++) {
    SPI.transfer(regData[i]);
  }
  digitalWrite(flashCSPin, HIGH);
}

/**
 * Read status register 1
 */
void Flash::readStatusRegister1(uint8_t *result) {
  writeCommand(0x05, result, 1);
}

/**
 * Write status register 1 
 */
void Flash::writeStatusRegister1(uint8_t *regData) {
    writeRegister(0x01, regData, 1);
}

/**
 * Read status register 2
 */
void Flash::readStatusRegister2(uint8_t *result) {
  writeCommand(0x35, result, 1);
}

/**
 * Write status register 2 
 */
void Flash::writeStatusRegister2(uint8_t *regData) {
  writeRegister(0x31, regData, 1);
}

/**
 * Read status register 3
 */
void Flash::readStatusRegister3(uint8_t *result) {
  writeCommand(0x15, result, 1);
}

/**
 * Write status register 3
 */
void Flash::writeStatusRegister3(uint8_t *regData) {
  writeRegister(0x11, regData, 1);
}

/**
 * The W25Q32JV features a 256-Byte Serial Flash Discoverable Parameter (SFDP) register that contains
 * information about device configurations, available instructions and other features.
 */
void Flash::readSFDPRegister() {

}

/**
 * The W25Q32JV offers three 256-byte Security Registers which can be erased and programmed
 * individually.
 */
void Flash::eraseSecurityRegister() {

}

/**
 * The Program Security Register instruction is similar to the Page Program instruction. It allows from one
 * byte to 256 bytes of security register data to be programmed at previously erased (FFh) memory locations.
 */
void Flash::programSecurityRegister() {

}

/**
 * The Read Security Register instruction is similar to the Fast Read instruction and allows one or more data
 * bytes to be sequentially read from one of the four security registers.
 */
void Flash::readSecurityRegister() {

}

/**
 * All Block/Sector Lock bits can be set to 1 by the Global Block/Sector Lock instruction.
 */
void Flash::globalBlockLock() {
  writeCommand(0x7E);
}

/**
 * All Block/Sector Lock bits can be set to 0 by the Global Block/Sector Unlock instruction.
 */
void Flash::globalBlockUnlock() {
  writeCommand(0x98);
}

/**
 * The Individual Block/Sector Lock provides an alternative way to protect the memory array from adverse
 * Erase/Program.
 */
void Flash::readBlockLock() {

}

/**
 * The Individual Block/Sector Lock provides an alternative way to protect the memory array from adverse
 * Erase/Program.
 */
void Flash::individualBlockLock() {

}

/**
 * The Individual Block/Sector Lock provides an alternative way to protect the memory array from adverse
 * Erase/Program.
 */
void Flash::individualBlockUnlock() {

}

/**
 * The Erase/Program Suspend instruction “75h”, allows the system to interrupt a Sector or Block Erase
 * operation or a Page Program operation and then read from or program/erase data to, any other sectors or
 * blocks.
 */
void Flash::eraseProgramSuspend() {

}

/**
 * The Erase/Program Resume instruction “7Ah” must be written to resume the Sector or Block Erase
 * operation or the Page Program operation after an Erase/Program Suspend.
 */
void Flash::eraseProgramResume() {

}

/**
 * Power down the device
 */
void Flash::powerDown() {
  writeCommand(0xB9);
}

/**
 * The Release from Power-down / Device ID instruction is a multi-purpose instruction. It can be used to
 * release the device from the power-down state, or obtain the devices electronic identification (ID) number.
 */
void Flash::releasePowerdown() {
  writeCommand(0xAB, result, 4);
}

/**
 * Enable device reset
 */
void Flash::enableReset() {
  writeCommand(0x66);
}

/**
 * Reset the device. enableReset() must be called first.
 */
void Flash::resetDevice() {
  writeCommand(0x99);
}

/**
 * Public convenience function to reset the chip. Enables reset and resets.
 */
void Flash::reset() {
  enableReset();
  delay(1);
  resetDevice();
}