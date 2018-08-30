
/**
 * \file
 *         read temperature internal in cc2538
 * \author
 *         nghuu.hoang@outlook.com
 */

#include "contiki.h"

#include <stdio.h> /* For printf() */
#include "lib/sensors.h"
#include "dev/cc2538-sensors.h"
#include "dev/vdd3-sensor.h"

extern const struct sensors_sensor vdd3_sensor;
//extern const struct sensors_sensor cc2538_temp_sensor;
/*---------------------------------------------------------------------------*/
PROCESS(read_vdd3, "read vdd3 cc2538");
AUTOSTART_PROCESSES(&read_vdd3);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(read_vdd3, ev, data)
{
  static struct etimer timer;
  static int val;
  
  PROCESS_BEGIN();
  printf("Demo read vdd in cc2538\n");
  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 10);

  while(1) {
    SENSORS_ACTIVATE(vdd3_sensor);
    val = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
    printf("vdd : %u mV\n",val);

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
