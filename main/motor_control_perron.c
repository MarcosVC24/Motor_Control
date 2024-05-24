#include <stdio.h>
#include <sdkconfig.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_sleep.h"

static const char *TAG = "example";

// Definir parámetros del PWM
#define PWM_TIMER              LEDC_TIMER_0
#define PWM_MODE               LEDC_LOW_SPEED_MODE
#define PWM_OUTPUT_IO          (18) // Pin de salida
#define LEDC_CHANNEL           LEDC_CHANNEL_0
#define PWM_DUTY_RES           LEDC_TIMER_12_BIT // Resolución de 12 bits
#define PWM_FREQUENCY          (5000) // Frecuencia en Hertz

// Definir parametros del contador de pulsos
#define PCNT_HIGH_LIMIT 16000
#define PCNT_LOW_LIMIT  -16000

#define PHASE_A_GPIO 4
#define PHASE_B_GPIO 5

/* static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // send event data to queue, from this interrupt callback
    xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}

static void vPcntTask(void *pvParameters){
    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = PHASE_A_GPIO,
        .level_gpio_num = PHASE_B_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = PHASE_B_GPIO,
        .level_gpio_num = PHASE_A_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    pcnt_event_callbacks_t cbs = {
        .on_reach = example_pcnt_on_reach,
    };
    QueueHandle_t queue = xQueueCreate(10, sizeof(int));
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue));

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
    BaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "Stack High Water Mark: %u palabras", stackHighWaterMark);

    // Report counter value
    int pulse_count = 0;
    int event_count = 0;
    int contador = 0;
    while (1) {
        if (xQueueReceive(queue, &event_count, pdMS_TO_TICKS(1000))) {
            ESP_LOGI(TAG, "Watch point event, count: %d", event_count);
        } else {
            ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));
            contador = contador + pulse_count;
            int rpm = (pulse_count/28) * 60; // 28 es el número de polos que tiene el imán codificado que tiene el encoder
            int rpmConv = rpm / 380;
            ESP_LOGI(TAG, "Pulse count: %d", contador);
            ESP_LOGI(TAG, "Pulses per second: %d", pulse_count);
            ESP_LOGI(TAG, "RPM: %d", rpm);
            ESP_LOGI(TAG, "RMP_CONV: %d", rpmConv);
            ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
        }
    }
} */

void app_main(void)
{
    ESP_LOGI(TAG, "Se va a iniciar la tarea");
    /* BaseType_t chekeoAlabestia;
    TaskHandle_t pcntHandle = NULL;
    chekeoAlabestia = xTaskCreate(vPcntTask, "pcntTask", 3000, NULL, 2, &pcntHandle);

    if (chekeoAlabestia == pdPASS){
        ESP_LOGI(TAG, "Se Generó la tarea ALABESTIA");
    } */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_DUTY_RES, // Resolución del PWM
        .freq_hz = PWM_FREQUENCY,        // Frecuencia del PWM
        .speed_mode = PWM_MODE,          // Modo de velocidad
        .timer_num = PWM_TIMER,          // Número del temporizador
        .clk_cfg = LEDC_AUTO_CLK,         // Configuración del reloj
    };
    ledc_timer_config(&ledc_timer);

    // Configurar el canal del LEDC
    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL,
        .duty       = 0,                  // Ciclo de trabajo inicial
        .gpio_num   = PWM_OUTPUT_IO,     // Número del pin GPIO
        .speed_mode = PWM_MODE,
        .hpoint     = 0,
        .timer_sel  = PWM_TIMER
    };
    ledc_channel_config(&ledc_channel);

    // Inicializar la salida del PWM
    int duty = 3069; // Ciclo de trabajo deseado (12 bits, 0-4095)
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    ESP_LOGI(TAG, "Ha comenzado el PWM");
    // Añadir un bucle para cambiar el ciclo de trabajo si es necesario
    while (1) {
        // Cambiar el ciclo de trabajo aquí si es necesario
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
