#include <iostream>
#include <vector>

#include "modbustcp.h"

int main()
{
    std::cout << "Modbus TCP Client Testing; \n";

    ModbusTcpClient mb;
    if (!mb.Init("127.0.0.1", 502)) {
        std::cerr << "Couldnt init modbus client!\n";
        return -1;
    }
    
    std::vector<INT32> vec = mb.ReadRegisters(0x0F, 0x01, 0, 80, 5000);

    if (vec.empty()) {
        std::cerr << "Error on Reading registers \n";
        return -1;
    }

    std::cout << "\n\n\n";
    std::cout << "Results: ";
    for (const auto &reg : vec) {
        std::cout << reg << ", ";
    }
    std::cout << std::endl;


    return 0;
}