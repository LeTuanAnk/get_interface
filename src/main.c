#include <stdio.h>
#include "log.h"
#include "defines.h"
#include "parser_option.h"
#include "get_interface.h"
#include "get_list_interface.h"

int main(int argc, char *argv[])
{
    LOG_set_level(LOG_LVL_DEBUG);
    LOG(LOG_LVL_DEBUG, "%s, %d: Start ", __func__, __LINE__);
    PO_parse_combined_options(argc, argv);
    // gInf_list_interface_get_test();
    // gInf_list_interface_print();
    // gInf_save_user_data();

   
    LOG(LOG_LVL_DEBUG, "%s, %d: End ", __func__, __LINE__);
    return 0;
}