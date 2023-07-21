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
// |                          Structures                          |
// +--------------------------------------------------------------+
enum SocketType_t
{
	SocketType_None = 0,
	SocketType_Udp,
	SocketType_Tcp,
	SocketType_NumTypes,
};
const char* GetSocketTypeStr(SocketType_t enumValue)
{
	switch (enumValue)
	{
		case SocketType_None: return "None";
		case SocketType_Udp:  return "Udp";
		case SocketType_Tcp:  return "Tcp";
		default: return "Unknown";
	}
}

struct OpenSocket_t
{
	SocketType_t type;
	IpAddressAndPort_t destAddress;
	
	bool isOpen;
	
	#if WINDOWS_COMPILATION
	SOCKET handle_win32;
	#else
	#error Unsupported platform for gy_socket.h OpenSocket_t
	#endif
};

//UDP datagram header (https://en.wikipedia.org/wiki/User_Datagram_Protocol)
START_PACK()
struct ATTR_PACKED UdpHeader_t
{
	u16 sourcePort; //optional
	u16 destPort;
	u16 length;
	u16 checksum; //optional for IPv4, not IPv6
};
END_PACK()

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
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
#endif

#if WINDOWS_COMPILATION
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
#endif

u16 CalculateUdpChecksum(u16 dataLength, const void* dataPntr)
{
	AssertIf(dataLength > 0, dataPntr != nullptr);
	const u8* dataPntrU8 = (const u8*)dataPntr;
	u16 result = 0x0000;
	for (u16 bIndex = 0; bIndex < dataLength; bIndex++)
	{
		//TODO: Implement me!
	}
	return result;
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
			//TODO: WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM, WSAEFAULT
			GyLibPrintLine_E("WSAStartup returned error: %s (%d)", Win32_GetWsaErrorStr(startupResult), startupResult);
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

// +--------------------------------------------------------------+
// |                         Open Socket                          |
// +--------------------------------------------------------------+
bool TryOpenNewSocket(SocketType_t type, IpAddressAndPort_t destAddress, OpenSocket_t* socketOut)
{
	NotNull(socketOut);
	ClearPointer(socketOut);
	socketOut->type = type;
	socketOut->destAddress = destAddress;
	socketOut->isOpen = false;
	
	#if WINDOWS_COMPILATION
	{
		ADDRESS_FAMILY win32_addressFamily = (destAddress.address.isIpv6 ? AF_INET6 : AF_INET);
		Assert(type == SocketType_Tcp || type == SocketType_Udp);
		int win32_type = ((type == SocketType_Tcp) ? SOCK_STREAM : SOCK_DGRAM);      //Other Values: SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET
		int win32_protocol = ((type == SocketType_Tcp) ? IPPROTO_TCP : IPPROTO_UDP); //TODO: This could be 0? Other Values: IPPROTO_ICMP, IPPROTO_IGMP, BTHPROTO_RFCOMM, IPPROTO_ICMPV6, IPPROTO_RM
		
		socketOut->handle_win32 = socket(win32_addressFamily, win32_type, win32_protocol);
		if (socketOut->handle_win32 == INVALID_SOCKET)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to create socket: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEAFNOSUPPORT, WSAEINPROGRESS, WSAEMFILE, WSAEINVAL, WSAEINVALIDPROVIDER, WSAEINVALIDPROCTABLE, WSAENOBUFS, WSAEPROTONOSUPPORT, WSAEPROTOTYPE, WSAEPROVIDERFAILEDINIT, WSAESOCKTNOSUPPORT
			return false;
		}
		
		socketOut->isOpen = true;
		
		#if 0
		sockaddr_in win32_address = Win32_GetSockAddrFromIpAddressAndPort(destAddress);
		int connectResult = connect(
			socketOut->handle_win32, //s
			(sockaddr*)&win32_address, //name
			sizeof(win32_address) //namelen
		);
		if (connectResult != 0)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to connect socket: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEADDRINUSE, WSAEINTR, WSAEINPROGRESS, WSAEALREADY, WSAEADDRNOTAVAIL, WSAEAFNOSUPPORT, WSAECONNREFUSED, WSAEFAULT, WSAEINVAL, WSAEISCONN, WSAENETUNREACH, WSAEHOSTUNREACH, WSAENOBUFS, WSAENOTSOCK, WSAETIMEDOUT, WSAEWOULDBLOCK, WSAEACCES
			CloseOpenSocket(socketOut);
			return false;
		}
		#endif
	}
	#else
	#error Unsupported platform for gy_socket.h TryOpenNewSocket
	#endif
	
	return socketOut->isOpen;
}

bool TryOpenNewListeningSocket(SocketType_t type, IpPort_t port, OpenSocket_t* socketOut)
{
	NotNull(socketOut);
	ClearPointer(socketOut);
	socketOut->type = type;
	socketOut->destAddress = NewIpAddressAndPort(IpAddress_Zero, port);
	socketOut->isOpen = false;
	
	#if WINDOWS_COMPILATION
	{
		ADDRESS_FAMILY win32_addressFamily = AF_INET; //TODO: Should we allow the calling code to choose this somehow? (https://www.ibm.com/docs/it/i/7.1?topic=families-using-af-inet6-address-family)
		Assert(type == SocketType_Tcp || type == SocketType_Udp);
		int win32_type = ((type == SocketType_Tcp) ? SOCK_STREAM : SOCK_DGRAM);      //Other Values: SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET
		int win32_protocol = ((type == SocketType_Tcp) ? IPPROTO_TCP : IPPROTO_UDP); //TODO: This could be 0? Other Values: IPPROTO_ICMP, IPPROTO_IGMP, BTHPROTO_RFCOMM, IPPROTO_ICMPV6, IPPROTO_RM
		
		socketOut->handle_win32 = socket(win32_addressFamily, win32_type, win32_protocol);
		if (socketOut->handle_win32 == INVALID_SOCKET)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to create socket: %s (%d)", socketErrorStr, socketError);
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
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to set listening socket to non-blocking mode: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			//Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS, WSAENOTSOCK, WSAEFAULT
			CloseOpenSocket(socketOut);
			return false;
		}
		
		sockaddr_in listenAddress;
		listenAddress.sin_family = win32_addressFamily;
		listenAddress.sin_port = MyHostToNetworkByteOrderU16(port);
		listenAddress.sin_addr.s_addr = MyHostToNetworkByteOrderU32(INADDR_ANY); //TODO: We should probably allow the calling code to specify address ranges which it wants to accept
		int bindError = bind(socketOut->handle_win32, (struct sockaddr*)&listenAddress, sizeof(listenAddress));
		if (bindError == SOCKET_ERROR)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to bind listening socket: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEADDRINUSE, WSAEADDRNOTAVAIL, WSAEFAULT, WSAEINPROGRESS, WSAEINVAL, WSAENOBUFS, WSAENOTSOCK
			CloseOpenSocket(socketOut);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h TryOpenNewListeningSocket
	#endif
	
	return socketOut->isOpen;
}

// +--------------------------------------------------------------+
// |                            Write                             |
// +--------------------------------------------------------------+
//https://gist.github.com/GreenRecycleBin/1273763
u16 udp_checksum(UdpHeader_t* udpHeaderPntr, size_t packetLength, u32 src_addr, u32 dest_addr)
{
	const u16* buf = (const u16*)udpHeaderPntr;
	u16* ip_src = (u16*)&src_addr;
	u16* ip_dst = (u16*)&dest_addr;
	u32 sum = 0;
	size_t length = packetLength;

	// Calculate the sum
	while (packetLength > 1)
	{
		sum += *buf++;
		if (sum & 0x80000000) { sum = (sum & 0xFFFF) + (sum >> 16); }
		packetLength -= 2;
	}

	if (packetLength & 1)
	{
		// Add the padding if the packet length is odd
		sum += *((u8*)buf);
	}

	// Add the pseudo-header
	sum += *(ip_src++);
	sum += *ip_src;

	sum += *(ip_dst++);
	sum += *ip_dst;

	sum += htons(IPPROTO_UDP);
	sum += htons((u16)length);

	// Add the carries
	while (sum >> 16)
	{
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	// Return the one's complement of sum
	return (u16)~sum;
}
bool SocketWriteStr(OpenSocket_t* socket, MyStr_t messageStr)
{
	NotNull(socket);
	Assert(socket->isOpen);
	NotNullStr(&messageStr);
	
	#if WINDOWS_COMPILATION
	{
		Assert(messageStr.length <= INT_MAX);
		
		#if 0
		u8 packetBuffer[sizeof(UdpHeader_t) + Kilobytes(8)];
		Assert(messageStr.length < ArrayCount(packetBuffer) - sizeof(UdpHeader_t));
		UdpHeader_t* header = (UdpHeader_t*)&packetBuffer[0];
		u8* payloadPntr = &packetBuffer[sizeof(UdpHeader_t)];
		
		header->sourcePort = socket->destAddress.port;
		header->destPort = socket->destAddress.port;
		header->length = (u16)messageStr.length;
		header->checksum = 0x0000;
		// header->checksum = CalculateUdpChecksum((u16)messageStr.length, messageStr.chars);
		
		MyMemCopy(payloadPntr, messageStr.chars, messageStr.length);
		
		const char* packetPntr = (const char*)&packetBuffer[0];
		int packetSize = sizeof(UdpHeader_t) + (int)messageStr.length;
		GyLibPrint_D("Checksumming[%d]: ", packetSize);
		for (int bIndex = 0; bIndex < packetSize; bIndex++) { GyLibPrint_D("%02X", packetBuffer[bIndex]); }
		GyLibWriteLine_D("");
		header->checksum = udp_checksum(header, packetSize, header->sourcePort, header->destPort);
		GyLibPrintLine_D("Checksum: 0x%04X", header->checksum);
		#endif
		
		sockaddr_in destAddress = Win32_GetSockAddrFromIpAddressAndPort(socket->destAddress);
		int sendResult = sendto(
			socket->handle_win32,        //s
			messageStr.chars,            //buf
			(int)messageStr.length,      //len
			0,                           //flags TODO: What is MSG_CONFIRM?
			(sockaddr*)&destAddress,     //dest_addr
			sizeof(destAddress)          //addrlen
		);
		if (sendResult == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			GyLibPrintLine_E("sendto failed with error: %s (%d)", Win32_GetWsaErrorStr(errorCode), errorCode);
			//WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEINVAL, WSAEINTR, WSAEINPROGRESS, WSAEFAULT, WSAENETRESET, WSAENOBUFS, WSAENOTCONN, WSAENOTSOCK, WSAEOPNOTSUPP, WSAESHUTDOWN, WSAEWOULDBLOCK, WSAEMSGSIZE, WSAEHOSTUNREACH, WSAECONNABORTED, WSAECONNRESET, WSAEADDRNOTAVAIL, WSAEAFNOSUPPORT, WSAEDESTADDRREQ, WSAENETUNREACH, WSAETIMEDOUT
			//TODO: Handle errors!
			return false;
		}
		else if (sendResult != (int)messageStr.length)
		{
			int errorCode = WSAGetLastError();
			GyLibPrintLine_E("send send all %d bytes, only sent %d: %s (%d)", (int)messageStr.length, sendResult, Win32_GetWsaErrorStr(errorCode), errorCode);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h SocketWriteStr
	#endif
	return true;
}

//TODO: Our return value should probably distinguish between "failed" and just "didn't receive any data"
//TODO: Output the fromAddress
bool SocketRead(OpenSocket_t* socket, void* outBufferPntr, u64 outBufferSize, u64* outReceivedNumBytes)
{
	NotNull(socket);
	NotNull(outBufferPntr);
	NotNull(outReceivedNumBytes);
	Assert(socket->isOpen);
	Assert(outBufferSize <= INT_MAX);
	
	#if WINDOWS_COMPILATION
	{
		struct sockaddr_in fromAddr;
		int fromAddrSize = sizeof(fromAddr);
		int recvResult = recvfrom(
			socket->handle_win32,        //sockfd
			(char*)outBufferPntr,        //buf
			(int)outBufferSize,          //len
			0,                           //flags (MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, ...)
			(struct sockaddr*)&fromAddr, //src_addr
			&fromAddrSize                //addrlen
		);
		if (recvResult == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK || errorCode == EAGAIN)
			{
				return false;
			}
			else
			{
				GyLibPrintLine_E("recvfrom failed with error: %s (%d)", Win32_GetWsaErrorStr(errorCode), errorCode);
				//TODO: Handle errors!
				return false;
			}
		}
		
		if (recvResult == 0) { return false; } //no bytes received (TODO: Winsock docs say this is a graceful close, but that doesn't mean the socket is closed because we are listening to everyone)
		
		Assert(recvResult >= 0);
		*outReceivedNumBytes = (u64)recvResult;
	}
	#else
	#error Unsupported platform for gy_socket.h SocketRead
	#endif
	
	return true;
}

#endif // SOCKETS_SUPPORTED

#endif //  _GY_SOCKET_H
