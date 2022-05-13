#include "stdio.h"
#include "stdint.h"

uint32_t caculate(uint32_t flow_width, uint32_t flow_height,uint32_t tile_width, uint32_t tile_height, uint32_t tile_shift)
{
    uint32_t num_tiles;
    uint32_t num_tiles_even;
    uint32_t num_tiles_odd;
    
    uint32_t num_even_stripes;
    uint32_t num_odd_stripes;
    uint32_t num_tiles_per_even_stripe;
    uint32_t num_tiles_per_odd_stripe;
    
    // uint32_t flow_width;
    // uint32_t flow_height;
    // uint32_t tile_width = p_sps_data->internal.reg_f2f_cfg.tile_width;
    // uint32_t tile_height = p_sps_data->internal.reg_f2f_cfg.tile_height;
    // uint32_t tile_shift = p_sps_data->internal.reg_f2f_cfg.tile_shift;

    /*If no DMA ROI is set*/
    // if(0u == p_sps_data->internal.input_flow.roi.roi_width)
    // {
    //     flow_width = p_sps_data->internal.input_flow.width;
    //     flow_height = p_sps_data->internal.input_flow.height;
    // }
    // else
    // {
    //     /*ROI is set*/
    //     flow_width = p_sps_data->internal.input_flow.roi.roi_width;
    //     flow_height = p_sps_data->internal.input_flow.roi.roi_height;
    // }
    
    num_even_stripes = (flow_height + tile_height) / (2u * tile_height);
    num_odd_stripes = flow_height / (2u * tile_height);
    num_tiles_per_even_stripe = (flow_width - tile_shift) / tile_width;
    num_tiles_per_odd_stripe = flow_width / tile_width;
    
    num_tiles_even = num_even_stripes * num_tiles_per_even_stripe;
    num_tiles_odd = num_odd_stripes * num_tiles_per_odd_stripe;
    
    num_tiles = num_tiles_even + num_tiles_odd;
    
    return num_tiles;
}

int main()
{
    printf("%d",caculate(16,10,6,4,4));
    return 0;
}