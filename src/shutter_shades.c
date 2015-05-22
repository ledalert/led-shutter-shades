/*! \file ws2812_blink.c
    \brief Example of blinking as ws2812 LED using lib-ledalert-stm32f100

    @details
	
	This example uses the following pins:

		@li PB15 	LED output pin

		@li PA9		UART TX

		@todo explain how to hook it up and how to change pin

	@todo Test stuff!

 
*/

#define NUMBER_OF_LEDS 		14		/*!< Number of WS2812 connected

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

GPIO_PIN_INSTANCE(Blinker_pin, &GPIO_PB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);	/*!< We are using a FET 
so we need pushpull in this variant and faster rise/fall times*/

TIMER_NOCONF(Blinker_timer, TIM3, RCC_TIM3);
DMA_CHANNEL_NOCONF(Blinker_dma, &Dma, DMA_CHANNEL2);
TIMER_CCR_NOCONF(Blinker_pwm, &Blinker_timer, timer_ccr_mode_pwm2, TIM_OC3, &Blinker_dma);	/*!< 	Since FET pulls down resistor in the shutter glasses
																									we will use inverted PWM */

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



/*! Application entry point */
int main(void) {

	//Setup main clock
	rcc_clock_setup_in_hsi_out_24mhz();
	//Enable perhipherals
	rcc_periph_clock_enable(RCC_AFIO);


	//Init hardware
	HW_INIT_ALL(hw_init);

	/*! Compensation for slow FET */
	Blinker.configuration->bit1 += 2;	
	Blinker.configuration->bit0 += 7;


	/*! Here the main application starts */

	/*! This is an example of a color table */
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

	/*! In this example we have a fixed intensity calculated like this

			LED_Gamma[(intensity * color_table[color].r) >> 8]

				LED_Gamma is a lookup table with 256 entries

			Here we multiply intensity with the color component and then we shift it right 8 bits
			which divides the result by 256, this means we can se intensity to 0 to 256 where 256 will be fully on

			(intensity * color_table[color].r) / 256


	  */

	while(1) {

		set_all(255, 0, 0);
		ms_sleep(100);
		set_all(0, 0, 0);
		ms_sleep(900);

	}

	int intensity = 100;

	while(1) {

		for (int color_offset=0; color_offset<8; color_offset++) {
			for (int led=0; led<NUMBER_OF_LEDS; led++) {
				int color = (led + color_offset) % 8;

				/*! ws2812_set_led(led_bus, led_number, red, green, blue)
					
					led_bus		Pointer to bus to use, in this case we only have Blinker, so use &Blinker here
					led_number	What LED in the chain to set color to, 0 is the first, 1 is next one etc.
					red, green, blue   PWM value of LED, 0-255   0 = off, 255 = fully on

					Example of orange without gamma correction
						
						ws2812_set_led(&Blinker, 0, 255, 127, 0);

					Example of orange with gamma correction

						ws2812_set_led(&Blinker, 0, LED_Gamma[255], LED_Gamma[127], LED_Gamma[0]);


				 */	

				
				ws2812_set_led(&Blinker, led, LED_Gamma[(intensity * color_table[color].r) >> 8], LED_Gamma[(intensity * color_table[color].g) >> 8], LED_Gamma[(intensity * color_table[color].b) >> 8]);
			}


			ws2812_update_blocking(&Blinker);		/*! Update LEDs on bus */
			ms_sleep(50);
		}
	}


	return 0;
}


/*! Example to set all LED to the same color */
void set_all(int r, int g, int b) {
	for (int led=0; led<NUMBER_OF_LEDS; led++) {
		ws2812_set_led(&Blinker, led, r, g, b);
	}
	ws2812_update_blocking(&Blinker);
}




/*! To test new code

	In root folder of project:

	make
	stm32flash /dev/ttyUSB0 -b 921600 -B cs.dtr -R cs.rts -w build/shutter_shades.bin

 */