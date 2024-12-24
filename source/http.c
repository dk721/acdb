#include <stdio.h>
#include <string.h>
#include "http.h"

struct HTTPreq
http_req_parse(const char *req)
{
    struct HTTPreq parsed_req = {0};

    // Метод
    const char *method_end = strchr(req, ' ');
    if (method_end) {
        size_t method_len = method_end - req;
        if (method_len < METHOD_SIZE) {
            strncpy(parsed_req.method, req, method_len);
            parsed_req.method[method_len] = '\0';
        } else {
            strncpy(parsed_req.method, req, METHOD_SIZE - 1);
            parsed_req.method[METHOD_SIZE - 1] = '\0';
        }
    }

    // Путь
    const char *path_start = method_end ? method_end + 1 : NULL;
    const char *path_end = path_start ? strchr(path_start, ' ') : NULL;
    if (path_start && path_end) {
        size_t path_len = path_end - path_start;
        if (path_len < PATH_SIZE) {
            strncpy(parsed_req.path, path_start, path_len);
            parsed_req.path[path_len] = '\0';
        } else {
            strncpy(parsed_req.path, path_start, PATH_SIZE - 1);
            parsed_req.path[PATH_SIZE - 1] = '\0';
        }
    }

    // Протокол
    const char *protocol_start = path_end ? path_end + 1 : NULL;
    const char *protocol_end = protocol_start ? strchr(protocol_start, '\r') : NULL;
    if (protocol_start && protocol_end) {
        size_t protocol_len = protocol_end - protocol_start;
        if (protocol_len < PROTOCOL_SIZE) {
            strncpy(parsed_req.protocol, protocol_start, protocol_len);
            parsed_req.protocol[protocol_len] = '\0';
        } else {
            strncpy(parsed_req.protocol, protocol_start, PROTOCOL_SIZE - 1);
            parsed_req.protocol[PROTOCOL_SIZE - 1] = '\0';
        }
    }

    // Cookie
    const char *cookie_header = strstr(req, "Cookie: ");
    if (cookie_header) {
        const char *cookie_start = cookie_header + 8;
        const char *cookie_end = strstr(cookie_start, "\r\n");
        if (cookie_end) {
            size_t cookie_len = cookie_end - cookie_start;
            if (cookie_len < COOKIE_SIZE) {
                strncpy(parsed_req.cookie, cookie_start, cookie_len);
                parsed_req.cookie[cookie_len] = '\0';
            } else {
                strncpy(parsed_req.cookie, cookie_start, COOKIE_SIZE - 1);
                parsed_req.cookie[COOKIE_SIZE - 1] = '\0';
            }
        }
    }

    return parsed_req;
}

char *
http_resp_gen(struct HTTPresp resp)
{
    static char response[4096];
    memset(response, 0, sizeof(response));

    sprintf(response, "HTTP/1.1 %d OK\r\n", resp.status_code);
    strcat(response, "Server: "PROGRAM" "VERSION"\r\n");

    if (strlen(resp.mime) > 0) {
        strcat(response, "Content-Type: ");
        strcat(response, resp.mime);
        strcat(response, "\r\n");
    }

    if (strlen(resp.length) > 0) {
        strcat(response, "Content-Length: ");
        strcat(response, resp.length);
        strcat(response, "\r\n");
    }

    if (strlen(resp.cookie) > 0) {
        strcat(response, "Set-Cookie: ");
        strcat(response, resp.cookie);
        strcat(response, "\r\n");
    }

    strcat(response, "\r\n");

    if (strlen(resp.body) > 0) {
        strcat(response, resp.body);
    }

    return response;
}

char *
http_mime(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (ext == NULL) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".txt") == 0) return "text/plain";
    if (strcmp(ext, ".svg") == 0) return "image/svg+xml";

    return "application/octet-stream";
}
