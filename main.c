#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/types.h"


/**
* To see the output, minicom -b 115200 -o -D /dev/ttyS3
* 
*/

#ifndef PICO_DEFAULT_LED_PIN
#error blink example requires a board with a regular LED
#endif

uint TRIGGER_PIN = 17;
uint ECHO_PIN = 16;


// -----------------------------------------------------------
float   measure_distance(){
    float   distance_cm = 0.0;

    gpio_set_input_enabled( ECHO_PIN, 1 );
    gpio_put( TRIGGER_PIN, 0 ); // make sure that the pin is low
    sleep_ms(2);                // wait a bit to let things stabalize
    uint echo_pin = gpio_get( ECHO_PIN);
	// printf("\nPulsed the pin high echo pin is %d", echo_pin );
	
    gpio_put( TRIGGER_PIN, 1);  // go high
    sleep_ms( 10 );             // sleep 3 mili seconds
    gpio_put( TRIGGER_PIN, 0);  // Switch the trigger low again to let the SR04 send the pulse
    
	/*
		The echo pin outputs a pulse between 150 micro-seconds and 25 mili-seconds, or if no object is found, it will send a 38 ms pulse. 
		speed of sound is 343 meters per second. This would depend upon the elevation and hummidity, but 
		sufficiently accurate for our application. 

		speed = distance travelled / time taken
		
		so, distance = (speed * time taken)/2 -- Divide by 2 because we are listening to the echo
		
		Now, speed of sound is 343 meters/second, whch is 3.43 centimeters per second, which is
		3.43/ 1000000 = 0.0343 cm/microsecond. 
		
		
	*/
    absolute_time_t  listen_start_time = get_absolute_time();
    absolute_time_t  max_wait_time  = delayed_by_ms( listen_start_time, 30 );  // No point waiting more than 30 Mili sec. 
	
    do{
        absolute_time_t  t_now = get_absolute_time();
        if( t_now > max_wait_time ){
            break;
        }
        echo_pin = gpio_get( ECHO_PIN);
        if( echo_pin != 0 ){   // We got an echo!
            absolute_time_t  first_echo_time = t_now;
            printf(" the pin went high ");
            while( echo_pin == 1 && t_now < max_wait_time ){
                echo_pin = gpio_get( ECHO_PIN );
                t_now = get_absolute_time();
            }
			printf(" pin is at %d", echo_pin );
			if( echo_pin == 1 ){
				break;	// will return 0 cm.
			}
			//printf("Start %llu ", listen_start_time );
			//printf( " end %llu ", t_now );
            int64_t pulse_high_time = absolute_time_diff_us( first_echo_time, t_now );
			float pulse_high_time_float = (float) pulse_high_time;
            printf(" High Time %f ", pulse_high_time_float ); 
            distance_cm = pulse_high_time_float * 0.01715; //pulse_high_time_float / 58.0 ;
            break;
        }
    }
    while( true );
    gpio_set_input_enabled( ECHO_PIN, 0 );
    return distance_cm;
}







// ---------------------------------------------------------------
int main() {
    stdio_init_all();

    // setup the trigger pin in putput mode and echo pin to input mode
    gpio_init( TRIGGER_PIN );
    gpio_init( ECHO_PIN );
    gpio_set_dir( TRIGGER_PIN, GPIO_OUT );
    gpio_set_dir( ECHO_PIN, GPIO_IN );


    gpio_init( PICO_DEFAULT_LED_PIN );
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT);
    while (true) {
        // flash the on-board LED to show that we are still alive.
        gpio_put( PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(500 * 2 );
        gpio_put( PICO_DEFAULT_LED_PIN, 0);
        sleep_ms( 500 * 2 );

        float  distance = measure_distance();
        printf(" Closest object is %5.2f cm away\n", distance );
    }
}

