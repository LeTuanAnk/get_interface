#ifndef PARSE_H
#define PARSE_H

void parser_name(const char *line, char *name);
void parser_mac(const char *line, char *mac);
void parser_ipv4(const char *line, char *ipv4, char *bcast, char *mask);
void parser_ipv6(const char *line, char *ipv6);
void parser_status(const char *line, char *status);

#endif