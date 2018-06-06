//#include <stdio.h> /* printf, sprintf */
//#include <stdlib.h> /* exit, atoi, malloc, free */
//#include <unistd.h> /* read, write, close */
//#include <string.h> /* memcpy, memset */
//#include <sys/socket.h> /* socket, connect */
//#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
//#include <netdb.h> /* struct hostent, gethostbyname */
//
//void error(const char *msg) { perror(msg); exit(0); }
//
//int main(int argc,char *argv[])
//{
//    int i;
//
//    /* first where are we going to send it? */
//    int portno = atoi(argv[2])>0?atoi(argv[2]):80;
//    const char *host = strlen(argv[1])>0?argv[1]:"localhost";
//
//    struct hostent *server;
//    struct sockaddr_in serv_addr;
//    int sockfd, bytes, sent, received, total, message_size;
//    char *message, response[4096];
//
//    if (argc < 5) { puts("Parameters: <host> <port> <method> <path> [<data> [<headers>]]"); exit(0); }
//
//    /* How big is the message? */
//    message_size=0;
//    if(!strcmp(argv[3],"GET"))
//    {
//        message_size+=strlen("%s %s%s%s HTTP/1.0\r\n");        /* method         */
//        message_size+=strlen(argv[3]);                         /* path           */
//        message_size+=strlen(argv[4]);                         /* headers        */
//        if(argc>5)
//            message_size+=strlen(argv[5]);                     /* query string   */
//        for(i=6;i<argc;i++)                                    /* headers        */
//            message_size+=strlen(argv[i])+strlen("\r\n");
//        message_size+=strlen("\r\n");                          /* blank line     */
//    }
//    else
//    {
//        message_size+=strlen("%s %s HTTP/1.0\r\n");
//        message_size+=strlen(argv[3]);                         /* method         */
//        message_size+=strlen(argv[4]);                         /* path           */
//        for(i=6;i<argc;i++)                                    /* headers        */
//            message_size+=strlen(argv[i])+strlen("\r\n");
//        if(argc>5)
//            message_size+=strlen("Content-Length: %d\r\n")+10; /* content length */
//        message_size+=strlen("\r\n");                          /* blank line     */
//        if(argc>5)
//            message_size+=strlen(argv[5]);                     /* body           */
//    }
//
//    /* allocate space for the message */
//    message=(char*)malloc(message_size);
//
//    /* fill in the parameters */
//    if(!strcmp(argv[3],"GET"))
//    {
//        if(argc>5)
//            sprintf(message,"%s %s%s%s HTTP/1.0\r\n",
//                    strlen(argv[3])>0?argv[3]:"GET",               /* method         */
//                    strlen(argv[4])>0?argv[4]:"/",                 /* path           */
//                    strlen(argv[5])>0?"?":"",                      /* ?              */
//                    strlen(argv[5])>0?argv[5]:"");                 /* query string   */
//        else
//            sprintf(message,"%s %s HTTP/1.0\r\n",
//                    strlen(argv[3])>0?argv[3]:"GET",               /* method         */
//                    strlen(argv[4])>0?argv[4]:"/");                /* path           */
//        for(i=6;i<argc;i++)                                    /* headers        */
//        {strcat(message,argv[i]);strcat(message,"\r\n");}
//        strcat(message,"\r\n");                                /* blank line     */
//    }
//    else
//    {
//        sprintf(message,"%s %s HTTP/1.0\r\n",
//                strlen(argv[3])>0?argv[3]:"POST",                  /* method         */
//                strlen(argv[4])>0?argv[4]:"/");                    /* path           */
//        for(i=6;i<argc;i++)                                    /* headers        */
//        {strcat(message,argv[i]);strcat(message,"\r\n");}
//        if(argc>5)
//            sprintf(message+strlen(message),"Content-Length: %d\r\n",strlen(argv[5]));
//        strcat(message,"\r\n");                                /* blank line     */
//        if(argc>5)
//            strcat(message,argv[5]);                           /* body           */
//    }
//
//    /* What are we going to send? */
//    printf("Request:\n%s\n",message);
//
//    /* create the socket */
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0) error("ERROR opening socket");
//
//    /* lookup the ip address */
//    server = gethostbyname(host);
//    if (server == NULL) error("ERROR, no such host");
//
//    /* fill in the structure */
//    memset(&serv_addr,0,sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(portno);
//    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
//
//    /* connect the socket */
//    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
//        error("ERROR connecting");
//
//    /* send the request */
//    total = strlen(message);
//    sent = 0;
//    do {
//        bytes = write(sockfd,message+sent,total-sent);
//        if (bytes < 0)
//            error("ERROR writing message to socket");
//        if (bytes == 0)
//            break;
//        sent+=bytes;
//    } while (sent < total);
//
//    /* receive the response */
//    memset(response,0,sizeof(response));
//    total = sizeof(response)-1;
//    received = 0;
//    do {
//        bytes = read(sockfd,response+received,total-received);
//        if (bytes < 0)
//            error("ERROR reading response from socket");
//        if (bytes == 0)
//            break;
//        received+=bytes;
//    } while (received < total);
//
//    if (received == total)
//        error("ERROR storing complete response from socket");
//
//    /* close the socket */
//    close(sockfd);
//
//    /* process response */
//    printf("Response:\n%s\n",response);
//
//    free(message);
//    return 0;
//}
//
//#include <cstdio>
//#include <curl/curl.h>
//
//int main()
//{
//    CURL *curl;
//    CURLcode res;
//
//    curl = curl_easy_init();
////    http://47.89.179.202:5000/turn/2 http://47.89.179.202:5000/board_string/3
//    if(curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, "http://47.89.179.202:5000/turn/2");
//        /* example.com is redirected, so we tell libcurl to follow redirection */
//        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//
//        /* Perform the request, res will get the return code */
//        res = curl_easy_perform(curl);
//
//        printf("\n%s\n",res);
//        /* Check for errors */
//        if(res != CURLE_OK)
//            fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                    curl_easy_strerror(res));
//
//        /* always cleanup */
//        curl_easy_cleanup(curl);
//    }
//    return 0;
//}
#include <iostream>
#include <string>
#include <curl/curl.h>

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }

    std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
    return size*nmemb;
}
int main()
{
    CURL *curl;
    CURLcode res;

    //curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    std::string s;

//    while (){
//
//    }
    if(curl)
    {

        curl_easy_setopt(curl, CURLOPT_URL, "http://47.89.179.202:5000/turn/2");

//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);



        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    std::cout<<s<<std::endl;

    std::cout<< "Program finished!" << std::endl;
}