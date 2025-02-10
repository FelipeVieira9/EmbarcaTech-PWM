#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

#define servo_motor 22
#define LED 12

// Configurações do PWM do LED
const uint16_t PERIOD = 2000;
const float DIVIDER_PWM = 16.0;
const uint16_t LED_STEP = 100;
uint16_t led_level = 100;

// Funções para iniciar o PWM do led e do motor
void setup_pwm_led();
void setup_pwm_motor();

// Interrupção conveniente
int64_t alarm_callback(alarm_id_t id, void *user_data);
static volatile int rodando = 0;

// Controle de ações
volatile int tarefa = 2;

int main()
{
    // Controle da luz do LED
    uint up_down = 1;

    // Iniciar os PMW do led e do motor
    setup_pwm_led();
    setup_pwm_motor();
    stdio_init_all();

    // Controle de ações
    int minimo = 500;
    int maximo = 2400;
    int atual = 500;
    int direcao = 1;

    while (true) {
        // Mudar o valor do led
        pwm_set_gpio_level(LED, led_level);
        sleep_ms(10);
        if (up_down) {
            led_level += LED_STEP;
            if (led_level >= PERIOD) {
                up_down = 0;
            }
        } else {
            led_level -= LED_STEP;
            if (led_level <= LED_STEP) {
                up_down = 1;
            }
        }

        // Escolha da ação do motor
        if (rodando == 0) {
            rodando = 1;

            if (tarefa == 2) {
                printf("tarefa 2:\n");
                tarefa = 3;
                pwm_set_gpio_level(servo_motor, 2400);

                // Interrupção para mudar o valor do rodando
                add_alarm_in_ms(5000, alarm_callback, NULL, true);
            } else if (tarefa == 3) {
                printf("tarefa 3:\n");
                tarefa = 4;
                pwm_set_gpio_level(servo_motor, 1470);
                add_alarm_in_ms(5000, alarm_callback, NULL, true);
            } else if (tarefa == 4) {
                printf("tarefa 4:\n");
                tarefa = 5;
                pwm_set_gpio_level(servo_motor, 500);
                add_alarm_in_ms(5000, alarm_callback, NULL, true);
            } else if (tarefa == 5) {
                rodando = 0;
                if (direcao == 1) {
                    if (atual <= maximo) {
                        atual += 5;
                    } else {
                        atual -= 5;
                        direcao = 0;
                    }
                } else {
                    if (atual >= minimo) {
                        atual -= 5;
                    } else {
                        atual += 5;
                        direcao = 1;
                    }
                }
                pwm_set_gpio_level(servo_motor, atual);
            }
        }
        
    }
}

void setup_pwm_motor() {
    uint slice;
    gpio_set_function(servo_motor, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(servo_motor);

    // Configura um clock divisor apropriado para 50 Hz
    pwm_set_clkdiv(slice, 125.0);
    
    // Define o "wrap" para obter um período de 20ms (50 Hz)
    pwm_set_wrap(slice, 20000);
    
    // Define o nível de PWM para um pulso inicial de 2.4 ms
    pwm_set_gpio_level(servo_motor, 2400);

    // Ativa o PWM
    pwm_set_enabled(slice, true);
}

void setup_pwm_led() {
    uint slice;
    gpio_set_function(LED, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(LED);
    pwm_set_clkdiv(slice, DIVIDER_PWM);
    pwm_set_wrap(slice, PERIOD);
    pwm_set_gpio_level(LED, led_level);
    pwm_set_enabled(slice, true);
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    rodando = 0;

    if (tarefa == 5) {
        printf("Tarefa 5:");
    }
    return 0;
}