#define LOG_TAG    "main"
#include <elog.h>
#include <stdio.h>
#include <stdlib.h>
#include "console.h"
#include "main.h"

extern void board_lowlevel_init(void);
static void test_elog(void);

int main(void){
    
   	board_lowlevel_init();
	uart_init();

    setvbuf(stdout, NULL, _IONBF, 0); // 禁用 stdout 缓冲

    /* initialize EasyLogger */
    elog_init();
	elog_set_text_color_enabled(true);

    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG );
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG );
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG );
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);
    /* start EasyLogger */
    elog_start();

    printf("EasyLogger started.\n");
    test_elog();
    while(1) {
		
    }
    
    return 0;
}

/**
 * EasyLogger demo
 */
void test_elog(void) {
    /* test log output for all level */
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
    elog_raw("Hello EasyLogger!");
}
                                              
