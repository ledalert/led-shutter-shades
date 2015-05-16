/*! \file ws2812_blink.c
    \brief Example of blinking as ws2812 LED using lib-ledalert-stm32f100

    @details
	
	This example uses the following pins:

		@li PB15 	LED output pin

		@li PA9		UART TX

		@todo explain how to hook it up and how to change pin

	@todo Test stuff!

 
*/

#define NUMBER_OF_LEDS 		1			/*!< Number of WS2812 connected

	@note 	Currently the entire PWM buffer is prefilled with all LED data, this means that for long LED arrays \n
			the memory may not suffice. Preferably a circular buffer should be used and IRQ should fill it up as it drains.

	@todo Implement support in example for several LEDs


*/
#define BAUDRATE			921600		/*!< Baud rate for serial data */



#include "hal/gpio_macros.h"
#include "hal/usart_macros.h"
#include "hal/dma_macros.h"
#include "hal/timer_macros.h"
#include "hal/ws2812_macros.h"
#include "hal/systick_macros.h"
#include "hal/led_macros.h"

GPIO_PORT_INSTANCE(GPIO_PA, GPIOA, RCC_GPIOA);
GPIO_PORT_INSTANCE(GPIO_PB, GPIOB, RCC_GPIOB);

GPIO_PIN_NOCONF(Serial_tx, &GPIO_PA, GPIO_USART1_TX);
USART_8N1(Serial, USART1, RCC_USART1, BAUDRATE, 0, &Serial_tx);

DMA_INSTANCE(Dma, DMA1, RCC_DMA1);

GPIO_PIN_NOCONF(Blinker_pin, &GPIO_PB, GPIO15);
TIMER_NOCONF(Blinker_timer, TIM1, RCC_TIM1);
DMA_CHANNEL_NOCONF(Blinker_dma, &Dma, DMA_CHANNEL6);
TIMER_CCR_NOCONF(Blinker_pwm, &Blinker_timer, TIM_OC3N, &Blinker_dma);

WS2812_SIMPLE_CONF(Blinker, &Blinker_pin, &Blinker_pwm, NUMBER_OF_LEDS);

SYSTICK_AUTO_CONFIG(Systick, 1000);

GAMMA_LUT_8_8_E_INSTANCE(LED_Gamma);


volatile int sleep_ms=0;

/*! Init hardware */
void hw_init(enum hw_init_state state) {
	ws2812_init(&Blinker, state);
	usart_init(&Serial, state);
	systick_init(&Systick, state);
}



/*! Systick IRQ handler */
void sys_tick_handler(void) {
	if (sleep_ms) {
		sleep_ms--;
	}
}


/*! Sleep in millisecond durations */
void ms_sleep(int time) {
	sleep_ms = time;
	while (sleep_ms);
}


/*! Update LED */
void set_led_color(int r, int g, int b) {
	ws2812_set_led(&Blinker, 0, LED_Gamma[r], LED_Gamma[g], LED_Gamma[b]);
	ws2812_update_blocking(&Blinker);
}

/*! Application entry point */
int main(void) {

	//Setup main clock
	rcc_clock_setup_in_hsi_out_24mhz();
	//Enable perhipherals
	rcc_periph_clock_enable(RCC_AFIO);

	//Init hardware
	HW_INIT_ALL(hw_init);

	struct ws2812_rgb color_table[8] = {
		{255, 	0, 		0},		/*!< Red */
		{0, 	255, 	0},		/*!< Green */
		{255, 	255, 	0},		/*!< Yellow */
		{0, 	0, 		255},	/*!< Blue */
		{0, 	255, 	255},	/*!< Cyan */
		{255, 	0, 		255},	/*!< Magenta */
		{255, 	255, 	255},	/*!< White */
		{0,		0,		0},		/*!< Off */
	};

	const char* color_name[8] = {
		"Red",
		"Green",
		"Yellow",
		"Blue",
		"Cyan",
		"Magenta",
		"White",
		"Off",
	};


	while(1) {
		for (int color=0; color<8; color++) {
			set_led_color(color_table[color].r, color_table[color].g, color_table[color].b);
			usart_blocking_strz(&Serial, color_name[color]);
			usart_blocking_strz(&Serial, "\r\n");
			ms_sleep(250);
		}
	}


	return 0;
}
