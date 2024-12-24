#ifndef HTTP_H
#define HTTP_H

#define METHOD_SIZE 16
#define PATH_SIZE 256
#define PROTOCOL_SIZE 16
#define HEADERS_SIZE 1024
#define BODY_SIZE 2048
#define COOKIE_SIZE 256
#define MIME_SIZE 64
#define LENGTH_SIZE 32

struct HTTPreq {
    char method[METHOD_SIZE];
    char path[PATH_SIZE];
    char protocol[PROTOCOL_SIZE];
    char headers[HEADERS_SIZE];
    char body[BODY_SIZE];
    char cookie[COOKIE_SIZE];
};

struct HTTPresp {
    int status_code;
    char mime[MIME_SIZE];
    char length[LENGTH_SIZE];
    char headers[HEADERS_SIZE];
    char body[BODY_SIZE];
    char cookie[COOKIE_SIZE];
};

char *http_resp_gen(struct HTTPresp resp);
struct HTTPreq http_req_parse(const char *req);
char *http_mime(const char *filename);

#endif
