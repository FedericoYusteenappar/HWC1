//
//  main.c
//  hwc1
//
//  Created by Federico Yusteenappar on 25/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#include <stdio.h>
#include "msg.h"
#include "/usr/local/opt/cunit/include/CUnit/Basic.h"
#include "buffer_test.c"

int main()
{
    CU_pSuite pSuite = NULL;
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    
    /* add a suite to the registry */
    pSuite = CU_add_suite("Produzione in un buffer unitario pieno", setup_buffer_unitario_pieno,clean_buffer_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    /* add test scenario put_non_bloccante in un buffer unitario pieno */
    if (NULL == CU_add_test(pSuite,"test put_non_bloccante buffer unitario pieno",test_produzione_non_bloccante_buffer_unitario_pieno)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
