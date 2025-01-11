#ifndef w25q32jv_h
#define w25q32jv_h

#include <Arduino.h>
#include <SPI.h>

#define STATUS_OK 0
#define STATUS_ERR 1

class Flash {
  private:
    int flashCSPin;
    void eraseMemory(uint8_t cmd, uint32_t address);
    uint8_t writeCommand(uint8_t cmd);
    uint8_t writeCommand(uint8_t cmd, uint8_t *regdata, uint8_t size);
    void writeAddress(uint8_t cmd, uint32_t address);
    void writeRegister(uint8_t cmd, uint8_t *regData, uint8_t size);
    bool isWriteEnabled();
    void readBlockProtectBits(uint8_t *output);
    void writeEnable();
    void volatileSRWriteEnable();
    void writeDisable();
    void fastRead(uint32_t address, uint8_t *result, uint32_t size);
    void readStatusRegister1(uint8_t *result);
    void writeStatusRegister1(uint8_t *regData);
    void readStatusRegister2(uint8_t *result);
    void writeStatusRegister2(uint8_t *regData);
    void readStatusRegister3(uint8_t *result);
    void writeStatusRegister3(uint8_t *regData);
    void readSFDPRegister();
    void eraseSecurityRegister();
    void programSecurityRegister();
    void readSecurityRegister();
    void globalBlockLock();
    void globalBlockUnlock();
    void readBlockLock();
    void individualBlockLock();
    void individualBlockUnlock();
    void eraseProgramSuspend();
    void eraseProgramResume();
    void enableReset();
    void resetDevice();
    

  public:
    void initFlash(int cs);
    void jedecid(uint8_t *result);
    void readUniqueId(uint8_t *result);
    bool isBusy();
    void reset();
    void powerDown();
    void releasePowerdown();
    void chipErase();
    void pageProgram(uint32_t address, uint8_t *data, uint32_t size);
    void sectorErase(uint32_t address);
    void blockErase(uint32_t address, uint8_t cmd);
    void readData(uint32_t address, uint8_t *result, uint32_t size);
    void readData(uint32_t address, uint16_t *result, uint32_t size) 
    void manufacturerDeviceId(uint8_t *result);
    void waitForFlash();
};

#endif