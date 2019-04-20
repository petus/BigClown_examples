/*
*   Example FW for BigClown IoT Platform
*   CO2 module, LCD module, Battery module, Humidity Tag, Temperature Tag
*   
*   This example shows value from CO2, temperature of temperature tag (TMP112), 
*   temperature&humidity of humidity tag (SHT20), votage of battery on LCD. 
*   The screen is updated every 5 minutes (can be changed by define CO2_UPDATE_NORMAL_INTERVAL).
*
*   www.chiptron.cz (czech)
*   www.time4ee.com (english)
*/
#include <application.h>

#define BLACK true
#define CO2_PUB_VALUE_CHANGE 50.0f
#define CO2_UPDATE_NORMAL_INTERVAL (5 * 60 * 1000)

// LED instance
bc_led_t led;

// Temperature tag instance
bc_tag_temperature_t temperature;
bc_tag_temperature_event_t temperature_event_param;

// Humidity tag instance
bc_tag_humidity_t humidity;
bc_tag_humidity_event_t humidity_event_param;

// CO2
bc_lp8_event_t co2_module;

float co2_value = 0;
float temperature_value = 0;
float voltage_value = 0;
float humidity_value = 0;
float hum_temperature_value = 0;
char buffer[6];

// SHT20 tag event
void humidity_tag_event_handler(bc_tag_humidity_t *self, bc_tag_humidity_event_t event, void *event_param)
{

    if (event != BC_TAG_HUMIDITY_EVENT_UPDATE)
    {
        return;
    }

    bc_tag_humidity_get_humidity_percentage(self, &humidity_value);
    bc_tag_humidity_get_temperature_celsius(self, &hum_temperature_value);
}

// TMP112 tag event
void temperature_tag_event_handler(bc_tag_temperature_t *self, bc_tag_temperature_event_t event, void *event_param)
{

    if (event != BC_TAG_TEMPERATURE_EVENT_UPDATE)
    {
        return;
    }

    bc_tag_temperature_get_temperature_celsius(self, &temperature_value);
}

// CO2 module event
void co2_event_handler(bc_module_co2_event_t event, void *event_param)
{
    if (event == BC_MODULE_CO2_EVENT_ERROR) // error event
    {
        bc_lp8_error_t error;
        bc_module_co2_get_error(&error);
        int error_int = (int)error;

        printf(buffer, "%04.2f", error_int);
        buffer[5] = '\0';

        bc_module_lcd_clear();
        bc_module_lcd_draw_string(10, 5, "Error", BLACK);
        bc_module_lcd_draw_string(10, 20, &buffer[0], BLACK);
        bc_module_lcd_update();
    }

    if (event == BC_MODULE_CO2_EVENT_UPDATE) // update event
    {
        if (bc_module_co2_get_concentration_ppm(&co2_value))
        {
            bc_module_lcd_clear();

            bc_module_battery_get_voltage(&voltage_value);

            // Battery
            sprintf(buffer, "%02.2f", voltage_value);
            bc_module_lcd_draw_string(3, 10, "Bat: ", BLACK);
            bc_module_lcd_draw_string(45, 10, &buffer[0], BLACK);
            bc_module_lcd_draw_string(85, 10, "V", BLACK);

            // CO2
            sprintf(buffer, "%04.0f", co2_value);
              bc_module_lcd_draw_string(3, 25, "CO2: ", BLACK);
            bc_module_lcd_draw_string(45, 25, &buffer[0], BLACK);
            bc_module_lcd_draw_string(85, 25, "ppm", BLACK);

            //SHT20
            // Humidity Tag
            sprintf(buffer, "%02.2f", humidity_value);
            bc_module_lcd_draw_string(3, 40, "Hum: ", BLACK);
            bc_module_lcd_draw_string(45, 40, &buffer[0], BLACK);
            bc_module_lcd_draw_string(85, 40, "%", BLACK);

            sprintf(buffer, "%02.2f", hum_temperature_value);
            bc_module_lcd_draw_string(3, 55, "Temp: ", BLACK);
            bc_module_lcd_draw_string(45, 55, &buffer[0], BLACK);
            bc_module_lcd_draw_string(85, 55, "°C", BLACK);

            //TMP112
            // Temperature Tag
            sprintf(buffer, "%02.2f", temperature_value);
            bc_module_lcd_draw_string(3, 70, "Temp: ", BLACK);
            bc_module_lcd_draw_string(45, 70, &buffer[0], BLACK);
            bc_module_lcd_draw_string(85, 70, "°C", BLACK);

            bc_module_lcd_draw_line(0, 108, 128, 108, BLACK);
            bc_module_lcd_draw_string(25, 110, "chiptron.cz", BLACK);

            bc_module_lcd_update();
        }
    }
}

void application_init(void)
{
    // The parameter is internal buffer in SDK, no need to define it
    bc_module_lcd_init();

    // Init default font, this is necessary
    // See other fonts in sdk/bcl/inc/bc_font_common.h
    bc_module_lcd_set_font(&bc_font_ubuntu_15);

    // Draw string at X, Y location
    bc_module_lcd_draw_string(10, 5, "chiptron.cz", BLACK);

    bc_module_lcd_draw_line(5, 20, 115, 20, BLACK);
    
     // Don't forget to update
    bc_module_lcd_update();

    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize CO2 module 
    bc_module_co2_init();
    bc_module_co2_set_update_interval(CO2_UPDATE_NORMAL_INTERVAL);
    bc_module_co2_set_event_handler(co2_event_handler, &co2_module);

    // Initialize battery
    bc_module_battery_init();
    bc_module_battery_set_update_interval(CO2_UPDATE_NORMAL_INTERVAL);

    // Initialize TMP112 tag temperature
    bc_tag_temperature_init(&temperature, BC_I2C_I2C0, BC_TAG_TEMPERATURE_I2C_ADDRESS_DEFAULT);
    bc_tag_temperature_set_update_interval(&temperature, CO2_UPDATE_NORMAL_INTERVAL);
    bc_tag_temperature_set_event_handler(&temperature, temperature_tag_event_handler, &temperature_event_param);

    // Initialize SHT20 tag temperature&humidity
    bc_tag_humidity_init(&humidity, BC_TAG_HUMIDITY_REVISION_R3, BC_I2C_I2C0, BC_TAG_HUMIDITY_I2C_ADDRESS_DEFAULT);
    bc_tag_humidity_set_update_interval(&humidity, CO2_UPDATE_NORMAL_INTERVAL);
    bc_tag_humidity_set_event_handler(&humidity, humidity_tag_event_handler, &humidity_event_param);
}

void application_task(void)
{
    // Logging in action
    bc_log_debug("application_task run");

    // Plan next run this function after 1000 ms
    bc_scheduler_plan_current_from_now(1000);
}
