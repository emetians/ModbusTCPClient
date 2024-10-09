#include "modbustcp.h"


WSAData g_data;
SOCKET g_clientSocket = INVALID_SOCKET;
sockaddr_in g_connectService;

static std::vector<INT32>SendRequest(const MbRequest &request)
{
    std::vector<INT32> responseVec;
    UINT8 len = request.pkgLen + 6; // tcp package also has 6 byte for trans id(2), protocol id(2) and pkglen(2) 
    unsigned char data[len];
    
    data[0] = (request.transId & 0xFF00) << 2;
    data[1] = request.transId & 0x00FF;
    data[2] = (request.protocolId & 0xFF00) << 2;
    data[3] = request.protocolId & 0x00FF;
    data[4] = (request.pkgLen & 0xFF00) << 2;
    data[5] = request.pkgLen & 0x00FF;
    data[6] = request.slaveId;
    data[7] = request.functionCode;
    data[8] = (request.addr & 0xFF00) << 2;
    data[9] = request.addr & 0x00FF;
    data[10] = (request.dataLen & 0xFF00) << 2;
    data[11] = request.dataLen & 0x00FF;
    

    std::cout << std::dec << "raw data: (len = " << static_cast<int>(len) << ") \n";

    for (int i = 0; i < len; i++) {
        //std::cout << "i: " << i << " " << static_cast<int>(data[i]) << " \n";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << "";
    }
    std::cout << "\n";
    //std::cout << "data[10]: " << static_cast<int>(data[10]) << " data[11]: " << static_cast<int>(data[11]) << "\n";

    int res = send(g_clientSocket, (const char *)data, len, 0);
    if (res == SOCKET_ERROR) {
        std::cerr << "Socket send error!\n";
        WSACleanup();
        closesocket(g_clientSocket);
    }

    char receiveBuffer[1024] = {0};
    int rbyteCount = recv(g_clientSocket, receiveBuffer, 1024, 0);
	if (rbyteCount < 0) {
		std::cerr << "Socket recv error!\n";
        WSACleanup();
        closesocket(g_clientSocket);
	}
    receiveBuffer[rbyteCount + 1] = '\0';

    std::cout << std::dec << "RECEIVED data: (len = " << static_cast<int>(rbyteCount) << ") \n";

    for (int i = 0; i < rbyteCount; i++) {
        //std::cout << "i: " << i << " " << static_cast<int>(data[i]) << " \n";
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(receiveBuffer[i]) << "";
    }
    std::cout << "\n";

    UINT16 transId = (receiveBuffer[0] << 2) + receiveBuffer[1];
    UINT16 protocolId = (receiveBuffer[2] << 2) + receiveBuffer[3];
    UINT16 pkgLen = (receiveBuffer[4] << 2) + receiveBuffer[5];
    UINT8 slaveId = receiveBuffer[6];
    UINT8 functionCode = receiveBuffer[7];
    UINT8 byteCount = receiveBuffer[8];
    std::cout << std::dec << "Byte Count: " << static_cast<int>(byteCount) << " \n";
   
    for (int i = 9; i < rbyteCount; i += 2) {
        INT16 temp = (receiveBuffer[i] << 2) + receiveBuffer[i + 1];
        responseVec.push_back(static_cast<UINT32>(temp));
    }

    // UINT16 temp = (receiveBuffer[9] << 2) + receiveBuffer[10];
    // responseVec.push_back(static_cast<UINT32>(temp));

    // temp = (receiveBuffer[11] << 2) + receiveBuffer[12];
    // responseVec.push_back(static_cast<UINT32>(temp));

    // temp = (receiveBuffer[13] << 2) + receiveBuffer[14];
    // responseVec.push_back(static_cast<UINT32>(temp));

    // temp = (receiveBuffer[15] << 2) + receiveBuffer[16];
    // responseVec.push_back(static_cast<UINT32>(temp));


    std::cout << std::dec << "transID " << static_cast<int>(transId) << "\n";
    std::cout << std::dec << "protocolID " << static_cast<int>(protocolId) << "\n";
    std::cout << std::dec << "pkgLen " << static_cast<int>(pkgLen) << "\n";
    

    return responseVec;
}

ModbusTcpClient::ModbusTcpClient()
{

}

ModbusTcpClient::~ModbusTcpClient()
{
    WSACleanup();
    closesocket(g_clientSocket);
}

bool ModbusTcpClient::Init(const std::string &ip, const UINT16 &port)
{
    // Initialize WinSock
	
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &g_data);
	if (wsResult != 0) {
		std::cerr << "Can't start Winsock " << wsResult << "\n";
		return false;
	}

    // Create socket
	g_clientSocket = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP
    if (g_clientSocket == INVALID_SOCKET) {
		std::cerr << "Error at clientsocket " << WSAGetLastError() << "\n";
		WSACleanup();
		return false;
	}

    
	g_connectService.sin_family = AF_INET; // ipv4
	g_connectService.sin_port = htons(port);
	g_connectService.sin_addr.s_addr = inet_addr(ip.c_str());

    //connect 
	if (connect(g_clientSocket, (sockaddr*)&g_connectService, sizeof(g_connectService)) == SOCKET_ERROR) {
		std::cerr << "Can't connect to server " << WSAGetLastError() << "\n";
		WSACleanup();
		return false;
	}

    return true;
}

std::vector<INT32> ModbusTcpClient::ReadCoils(UINT32 addr, UINT32 len)
{
    std::vector<INT32> vec;

    return vec;
}

//mb.ReadRegisters(0x01, 0x01, 0, 10, 5000);
std::vector<INT32> ModbusTcpClient::ReadRegisters(const UINT16 transId, const UINT8 slaveId, const UINT16 addr, const UINT16 len, const UINT16 timeout)
{
    MbRequest request;
    UINT8 functionCode = 0x03;
    request.transId = transId;
    request.pkgLen = sizeof(slaveId) + sizeof(functionCode) + sizeof(addr) + sizeof(len);
    request.slaveId = slaveId;
    request.functionCode = functionCode;
    request.addr = addr;
    request.dataLen = len;

    // std::cout << "expected len : 6 " << "calculated len: " << request.pkgLen << "\n";
    // std::cout << "sizeof(slaveId): " << sizeof(slaveId) << " sizeof(READ_REGS): " << sizeof(functionCode) << " sizeof(addr): " << sizeof(addr) << " sizeof(len): " << sizeof(len) << "\n";

    std::string space = " ";
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << request.transId << space;
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << request.protocolId << space;
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << request.pkgLen << space;
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(request.slaveId) << space;
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(request.functionCode) << space;
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << request.addr << space;
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << request.dataLen << space;
    std::cout << "\n";

    std::vector<INT32> vec = SendRequest(request);
    return vec;
}
