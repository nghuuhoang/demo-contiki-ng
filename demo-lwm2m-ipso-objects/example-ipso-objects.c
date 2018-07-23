
#include "contiki.h"
#include "dev/leds.h"
#include "services/lwm2m/lwm2m-engine.h"
#include "services/lwm2m/lwm2m-rd-client.h"
#include "services/lwm2m/lwm2m-device.h"
#include "services/lwm2m/lwm2m-server.h"
#include "services/lwm2m/lwm2m-security.h"
#include "services/lwm2m/lwm2m-firmware.h"
// #include "services/ipso-objects/ipso-objects.h"
#include "services/ipso-objects/ipso-sensor-template.h"
#include "services/ipso-objects/ipso-control-template.h"
#include "dev/leds.h"
#include "lib/sensors.h"
#include "dev/cc2538-sensors.h"
#include "dev/cc2538-temp-sensor.h"

#define DEBUG DEBUG_NONE
#include "net/ipv6/uip-debug.h"

#ifndef REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER
#define REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER 0
#endif

#ifndef REGISTER_WITH_LWM2M_SERVER
#define REGISTER_WITH_LWM2M_SERVER 1
#endif

#ifndef LWM2M_SERVER_ADDRESS
#define LWM2M_SERVER_ADDRESS "coap://[fd00::1]"
#endif


//#include "board-peripherals.h"

/* Temperature reading */
static lwm2m_status_t
read_temp_value(const ipso_sensor_t *s, int32_t *value)
{
  *value = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  return LWM2M_STATUS_OK;
}

/* LED control */
static lwm2m_status_t
leds_set_val_0(ipso_control_t *control, uint8_t value)
{
  if(value > 0) {
    leds_single_on(LEDS_LED1);
  } else {
    leds_single_off(LEDS_LED1);
  }
  return LWM2M_STATUS_OK;
}


static lwm2m_status_t
leds_set_val_1(ipso_control_t *control, uint8_t value)
{
  if(value > 0) {
    leds_single_on(LEDS_LED2);
  } else {
    leds_single_off(LEDS_LED2);
  }
  return LWM2M_STATUS_OK;
}


static lwm2m_status_t
leds_set_val_2(ipso_control_t *control, uint8_t value)
{
  if(value > 0) {
    leds_single_on(LEDS_LED3);
  } else {
    leds_single_off(LEDS_LED3);
  }
  return LWM2M_STATUS_OK;
}
/*---------------------------------------------------------------------------*/

IPSO_CONTROL(led_control_0, 3311, 0, leds_set_val_0);

IPSO_CONTROL(led_control_1, 3311, 1, leds_set_val_1);

IPSO_CONTROL(led_control_2, 3311, 2, leds_set_val_2);

IPSO_SENSOR(temp_sensor, 3303, read_temp_value,
            .max_range = 100000, /* 100 cel milli celcius */
            .min_range = -10000, /* -10 cel milli celcius */
            .unit = "Cel",
            .update_interval = 30
            );



PROCESS(example_ipso_objects, "IPSO object example");
AUTOSTART_PROCESSES(&example_ipso_objects);
/*---------------------------------------------------------------------------*/
static void
setup_lwm2m_servers(void)
{
#ifdef LWM2M_SERVER_ADDRESS
  coap_endpoint_t server_ep;
  if(coap_endpoint_parse(LWM2M_SERVER_ADDRESS, strlen(LWM2M_SERVER_ADDRESS),
                         &server_ep) != 0) {
    lwm2m_rd_client_register_with_bootstrap_server(&server_ep);
    lwm2m_rd_client_register_with_server(&server_ep);
  }
#endif /* LWM2M_SERVER_ADDRESS */

  lwm2m_rd_client_use_bootstrap_server(REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER);
  lwm2m_rd_client_use_registration_server(REGISTER_WITH_LWM2M_SERVER);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_ipso_objects, ev, data)
{
  static struct etimer periodic;
  static int val;
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  PRINTF("Starting IPSO objects example%s\n",
         REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER ? " (bootstrap)" : "");
  /* Initialize the OMA LWM2M engine */
  lwm2m_engine_init();

  /* Register default LWM2M objects */
  lwm2m_device_init();
  lwm2m_security_init();
  lwm2m_server_init();
  lwm2m_firmware_init();
  
  ipso_sensor_add(&temp_sensor);
  ipso_control_add(&led_control_0);
  ipso_control_add(&led_control_1);
  ipso_control_add(&led_control_2);
  //ipso_button_init();



  setup_lwm2m_servers();
  /* Tick loop each 5 seconds */
  etimer_set(&periodic, CLOCK_SECOND * 5);
  SENSORS_ACTIVATE(cc2538_temp_sensor);
  while(1) {
    val = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
    printf("temperature : %u mC\n",val);
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && etimer_expired(&periodic)) {

      etimer_reset(&periodic);
    }
  }
  PROCESS_END();
}
