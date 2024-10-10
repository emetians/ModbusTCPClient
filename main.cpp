#include <iostream>
#include <vector>

#include "modbustcp.h"

int main()
{
    std::cout << "Modbus TCP Client Testing; \n";

    ModbusTcpClient mb;
    if (!mb.Connect("127.0.0.1", 502)) {
        std::cerr << "Couldnt init modbus client!\n";
        return -1;
    }
    
    std::vector<int> vec = mb.ReadRegisters(0x0F, 0x01, 0, 10, 5000);

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