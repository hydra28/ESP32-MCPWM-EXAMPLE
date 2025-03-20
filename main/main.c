#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/mcpwm_prelude.h"

static const char *TAG = "example"; 

void app_main(void)
{
    // ====== One Timer Example Configuration ====== //
    ESP_LOGI(TAG, "Create timer and operator");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT, // PLL Clock 160 MHz
        .resolution_hz = 10000000, //10 MHz Prescaler, 0.1us/ticks
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
        .period_ticks = 1000, // 1000 ticks for 1 cycle, 10 MHz/1000 = 10 KHz Timer
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    // ====== Two Timer Example Configuration with Different Frequency ====== //
    // ESP_LOGI(TAG, "Create timer and operator");
    // mcpwm_timer_handle_t timer[0];
    // mcpwm_timer_config_t timer_config = {
    //     .group_id = 0,
    //     .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT, // PLL Clocl 160 MHz
    //     .resolution_hz = 10000000, //10 MHz Prescaler, 0.1us/ticks
    //     .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
    //     .period_ticks = 1000, // 1000 ticks for 1 cycle, 10 MHz/1000 = 10 KHz Timer
    // };
    // ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer[0]));

    // mcpwm_timer_handle_t timer[1];
    // mcpwm_timer_config_t timer_config = {
    //     .group_id = 0,
    //     .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT, // PLL Clocl 160 MHz
    //     .resolution_hz = 10000000, //10 MHz Prescaler, 0.1us/ticks
    //     .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
    //     .period_ticks = 500, // 500 ticks for 1 cycle, 10 MHz/500 = 20 KHz Timer
    // };
    // ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer[1]));

    // ====== END of Timer Example ====== //


    // ====== One Operator to One Timer Example Configuration ====== //
    ESP_LOGI(TAG, "Create operators");
    mcpwm_oper_handle_t operators = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators));
    ESP_LOGI(TAG, "Connect operators to the same timer");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators, timer));

    // ====== Two Operators Example Configuration ====== //
    // ESP_LOGI(TAG, "Create operators");
    // mcpwm_oper_handle_t operators[2];
    // mcpwm_operator_config_t operator_config = {
    //     .group_id = 0,
    // };
    // for(i = 0; i <= 1; i++){
    // ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators[i]));
    // }

    // ====== Connect Two Operators to Single Timer ====== //
    // ESP_LOGI(TAG, "Connect operators to the same timer");
    // for(i = 0; i <= 1; i++){
    //     ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timer));
    //     }

    // ====== Connect Two Operators to Two Different Timer ====== //
    // ESP_LOGI(TAG, "Connect operators to the same timer");
    // for(i = 0; i <= 1; i++){
    //     ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timer[i]));
    //     }

    // ====== END of Operator Example Configuration ====== //

    // ====== Assign Comparator to One Operator Example Configuration ====== //
    ESP_LOGI(TAG, "Create comparators");
    mcpwm_cmpr_handle_t comparators = NULL;
    mcpwm_comparator_config_t compare_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(operators, &compare_config, &comparators));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators, 0));

    // ====== Assign Each Comparator to Two Operator Example Configuration ====== //
    // ESP_LOGI(TAG, "Create comparators");
    // mcpwm_cmpr_handle_t comparators[2];
    // mcpwm_comparator_config_t compare_config = {
    //     .flags.update_cmp_on_tez = true,
    // };
    // for(i = 0; i <= 1; i++){
    // ESP_ERROR_CHECK(mcpwm_new_comparator(operators[i], &compare_config, &comparators[i]));
    // ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], 0));
    // }

    // ====== END of Comparator Example Configuration

    // ====== Create Generators for PWM Pin Output ====== //
    ESP_LOGI(TAG, "Create generators");
    mcpwm_gen_handle_t generators = NULL;
    mcpwm_generator_config_t gen_config = {};
    const int gen_gpios = 18; 
    gen_config.gen_gpio_num = gen_gpios;
    ESP_ERROR_CHECK(mcpwm_new_generator(operators, &gen_config, &generators));

    // ====== Create Two Generator for Two PWM Pin Output ====== //
    // ESP_LOGI(TAG, "Create generators");
    // mcpwm_gen_handle_t generators[2];
    // mcpwm_generator_config_t gen_config = {};
    // const int gen_gpios[2] = {18, 19};
    // for (int i = 0; i < 2; i++) {
    //     gen_config.gen_gpio_num = gen_gpios[i];
    //     ESP_ERROR_CHECK(mcpwm_new_generator(operators[i], &gen_config, &generators[i]));
    // }

    // ====== END of Generator Example Configuration ====== //

    // ====== Generator Action Example ====== //
    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    // PWM Start with HIGH State when Timer is 0 and LOW State when Comparators value is equal Timer, For MCPWM_TIMER_COUNT_MODE_UP
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators,MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators,MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators, MCPWM_GEN_ACTION_LOW)));

    // PWM Start with LOW State when Timer is 0 and HIGH State when Comparators value is equal Timer, For MCPWM_TIMER_COUNT_MODE_UP
    // ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators,MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW)));
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators,MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators, MCPWM_GEN_ACTION_HIGH)));

    // PWM Start with HIGH State when Timer is Full and LOW State when Comparators value is equal Timer, For MCPWM_TIMER_COUNT_MODE_DOWN
    // ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators,MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_HIGH)));
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators,MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, comparators, MCPWM_GEN_ACTION_LOW)));

    // PWM Start with LOW State when Timer is Full and HIGH State when Comparators value is equal Timer, For MCPWM_TIMER_COUNT_MODE_DOWN
    // ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators,MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators,MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators, MCPWM_GEN_ACTION_HIGH)));

    // PWM Start with HIGH State when Timer is equal comparator (count up) and LOW State when Comparators value is equal Timer (count down), For MCPWM_TIMER_COUNT_MODE_UP_DOWN
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators, MCPWM_GEN_ACTION_HIGH),
    //                                                                             MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, comparators, MCPWM_GEN_ACTION_LOW),
    //                                                                             MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

    // PWM Start with LOW State when Timer is equal comparator (count up) and HIGH State when Comparators value is equal Timer (count down), For MCPWM_TIMER_COUNT_MODE_UP_DOWN
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators, MCPWM_GEN_ACTION_LOW),
    //                                                                             MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, comparators, MCPWM_GEN_ACTION_HIGH),
    //                                                                             MCPWM_GEN_COMPARE_EVENT_ACTION_END()));   

    //For more than one generator or comparator, use this example
    // PWM Start with HIGH State when Timer is 0 and LOW State when Comparators value is equal Timer, For MCPWM_TIMER_COUNT_MODE_UP
    // ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[0],MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generators[0],MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[0], MCPWM_GEN_ACTION_LOW)));

    // ====== END of Generator Action Example ====== //

    // Deadtime only can be assign one posedge or negedge for both PWM on the same operator.
    // ESP_LOGI(TAG, "Setup deadtime");
    // mcpwm_dead_time_config_t dt_config = {
    //     .posedge_delay_ticks = 3,
    //     .negedge_delay_ticks = 0
    // };
    // ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[0],generators[0], &dt_config));
    // dt_config = (mcpwm_dead_time_config_t) {
    //   .posedge_delay_ticks = 0,
    //   .negedge_delay_ticks = 3,
    //   .flags.invert_output = true,
    // };
    // ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[0],generators[1], &dt_config));
    
    // For attached the isr function to timer, use function below
    // mcpwm_timer_event_callbacks_t timer_cbs = {
    // .on_empty = (void*)name_of_function,
    // // .on_full = (void*)name_of_function,
    // };
    // ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(timer, &timer_cbs, NULL));   

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

     while (1) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
     }
}