#include "modbustcp.h"


WSAData g_data;
SOCKET g_clientSocket = INVALID_SOCKET;
sockaddr_in g_connectService;

static void PrintData(const unsigned char *array, const char *space, const size_t len, const char *header)
{
    std::cout << header << "\n";
    for (int i = 0; i < len; i++) {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(array[i]) << space;
    }
    std::cout << "\n";
}

ModbusTcpClient::ModbusTcpClient()
{

}

ModbusTcpClient::~ModbusTcpClient()
{
    WSACleanup();
    closesocket(g_clientSocket);
}

bool ModbusTcpClient::Connect(const char *ip, const UINT16 &port)
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
	g_connectService.sin_addr.s_addr = inet_addr(ip);

    //connect 
	if (connect(g_clientSocket, (sockaddr*)&g_connectService, sizeof(g_connectService)) == SOCKET_ERROR) {
		std::cerr << "Can't connect to server " << WSAGetLastError() << "\n";
		WSACleanup();
		return false;
	}

    return true;
}

std::vector<bool> ModbusTcpClient::ReadCoils(UINT32 addr, UINT32 len)
{
    std::vector<bool> vec;

    return vec;
}

std::vector<int> ModbusTcpClient::ReadRegisters(const UINT16 transId, const UINT8 slaveId, const UINT16 addr, const UINT16 len, const UINT16 timeout)
{
    std::vector<int> responseVec;
    const UINT16 protocolId = 0x0000; // default protocol id
    const UINT16 mbPkgLen = 0x0006; // 12 is default Read Register modbus request data length
    const UINT8 functionCode = 0x03; // read register fuunction code of Modbus Protocol
    const size_t tcpPkgLen = 12; // 12 is default Read Register request raw data length

    // generating raw tcp package for modbus request
    unsigned char request[tcpPkgLen];
    request[0] = (transId & 0xFF00) << 2; // splitting a 16 bit into the 2x 8 bit variables
    request[1] = transId & 0x00FF;
    request[2] = (protocolId & 0xFF00) << 2;
    request[3] = protocolId & 0x00FF;
    request[4] = (mbPkgLen & 0xFF00) << 2;
    request[5] = mbPkgLen & 0x00FF;
    request[6] = slaveId;
    request[7] = functionCode;
    request[8] = (addr & 0xFF00) << 2;
    request[9] = addr & 0x00FF;
    request[10] = (len & 0xFF00) << 2;
    request[11] = len & 0x00FF;

    int res = send(g_clientSocket, (const char *)request, tcpPkgLen, 0); // sending modbus request tcp pakcage to the modbus slave
    if (res == SOCKET_ERROR) {
        std::cerr << "Socket send error!\n";
        WSACleanup();
        closesocket(g_clientSocket);
        return responseVec;
    }

    char receiveBuffer[1024] = {0}; // 1024 static for now, we'll get there soon
    int rbyteCount = recv(g_clientSocket, receiveBuffer, 1024, 0);
	if (rbyteCount < 0) {
		std::cerr << "Socket recv error!\n";
        WSACleanup();
        closesocket(g_clientSocket);
        return responseVec;
	}
    receiveBuffer[rbyteCount + 1] = '\0'; // adding terminator to end of the received package.

    UINT16 respTransId = (receiveBuffer[0] << 2) + receiveBuffer[1];
    UINT16 respProtocolId = (receiveBuffer[2] << 2) + receiveBuffer[3];
    UINT16 respPkgLen = (receiveBuffer[4] << 2) + receiveBuffer[5];
    UINT8 respSlaveId = receiveBuffer[6];
    UINT8 respPfunctionCode = receiveBuffer[7];
    UINT8 respByteCount = receiveBuffer[8];
    for (int i = 9; i < rbyteCount; i += 2) {
        int temp = (receiveBuffer[i] << 2) + receiveBuffer[i + 1];
        responseVec.push_back(static_cast<int>(temp));
    }
 
    return responseVec;

}
