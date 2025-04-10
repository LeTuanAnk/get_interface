#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_list_interface.h"
#include "log.h"
#include "defines.h"
#include "parse.h"

Node *dataList = NULL;
static void _reset_interface_data(char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status);
static void _append(Node **head_ref, char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status);
static void _get_data_from_file(char *file, char *buf);
static void _get_data_from_dev(char *cmd, char *buf);
static void _process_data(char *buf);
static int _check_list(Node **head, char *name);
static char buf[BUFFER_SIZE];

static void _reset_interface_data(char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status)
{
    strcpy(name, "");
    strcpy(mac, "");
    strcpy(ipv4, "");
    strcpy(bcast, "");
    strcpy(mask, "");
    strcpy(ipv6, "");
    strcpy(status, "DOWN");
} // reset  => status clean

static int _check_list(Node **head, char *name)
{
    Node *current = *head;

    while (current != NULL)
    {
        if (strncmp(current->name, name, 128) == 0)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

static void _append(Node **head_ref, char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status)
{

    Node *new_node = (Node *)malloc(sizeof(Node));
    memset(new_node, 0, sizeof(Node));
    new_node->name = strdup(name);
    new_node->mac = strdup(mac);
    new_node->ipv4 = strdup(ipv4);
    new_node->bcast = strdup(bcast);
    new_node->mask = strdup(mask);
    new_node->ipv6 = strdup(ipv6);
    new_node->status = strdup(status);
    new_node->next = NULL;

    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }

    Node *last = *head_ref;
    while (last->next != NULL)
    {
        last = last->next;
    }
    last->next = new_node;
}

static void _get_data_from_file(char *file, char *buf)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Cannot open file", __func__, __LINE__);
        return;
    }
    size_t bytes_read = fread(buf, 1, BUFFER_SIZE - 1, fp);
    buf[bytes_read] = '\0';
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
    pclose(fp);
}

static void _process_data(char *buf)
{
    char name[128], mac[128], ipv4[128], bcast[128], mask[128], ipv6[128], status[128];
    char *line;
    char *saveptr;

    _reset_interface_data(name, mac, ipv4, bcast, mask, ipv6, status);
    line = strtok_r(buf, "\n", &saveptr);
    while (line != NULL)
    {
        if (line[0] != ' ' && strstr(line, "Link encap:"))
        {
            if (name[0] != '\0')
            {
                _append(&dataList, name, mac, ipv4, bcast, mask, ipv6, status);
                _reset_interface_data(name, mac, ipv4, bcast, mask, ipv6, status);
            }
            parser_name(line, name);
            parser_mac(line, mac);
        }
        else if (strstr(line, "inet addr:"))
        {
            parser_ipv4(line, ipv4, bcast, mask);
        }
        else if (strstr(line, "inet6 addr: "))
        {
            parser_ipv6(line, ipv6);
        }
        else if (strstr(line, "MTU:"))
        {
            parser_status(line, status);
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }
    if (name[0] != '\0')
    {
        if (!_check_list(&dataList, name))
        {
            _append(&dataList, name, mac, ipv4, bcast, mask, ipv6, status);
        }
        else
        {
            LOG(LOG_LVL_ERROR, "%s, %d: Interface has existed ", __func__, __LINE__);
        }
    }
}

void gInf_list_interface_get(void)
{
    _get_data_from_dev(PATH_CMD, buf);
    _process_data(buf);
}

void gInf_list_interface_get_test(void)
{
    _get_data_from_file(PATH_DATA_FILE, buf);
    _process_data(buf);
}

void gInf_list_interface_print(void)
{
    FILE *fp = fopen(PATH_LIST_INTERFACE, "w");
    if (!fp)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Cannot open file for writing", __func__, __LINE__);
        return;
    }

    Node *current = dataList;
    while (current != NULL)
    {
        fprintf(fp, "Name: %s\n", current->name);
        fprintf(fp, "MAC: %s\n", current->mac);
        fprintf(fp, "IPv4: %s\n", current->ipv4);
        fprintf(fp, "Bcast: %s\n", current->bcast);
        fprintf(fp, "Mask: %s\n", current->mask);
        fprintf(fp, "IPv6: %s\n", current->ipv6);
        fprintf(fp, "Status: %s\n", current->status);
        fprintf(fp, "--------------------------\n");
        current = current->next;
    }

    fclose(fp);
}

void gInf_load_user_data(void)
{
    FILE *fp;
    Node *node;
    Node *head;
    Node *tail;
    char line[256];

    fp = fopen(PATH_LIST_INTERFACE, "r");
    if (!fp)
    {
        LOG(LOG_LVL_ERROR, "%s, %d: Cannot open user_data.txt", __func__, __LINE__);
        return;
    }

    node = head = tail = NULL;
    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\n")] = 0;
        if (strncmp(line, "Name:", 5) == 0)
        {
            node = malloc(sizeof(Node));
            if (!node)
            {
                LOG(LOG_LVL_ERROR, "%s, %d: malloc failed", __func__, __LINE__);
                continue;
            }
            memset(node, 0, sizeof(Node));
            node->name = strdup(line + 6);
        }
        else if (strncmp(line, "MAC:", 4) == 0)
        {
            node->mac = strdup(line + 5);
        }
        else if (strncmp(line, "IPv4:", 5) == 0)
        {
            node->ipv4 = strdup(line + 6);
        }
        else if (strncmp(line, "Bcast:", 6) == 0)
        {
            node->bcast = strdup(line + 7);
        }
        else if (strncmp(line, "Mask:", 5) == 0)
        {
            node->mask = strdup(line + 6);
        }
        else if (strncmp(line, "IPv6:", 5) == 0)
        {
            node->ipv6 = strdup(line + 6);
        }
        else if (strncmp(line, "Status:", 7) == 0)
        {
            node->status = strdup(line + 8);
        }
        else if (strncmp(line, "----------------", 16) == 0 && node != NULL)
        {
            node->next = NULL;
            if (head == NULL)
            {
                head = node;
                tail = node;
            }
            else
            {
                tail->next = node;
                tail = node;
            }
            node = NULL;
        }
    }
    dataList = head;
    fclose(fp);
}

void gInf_clear_list(Node **head)
{
    Node *temp;
    while (*head != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp->name);
        free(temp->mac);
        free(temp->ipv4);
        free(temp->bcast);
        free(temp->mask);
        free(temp->ipv6);
        free(temp->status);
        free(temp);
    }
}

// add
void add_new(char *name, char *mac, char *ipv4, char *bcast, char *mask, char *ipv6, char *status)
{
    if (!_check_list(&dataList,name))
    {
        _append(&dataList, name, mac, ipv4, bcast, mask, ipv6, status);
    }
    
}

// Delele_1 entry
int delete_entry(char *name)
{
    Node *current = dataList, *previou = NULL;

    while (current != NULL && strcmp(current->name, name) != 0)
    {
        previou = current;
        current = current->next;
    }
    if (current == NULL)
        return 0;
    if (previou == NULL)
        dataList = current->next;
    else
        previou->next = current->next;
    free(current->name);
    free(current->mac);
    free(current->ipv4);
    free(current->bcast);
    free(current->mask);
    free(current->ipv6);
    free(current->status);
    free(current);
    return 1;
}

// delete_all_entry
void delete_all(void)
{
    Node *current = dataList;
    Node *next;
    while (current != NULL)
    {
        next = current->next;
        free(current->name);
        free(current->mac);
        free(current->ipv4);
        free(current->bcast);
        free(current->mask);
        free(current->ipv6);
        free(current->status);
        free(current);
        current = next;
    }
    dataList = NULL;
}

// Search entry
Node *search_entry(char *name)
{
    Node *current = dataList;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}
