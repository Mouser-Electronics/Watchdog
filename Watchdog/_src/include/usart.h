/*
 * usart.h
 *
 * Created: 7/10/2018 1:40:39 PM
 *  Author: braddock.bresnahan
 */ 


#ifndef USART_H_
#define USART_H_

#include <stdbool.h>
#include "driver_init.h"
#include "stdtypes.h"
#include "array.h"
#include "queue.h"
#include "strfuncs.h"

/// Macros

#define BLE_DEVICE_NAME			",Watchdog" // What name will appear to other devices.

// Bluetooth Addresses [a hex string] {leave comma as first character} <all letters must be capitalized>
#define BLE_DEVICE_ADDRESS		",############" // My Public Bluetooth Address (Who I am)
#define BLE_PARTNER_ADDRESS		",############" // Partner's Public Bluetooth Address (Who to connect to)

#define BLE_FEATURES_CODE		",D400" // Flow control, ignore beacons, and other settings
#define BLE_APPEARANCE_CODE		",1440" // Generic outdoor activity
#define BLE_SERVICES_CODE		",80" // Supports device information
#define BLE_ADVERTISEMENT_POWER	",0" // 0 - Highest Power, 5 - Lowest power (signal strength numbers will change)
#define BLE_POWER_MODE			",1" // Low power mode enabled (does not go into low power mode)

#define BLE_MAXIMUM_DEVICES	8 // Limited by hardware (8 bonded slots)

#define USART_GENERAL_TIMEOUT		1000 // In Milliseconds
#define USART_INIT_RETRIES			3
#define USART_REBOOT_RETRIES		3 // IE. reboot can take up to 6 seconds before timeout
#define USART_STANDARD_BUFFER_SIZE	(3 * 0x18) // The max size a message from the BLE can be (including '\0')

#define USART_SUCCESS	true
#define USART_FAIL		false

#define USART_BUMP_DELAY			10 // In milliseconds, used for debugging
#define USART_RESPONSE_STRING		"_--------_"
#define USART_NO_RESPONSE_STRING	"_------------_"

/*#define RANDOM_16BIT_UUID_0		",8FE8"
#define RANDOM_16BIT_UUID_1		",1CCA"
#define RANDOM_16BIT_UUID_2		",9363"
#define RANDOM_16BIT_UUID_3		",EB94"
#define RANDOM_128BIT_UUID_0	",DF97162AA07E36CF9F06CE769EF45721"
#define RANDOM_128BIT_UUID_1	",C9008618CDD8BD2A52BD904EAFDD5A13"
#define RANDOM_128BIT_UUID_2	",13EFC585534C575DD96217728E554FEC"
#define RANDOM_128BIT_UUID_3	",4DDF79A3EAE15D9F40ECBCB1E51981BE"
#define RANDOM_128BIT_UUID_4	",584B51160B0ECC39E0074EFF22FC363B"
#define RANDOM_128BIT_UUID_5	",B721D3799D100285F06CCA19EA17A040"
#define RANDOM_128BIT_UUID_6	",1CD499968C9CE6ABDDCED37AC7F76FA8"
#define RANDOM_128BIT_UUID_7	",30ADF2A553D45091387A841D6B69743F"*/

/*#define ZERO	"0"
#define ONE		"1"
#define TWO		"2"
#define THREE	"3"
#define FOUR	"4"
#define FIVE	"5"
#define SIX		"6"
#define SEVEN	"7"
#define EIGHT	"8"
#define NINE	"9"

#define CZERO	",0"
#define CONE	",1"
#define CTWO	",2"
#define CTHREE	",3"
#define CFOUR	",4"
#define CFIVE	",5"
#define CSIX	",6"
#define CSEVEN	",7"
#define CEIGHT	",8"
#define CNINE	",9"*/

/// Enums

// RN4870 Commands (documentation on RN4870 Datasheet)
typedef enum{
	BLE_CMD_NONE,
	BLE_CMD_SET_SERIAL_NAME,					// 2.4.1  pg.16
	BLE_CMD_SET_COMMAND_MODE_CHAR,				// 2.4.2  pg.16
	BLE_CMD_SET_PRE_POST_DELIM,					// 2.4.3  pg.16
	BLE_CMD_SET_NVM_CONFIG,						// 2.4.4  pg.17
	BLE_CMD_SET_AUTHENTICATION,					// 2.4.5  pg.17
	BLE_CMD_SET_BAUD_RATE,						// 2.4.6  pg.18
	BLE_CMD_SET_BEACON_CONFIG,					// 2.4.7  pg.18
	BLE_CMD_SET_APPEARANCE,						// 2.4.8  pg.19
	BLE_CMD_SET_FIRMWARE_CHARACTERISTIC,		// 2.4.9  pg.19
	BLE_CMD_SET_HARDWARE_CHARACTERISTIC,		// 2.4.10 pg.19
	BLE_CMD_SET_MODEL_NAME_CHARACTERISTIC,		// 2.4.11 pg.19
	BLE_CMD_SET_MANUFACTURER_CHARACTERISTIC,	// 2.4.12 pg.20
	BLE_CMD_SET_SOFTWARE_CHARACTERISTIC,		// 2.4.13 pg.20
	BLE_CMD_SET_SERIAL_NUMBER_CHARACTERISTIC,	// 2.4.14 pg.20
	BLE_CMD_RESET_TO_FACTORY_DEFAULT,			// 2.4.15 pg.20
	BLE_CMD_SET_ADVERTISEMENT_POWER,			// 2.4.16 pg.21
	BLE_CMD_START_STOP_TIMER,					// 2.4.17 pg.21
	BLE_CMD_SET_DEVICE_NAME,					// 2.4.18 pg.21
	BLE_CMD_SET_POWER_MODE,						// 2.4.19 pg.21
	BLE_CMD_SET_FIXED_SECURITY_PIN,				// 2.4.20 pg.22
	BLE_CMD_SET_SUPPORTED_FEATURES,				// 2.4.21 pg.22
	BLE_CMD_SET_DEFAULT_SERVICES,				// 2.4.22 pg.23
	BLE_CMD_SET_INITIAL_CONNECTION_PARAMETERS,	// 2.4.23 pg.24
	BLE_CMD_SET_PIN_FUNCTIONS,					// 2.4.24 pg.25
	
	BLE_CMD_GET_REG,				// 2.5.1  pg.26
	BLE_CMD_GET_CONNECTION_STATUS,	// 2.5.2  pg.26
	BLE_CMD_GET_PEER_DEVICE_NAME,	// 2.5.3  pg.26
	BLE_CMD_GET_SETTING,			// 2.5.4  pg.26
	
	BLE_CMD_TOGGLE_LOCAL_ECHO,							// 2.6.1  pg.27
	BLE_CMD_ENTER_COMMAND_MODE,							// 2.6.2  pg.27
	BLE_CMD_EXIT_COMMAND_MODE,							// 2.6.3  pg.27
	BLE_CMD_READ_ANALOG_CHANNEL,						// 2.6.5  pg.28
	BLE_CMD_READ_DIGITAL_IO,							// 2.6.6  pg.28
	BLE_CMD_SET_DIGITAL_IO,								// 2.6.7  pg.29
	BLE_CMD_SET_PWM,									// 2.6.8  pg.29
	BLE_CMD_ASSIGN_RANDOM_ADDRESS,						// 2.6.9  pg.30
	BLE_CMD_CLEAR_RANDOM_ADDRESS,						// 2.6.10 pg.31
	BLE_CMD_ASSIGN_RESOLVABLE_RANDOM_ADDRESS,			// 2.6.11 pg.31
	BLE_CMD_START_ADVERTISING,							// 2.6.12 pg.31
	BLE_CMD_START_ADVERTISING_ADV,
	BLE_CMD_BOND_CONNECTED_DEVICE,						// 2.6.13 pg.31
	BLE_CMD_CONNECT_WITH_LAST_BOND,						// 2.6.14 pg.32
	BLE_CMD_CONNECT_TO_ADDRESS,							// 2.6.15 pg.32
	BLE_CMD_CONNECT_TO_PUBLIC_ADDRESS,
	BLE_CMD_CONNECT_TO_PRIVATE_ADDRESS,
	BLE_CMD_CONNECT_TO_STORED_DEVICE,					// 2.6.16 pg.33
	BLE_CMD_DISPLAY_CRITICAL_INFORMATION,				// 2.6.17 pg.33
	BLE_CMD_START_SCANNING,								// 2.6.18 pg.33
	BLE_CMD_START_SCANNING_ADV,
	BLE_CMD_INITIATE_UART_TRANSPARENT,					// 2.6.19 pg.34
	BLE_CMD_ADD_TO_WHITELIST,							// 2.6.21 pg.35
	BLE_CMD_ADD_ALL_BONDED_TO_WHITELIST,				// 2.6.22 pg.35
	BLE_CMD_CLEAR_WHITELIST,							// 2.6.23 pg.36
	BLE_CMD_DISPLAY_WHITELIST,							// 2.6.24 pg.36
	BLE_CMD_DISCONNECT_ACTIVE_LINK,						// 2.6.25 pg.36
	BLE_CMD_GET_SIGNAL_STRENGTH_OF_LAST_COMMUNICATION,	// 2.6.26 pg.36
	BLE_CMD_ENTER_DORMANT_MODE,							// 2.6.27 pg.37
	BLE_CMD_REBOOT,										// 2.6.28 pg.37
	BLE_CMD_CHANGE_CONNECTION_PARAMETERS,				// 2.6.29 pg.37
	BLE_CMD_REMOVE_EXISTING_BONDING,					// 2.6.30 pg.38
	BLE_CMD_DISPLAY_FIRMWARE_VERSION,					// 2.6.31 pg.38
	BLE_CMD_STOP_SCANNING,								// 2.6.32 pg.38
	BLE_CMD_STOP_ADVERTISING,							// 2.6.33 pg.38
	BLE_CMD_CANCEL_CONNECTION_ATTEMPT,					// 2.6.34 pg.38
	
	BLE_CMD_LIST_BONDED_DEVICES,	// 2.7.1  pg.39
	BLE_CMD_LIST_CLIENT_SERVICES,	// 2.7.2  pg.39
	BLE_CMD_LIST_SERVER_SERVICES,	// 2.7.3  pg.40
	BLE_CMD_LIST_CURRENT_SCRIPT,	// 2.7.4  pg.40
	
	BLE_CMD_SET_PRIVATE_CHARACTERISTIC,			// 2.8.1  pg.41
	BLE_CMD_SET_UUID,							// 2.8.2  pg.42
	BLE_CMD_CLEAR_ALL_SERVICE_SETTINGS,			// 2.8.3  pg.42
	BLE_CMD_READ_CONTENT_OF_REMOTE_DEVICE,		// 2.9.2  pg.44
	BLE_CMD_WRITE_CONTENT_OF_REMOTE_DEVICE,		// 2.9.3  pg.44
	BLE_CMD_START_CLIENT_OPERATION,				// 2.9.4  pg.45
	BLE_CMD_READ_CONTENT_OF_SERVER_SERVICE,		// 2.9.5  pg.45
	BLE_CMD_WRITE_CONTENT_OF_SERVER_SERVICE,	// 2.9.6  pg.46
	
	BLE_CMD_CLEAR_SCRIPTS,				// 2.10.1  pg.46
	BLE_CMD_STOP_SCRIPTS,				// 2.10.2  pg.47
	BLE_CMD_START_SCRIPTS,				// 2.10.3  pg.47
	BLE_CMD_ENTER_SCRIPT_INPUT_MODE,	// 2.10.4  pg.47
} BLECommand;

typedef enum{
	USART_DIR_READ,
	USART_DIR_WRITE,
} USARTDir; // Used to prevent talking over RN4870

/// Functions

/*
// Name: InitUSART
// Desc: Initializes the communication between the MCU and the RN4870. Also configures the
//       RN4870.
*/
void InitUSART(void);

/*
// Name: EnableUSART
// Desc: Enables communication with the BLE.
*/
void EnableUSART(void);
/*
// Name: DisableUSART
// Desc: Disabled communication with the BLE.
*/
void DisableUSART(void);

/*
// Name: ReadUSART
// Desc: Reads the next received message from the BLE over USART.
// Input: (char* const) data: Where to store the read message. Ignores if NULL.
// Return: (bool) True if the read was successful, else false.
*/
bool ReadUSART(char* const data);
/*
// Name: PeekUSART
// Desc: Peeks at the next received message from the BLE over USART.
// Input: (char* const) data: Where to store the peeked message. Should never be NULL.
// Return: (bool) True if the peek was successful, else false.
*/
bool PeekUSART(char* const data);
/*
// Name: WriteUSART
// Desc: Writes the given message string to the BLE over USART.
// Input: (const string) data: The message to send.
// Return: (bool) True if the write was successful, else false.
*/
bool WriteUSART(const string data);
/*
// Name: FlushUSART
// Desc: Flushes all pending messages to read.
// Return: (bool) True if the flush was successful, else false.
*/
bool FlushUSART(void);
/*
// Name: WaitForUSART
// Desc: Patiently waits for the BLE to finish writing or reading.
// Return: (bool) False if the function timed out, else true.
*/
bool WaitForUSART(void);

/*
// Name: CommandBLE
// Desc: Interpenetrates and sends commands to the BLE.
// Input: (BLECommand)    cmd: The command to send.
//        (const char*) param: Any additional parameters to send with the command. Can be
//                             NULL.
// Return: (bool) True if the command and BLE response were successful, else false.
*/
bool CommandBLE(BLECommand cmd, const char* param);
/*
// Name: CommandBLEReturn
// Desc: Interpenetrates and sends commands to the BLE and stores the response in param.
// Input: (BLECommand) cmd: The command to send.
//        (char*) param: Doubles as any additional parameters to send with the command and the storage container to store the response.
// Return: (bool) True if the command and BLE response were successful, else false.
*/
bool CommandBLEReturn(BLECommand cmd, char* param);

/// Global Variables

/*
// Name: usartMaster
// Desc: The master USART descriptor.
*/
struct io_descriptor* usartMaster;
/*
// Name: USARTBusy
// Desc: A variables that indicates USART is being used.
*/
volatile bool USARTBusy;
/*
// Name: USARTDirection
// Desc: Whether USART is reading from or writing to USART.
*/
USARTDir USARTDirection;
/*
// Name: USARTIn
// Desc: A queue that holds all read messages over USART to read. (Async to polled)
*/
volatile struct Queue USARTIn; // Queue of strings

/*
// Name: bondNum
// Desc: The number of bonded devices (members in group).
*/
volatile char bondNum;


#endif /* USART_H_ */