#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parser_name(const char *line, char *name) {
    char buffer_copy[256];
    strncpy(buffer_copy, line, sizeof(buffer_copy) - 1);
    buffer_copy[sizeof(buffer_copy) - 1] = '\0';

    char *token = strtok(buffer_copy, " ");
    if (token) {
        strncpy(name, token, 127);
        name[127] = '\0';
    }
}

void parser_mac(const char *line, char *mac) {
    char mac_copy[64];
    const char *mac_pointer = strstr(line, "HWaddr");
    if (mac_pointer) {
        mac_pointer += strlen("HWaddr");
        while (*mac_pointer == ' ' || *mac_pointer == '\t') {
            mac_pointer++;
        }

        strncpy(mac_copy, mac_pointer, sizeof(mac_copy) - 1);
        mac_copy[sizeof(mac_copy) - 1] = '\0';

        char *token = strtok(mac_copy, " \t\n");
        strncpy(mac, token ? token : "*", 127);
        mac[127] = '\0';
    }
}

void parser_ipv4(const char *line, char *ipv4, char *bcast, char *mask) {
    const char *token = strstr(line, "inet addr:");
    if (token) {
        token += strlen("inet addr:");
        token = strtok((char *)token, " ");
        strncpy(ipv4, token ? token : "*", 127);
        ipv4[127] = '\0';

        while ((token = strtok(NULL, " ")) != NULL) {
            if (strncmp(token, "Bcast:", 6) == 0) {
                strncpy(bcast, token + 6, 127);
                bcast[127] = '\0';
            } else if (strncmp(token, "Mask:", 5) == 0) {
                strncpy(mask, token + 5, 127);
                mask[127] = '\0';
            }
        }
    }
}


void parser_ipv6(const char *line, char *ipv6) {
    const char *token = strstr(line, "inet6 addr: ");
    if (token) {
        token += strlen("inet6 addr: ");
        token = strtok((char *)token, " \n");
        strncpy(ipv6, token ? token : "*", 127);
        ipv6[127] = '\0';
    }
}

void parser_status(const char *line, char *status) {
    if (strstr(line, "MTU:") && strstr(line, "UP")) {
        strcpy(status, "UP");
    }
}
