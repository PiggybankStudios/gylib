/*
File:   gy_socket.h
Author: Taylor Robbins
Date:   07\09\2023
Description:
	** Holds functions that help us open, read, write, etc. to TCP\UDP sockets
*/

#ifndef _GY_SOCKET_H
#define _GY_SOCKET_H

#if SOCKETS_SUPPORTED
//NOTE: You must link Ws2_32.lib for this file to link successfully when SOCKETS_SUPPORTED

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define BUFFERED_SOCKET_MAX_NUM_BUFFERS 32 //this is pretty cheap and can be increased if needed
#define MAX_NUM_RECEIVE_ITERATIONS      10

// +--------------------------------------------------------------+
// |                         Enumerations                         |
// +--------------------------------------------------------------+
enum SocketProtocol_t
{
	SocketProtocol_None = 0,
	SocketProtocol_Udp,
	SocketProtocol_Tcp,
	SocketProtocol_NumProtocols,
};
const char* GetSocketProtocolStr(SocketProtocol_t enumValue)
{
	switch (enumValue)
	{
		case SocketProtocol_None: return "None";
		case SocketProtocol_Udp:  return "Udp";
		case SocketProtocol_Tcp:  return "Tcp";
		default: return "Unknown";
	}
}

enum SocketType_t
{
	SocketType_None = 0,
	SocketType_SingleDestination, //aka client, bi-directional, only 1 destination
	SocketType_MultiDestination, //aka server, multiple destinations
	SocketType_NumTypes,
};
const char* GetSocketTypeStr(SocketType_t enumValue)
{
	switch (enumValue)
	{
		case SocketType_None:              return "None";
		case SocketType_SingleDestination: return "SingleDestination";
		case SocketType_MultiDestination:  return "MultiDestination";
		default: return "Unknown";
	}
}

enum SocketError_t
{
	SocketError_None = 0,
	SocketError_Generic,
	SocketError_CreationFailed,
	SocketError_CreationBindFailed,
	SocketError_CreationNonBlockingFailed,
	SocketError_ReadError,
	SocketError_WriteError,
	SocketError_NumErrors,
};
const char* GetSocketErrorStr(SocketError_t enumValue)
{
	switch (enumValue)
	{
		case SocketError_None:                      return "None";
		case SocketError_Generic:                   return "Generic";
		case SocketError_CreationFailed:            return "CreationFailed";
		case SocketError_CreationBindFailed:        return "CreationBindFailed";
		case SocketError_CreationNonBlockingFailed: return "CreationNonBlockingFailed";
		case SocketError_ReadError:                 return "ReadError";
		case SocketError_WriteError:                return "WriteError";
		default: return "Unknown";
	}
}

enum SocketWarning_t
{
	SocketWarning_None = 0,
	SocketWarning_TooManySourceAddresses,
	SocketWarning_BufferIsFull,
	SocketWarning_NumWarnings,
};
const char* GetSocketWarningStr(SocketWarning_t enumValue)
{
	switch (enumValue)
	{
		case SocketWarning_None:                   return "None";
		case SocketWarning_TooManySourceAddresses: return "TooManySourceAddresses";
		case SocketWarning_BufferIsFull:           return "BufferIsFull";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct OpenSocket_t
{
	SocketType_t type;
	SocketProtocol_t protocol;
	IpAddressAndPort_t destAddress;
	
	bool isOpen;
	SocketError_t error;
	SocketWarning_t warning;
	
	#if WINDOWS_COMPILATION
	SOCKET handle_win32;
	#else
	#error Unsupported platform for gy_socket.h OpenSocket_t
	#endif
};

struct BufferedSocketBuffer_t
{
	bool isUsed;
	IpAddressAndPort_t address;
	u64 lastReceiveTime;
	u8* pntr;
	u64 size;
	u64 used;
};
struct BufferedSocket_t
{
	MemArena_t* allocArena;
	u64 newBufferSize;
	
	BufferedSocketBuffer_t* mainBuffer;
	BufferedSocketBuffer_t* mostRecentBuffer;
	BufferedSocketBuffer_t buffers[BUFFERED_SOCKET_MAX_NUM_BUFFERS];
	OpenSocket_t socket;
};

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
// +==============================+
// |   Windows Helper Functions   |
// +==============================+
#if WINDOWS_COMPILATION

const char* Win32_GetWsaErrorStr(int wsaErrorCode)
{
	switch (wsaErrorCode)
	{
		case WSASYSNOTREADY:         return "WSASYSNOTREADY";
		case WSAVERNOTSUPPORTED:     return "WSAVERNOTSUPPORTED";
		case WSANOTINITIALISED:      return "WSANOTINITIALISED";
		case WSAEPROCLIM:            return "WSAEPROCLIM";
		case WSAEFAULT:              return "WSAEFAULT";
		case WSAENETDOWN:            return "WSAENETDOWN";
		case WSAEAFNOSUPPORT:        return "WSAEAFNOSUPPORT";
		case WSAEINPROGRESS:         return "WSAEINPROGRESS";
		case WSAEMFILE:              return "WSAEMFILE";
		case WSAEINVAL:              return "WSAEINVAL";
		case WSAEINVALIDPROVIDER:    return "WSAEINVALIDPROVIDER";
		case WSAEINVALIDPROCTABLE:   return "WSAEINVALIDPROCTABLE";
		case WSAENOBUFS:             return "WSAENOBUFS";
		case WSAEPROTONOSUPPORT:     return "WSAEPROTONOSUPPORT";
		case WSAEPROTOTYPE:          return "WSAEPROTOTYPE";
		case WSAEPROVIDERFAILEDINIT: return "WSAEPROVIDERFAILEDINIT";
		case WSAESOCKTNOSUPPORT:     return "WSAESOCKTNOSUPPORT";
		case WSAEACCES:              return "WSAEACCES";
		case WSAEINTR:               return "WSAEINTR";
		case WSAENETRESET:           return "WSAENETRESET";
		case WSAENOTCONN:            return "WSAENOTCONN";
		case WSAENOTSOCK:            return "WSAENOTSOCK";
		case WSAEOPNOTSUPP:          return "WSAEOPNOTSUPP";
		case WSAESHUTDOWN:           return "WSAESHUTDOWN";
		case WSAEWOULDBLOCK:         return "WSAEWOULDBLOCK";
		case WSAEMSGSIZE:            return "WSAEMSGSIZE";
		case WSAEHOSTUNREACH:        return "WSAEHOSTUNREACH";
		case WSAECONNABORTED:        return "WSAECONNABORTED";
		case WSAECONNRESET:          return "WSAECONNRESET";
		case WSAETIMEDOUT:           return "WSAETIMEDOUT";
		case WSAEADDRINUSE:          return "WSAEADDRINUSE";
		case WSAEALREADY:            return "WSAEALREADY";
		case WSAEADDRNOTAVAIL:       return "WSAEADDRNOTAVAIL";
		case WSAECONNREFUSED:        return "WSAECONNREFUSED";
		case WSAEISCONN:             return "WSAEISCONN";
		case WSAENETUNREACH:         return "WSAENETUNREACH";
		case WSAEDESTADDRREQ:        return "WSAEDESTADDRREQ";
		default: return "Unknown";
	}
}

sockaddr_in Win32_GetSockAddrFromIpAddressAndPort(IpAddressAndPort_t addressAndPort)
{
	sockaddr_in result = {};
	result.sin_family = (addressAndPort.address.isIpv6 ? AF_INET6 : AF_INET);
	result.sin_port = MyHostToNetworkByteOrderU16(addressAndPort.port);
	if (addressAndPort.address.isIpv6)
	{
		sockaddr_in6* result6 = (sockaddr_in6*)&result;
		result6->sin6_flowinfo = 0;
		result6->sin6_addr.u.Word[0] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part0);
		result6->sin6_addr.u.Word[1] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part1);
		result6->sin6_addr.u.Word[2] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part2);
		result6->sin6_addr.u.Word[3] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part3);
		result6->sin6_addr.u.Word[4] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part4);
		result6->sin6_addr.u.Word[5] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part5);
		result6->sin6_addr.u.Word[6] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part6);
		result6->sin6_addr.u.Word[7] = MyHostToNetworkByteOrderU16(addressAndPort.address.ipv6.part7);
	}
	else
	{
		result.sin_addr.S_un.S_un_b.s_b1 = addressAndPort.address.ipv4.part0;
		result.sin_addr.S_un.S_un_b.s_b2 = addressAndPort.address.ipv4.part1;
		result.sin_addr.S_un.S_un_b.s_b3 = addressAndPort.address.ipv4.part2;
		result.sin_addr.S_un.S_un_b.s_b4 = addressAndPort.address.ipv4.part3;
	}
	return result;
}

IpAddressAndPort_t Win32_GetIpAddressAndPortFromSockAddr(sockaddr_in sockAddr)
{
	Assert(sockAddr.sin_family == AF_INET || sockAddr.sin_family == AF_INET6);
	IpAddressAndPort_t result = {};
	result.port = MyNetworkToHostByteOrderU16(sockAddr.sin_port);
	result.address.isIpv6 = (sockAddr.sin_family == AF_INET6);
	if (result.address.isIpv6)
	{
		sockaddr_in6* sockAddr6 = (sockaddr_in6*)&sockAddr;
		result.address.ipv6.part0 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[0]);
		result.address.ipv6.part1 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[1]);
		result.address.ipv6.part2 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[2]);
		result.address.ipv6.part3 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[3]);
		result.address.ipv6.part4 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[4]);
		result.address.ipv6.part5 = MyNetworkToHostByteOrderU16(sockAddr6->sin6_addr.u.Word[5]);
	}
	else
	{
		result.address.ipv4.part0 = sockAddr.sin_addr.S_un.S_un_b.s_b1;
		result.address.ipv4.part1 = sockAddr.sin_addr.S_un.S_un_b.s_b2;
		result.address.ipv4.part2 = sockAddr.sin_addr.S_un.S_un_b.s_b3;
		result.address.ipv4.part3 = sockAddr.sin_addr.S_un.S_un_b.s_b4;
	}
	return result;
}

#endif //WINDOWS_COMPILATION

SocketError_t PrintSocketError(int errorCode, const char* message, const char* functionName)
{
	SocketError_t result = SocketError_Generic;
	
	#if WINDOWS_COMPILATION
	{
		GyLibPrintLine_E("%s (%s): %s (%d)", message, functionName, Win32_GetWsaErrorStr(errorCode), errorCode);
		if (MyStrCompareNt(functionName, "recv") == 0 || MyStrCompareNt(functionName, "recvfrom") == 0) { result = SocketError_ReadError; }
		if (MyStrCompareNt(functionName, "send") == 0 || MyStrCompareNt(functionName, "sendto") == 0) { result = SocketError_WriteError; }
		if (MyStrCompareNt(functionName, "socket") == 0) { result = SocketError_CreationFailed; }
		if (MyStrCompareNt(functionName, "bind") == 0) { result = SocketError_CreationBindFailed; }
		if (MyStrCompareNt(functionName, "ioctlsocket") == 0) { result = SocketError_CreationNonBlockingFailed; }
	}
	#else
	#error Unsupported platform for gy_socket.h PrintSocketError
	#endif
	
	return result;
}

inline bool IsSocketOpen(const OpenSocket_t* socket)
{
	if (socket == nullptr) { return false; }
	return socket->isOpen;
}
inline bool IsSocketOpen(const BufferedSocket_t* socket)
{
	if (socket == nullptr) { return false; }
	return socket->socket.isOpen;
}

inline bool DoesSocketHaveErrors(const OpenSocket_t* socket)
{
	if (socket == nullptr) { return false; }
	return (socket->error != SocketError_None);
}
inline bool DoesSocketHaveErrors(const BufferedSocket_t* socket)
{
	if (socket == nullptr) { return false; }
	return (socket->socket.error != SocketError_None);
}

BufferedSocketBuffer_t* FindBufferForAddress(BufferedSocket_t* socket, IpAddress_t address, bool findFreeBufferIfNeeded = true)
{
	NotNull(socket);
	BufferedSocketBuffer_t* freeBuffer = nullptr;
	for (u64 bufferIndex = 0; bufferIndex < BUFFERED_SOCKET_MAX_NUM_BUFFERS; bufferIndex++)
	{
		if (socket->buffers[bufferIndex].isUsed && socket->mainBuffer != &socket->buffers[bufferIndex])
		{
			if (AreIpAddressesEqual(address, socket->buffers[bufferIndex].address.address))
			{
				return &socket->buffers[bufferIndex];
			}
		}
		if (!socket->buffers[bufferIndex].isUsed && freeBuffer == nullptr) { freeBuffer = &socket->buffers[bufferIndex]; }
	}
	return (findFreeBufferIfNeeded ? freeBuffer : nullptr);
}
BufferedSocketBuffer_t* FindBufferForAddressAndPort(BufferedSocket_t* socket, IpAddressAndPort_t address, bool findFreeBufferIfNeeded = true)
{
	NotNull(socket);
	BufferedSocketBuffer_t* freeBuffer = nullptr;
	for (u64 bufferIndex = 0; bufferIndex < BUFFERED_SOCKET_MAX_NUM_BUFFERS; bufferIndex++)
	{
		if (socket->buffers[bufferIndex].isUsed && socket->mainBuffer != &socket->buffers[bufferIndex])
		{
			if (AreIpAddressAndPortsEqual(address, socket->buffers[bufferIndex].address))
			{
				return &socket->buffers[bufferIndex];
			}
		}
		if (!socket->buffers[bufferIndex].isUsed && freeBuffer == nullptr) { freeBuffer = &socket->buffers[bufferIndex]; }
	}
	return (findFreeBufferIfNeeded ? freeBuffer : nullptr);
}

void BufferedSocketBufferPop(BufferedSocketBuffer_t* buffer, u64 numBytesToPop)
{
	NotNull(buffer);
	NotNull(buffer->pntr);
	Assert(numBytesToPop <= buffer->used);
	u64 numBytesLeft = buffer->used - numBytesToPop;
	for (u64 bIndex = 0; bIndex < numBytesLeft; bIndex++)
	{
		buffer->pntr[bIndex] = buffer->pntr[numBytesToPop + bIndex];
	}
	buffer->used = numBytesLeft;
}

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
bool InitializeSockets()
{
	#if WINDOWS_COMPILATION
	{
		WSADATA startupData = {};
		int startupResult = WSAStartup(
		    MAKEWORD(2, 2), //wVersionRequired
			&startupData
		);
		if (startupResult != 0)
		{
			PrintSocketError(startupResult, "Failed to startup sockets system", "WSAStartup");
			//Possible Errors: WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM, WSAEFAULT
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h InitializeSockets
	#endif
	
	return true;
}

// +--------------------------------------------------------------+
// |                         Close Socket                         |
// +--------------------------------------------------------------+
void CloseOpenSocket(OpenSocket_t* socket)
{
	NotNull(socket);
	if (!socket->isOpen) { return; }
	
	#if WINDOWS_COMPILATION
	{
		Assert(socket->handle_win32 != INVALID_SOCKET);
		int closeResult = closesocket(socket->handle_win32);
		Assert(closeResult == 0); //TODO: Should we handle some kinds of errors? WSANOTINITIALISED, WSAENETDOWN, WSAENOTSOCK, WSAEINPROGRESS, WSAEINTR, WSAEWOULDBLOCK
		socket->handle_win32 = INVALID_SOCKET;
	}
	#else
	#error Unsupported platform for gy_socket.h CloseOpenSocket
	#endif
	
	socket->isOpen = false;
}
SocketError_t CloseOpenSocketIfErrors(OpenSocket_t* socket, bool printOutError = false)
{
	NotNull(socket);
	SocketError_t result = SocketError_None;
	if (DoesSocketHaveErrors(socket))
	{
		if (printOutError) { GyLibPrintLine_E("Socket closing because of error: %s", GetSocketErrorStr(socket->error)); }
		result = socket->error;
		CloseOpenSocket(socket);
	}
	return result;
}
void FreeBufferedSocketBuffer(BufferedSocket_t* socket, BufferedSocketBuffer_t* buffer)
{
	NotNull2(socket, buffer);
	if (buffer->pntr != nullptr)
	{
		NotNull(socket->allocArena);
		FreeMem(socket->allocArena, buffer->pntr, buffer->size);
	}
	ClearPointer(buffer);
}
void DestroyBufferedSocket(BufferedSocket_t* socket)
{
	CloseOpenSocket(&socket->socket);
	for (u64 bufferIndex = 0; bufferIndex < BUFFERED_SOCKET_MAX_NUM_BUFFERS; bufferIndex++)
	{
		FreeBufferedSocketBuffer(socket, &socket->buffers[bufferIndex]);
	}
	ClearPointer(socket);
}
SocketError_t DestroyBufferedSocketIfErrors(BufferedSocket_t* socket, bool printOutError = false)
{
	NotNull(socket);
	SocketError_t result = SocketError_None;
	if (DoesSocketHaveErrors(socket))
	{
		if (printOutError) { GyLibPrintLine_E("Socket closing because of error: %s", GetSocketErrorStr(socket->socket.error)); }
		result = socket->socket.error;
		DestroyBufferedSocket(socket);
	}
	return result;
}

// +--------------------------------------------------------------+
// |                         Open Socket                          |
// +--------------------------------------------------------------+
bool TryOpenNewSocket(SocketProtocol_t protocol, IpAddressAndPort_t destAddress, OpenSocket_t* socketOut)
{
	NotNull(socketOut);
	ClearPointer(socketOut);
	socketOut->type = SocketType_SingleDestination;
	socketOut->protocol = protocol;
	socketOut->destAddress = destAddress;
	socketOut->isOpen = false;
	
	#if WINDOWS_COMPILATION
	{
		ADDRESS_FAMILY win32_addressFamily = (destAddress.address.isIpv6 ? AF_INET6 : AF_INET);
		Assert(protocol == SocketProtocol_Tcp || protocol == SocketProtocol_Udp);
		int win32_type = ((protocol == SocketProtocol_Tcp) ? SOCK_STREAM : SOCK_DGRAM);      //Other Values: SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET
		int win32_protocol = ((protocol == SocketProtocol_Tcp) ? IPPROTO_TCP : IPPROTO_UDP); //TODO: This could be 0? Other Values: IPPROTO_ICMP, IPPROTO_IGMP, BTHPROTO_RFCOMM, IPPROTO_ICMPV6, IPPROTO_RM
		
		socketOut->handle_win32 = socket(win32_addressFamily, win32_type, win32_protocol);
		if (socketOut->handle_win32 == INVALID_SOCKET)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to create socket", "socket");
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEAFNOSUPPORT, WSAEINPROGRESS, WSAEMFILE, WSAEINVAL, WSAEINVALIDPROVIDER, WSAEINVALIDPROCTABLE, WSAENOBUFS, WSAEPROTONOSUPPORT, WSAEPROTOTYPE, WSAEPROVIDERFAILEDINIT, WSAESOCKTNOSUPPORT
			return false;
		}
		
		socketOut->isOpen = true;
		
		unsigned long trueValueULong = 1;
		int setNonBlockingResult = ioctlsocket(
			socketOut->handle_win32, //s
			FIONBIO,                 //cmd (FIONBIO = disables blockings when != 0)
			&trueValueULong          //argp
		);
		if (setNonBlockingResult != 0)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to set socket to non-blocking mode", "ioctlsocket");
			//Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS, WSAENOTSOCK, WSAEFAULT
			CloseOpenSocket(socketOut);
			return false;
		}
		
		sockaddr_in destAddr = Win32_GetSockAddrFromIpAddressAndPort(destAddress);
		int connectResult = connect(
			socketOut->handle_win32, //s
			(sockaddr*)&destAddr, //addr
			sizeof(destAddr) //namelen
		);
		if (connectResult != 0)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to connect socket", "connect");
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEADDRINUSE, WSAEADDRNOTAVAIL, WSAEFAULT, WSAEINPROGRESS, WSAEINVAL, WSAENOBUFS, WSAENOTSOCK
			CloseOpenSocket(socketOut);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h TryOpenNewSocket
	#endif
	
	return socketOut->isOpen;
}

bool TryOpenNewMultiSocket(SocketProtocol_t protocol, IpPort_t port, OpenSocket_t* socketOut)
{
	NotNull(socketOut);
	ClearPointer(socketOut);
	socketOut->type = SocketType_MultiDestination;
	socketOut->protocol = protocol;
	socketOut->destAddress = NewIpAddressAndPort(IpAddress_Zero, port);
	socketOut->isOpen = false;
	
	#if WINDOWS_COMPILATION
	{
		ADDRESS_FAMILY win32_addressFamily = AF_INET; //TODO: Should we allow the calling code to choose this somehow? (https://www.ibm.com/docs/it/i/7.1?topic=families-using-af-inet6-address-family)
		Assert(protocol == SocketProtocol_Tcp || protocol == SocketProtocol_Udp);
		int win32_type = ((protocol == SocketProtocol_Tcp) ? SOCK_STREAM : SOCK_DGRAM);      //Other Values: SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET
		int win32_protocol = ((protocol == SocketProtocol_Tcp) ? IPPROTO_TCP : IPPROTO_UDP); //TODO: This could be 0? Other Values: IPPROTO_ICMP, IPPROTO_IGMP, BTHPROTO_RFCOMM, IPPROTO_ICMPV6, IPPROTO_RM
		
		socketOut->handle_win32 = socket(win32_addressFamily, win32_type, win32_protocol);
		if (socketOut->handle_win32 == INVALID_SOCKET)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to create socket", "socket");
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEAFNOSUPPORT, WSAEINPROGRESS, WSAEMFILE, WSAEINVAL, WSAEINVALIDPROVIDER, WSAEINVALIDPROCTABLE, WSAENOBUFS, WSAEPROTONOSUPPORT, WSAEPROTOTYPE, WSAEPROVIDERFAILEDINIT, WSAESOCKTNOSUPPORT
			return false;
		}
		
		socketOut->isOpen = true;
		
		unsigned long trueValueULong = 1;
		int setNonBlockingResult = ioctlsocket(
			socketOut->handle_win32, //s
			FIONBIO,                 //cmd (FIONBIO = disables blockings when != 0)
			&trueValueULong          //argp
		);
		if (setNonBlockingResult != 0)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to set multi socket to non-blocking mode", "ioctlsocket");
			//Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS, WSAENOTSOCK, WSAEFAULT
			CloseOpenSocket(socketOut);
			return false;
		}
		
		sockaddr_in listenAddress;
		listenAddress.sin_family = win32_addressFamily;
		listenAddress.sin_port = MyHostToNetworkByteOrderU16(port);
		listenAddress.sin_addr.s_addr = MyHostToNetworkByteOrderU32(INADDR_ANY); //TODO: We should probably allow the calling code to specify address ranges which it wants to accept
		int bindError = bind(
			socketOut->handle_win32,
			(struct sockaddr*)&listenAddress,
			sizeof(listenAddress)
		);
		if (bindError == SOCKET_ERROR)
		{
			socketOut->error = PrintSocketError(WSAGetLastError(), "Failed to bind multi socket", "bind");
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEADDRINUSE, WSAEADDRNOTAVAIL, WSAEFAULT, WSAEINPROGRESS, WSAEINVAL, WSAENOBUFS, WSAENOTSOCK
			CloseOpenSocket(socketOut);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h TryOpenNewMultiSocket
	#endif
	
	return socketOut->isOpen;
}

bool TryOpenNewBufferedSocket(SocketProtocol_t protocol, IpAddressAndPort_t destAddress, BufferedSocket_t* socketOut, MemArena_t* memArena, u64 bufferSize)
{
	NotNull(memArena);
	Assert(bufferSize > 0);
	ClearPointer(socketOut);
	if (!TryOpenNewSocket(protocol, destAddress, &socketOut->socket)) { return false; }
	socketOut->allocArena = memArena;
	socketOut->mainBuffer = &socketOut->buffers[0];
	socketOut->mainBuffer->isUsed = true;
	socketOut->mainBuffer->pntr = AllocArray(memArena, u8, bufferSize);
	NotNull(socketOut->mainBuffer->pntr);
	socketOut->mainBuffer->size = bufferSize;
	socketOut->mainBuffer->used = 0;
	socketOut->mostRecentBuffer = nullptr;
	return true;
}
//NOTE: We use one mainBuffer for receiving data before we know where it's come from. After that we store the data into separate buffers for each sourceAddress
bool TryOpenNewBufferedMultiSocket(SocketProtocol_t protocol, IpPort_t port, BufferedSocket_t* socketOut, MemArena_t* memArena, u64 mainBufferSize, u64 connectionBufferSize)
{
	NotNull(memArena);
	Assert(mainBufferSize > 0);
	Assert(connectionBufferSize > 0);
	ClearPointer(socketOut);
	if (!TryOpenNewMultiSocket(protocol, port, &socketOut->socket)) { return false; }
	socketOut->allocArena = memArena;
	socketOut->newBufferSize = connectionBufferSize;
	socketOut->mainBuffer = &socketOut->buffers[0];
	socketOut->mainBuffer->isUsed = true;
	socketOut->mainBuffer->pntr = AllocArray(memArena, u8, mainBufferSize);
	NotNull(socketOut->mainBuffer->pntr);
	socketOut->mainBuffer->size = mainBufferSize;
	socketOut->mainBuffer->used = 0;
	socketOut->mostRecentBuffer = nullptr;
	return true;
}

// +--------------------------------------------------------------+
// |                            Write                             |
// +--------------------------------------------------------------+
bool SocketWriteTo(OpenSocket_t* socket, IpAddressAndPort_t destAddress, u64 numBytes, const u8* bytesPntr)
{
	NotNull(socket);
	Assert(socket->isOpen);
	Assert(socket->type == SocketType_MultiDestination);
	AssertIf(numBytes > 0, bytesPntr != nullptr);
	
	#if WINDOWS_COMPILATION
	{
		Assert(numBytes <= INT_MAX);
		
		sockaddr_in destAddr = Win32_GetSockAddrFromIpAddressAndPort(destAddress);
		int sendResult = sendto(
			socket->handle_win32,    //s
			(const char*)bytesPntr,  //buf
			(int)numBytes,           //len
			0,                       //flags
			(sockaddr*)&destAddr,    //to
			sizeof(destAddr)         //tolen
		);
		
		if (sendResult == SOCKET_ERROR)
		{
			socket->error = PrintSocketError(WSAGetLastError(), "Failed to send data to specified address", "sendto");
			//WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEINVAL, WSAEINTR, WSAEINPROGRESS, WSAEFAULT, WSAENETRESET, WSAENOBUFS, WSAENOTCONN, WSAENOTSOCK, WSAEOPNOTSUPP, WSAESHUTDOWN, WSAEWOULDBLOCK, WSAEMSGSIZE, WSAEHOSTUNREACH, WSAECONNABORTED, WSAECONNRESET, WSAEADDRNOTAVAIL, WSAEAFNOSUPPORT, WSAEDESTADDRREQ, WSAENETUNREACH, WSAETIMEDOUT
			return false;
		}
		else if (sendResult != (int)numBytes)
		{
			GyLibPrintLine_E("Only sent %d / %d bytes", sendResult, (int)numBytes);
			socket->error = PrintSocketError(WSAGetLastError(), "Failed to send some data", "sendto");
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h SocketWriteTo
	#endif
	
	return true;
}
bool SocketWriteToStr(OpenSocket_t* socket, IpAddressAndPort_t destAddress, MyStr_t messageStr)
{
	return SocketWriteTo(socket, destAddress, messageStr.length, messageStr.bytes);
}

bool SocketWrite(OpenSocket_t* socket, u64 numBytes, const u8* bytesPntr)
{
	NotNull(socket);
	Assert(socket->isOpen);
	Assert(socket->type == SocketType_SingleDestination);
	AssertIf(numBytes > 0, bytesPntr != nullptr);
	
	#if WINDOWS_COMPILATION
	{
		Assert(numBytes <= INT_MAX);
		
		int sendResult = send(
			socket->handle_win32,    //s
			(const char*)bytesPntr,  //buf
			(int)numBytes,           //len
			0                        //flags
		);
		
		if (sendResult == SOCKET_ERROR)
		{
			socket->error = PrintSocketError(WSAGetLastError(), "Failed to send data", "send");
			//WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEINVAL, WSAEINTR, WSAEINPROGRESS, WSAEFAULT, WSAENETRESET, WSAENOBUFS, WSAENOTCONN, WSAENOTSOCK, WSAEOPNOTSUPP, WSAESHUTDOWN, WSAEWOULDBLOCK, WSAEMSGSIZE, WSAEHOSTUNREACH, WSAECONNABORTED, WSAECONNRESET, WSAEADDRNOTAVAIL, WSAEAFNOSUPPORT, WSAEDESTADDRREQ, WSAENETUNREACH, WSAETIMEDOUT
			return false;
		}
		else if (sendResult != (int)numBytes)
		{
			GyLibPrintLine_E("Only sent %d / %d bytes", sendResult, (int)numBytes);
			socket->error = PrintSocketError(WSAGetLastError(), "Failed to send some data", "send");
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h SocketWrite
	#endif
	
	return true;
}
bool SocketWriteStr(OpenSocket_t* socket, MyStr_t messageStr)
{
	return SocketWrite(socket, messageStr.length, messageStr.bytes);
}

bool BufferedSocketWrite(BufferedSocket_t* socket, u64 numBytes, const u8* bytesPntr)
{
	Unimplemented(); //TODO: Implement me!
	return false;
}

// +--------------------------------------------------------------+
// |                             Read                             |
// +--------------------------------------------------------------+
//NOTE: If any of these functions fail, please check socket->error and call CloseOpenSocket if there are any errors

bool SocketReadFromAny(OpenSocket_t* socket, void* outBufferPntr, u64 outBufferSize, u64* outReceivedNumBytes, IpAddressAndPort_t* addressOut = nullptr)
{
	NotNull(socket);
	NotNull(outBufferPntr);
	NotNull(outReceivedNumBytes);
	Assert(socket->isOpen);
	Assert(socket->type == SocketType_MultiDestination);
	
	#if WINDOWS_COMPILATION
	{
		Assert(outBufferSize <= INT_MAX);
		
		sockaddr_in fromAddr;
		int fromAddrSize = sizeof(fromAddr);
		int recvResult = recvfrom(
			socket->handle_win32, //sockfd
			(char*)outBufferPntr, //buf
			(int)outBufferSize,   //len
			0,                    //flags
			(sockaddr*)&fromAddr, //src_addr
			&fromAddrSize         //addrlen
		);
		Assert(fromAddrSize <= sizeof(fromAddr)); //TODO: We should maybe handle this gracefully?
		if (recvResult == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK || errorCode == EAGAIN)
			{
				//For a non-blocking socket, this just means there's no data, so these "errors" are totally acceptable
				return false;
			}
			else
			{
				socket->error = PrintSocketError(errorCode, "Error while receiving data", "recvfrom");
				return false;
			}
		}
		
		//TODO: Winsock docs say this is a graceful close, but that doesn't mean the socket is closed because we are listening to everyone, so we just treat it as if nothing happened
		if (recvResult == 0) { return false; } //no bytes received
		
		Assert(recvResult > 0);
		*outReceivedNumBytes = (u64)recvResult;
		SetOptionalOutPntr(addressOut, Win32_GetIpAddressAndPortFromSockAddr(fromAddr));
	}
	#else
	#error Unsupported platform for gy_socket.h SocketReadFromAny
	#endif
	
	return true;
}
//You're expected to call FreeString on the result of this function with the memArena passed
MyStr_t SocketReadFromAnyStr(OpenSocket_t* socket, MemArena_t* memArena, u64 maxReadSize, IpAddressAndPort_t* addressOut = nullptr)
{
	NotNull(socket);
	NotNull(memArena);
	Assert(socket->isOpen);
	Assert(maxReadSize > 0);
	
	char* resultPntr = AllocArray(memArena, char, maxReadSize+1);
	NotNull(resultPntr);
	
	u64 numBytesReceived = 0;
	if (SocketReadFromAny(socket, resultPntr, maxReadSize, &numBytesReceived, addressOut))
	{
		Assert(numBytesReceived <= maxReadSize);
		Assert(numBytesReceived > 0);
		if (numBytesReceived < maxReadSize) { ShrinkMem(memArena, resultPntr, maxReadSize+1, numBytesReceived+1); }
		resultPntr[numBytesReceived] = '\0';
		return NewStr(numBytesReceived, resultPntr);
	}
	else
	{
		FreeMem(memArena, resultPntr, maxReadSize+1);
		return MyStr_Empty;
	}
}

bool SocketRead(OpenSocket_t* socket, void* outBufferPntr, u64 outBufferSize, u64* outReceivedNumBytes)
{
	NotNull(socket);
	NotNull(outBufferPntr);
	NotNull(outReceivedNumBytes);
	Assert(socket->isOpen);
	Assert(socket->type == SocketType_SingleDestination);
	
	#if WINDOWS_COMPILATION
	{
		Assert(outBufferSize <= INT_MAX);
		
		int recvResult = recv(
			socket->handle_win32, //sockfd
			(char*)outBufferPntr, //buf
			(int)outBufferSize,   //len
			0                     //flags
		);
		if (recvResult == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK || errorCode == EAGAIN)
			{
				//For a non-blocking socket, this just means there's no data, so these "errors" are totally acceptable
				return false;
			}
			else
			{
				socket->error = PrintSocketError(errorCode, "Error while receiving data", "recv");
				return false;
			}
		}
		
		//TODO: Winsock docs say this is a graceful close, but this isn't a connection oriented socket so why would that happen?
		if (recvResult == 0) { return false; } //no bytes received
		
		Assert(recvResult > 0);
		*outReceivedNumBytes = (u64)recvResult;
	}
	#else
	#error Unsupported platform for gy_socket.h SocketRead
	#endif
	
	return true;
}
MyStr_t SocketReadStr(OpenSocket_t* socket, MemArena_t* memArena, u64 maxReadSize)
{
	NotNull(socket);
	NotNull(memArena);
	Assert(socket->isOpen);
	Assert(maxReadSize > 0);
	
	char* resultPntr = AllocArray(memArena, char, maxReadSize+1);
	NotNull(resultPntr);
	
	u64 numBytesReceived = 0;
	if (SocketRead(socket, resultPntr, maxReadSize, &numBytesReceived))
	{
		Assert(numBytesReceived <= maxReadSize);
		Assert(numBytesReceived > 0);
		if (numBytesReceived < maxReadSize) { ShrinkMem(memArena, resultPntr, maxReadSize+1, numBytesReceived+1); }
		resultPntr[numBytesReceived] = '\0';
		return NewStr(numBytesReceived, resultPntr);
	}
	else
	{
		FreeMem(memArena, resultPntr, maxReadSize+1);
		return MyStr_Empty;
	}
}

void UpdateBufferedSocket(BufferedSocket_t* socket, u64 programTime)
{
	NotNull(socket);
	Assert(socket->socket.isOpen);
	NotNull2(socket->mainBuffer, socket->mainBuffer->pntr);
	
	u64 numReceiveIterations = 0;
	while (numReceiveIterations < MAX_NUM_RECEIVE_ITERATIONS)
	{
		numReceiveIterations++;
		
		if (socket->socket.type == SocketType_SingleDestination)
		{
			Assert(socket->mainBuffer->used <= socket->mainBuffer->size);
			u64 numFreeBytes = socket->mainBuffer->size - socket->mainBuffer->used;
			u8* freeBytesPntr = &socket->mainBuffer->pntr[socket->mainBuffer->used];
			if (numFreeBytes == 0) { break; }
			
			u64 numNewBytesReceived = 0;
			bool readSuccess = SocketRead(&socket->socket, freeBytesPntr, numFreeBytes, &numNewBytesReceived);
			if (!readSuccess) { break; }
			Assert(numNewBytesReceived > 0);
			socket->mainBuffer->used += numNewBytesReceived;
			socket->mainBuffer->lastReceiveTime = programTime;
		}
		else if (socket->socket.type == SocketType_MultiDestination)
		{
			u64 numNewBytesReceived = 0;
			IpAddressAndPort_t sourceAddress = {};
			if (socket->mainBuffer->used != 0)
			{
				//We have pending data for an existing connection, we need to clear it from mainBuffer before we can receive any more data
				numNewBytesReceived = socket->mainBuffer->used;
				sourceAddress = socket->mainBuffer->address;
			}
			else
			{
				bool readSuccess = SocketReadFromAny(&socket->socket, socket->mainBuffer->pntr, socket->mainBuffer->size, &numNewBytesReceived, &sourceAddress);
				socket->mainBuffer->used = numNewBytesReceived;
				if (!readSuccess) { return; }
			}
			
			Assert(numNewBytesReceived > 0);
			Assert(numNewBytesReceived <= socket->mainBuffer->size);
			
			BufferedSocketBuffer_t* buffer = FindBufferForAddressAndPort(socket, sourceAddress, true);
			if (buffer == nullptr)
			{
				socket->socket.warning = SocketWarning_TooManySourceAddresses;
				//We're just gonna drop the data that came from that address
				continue;
			}
			if (!buffer->isUsed)
			{
				//We got data from a new sourceAddress, we need to spin up a new buffer for it
				NotNull(socket->allocArena);
				Assert(socket->newBufferSize > 0);
				buffer->isUsed = true;
				buffer->address = sourceAddress;
				buffer->lastReceiveTime = programTime;
				buffer->pntr = AllocArray(socket->allocArena, u8, socket->newBufferSize);
				NotNull(buffer->pntr);
				buffer->size = socket->newBufferSize;
				buffer->used = 0;
			}
			
			Assert(buffer->used <= buffer->size);
			u64 numBytesFree = buffer->size - buffer->used;
			u8* freeBytesPntr = &buffer->pntr[buffer->used];
			if (numBytesFree >= numNewBytesReceived)
			{
				MyMemCopy(freeBytesPntr, socket->mainBuffer->pntr, numNewBytesReceived);
				buffer->used += numNewBytesReceived;
				buffer->lastReceiveTime = programTime;
				socket->mostRecentBuffer = buffer;
				
				socket->mainBuffer->address = NewIpAddressAndPort(IpAddress_Zero, 0);
				socket->mainBuffer->used = 0;
				socket->socket.warning = SocketWarning_None;
			}
			else
			{
				if (numBytesFree > 0)
				{
					//Move what we can into the buffer for that connection
					MyMemCopy(freeBytesPntr, socket->mainBuffer->pntr, numBytesFree);
					buffer->used += numBytesFree;
					buffer->lastReceiveTime = programTime;
					socket->mostRecentBuffer = buffer;
					
					//mainBuffer will have some non-zero "used" value, which acts as a flag saying we are blocked from receiving more bytes
					BufferedSocketBufferPop(socket->mainBuffer, numBytesFree);
					//Set sourceAddress so we know where this data was originally bound to
					socket->mainBuffer->address = sourceAddress;
					
					socket->socket.warning = SocketWarning_BufferIsFull;
					break;
				}
			}
		}
	}
}

#endif // SOCKETS_SUPPORTED

#endif //  _GY_SOCKET_H
