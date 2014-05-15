// this is needed to verify users not to abuse spineminer.
// you can skip this procedure by
// #define NOVERIFICATION
#ifndef NOVERIFICATION

//#include <wininet.h>
#include <windows.h>
#include <iphlpapi.h>
//#include <stdio.h>


#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#define URL "spineminer.sniffler.net"
#define VHOST "www.stadtmauer-wachenheim.de"
#define REPLY "spineminer:00-granted"
//#pragma comment(lib, "iphlpapi.lib")


void IsVerified( void )
{
    string identifyer;
    stringstream ss;

    _stdoutput << "Info> Verification ";

    // get serial of the hdd
    TCHAR volumeName[MAX_PATH + 1] = { 0 };
    TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;
    if (GetVolumeInformation("C:\\", volumeName, ARRAYSIZE(volumeName), &serialNumber, &maxComponentLen, &fileSystemFlags, fileSystemName, ARRAYSIZE(fileSystemName)))
    {
        ss << std::hex << int(serialNumber);
        //ss << serialNumber;
        //identifyer = serialNumber;
    }
    else
    {
        ss << "EE";
    }

    _stdoutput << ".";
    ss << "-";


    // get mac address.
    IP_ADAPTER_INFO AdapterInfo[1];       // Allocate information for up to 1 NICs
    DWORD dwBufLen = sizeof (AdapterInfo);  // Save memory size of buffer
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);  // [in] size of receive data buffer

    if (dwStatus != ERROR_SUCCESS)
    {
        //GetAdaptersInfo failed
        ss << "FFFFFFFFFFFF";
        //identifyer += "FFFFFFFFFFFF";
    }
    else
    {
        // convert mac address to string
        for (int i = 0; i < 6; i++)
        {
            ss << std::hex << int(AdapterInfo->Address[i]);
        }

        //identifyer += ss.str();
    }

    // construction of identifyer complete
    identifyer = ss.str();
    _stdoutput << ".";

    // now verify this id on licence server
    // make http request

    WSADATA wsaData;
    if ( WSAStartup(MAKEWORD(2,2), &wsaData ) != 0 )
    {
        _stdoutput << " failed (stopped by WSA)." << endl;
        exit(-1);
    }
    SOCKET Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    struct hostent *host;
    host = gethostbyname(URL);

    if (host == 0)
    {
        _stdoutput << " failed (stopped by resolver)." << endl;
        exit(-1);
    }

    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(80);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    _stdoutput << ".";
    if( connect( Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr) ) != 0 )
    {
        _stdoutput << " failed (stopped by socket).";
        exit(-1);
    }

    _stdoutput << ".";

    string transfer;
    string reply;

    transfer = "GET /spineminer/sm.php?id=" +identifyer+ " HTTP/1.1\r\nHost: " + std::string(VHOST) + "\r\n\r\n";


//    send(Socket,"GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n", strlen("GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n"),0);
    send(Socket, transfer.c_str(), transfer.length(),0);

    _stdoutput << ".";

    char buf[1024*10];
    //while ( recv( Socket, buf, 100, 0 ) > 0 )
    {
        recv( Socket, (char *)buf, 1024*10, 0x8 );
    }
    _stdoutput << ".";
    closesocket(Socket);
    WSACleanup();
    reply = std::string(buf);

    // now thest the reply for "spineminer:00-granted"
    if (reply.find(REPLY)!=string::npos)
    {
        _stdoutput << " done." << endl;
        return;
    }
    else
    {
        _stdoutput << " failed.";
        exit(-1);
    }

    return;
}

#endif // NOVERIFICATION
