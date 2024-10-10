#ifndef __MODBUSTCP_H
#define __MODBUSTCP_H

#include <iostream>
#include <iomanip>
#include <winsock2.h>
#include <string>
#include <vector>

#define READ_COILS 0x01
#define READ_INPUT_BITS 0x02
#define READ_REGS 0x03
#define READ_INPUT_REGS 0x04
#define WRITE_COIL 0x05
#define WRITE_REG 0x06
#define WRITE_COILS 0x0F
#define WRITE_REGS 0x10


struct MbRequest {
    UINT16 transId;
    UINT16 protocolId = 0x00;
    UINT16 pkgLen;
    UINT8 slaveId;
    UINT8 functionCode;
    UINT16 addr;
    UINT16 dataLen;
};

class ModbusTcpClient {
private:

public:
    ModbusTcpClient();
    ~ModbusTcpClient();

    bool Connect(const char *ip, const UINT16 &port);
    std::vector<bool> ReadCoils(UINT32 addr, UINT32 len);
    std::vector<int> ReadRegisters(const unsigned short int transId, const UINT8 slaveId, const unsigned short int addr, const unsigned short int len, const unsigned short int timeout);
};

#endif