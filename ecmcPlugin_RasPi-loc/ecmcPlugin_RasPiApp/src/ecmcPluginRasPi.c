/*************************************************************************\
* Copyright (c) 2019 European Spallation Source ERIC
* ecmc is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
*
*  ecmcPluginExample.cpp
*
*  Created on: Mar 21, 2020
*      Author: anderssandstrom
*
\*************************************************************************/

// Needed to get headers in ecmc right...
#define ECMC_IS_PLUGIN
#define ECMC_RASPI_PLUGIN_VERSION 1
#define ECMC_RASPI_PLUGIN_NAME "ecmcRasPi"
#define ECMC_RASPI_PLUGIN_PLC_FUNC_PREFIX "rpi_"

#ifdef __cplusplus
extern "C" {
#endif  // ifdef __cplusplus

#include <stdio.h>
#include "ecmcPluginDefs.h"
#include "ecmcRasPiGPIO.h"
#include <wiringPi.h>

static double ecmcSampleRate = -1;
static void*  ecmcDataRefs   = 0;  //ecmcRefs form raspiEnterRT()
static int    lastEcmcError  = 0;
static void*  ecmcAsynPort   = NULL;

/** Optional. 
 *  Will be called once just before ecmc goes into realtime mode.
 *  Return value other than 0 will be considered error.
 **/
int rpi_Construct(void)
{
  printf("Ecmc plugin, "ECMC_RASPI_PLUGIN_NAME", for RasPi GPIO support initiating...\n");
  wiringPiSetup ();  // Use wiringPi pin numbering
  return 0;
}

/** Optional function.
 *  Will be called once at unload.
 **/
void rpi_Destruct(void)
{
  printf("Plugin "ECMC_RASPI_PLUGIN_NAME" unloading...\n");
}

/** Optional function.
 *  Will be called each realtime cycle if definded
 *  ecmcError: Error code of ecmc. Makes it posible for 
 *  this plugin to react on ecmc errors
 *  Return value other than 0 will be considered error.
 **/
int rpi_Realtime(int ecmcError)
{
  lastEcmcError = ecmcError;
  return 0;
}

/** Optional function.
 *  Will be called once just before going to realtime mode
 *  Return value other than 0 will be considered error.
 *  ecmcRefs is only valid after "raspiEnterRT()"
 **/
int rpi_EnterRT(void* ecmcRefs){  
  // Save ecmcRefs if needed
  printf("Plugin "ECMC_RASPI_PLUGIN_NAME" enters realtime...\n");
  ecmcDataRefs   = ecmcRefs;
  ecmcSampleRate = getSampleRate(ecmcRefs);
  ecmcAsynPort   = getAsynPort(ecmcRefs);    
  return 0;
}

/** Optional function.,ECMC_RASPI_PLUGIN_NAME
 *  Will be called once just before leaving realtime mode
 *  Return value other than 0 will be considered error.
 **/
int rpi_ExitRT(void){
  printf("Plugin "ECMC_RASPI_PLUGIN_NAME" exits realtime...\n");
  return 0;
}

/** rpi_digitalWrite*/
double rpi_digitalWrite(double pin, double value)
{
  //printf("rpi_digitalWrite %lf, %lf.\n",pin,value);
  digitalWrite((int)pin, (int)value);
  return 0;
}

/** rpi_digitalRead*/
double rpi_digitalRead(double pin)
{
  //printf("rpi_digitalRead %lf.\n",pin);
  return (double)digitalRead ((int)pin);
}

/** rpi_pinMode*/
double rpi_pinMode(double pin, double mode)
{
  //printf("rpi_pinMode %lf.\n",pin);
  pinMode ((int)pin,(int)mode);
  return 0;
}

// Compile data for lib so ecmc now what to use
struct ecmcPluginData pluginDataDef = {
  // Name 
  .name = ECMC_RASPI_PLUGIN_NAME,
  // Plugin version
  .version = ECMC_RASPI_PLUGIN_VERSION,
  // Allways use ECMC_PLUG_VERSION_MAGIC
  .ifVersion = ECMC_PLUG_VERSION_MAGIC, 
  // Optional construct func, called once at load. NULL if not definded.
  .constructFnc = rpi_Construct,
  // Optional destruct func, called once at unload. NULL if not definded.
  .destructFnc = rpi_Destruct,
  // Optional func that will be called each rt cycle. NULL if not definded.
  .realtimeFnc = rpi_Realtime,
  // Optional func that will be called once just before enter realtime mode
  .realtimeEnterFnc = rpi_EnterRT,
  // Optional func that will be called once just before exit realtime mode
  .realtimeExitFnc = rpi_ExitRT,

  // Allow max ECMC_PLUGIN_MAX_FUNC_COUNT custom funcs
  .funcs[0] =      
      { /*----rpi_digitalWrite----*/
        // Function name (this is the name you use in ecmc plc-code)
        .funcName = "rpi_digitalWrite",
        // Function description
        .funcDesc = "WiringPi: void digitalWrite(pin,level)",
        // Number of arguments in the function prototytpe
        .argCount = 2,
        /**
        * 7 different prototypes allowed (only doubles since reg in plc).
        * Only funcArg${argCount} func shall be assigned the rest set to NULL.
        * funcArg${argCount}. These need to match. 
        **/
        .funcArg0 = NULL,
        .funcArg1 = NULL,
        .funcArg2 = rpi_digitalWrite, // Func 1 has 2 args
        .funcArg3 = NULL,
        .funcArg4 = NULL,
        .funcArg6 = NULL,
        .funcArg6 = NULL
      },
  .funcs[1] =
      { /*----rpi_digitalRead----*/
        // Function name (this is the name you use in ecmc plc-code)
        .funcName = "rpi_digitalRead",
        // Function description
        .funcDesc = "WiringPi: level = digitalRead(pin)",
        // Number of arguments in the function prototytpe
        .argCount = 1,
        /**
        * 7 different prototypes allowed (only doubles since reg in plc).
        * Only funcArg${argCount} func shall be assigned the rest set to NULL.
        * funcArg${argCount}. These need to match. 
        **/
        .funcArg0 = NULL,
        .funcArg1 = rpi_digitalRead, // 1 Arg
        .funcArg2 = NULL,
        .funcArg3 = NULL,
        .funcArg4 = NULL,
        .funcArg6 = NULL,
      },
  .funcs[2] =
      { /*----rpi_pinMode----*/
        // Function name (this is the name you use in ecmc plc-code)
        .funcName = "rpi_pinMode",
        // Function description
        .funcDesc = "WiringPi: void pinMode(pin,mode)",        
        // Number of arguments in the function prototytpe
        .argCount = 2,
        /**
        * 7 different prototypes allowed (only doubles since reg in plc).
        * Only funcArg${argCount} func shall be assigned the rest set to NULL.
        * funcArg${argCount}. These need to match. 
        **/
        .funcArg0 = NULL,
        .funcArg1 = NULL,
        .funcArg2 = rpi_pinMode,  // 2 Arg
        .funcArg3 = NULL,
        .funcArg4 = NULL,
        .funcArg6 = NULL,
        .funcArg6 = NULL
      },
  .funcs[3] = {0}, //last element set all to zero..

  /** Plugin specific constants (add prefix to not risc collide with other modules)
   *  Constants from wiringPi
   */
  /* LOW = 0 */
  .consts[0] = {
        .constName = "rpi_LOW",
        .constDesc = "WiringPi: Signal level LOW (=0)",
        .constValue = LOW
      },
  /* HIGH = 1 */
  .consts[1] = {
        .constName = "rpi_HIGH",
        .constDesc = "WiringPi: Signal level HIGH (=1)",
        .constValue = HIGH
      },
  /* INPUT = 0 */
  .consts[2] = {
        .constName = "rpi_INPUT",
        .constDesc = "WiringPi: Pin mode INPUT (=0)",
        .constValue = INPUT
      },
  /* OUTPUT = 1 */
  .consts[3] = {
        .constName = "rpi_OUTPUT",
        .constDesc = "WiringPi: Pin mode OUTPUT (=1)",
        .constValue = OUTPUT
      },
  /* PWM_OUTPUT = 2 */
  .consts[4] = {
        .constName = "rpi_PWM_OUTPUT",
        .constDesc = "WiringPi: Pin mode PWM_OUTPUT (=2)",
        .constValue = PWM_OUTPUT
      },
  /* GPIO_CLOCK = 3 */
  .consts[5] = {
        .constName = "rpi_GPIO_CLOCK",
        .constDesc = "WiringPi: Pin mode GPIO_CLOCK (=3)",
        .constValue = GPIO_CLOCK
      },
  /* SOFT_PWM_OUTPUT = 4 */
  .consts[6] = {
        .constName = "rpi_SOFT_PWM_OUTPUT",
        .constDesc = "WiringPi: Pin mode SOFT_PWM_OUTPUT (=4)",
        .constValue = SOFT_PWM_OUTPUT
      },
  /* SOFT_TONE_OUTPUT = 5 */
  .consts[7] = {
        .constName = "rpi_SOFT_TONE_OUTPUT",
        .constDesc = "WiringPi: Pin mode SOFT_TONE_OUTPUT (=5)",
        .constValue = SOFT_TONE_OUTPUT
      },
  /* PWM_TONE_OUTPUT = 6 */
  .consts[8] = {
        .constName = "rpi_PWM_TONE_OUTPUT",
        .constDesc = "WiringPi: Pin mode PWM_TONE_OUTPUT (=6)",
        .constValue = PWM_TONE_OUTPUT
      },
  /* PUD_OFF = 0 */
  .consts[9] = {
        .constName = "rpi_PUD_OFF",
        .constDesc = "WiringPi: Pull up/down resistor mode, PUD_OFF (=0)",
        .constValue = PUD_OFF
      },
  /* PUD_DOWN = 1 */
  .consts[10] = {
        .constName = "rpi_PUD_DOWN",
        .constDesc = "WiringPi: Pull up/down resistor mode, PUD_DOWN (=1)",
        .constValue = PUD_DOWN
      },
  /* PUD_UP = 2 */
  .consts[11] = {
        .constName = "rpi_PUD_UP",
        .constDesc = "WiringPi: Pull up/down resistor mode, PUD_UP (=2)",
        .constValue = PUD_UP
      },
  /* PWM_MODE_MS = 0 */
  .consts[12] = {
        .constName = "rpi_PWM_MODE_MS",
        .constDesc = "WiringPi: PWM mode PWM_MODE_MS (=0)",
        .constValue = PWM_MODE_MS
      },
  /* PWM_MODE_BAL = 1 */
  .consts[13] = {
        .constName = "rpi_PWM_MODE_BAL",
        .constDesc = "WiringPi: PWM mode PWM_MODE_BAL (=1)",
        .constValue = PWM_MODE_BAL
      },
   /* WPI_MODE_PINS = 0 */
  .consts[14] = {
        .constName = "rpi_WPI_MODE_PINS",
        .constDesc = "WiringPi: Mode WPI_MODE_PINS (=0)",
        .constValue = WPI_MODE_PINS
      },
  /* WPI_MODE_GPIO = 1 */
  .consts[15] = {
        .constName = "rpi_WPI_MODE_GPIO",
        .constDesc = "WiringPi: Mode WPI_MODE_GPIO (=1)",
        .constValue = WPI_MODE_GPIO
      },
  /* WPI_MODE_GPIO_SYS = 2 */
  .consts[16] = {
        .constName = "rpi_WPI_MODE_GPIO_SYS",
        .constDesc = "WiringPi: Mode WPI_MODE_GPIO_SYS (=2)",
        .constValue = WPI_MODE_GPIO_SYS
      },
  /* WPI_MODE_PIFACE = 3 */
  .consts[17] = {
        .constName = "rpi_WPI_MODE_PHYS",
        .constDesc = "WiringPi: Mode WPI_MODE_PHYS (=3)",
        .constValue = WPI_MODE_PHYS
      },
  /* WPI_MODE_PIFACE = 4 */
  .consts[18] = {
        .constName = "rpi_WPI_MODE_PIFACE",
        .constDesc = "WiringPi: Mode WPI_MODE_PIFACE (=4)",
        .constValue = WPI_MODE_PIFACE
      },
  // End with a NULL
  .consts[19] = {0}

};

ecmc_plugin_register(pluginDataDef);

# ifdef __cplusplus
}
# endif  // ifdef __cplusplus
