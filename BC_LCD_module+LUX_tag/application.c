/*
*   Example FW for BigClown IoT Platform
*   LCD module and Luxmeter Tag
*   
*   This example shows value from LUXmeter LCD. 
*   The screen is updated every 5 minutes (can be changed by define CO2_UPDATE_NORMAL_INTERVAL).
*
*   www.chiptron.cz (czech)
*   www.time4ee.com (english)
*/

#include "application.h"

#define BLACK true
// lux meter instance
bc_opt3001_t lux;

void lux_module_event_handler(bc_opt3001_t *self, bc_opt3001_event_t event, void *event_param) 
{
    (void) event_param;
    char buffer[13];
    float illumination = 0.0;

    if (event == BC_OPT3001_EVENT_UPDATE) {
        bc_opt3001_get_illuminance_lux(self, &illumination);

        sprintf(buffer, "%08.4f", illumination);
        buffer[12] = '\0';

        // Use big font
        bc_module_lcd_set_font(&bc_font_ubuntu_24);

        bc_module_lcd_draw_string(5, 30, &buffer[0], BLACK);

        // Don't forget to update
        bc_module_lcd_update();
    } 
    else 
    {
        illumination = 0.0;
        // Use big font
        bc_module_lcd_set_font(&bc_font_ubuntu_24);

        bc_module_lcd_draw_string(5, 30, "0.0", BLACK);

        // Don't forget to update
        bc_module_lcd_update();
    }
}

void application_init(void)
{
    // The parameter is internal buffer in SDK, no need to define it
    bc_module_lcd_init(&_bc_module_lcd_framebuffer);

    // lux meter initialization
    bc_opt3001_init(&lux, BC_I2C_I2C0, 0x44);

    // set update interval
    bc_opt3001_set_update_interval(&lux, 1000);

    // set evend handler (what to do when tag update is triggered)
    bc_opt3001_set_event_handler(&lux, lux_module_event_handler, NULL);

    // Init default font, this is necessary
    // See other fonts in sdk/bcl/inc/bc_font_common.h
    bc_module_lcd_set_font(&bc_font_ubuntu_15);

    // Draw string at X, Y location
    bc_module_lcd_draw_string(10, 5, "chiptron.cz", BLACK);

    bc_module_lcd_draw_line(5, 20, 115, 20, BLACK);
    
     // Don't forget to update
    bc_module_lcd_update();
}
