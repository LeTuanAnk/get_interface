#include <stdio.h>
#include <stdlib.h>
#include "get_interface.h"
#include "log.h"
#include "defines.h"
#include <string.h>

// khai bao list
static void _get_data_from_file(char *file, char *buf);
static void _get_data_from_dev(char *cmd, char *buf);
static void _process_data(char *buf);

static char buf[BUFFER_SIZE];
interface_info interfaces[MAX_INTERFACES];
static int count = 0;

static void _get_data_from_file(char *file, char *buf)
{

    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Cannot open file", __func__, __LINE__);
        return;
    }

    // Đọc dữ liệu vào buffer
    size_t bytes_read = fread(buf, 1, BUFFER_SIZE - 1, fp);
    buf[bytes_read] = '\0';
    // Đóng file
    fclose(fp);
}

static void _get_data_from_dev(char *cmd, char *buf)
{

    FILE *fp = popen(cmd, "r");
    if (!fp)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Popen Failed", __func__, __LINE__);
        return;
    }
    size_t bytes_read = fread(buf, 1, BUFFER_SIZE - 1, fp);
    buf[bytes_read] = '\0';
    // Đóng file
    pclose(fp);
}

// process
static void _process_data(char *buf)
{
    int iface_index = -1;
    //char *ptr = buf;
    char *ptr = strdup(buf);
    char *line;

    char buffer_copy[BUFFER_SIZE];
    char mac_copy[BUFFER_SIZE];

    while ((line = strtok(ptr, "\n")) != NULL) //
    {
        if (line[0] != ' ' && strstr(line, "Link encap:")) // name interface
        {
            iface_index++;
            strncpy(buffer_copy, line, sizeof(buffer_copy) - 1);
            buffer_copy[sizeof(buffer_copy) - 1] = '\0';

            char *token = strtok(buffer_copy, " ");
            if (token)
            {
                strncpy(interfaces[iface_index].name, token, sizeof(interfaces[iface_index].name) - 1);
                interfaces[iface_index].name[sizeof(interfaces[iface_index].name) - 1] = '\0';
            }

            //  MAC Address
            char *mac_pointer = strstr(line, "HWaddr");
            if (mac_pointer)
            {
                mac_pointer += strlen("HWaddr");
                while (*mac_pointer == ' ' || *mac_pointer == '\t')
                    mac_pointer++;

                strncpy(mac_copy, mac_pointer, sizeof(mac_copy) - 1);
                mac_copy[sizeof(mac_copy) - 1] = '\0';

                token = strtok(mac_copy, " \t\n");
                strncpy(interfaces[iface_index].mac, token ? token : "*", sizeof(interfaces[iface_index].mac) - 1);
                interfaces[iface_index].mac[sizeof(interfaces[iface_index].mac) - 1] = '\0';
            }
            else
            {
                strcpy(interfaces[iface_index].mac, "*");
            }

            // defaul
            strcpy(interfaces[iface_index].ipv4, "*");
            strcpy(interfaces[iface_index].ipv6, "*");
            strcpy(interfaces[iface_index].status, "UNKNOWN");
            strcpy(interfaces[iface_index].bcast, "*");
            strcpy(interfaces[iface_index].mask, "*");
        }

        else if (strstr(line, "inet addr:")) // IPv4, Bcast, Mask
        {
            char *token = strstr(line, "inet addr:") + strlen("inet addr:");
            token = strtok(token, " ");
            strncpy(interfaces[iface_index].ipv4, token ? token : "*", sizeof(interfaces[iface_index].ipv4) - 1);
            interfaces[iface_index].ipv4[sizeof(interfaces[iface_index].ipv4) - 1] = '\0';

            while ((token = strtok(NULL, " ")) != NULL)
            {
                if (strncmp(token, "Bcast:", 6) == 0)
                {
                    strncpy(interfaces[iface_index].bcast, token + 6, sizeof(interfaces[iface_index].bcast) - 1);
                    interfaces[iface_index].bcast[sizeof(interfaces[iface_index].bcast) - 1] = '\0';
                }
                else if (strncmp(token, "Mask:", 5) == 0)
                {
                    strncpy(interfaces[iface_index].mask, token + 5, sizeof(interfaces[iface_index].mask) - 1);
                    interfaces[iface_index].mask[sizeof(interfaces[iface_index].mask) - 1] = '\0';
                }
            }
        }

        else if (strstr(line, "inet6 addr: ")) // IPv6
        {
            char *token = strstr(line, "inet6 addr: ") + strlen("inet6 addr: ");
            token = strtok(token, " \n");
            strncpy(interfaces[iface_index].ipv6, token ? token : "*", sizeof(interfaces[iface_index].ipv6) - 1);
            interfaces[iface_index].ipv6[sizeof(interfaces[iface_index].ipv6) - 1] = '\0';
        }

        else if (strstr(line, "MTU:")) // Status (UP/DOWN)
        {
            strcpy(interfaces[iface_index].status, strstr(line, "UP") ? "UP" : "DOWN");
        }
        ptr = NULL;
    }
    count = iface_index + 1;
}



void gInf_struct_interface_get(void)
{
    _get_data_from_dev(PATH_CMD, buf);
    _process_data(buf);
}

void gInf_struct_interface_get_test(void)
{
    _get_data_from_file(PATH_DATA_FILE, buf);
    _process_data(buf);
}



void gInf_struct_interface_print(void)
{
    LOG(LOG_LVL_DEBUG, "%s, %d: Start", __func__, __LINE__);

    FILE *file = fopen(PATH_FILE_INTERFACE_INFO, "w");
    if (!file)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Cannot open file %s", __func__, __LINE__, PATH_FILE_INTERFACE_INFO);
        return;
    }
    if (fprintf(file, "%-5s %-15s %-18s %-15s %-15s %-15s %-50s %-8s \n",
                "ID", "Name", "MAC Address", "IPv4", "Bcast", "Mask", "IPv6", "Status") < 0)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Failed to write header", __func__, __LINE__);
        fclose(file);
        return;
    }
    if (fprintf(file, "--------------------------------------------------------------------------------------------------------------------------------------------\n") < 0)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Failed to write separator", __func__, __LINE__);
        fclose(file);
        return;
    }
    for (int i = 0; i < count; i++)
    {

        if (fprintf(file, "%-5d %-15s %-18s %-15s %-15s %-15s %-50s %-8s \n", i + 1,
                    interfaces[i].name, interfaces[i].mac, interfaces[i].ipv4, interfaces[i].bcast,
                    interfaces[i].mask, interfaces[i].ipv6, interfaces[i].status) < 0)
        {
            LOG(LOG_LVL_ERROR, "%s, %d: Failed to write interface %d", __func__, __LINE__, i);
            break;
        }
    }
    if (fclose(file) != 0)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Failed to close file", __func__, __LINE__);
    }
    LOG(LOG_LVL_DEBUG, "%s, %d: End", __func__, __LINE__);
}
