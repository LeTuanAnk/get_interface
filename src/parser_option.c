#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "defines.h"
#include "log.h"
#include "get_interface.h"
#include "get_list_interface.h"


void PO_parse_combined_options(int argc, char *argv[])
{
    // I. Define
    int opt;
    int option_index;
    char *get_itf = NULL;
    char *add_itf = NULL;
    char *search_itf = NULL;
    char *delete_itf = NULL;
    bool alldelete_itf = false;
    bool interface = false;


    // II. Function
    static struct option long_options[] = {
        {"get-interface", required_argument, 0, 'g'},
        {"add", required_argument, 0, 'a'},
        {"delete", required_argument, 0, 'd'},
        {"search", required_argument, 0, 's'},
        {"delete-all", no_argument, 0, 'x'},
        {0, 0, 0, 0} // End array
    };

    while ((opt = getopt_long(argc, argv, "g:a:d:s:x", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'g':
            interface = true;
            get_itf = optarg;
            break;
        case 'a':
            add_itf = optarg;
            break;
        case 'd':
            delete_itf = optarg;
            break;
        case 's':
            search_itf = optarg;
            break;
        case 'x':
            alldelete_itf = true;
            break;
        default:
            break;
        }
    }

    if (interface)
    {
        if (0 == strcmp(get_itf, "cmd"))
        {
            gInf_list_interface_get();
            gInf_list_interface_print();
        }
        else if (0 == strcmp(get_itf, "test"))
        {
            gInf_list_interface_get_test();  
            gInf_list_interface_print();
        }
        else
        {
            LOG(LOG_LVL_ERROR, "%s, %d: Usage: get_info -g [test]", __func__, __LINE__);
        }
    }

    else if (add_itf != NULL)
    {
        if (0 == strcmp(add_itf, "br2"))
        {
            gInf_clear_list(&dataList);
            gInf_load_user_data();  
            add_new("br2", "00:11:22:33:44:55", "192.168.1.1", "192.168.1.255",
                    "255.255.255.0", "fe80::1", "UP");
            gInf_list_interface_print();
        }
        else if (0 == strcmp(add_itf, "eth0.5")){
            gInf_clear_list(&dataList);
            gInf_load_user_data();  
            add_new("eth0.5", "AA:11:BB:33:CC:55", "192.168.1.10", "192.168.1.255",
                    "255.255.255.0", "fe80::1", "UP");
            gInf_list_interface_print();  
        }
        else
        {
            LOG(LOG_LVL_ERROR, "%s, %d: ERROR ADD", __func__, __LINE__);
        }
    }
    
    else if (search_itf != NULL)
    {
        gInf_clear_list(&dataList);
        gInf_load_user_data();
        LOG(LOG_LVL_DEBUG, "%s, %d: Searching for: %s", __func__, __LINE__, search_itf);
        Node *n = search_entry(search_itf);
        if (n)
        {
            printf("Found interface: %s\nMAC: %s\nIpv4: %s\nIpv6: %s\nStatus: %s\n",
                   n->name, n->mac, n->ipv4, n->ipv6, n->status);  
            LOG(LOG_LVL_DEBUG, "%s, %d: DONE SEARCH", __func__, __LINE__);
        }
        else
        {
            LOG(LOG_LVL_DEBUG, "%s, %d: Interface not found. ", __func__, __LINE__);
        }
    }

    else if (delete_itf != NULL) // delete 1 entry
    {
        gInf_clear_list(&dataList);
        gInf_load_user_data();
        if (delete_entry(delete_itf))
        {
            gInf_list_interface_print();
            LOG(LOG_LVL_DEBUG, "%s, %d: DONE DELETE", __func__, __LINE__);
        }
        else
        {
            LOG(LOG_LVL_ERROR, "%s, %d: ERROR DELETE", __func__, __LINE__);
        }
    }

    else if (alldelete_itf) // delete all
    {
        gInf_clear_list(&dataList);
        gInf_load_user_data();
        delete_all();
        gInf_list_interface_print();
        LOG(LOG_LVL_DEBUG, "%s, %d: ERROR DELETE ALL", __func__, __LINE__);
    }

    else
    {
        LOG(LOG_LVL_WARN, "%s, %d: Usage: get_info [-R] MODE[,MODE]...", __func__, __LINE__);
        exit(1);
    }
}


            // gInf_struct_interface_get_test();
            // gInf_struct_interface_print();
