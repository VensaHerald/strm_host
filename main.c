#include "headers.h"

#define RECV_BUF_SIZE 1024
#define PORT "6969"
#define BACKLOG 5

//int accept_hand(int * parent_soc, const struct addrinfo *parent_addr);
DWORD WINAPI accept_hand(PVOID parent_soc);


int main(void)
{
    
    WSADATA wsadata;
    struct addrinfo hints, *local, *p;
    
    int loc_soc, acc_soc, rc;
    
    
    rc = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (rc != 0) pr_err_mess("WSAStartup");
    
    // get local
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(NULL, PORT, &hints, &local) != 0) pr_err_mess("getaddrinfo");

    // socket, bind and listen
    for (p = local; p != NULL; p = p->ai_next)
    {
        loc_soc = socket(local->ai_family, local->ai_socktype, local->ai_protocol);
        if(loc_soc == -1) pr_err_mess("socket");


        if(bind(loc_soc, local->ai_addr, local->ai_addrlen) == -1) 
        {
            closesocket(loc_soc);
            pr_err_mess("bind");
        }
    }


    if(listen(loc_soc, BACKLOG) == -1) pr_err_mess("listen");
    
    

    HANDLE h_threads[BACKLOG];
    DWORD d_thread_id[BACKLOG];




    for (int i = 0; i<3; i++)
    {
    h_threads[i] = CreateThread(
        NULL,
        0,
        accept_hand,
        &loc_soc,
        0,
        &d_thread_id[i]
        );
    
    }
    
    
    WaitForMultipleObjects(BACKLOG, h_threads, TRUE, INFINITE);


    closesocket(acc_soc);
    closesocket(loc_soc);
    freeaddrinfo(local);
    WSACleanup();


    return 0;
}


DWORD WINAPI accept_hand(PVOID parent_soc)
{
    int acc_soc;
    struct sockaddr_storage inbound;
    socklen_t inbound_size = sizeof inbound;
    acc_soc = accept((SOCKET)parent_soc, (struct sockaddr*)&inbound, &inbound_size);
    char recv_buf[RECV_BUF_SIZE];

    for(int i = 0; i < 10; i++)
    {
        memset(recv_buf, 0, sizeof recv_buf);
        recv(acc_soc, recv_buf, 30,0);
        send(acc_soc, recv_buf, 30,0);
        
    }


    return 0;
}

int pr_err_mess(char *funcname)
{
    DWORD er_mess = WSAGetLastError(); 
    LPSTR err_buf = malloc(500 * sizeof err_buf);
    if (err_buf == NULL)
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, er_mess, 0, err_buf, 500, NULL);
    fprintf(stderr, "%s: %s\n", funcname, err_buf);
    free(err_buf);
    ExitProcess(2);
}