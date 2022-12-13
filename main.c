#include <sys/socket.h>
#include <stdio.h>
#include <stddef.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <magic.h>

int read_all_file(char* file_path, char* dest) {
    int fd = open(file_path, O_RDONLY); 
    int filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    read(fd, dest, filesize);
    close(fd);
    return filesize;
}

void http_server(int port, int process_request(const char*, char*)) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_in = {AF_INET, htons(port), INADDR_ANY};
    if (bind(server_fd, (struct sockaddr *) &addr_in, sizeof(addr_in))) {
        printf("Socket unable to bind"); 
        return;
    }
    if (listen(server_fd, 10)) {
        printf("Socket unable to listen");
        return;
    }
    while(1){
        int clien = accept(server_fd, NULL, 0);
        char req[16384] = {0};
        char res[16384] = {0};
        if(!read(clien, req, 16384)) {
            close(clien); 
            continue;
        }
        process_request(req, res);
        write(clien, res, strlen(res));
        close(clien);
    }
}

int parse_request(const char* req, char* verb, char* url, char* header, char* data) {
    const char* begin = req;
    const char* end = req;
    while(*(end++) != ' '); // interating until end points to ' '
    memcpy(verb, begin, end - begin - 1); // copy from the begining of the request until end which will fill the verb
    begin = end;
    while(*(end++) != ' '); // interating until end points to ' '
    memcpy(url, begin, end - begin - 1); // copy from the end of the verb to the next ' ' that will get the url
    while(*(begin++) != '\n'); // skipping the version of http used
    end = begin;
    char* ptr_in_header = header;
    while(1) {
        if(*(begin) == '\r') break;
        while(*(end++) != '\n'); // interating until end points to ' '
        memcpy(ptr_in_header, begin, end - begin); // copy from the end of the verb to the next ' ' that will get the url
        ptr_in_header += end - begin;
        begin = end;
    }
    strcpy(data, ptr_in_header + 1);
    return 0;
}

int select_file(const char* url, char* path) {
    strcpy(".", path);
    strcat(path, url);

    struct stat filestat;
    if(stat(path, &filestat)) {
        char temppath[256];
        strcpy(temppath, path);
        char* end = temppath + strlen(temppath);
        while(*(end--) != '/');
        strcpy(end + 1, "rules.txt");
        struct stat newfilestat;
        if(stat(temppath, &newfilestat)) return -1;
        // read content from rules to determine what file to read
        // put it in path variable 
        // return
    }
    if(S_ISDIR(filestat)) {
        char rulescontent[16384];
        // read content from rules.txt first line to use
        // put it in path variable 
        // return
    }
    if(S_ISREG(filestat)) {
        char filecontent[16384];
        int filesize = read_all_file(path, filecontent);
        const char* type;
        magic_t magic = magic_open(MAGIC_MIME_TYPE);
        magic_load(magic, NULL);
        magic_compile(magic, NULL);
        type = magic_file(magic, filename);
        magic_close(magic);
        sprintf(res, "HTTP/1.1 200 OK \nContent-Type: %s\nContent-Length: %d\n\n", type, filesize);
        memcpy(res + strlen(res), filecontent, filesize);
    }
    return -1;
}

int response_generator(const char* url, char* res) {
    char path[256];
    select_file();

    char filecontent[16384];
    int filesize = read_all_file(path, filecontent);
}

int myclient(const char* req, char* res) {
    char verb[64];
    char url[128];
    char headers[4096];
    char data[4096];
    parse_request(req, verb, url, headers, data); 
    
    
}

int main(int argc, const char* argv[]) {
    http_server(8000, &myclient);
}

