#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 22  // GPIO do servo
#define PWM_FREQ 50   // Frequência de 50Hz (período de 20ms)
#define STEP_DELAY 10 // Atraso para movimentação suave (10ms)

// Converte tempo de pulso (em µs) para valor de duty cycle do PWM
uint16_t micros_to_pwm(uint16_t micros, uint16_t wrap) {
    return (wrap * micros) / 20000; // 20000µs = 20ms (período do PWM)
}

void move_servo(uint16_t pulse_width, uint16_t wrap) {
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(SERVO_PIN), micros_to_pwm(pulse_width, wrap));
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Pequeno atraso para estabilização
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);  // Configura GPIO para PWM
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    
    // Configuração do PWM para 50Hz
    float clkdiv = 64.0f;
    uint16_t wrap = 25000; // Define o valor máximo do contador
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    
    // Definição das posições do servo
    uint16_t pulse_180 = 4400; // 2.400µs (180°)
    uint16_t pulse_90  = 2300; // 1.470µs (90°)
    uint16_t pulse_0   = 500;  // 500µs (0°)

    
    // Movimento para 180°
    move_servo(4800, 20000);
    sleep_ms(5000);

    // Movimento para 90°
    move_servo(pulse_90, wrap);
    sleep_ms(5000);

    move_servo(pulse_0, wrap);

    // Movimento suave entre 0° e 180°
    while (true) {
        sleep_ms(1000); // Pausa de 1s
        // Movendo de 0° para 180°
        for (uint16_t pulse = pulse_0; pulse <= pulse_180; pulse += 5) {
            move_servo(pulse, wrap);
            sleep_ms(STEP_DELAY);
        }
        sleep_ms(1000); // Pausa de 1s
        
        // Movendo de 180° para 0°
        for (uint16_t pulse = pulse_180; pulse >= pulse_0; pulse -= 5) {
            move_servo(pulse, wrap);
            sleep_ms(STEP_DELAY);
        }
        
    }
}
