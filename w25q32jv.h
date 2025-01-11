#include <Arduino.h>
#include <SPI.h>

#define STATUS_OK 0
#define STATUS_ERR 1

class Flash {
  private:
    int flashCSPin;
    void eraseMemory(uint8_t cmd, uint32_t address);

  public:

    void initFlash(int cs);
    uint8_t writeCommand(uint8_t cmd);
    uint8_t writeCommand(uint8_t cmd, uint8_t *regdata, uint8_t size);
    void writeAddress(uint8_t cmd, uint32_t address);
    bool isBusy();
    void waitForFlash();
    void writeRegister(uint8_t cmd, uint8_t *regData, uint8_t size);
    bool isWriteEnabled();
    void readBlockProtectBits(uint8_t *output);
    
    void writeEnable();
    void volatileSRWriteEnable();
    void writeDisable();
    void releasePowerdown(uint8_t *result);
    void manufacturerDeviceId(uint8_t *result);
    void jedecid(uint8_t *result);
    void readUniqueId(uint8_t *result);
    void readData(uint32_t address, uint8_t *result, uint32_t size);
    void fastRead(uint32_t address, uint8_t *result, uint32_t size);
    void pageProgram(uint32_t address, uint8_t *data, uint32_t size);
    void sectorErase(uint32_t address); //4kb
    void blockErase(uint32_t address, uint8_t cmd);
    void chipErase();
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
    void powerDown();
    void enableReset();
    void resetDevice();
    void releasePowerdown();
};
