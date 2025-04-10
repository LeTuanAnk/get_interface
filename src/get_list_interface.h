#ifndef GET_LIST_INTERFACE_H
#define GET_LIST_INTERFACE_H


typedef struct Node {
    char *name;
    char *mac;
    char *ipv4;
    char *bcast;
    char *mask;
    char *ipv6;
    char *status;
    struct Node* next;
} Node;
extern Node *dataList;


void gInf_list_interface_get (void);
void gInf_list_interface_get_test (void);
void gInf_list_interface_print (void);
void gInf_clear_list(Node **head);
void gInf_load_user_data(void); 


void add_new(char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status);
int delete_entry(char *name);
void delete_all(void);
Node* search_entry(char *name);


#endif