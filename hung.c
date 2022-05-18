static bool sps_test_MOM_Warping(void)
{
    bool test_result = true;

    e_sps_conflib_result_t conflib_result = SPS_CONFLIB_OK;
    st_sps_conflib_config_t *p_config = &g_mem.config;
    st_sps_conflib_config_lut_t *p_config_lut = &g_mem.config_lut;
    st_sps_conflib_config_mom_t *p_config_mom = &g_mem.config_mom;

    st_vipdrv_ctl_t ctl = {0};
    e_vip_drv_error_t drv_error;

    const uint32_t addr_SCALE_FACTOR_MOM_live = 0x8B04u;
    const uint16_t MOM_SW_AGE = 0x600u;
    const uint16_t MOM_POST_AGE = 0x800u;
    const uint16_t MOM_SW_AGE_1 = 0x768u;
    const uint16_t MOM_POST_AGE_1 = 0x960u;

    //MOM size
    st_sps_mom_size_t size = {0};
    size.frame_width = g_mom_width;
    size.frame_height = g_mom_height;
    size.roi.roi_width = g_mom_width;
    size.roi.roi_height = g_mom_height;
    size.roi.offset_x = 0u;
    size.roi.offset_y = 0u;

    printf("\n\n*** sps_test_MOM_Warping starting ***\n\n");
    if(SPS_CONFLIB_OK == conflib_result)
    {
        st_sps_data_t* p_sps_data_physical = (st_sps_data_t*)p_config->p_config_dma_addr;
        printf("Addresses within the flow config:\n");
        printf("DescList CH0=%p, CH5=%p, CH6=%p\n", p_sps_data_physical->desc_list_0,
                p_sps_data_physical->desc_list_5, p_sps_data_physical->desc_list_6);
        printf("aei2apb dma=%p, core=%p\n", p_sps_data_physical->aei2apb_dma_execute, p_sps_data_physical->aei2apb_core);
    }

    /*Create MOM DATA */
    printf("\n*** Creating a SW-MOM ***\n");
    sps_create_mom(g_mem.p_mom_sw_addr_cpu, g_mom_width, g_mom_height,
                    g_tile_width, g_tile_height, g_tile_shift);

    printf("\n*** Creating a POST-MOM ***\n");
    sps_create_mom(g_mem.p_mom_post_addr_cpu, g_mom_width, g_mom_height,
                    g_tile_width, g_tile_height, g_tile_shift);

    /*Create a LUT */
    printf("\n*** Creating a LUT ***\n");
    sps_create_lut(g_mem.p_lut_addr_cpu);

    printf("\n*** Creating a LUT Configuration ***\n");
    sps_set_config_lut(p_config_lut, g_mem.p_lut_addr_cpu);

    /*Create a flow field */
    printf("\n*** Creating a Flow Field ***\n");
    sps_create_flow(g_mem.p_input_addr_cpu, g_flow_width, g_flow_height,
                    g_tile_width, g_tile_height, g_tile_shift);

    /*Prepare a basic configuration but don't finalize*/
    printf("\n*** Creating a Flow Configuration ***\n");
    sps_set_config_basic(p_config);

	/*/////////////////////////////////////////////////////////////////////////
	* Load SW-MOM configuration and confirm that ages of MOM are as configured.
	*/

    /*Create a SW-MOM configuration */
    if(SPS_CONFLIB_OK == conflib_result)
    {
        printf("\n*** Creating a SW-MOM Configuration ***\n");
        conflib_result = R_SPS_ConfLibSetMOM(p_config_mom, g_mem.p_mom_sw_addr_dma, &size, NULL, NULL);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibSetMOM returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Configure SW-MOM*/
    if(true == test_result)
    {
        printf("\n*** Configure SW-MOM ***\n");
        conflib_result = R_SPS_ConfLibMOMSWConfig(p_config, true, SPS_MOM_POLARITY_ZERO_IS_VALID, MOM_SW_AGE);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMSWConfig returned %d\n", conflib_result);
            test_result = false;
        }
    }

    if(true == test_result)
    {
        conflib_result = R_SPS_ConfLibMOMSWUsage(p_config, true, false, g_mom_width, g_mom_height);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMSWUsage returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Finish creating a configuration */
    if(true == test_result)
    {
        printf("\n*** Finalize Configuration ***\n");
        conflib_result = R_SPS_ConfLibFinalize(p_config);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibFinalize returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Start DRV */
    if(true == test_result)
    {
        printf("\n*** Starting the vipdrv ***\n");
        drv_error = sps_vipdrv_start(&ctl);
        if(R_VIPDRV_ERROR_OK != drv_error)
        {
            test_result = false;
        }
    }

    /*Execute LUT */
    if(true == test_result)
    {
        printf("\n*** Executing the LUT config ***\n");
        drv_error = sps_vipdrv_execute_lut(p_config_lut, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Execute SW-MOM configuration */
    if(true == test_result)
    {
        printf("\n*** Executing the SW-MOM Configuration ***\n");
        drv_error = sps_vipdrv_execute_mom(p_config_mom, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Execute Flow */
    if(true == test_result)
    {
        printf("\n*** Executing the Flow Configuration ***\n");
        drv_error = sps_vipdrv_execute(p_config, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Check results */
    if(true == test_result)
    {
        printf("\n*** Checking results - ONLY SW-MOM ***\n");
        uint32_t upper_word = 0u;
        upper_word |= ((uint32_t)MOM_SW_AGE << 16);     //SW-MOM age
        test_result = sps_check_register(&ctl, addr_SCALE_FACTOR_MOM_live, upper_word);
    }

	/*///////////////////////////////////////////////////////////////////////////
	* Load POST-MOM configuration and confirm that ages of MOM are as configured.
	*/

    /*Create a POST-MOM configuration */
    if(true == test_result)
    {
        printf("\n*** Creating a POST-MOM Configuration ***\n");

        /*Zero so previous config can't interfere.*/
        mmgr_buffer_zero(&g_mem.mmgr_buffer_config_mom);

        conflib_result = R_SPS_ConfLibSetMOM(p_config_mom, NULL, NULL, g_mem.p_mom_post_addr_dma, &size);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibSetMOM returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Configure POST-MOM*/
    if(true == test_result)
    {
        printf("\n*** Configure POST-MOM ***\n");
        conflib_result = R_SPS_ConfLibMOMPOSTConfig(p_config, true, SPS_MOM_POLARITY_ZERO_IS_VALID, MOM_POST_AGE);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMPOSTConfig returned %d\n", conflib_result);
            test_result = false;
        }
    }

    if(true == test_result)
    {
        conflib_result = R_SPS_ConfLibMOMPOSTUsage(p_config, true, false, g_mom_width, g_mom_height);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMPOSTUsage returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Finish creating a configuration */
    if(true == test_result)
    {
        printf("\n*** Finalize Configuration ***\n");
        conflib_result = R_SPS_ConfLibFinalize(p_config);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibFinalize returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Execute POST-MOM configuration */
    if(true == test_result)
    {
        printf("\n*** Executing the POST-MOM Configuration ***\n");
        drv_error = sps_vipdrv_execute_mom(p_config_mom, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Execute Flow */
    if(true == test_result)
    {
        printf("\n*** Executing the Flow Configuration ***\n");
        drv_error = sps_vipdrv_execute(p_config, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Check results */
    if(true == test_result)
    {
        printf("\n*** Checking results - ONLY POST-MOM ***\n");
        uint32_t upper_word = (uint32_t)MOM_POST_AGE;   //POST-MOM age
        upper_word |= ((uint32_t)MOM_SW_AGE << 16);     //SW-MOM age
        test_result = sps_check_register(&ctl, addr_SCALE_FACTOR_MOM_live, upper_word);
    }

	/*//////////////////////////////////////////////////////////////////////////////////
	* Load SW AND POST-MOM configurations and confirm that ages of MOM are as configured.
	*/

    /*Create a SW and POST-MOM configuration */
    if(true == test_result)
    {
        printf("\n*** Creating SW and POST_MOM Configuration ***\n");

        /*Zero so previous config can't interfere.*/
        mmgr_buffer_zero(&g_mem.mmgr_buffer_config_mom);

        conflib_result = R_SPS_ConfLibSetMOM(p_config_mom, g_mem.p_mom_sw_addr_dma, &size, g_mem.p_mom_post_addr_dma, &size);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibSetMOM returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Configure SW-MOM*/
    if(true == test_result)
    {
        printf("\n*** Configure SW-MOM ***\n");
        conflib_result = R_SPS_ConfLibMOMSWConfig(p_config, true, SPS_MOM_POLARITY_ZERO_IS_VALID, MOM_SW_AGE_1);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMSWConfig returned %d\n", conflib_result);
            test_result = false;
        }
    }

    if(true == test_result)
    {
        conflib_result = R_SPS_ConfLibMOMSWUsage(p_config, true, false, g_mom_width, g_mom_height);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMSWUsage returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Configure POST-MOM*/
    if(true == test_result)
    {
        printf("\n*** Configure POST-MOM ***\n");
        conflib_result = R_SPS_ConfLibMOMPOSTConfig(p_config, true, SPS_MOM_POLARITY_ZERO_IS_VALID, MOM_POST_AGE_1);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMPOSTConfig returned %d\n", conflib_result);
            test_result = false;
        }
    }

    if(true == test_result)
    {
        conflib_result = R_SPS_ConfLibMOMPOSTUsage(p_config, true, false, g_mom_width, g_mom_height);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibMOMPOSTUsage returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Finish creating a configuration */
    if(true == test_result)
    {
        printf("\n*** Finalize Configuration ***\n");
        conflib_result = R_SPS_ConfLibFinalize(p_config);
        if(SPS_CONFLIB_OK != conflib_result)
        {
            printf("Error: R_SPS_ConfLibFinalize returned %d\n", conflib_result);
            test_result = false;
        }
    }

    /*Execute SW and POST-MOM configuration */
    if(true == test_result)
    {
        printf("\n*** Executing SW and POST-MOM Configuration***\n");
        drv_error = sps_vipdrv_execute_mom(p_config_mom, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Execute Flow */
    if(true == test_result)
    {
        printf("\n*** Executing the Flow Configuration ***\n");
        drv_error = sps_vipdrv_execute(p_config, &ctl);
        if(drv_error != R_VIPDRV_ERROR_OK)
        {
            test_result = false;
        }
    }

    /*Check results */
    if(true == test_result)
    {
        printf("\n*** Checking results - SW and POST-MOM ***\n");
        uint32_t upper_word = (uint32_t)MOM_POST_AGE_1;   //POST-MOM age
        upper_word |= ((uint32_t)MOM_SW_AGE_1 << 16);     //SW-MOM age
        test_result = sps_check_register(&ctl, addr_SCALE_FACTOR_MOM_live, upper_word);
    }

    /*Stop DRV */
    printf("\n*** Stopping the vipdrv ***\n");
    drv_error = sps_vipdrv_stop(&ctl);
    if(drv_error != R_VIPDRV_ERROR_OK)
    {
        test_result = false;
    }

    return test_result;
}