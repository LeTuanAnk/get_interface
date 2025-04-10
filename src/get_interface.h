#ifndef GET_INTERFACE_H
#define GET_INTERFACE_H



typedef struct
{
    char name[16];
    char mac[18];
    char ipv4[16];
    char ipv6[50];
    char status[50];
    char bcast[16]; 
    char mask[16];  
} 
interface_info;

void gInf_struct_interface_get (void);

void gInf_struct_interface_get_test (void);

void gInf_struct_interface_print (void);

#endif