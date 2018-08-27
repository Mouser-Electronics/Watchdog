/*
 * usart.c
 *
 * Created: 7/10/2018 1:41:06 PM
 *  Author: braddock.bresnahan
 */ 

#include "usart.h"
#include <string.h>
#include "boardfuncs.h"
#include "led.h"

/// "Private" Functions

/*
// Name: USARTReportSuccess
// Desc: Blinks LED different colors depending on the given state.
// Input: (bool) command: The state to report.
// Return: (bool) An echo of the given state.
*/
bool USARTReportSuccess(bool command){
	uint8 led = LEDG;
	
	while (!LEDIsIdle());
	if (command == USART_FAIL) led = LEDR;
	
	SetLED(led, ON);
	delay_ms(USART_BUMP_DELAY);
	SetLED(led, OFF);
	
	return command;
}

/// Functions

/*
// Name: usart_tx_cb
// Desc: The TX callback when the MCU is done sending a message to the BLE.
*/
static void usart_tx_cb(const struct usart_async_descriptor *const io_descr){
	USARTBusy = false; // after write is done
}

/*
// Name: usart_rx_cb
// Desc: The RX callback when the MCU receives a character(s) over USART. The function
//       loads the read characters into USARTIn.
*/
static void usart_rx_cb(const struct usart_async_descriptor *const io_descr) {
	static uint8 buff[USART_STANDARD_BUFFER_SIZE];
	static uint count = 0;
	static uint8 value;
	static bool active = false;
	
	USARTBusy = true;
	
	do{
		ASSERT(count < USART_STANDARD_BUFFER_SIZE); // Overflow
		
		io_read(usartMaster, &value, 1);
		
		if (!active){ // Start of new read
			if (value == '\r' || value == ' ' || value == '\n') continue; // Ignores carriage returns, spaces and newlines as first entries
			active = true;
			count = 0;
		}
		else if (value == ' ' || value == '\n' || value == '%') // Possible ending characters
			active = false;
		else;
		
		buff[count++] = value;
		
		if (!active){ // Message has ended
			STR_Stringify((char*)buff, count);
			QUE_Enqueue(&USARTIn, buff);
			if (true) memset(buff, 0, USART_STANDARD_BUFFER_SIZE); // Debug purposes
		}
	} while (USART.rx.read_index != USART.rx.write_index); // If there's more to read, keep going.
	
	USARTBusy = active; // Only becomes free after completed message
}

void InitUSART(){
	uint timeout = 0;
	char buff[USART_STANDARD_BUFFER_SIZE];
	// BLE cannot read and write to USART simultaneously. System is set up with that in mind.
	
	usartMaster = NULL;
	USARTBusy = false;
	QUE_InitQueue(&USARTIn, USART_STANDARD_BUFFER_SIZE);
	
	usart_async_register_callback(&USART, USART_ASYNC_TXC_CB, usart_tx_cb);
	usart_async_register_callback(&USART, USART_ASYNC_RXC_CB, usart_rx_cb);
	usart_async_get_io_descriptor(&USART, &usartMaster);
	usart_async_enable(&USART);
	
	// Setup BLE
	FlushUSART();
	// Check for responsiveness
	STR_ClearString(buff);
	while (USARTReportSuccess(CommandBLEReturn(BLE_CMD_ENTER_COMMAND_MODE, buff)) == USART_FAIL){ // Checks for responsiveness
		if (STR_Contains(buff, "Err")) break; // It did respond, it just didn't like that command.
		else{
			if (timeout++ >= USART_INIT_RETRIES){
				LEDEnqueue(LEDR, USART_NO_RESPONSE_STRING);
				return; // Unresponsive
			}
		}
	}
	LEDEnqueue(LEDG, USART_RESPONSE_STRING);
	
	// Settings
	USARTReportSuccess(CommandBLE(BLE_CMD_SET_DEVICE_NAME, BLE_DEVICE_NAME));
	USARTReportSuccess(CommandBLE(BLE_CMD_SET_SUPPORTED_FEATURES, BLE_FEATURES_CODE));
	USARTReportSuccess(CommandBLE(BLE_CMD_SET_APPEARANCE, BLE_APPEARANCE_CODE));
	USARTReportSuccess(CommandBLE(BLE_CMD_SET_DEFAULT_SERVICES, BLE_SERVICES_CODE));
	USARTReportSuccess(CommandBLE(BLE_CMD_SET_ADVERTISEMENT_POWER, BLE_ADVERTISEMENT_POWER));
	
	// Clear bondings from previous init (if any)
	USARTReportSuccess(CommandBLE(BLE_CMD_REMOVE_EXISTING_BONDING, ",Z"));
	
	// Apply changes
	USARTReportSuccess(CommandBLE(BLE_CMD_REBOOT, NULL));
	FlushUSART();
}

void EnableUSART(){
	usart_async_enable(&USART);
}

void DisableUSART(){
	usart_async_disable(&USART);
}

bool ReadUSART(char* const data){
	uint16 timeout = 0;
	delay_ms(100);
	
	STR_ClearString(data);
	if (USARTDirection == USART_DIR_WRITE)
		if (WaitForUSART() == USART_FAIL) return USART_FAIL; // Wait until USART is done writing. Doesn't start timeout
	
	SetLED(LEDD, ON);
	USARTDirection = USART_DIR_READ;
	do{
		if (timeout++ >= USART_GENERAL_TIMEOUT){
			SetLED(LEDD, OFF);
			return USART_FAIL;
		}
		delay_ms(1);
	} while (QUE_IsEmpty(&USARTIn) || USARTBusy); // Only reads when the USART is finished receiving.
	QUE_Dequeue(&USARTIn, data);
	
	SetLED(LEDD, OFF);
	return USART_SUCCESS;
}

bool PeekUSART(char* const data){
	uint16 timeout = 0;
	
	STR_ClearString(data);
	if (USARTDirection == USART_DIR_WRITE)
		if (WaitForUSART() == USART_FAIL) return USART_FAIL; // Wait until usart is done writing. Doesn't start timeout
	
	SetLED(LEDD, ON);
	USARTDirection = USART_DIR_READ;
	do{
		if (timeout++ >= USART_GENERAL_TIMEOUT){
			SetLED(LEDD, OFF);
			return USART_FAIL;
		}
	} while (QUE_IsEmpty(&USARTIn) || USARTBusy); // Only reads when the USART is finished receiving.
	QUE_Peek(&USARTIn, data);
	
	SetLED(LEDD, OFF);
	return USART_SUCCESS;
}

bool WriteUSART(const string data){
	if (data == NULL) return USART_FAIL;
	
	if (WaitForUSART() == USART_FAIL) return USART_FAIL;
	USARTDirection = USART_DIR_WRITE;
	
	SetLED(LEDD, ON);
	if (io_write(usartMaster, (uint8*)data, STR_GetSize(data)) == 0){
		SetLED(LEDD, OFF);
		return USART_FAIL;
	}
	USARTBusy = true;
	
	SetLED(LEDD, OFF);
	return USART_SUCCESS;
}

bool FlushUSART(){
	SetLED(LEDD, ON);
	QUE_Clear(&USARTIn);
	
	SetLED(LEDD, OFF);
	return USART_SUCCESS;
}

bool WaitForUSART(void){
	uint timeout = 0;
	
	while (USARTBusy){
		if (timeout++ >= USART_GENERAL_TIMEOUT) return USART_FAIL;
		delay_ms(1);
	}
	
	return USART_SUCCESS;
}

void QwikWrite(const char* code, const char* param){
	static char buff[USART_STANDARD_BUFFER_SIZE];
	
	STR_ClearString(buff);
	STR_Stack(buff, code, param, "\r", NULL);
	WriteUSART(buff);
}

bool CommandBLE(BLECommand cmd, const char* param){
	char buff[USART_STANDARD_BUFFER_SIZE];
	
	STR_ClearString(buff);
	STR_Cat(buff, param);
	
	return CommandBLEReturn(cmd, buff);
}

bool CommandBLEReturn(BLECommand cmd, char* param){
	if (WaitForUSART() == USART_FAIL) return USART_FAIL;
	switch(cmd){
		case BLE_CMD_NONE: break;
		case BLE_CMD_SET_BAUD_RATE:								QwikWrite("SB", param);		break;
		case BLE_CMD_SET_APPEARANCE:							QwikWrite("SDA", param);	break;
		case BLE_CMD_RESET_TO_FACTORY_DEFAULT:					QwikWrite("SF,1", param);	break;
		case BLE_CMD_SET_ADVERTISEMENT_POWER:					QwikWrite("SGA", param);	break;
		case BLE_CMD_SET_DEVICE_NAME:							QwikWrite("SN", param);		break;
		case BLE_CMD_SET_POWER_MODE:							QwikWrite("SO", param);		break;
		case BLE_CMD_SET_SUPPORTED_FEATURES:					QwikWrite("SR", param);		break;
		case BLE_CMD_SET_DEFAULT_SERVICES:						QwikWrite("SS", param);		break;
		case BLE_CMD_SET_INITIAL_CONNECTION_PARAMETERS:			QwikWrite("ST", param);		break;
		case BLE_CMD_SET_PIN_FUNCTIONS:							QwikWrite("SW", param);		break;
		case BLE_CMD_GET_CONNECTION_STATUS:						QwikWrite("GK", param);		break;
		case BLE_CMD_GET_PEER_DEVICE_NAME:						QwikWrite("GNR", param);	break;
		case BLE_CMD_GET_SETTING:								QwikWrite("G", param);		break;
		case BLE_CMD_ENTER_COMMAND_MODE:
			delay_ms(100);
			WriteUSART("$$$");
			if (PeekUSART(param) == USART_FAIL) // Will not respond if already in command mode
				WriteUSART("\r"); // "Shake" the command so it does not get stuck (will most likely return Err [that's a good thing])
		break;
		case BLE_CMD_EXIT_COMMAND_MODE:							QwikWrite("---", param);	break;
		case BLE_CMD_ASSIGN_RANDOM_ADDRESS:						QwikWrite("&", param);		break;
		case BLE_CMD_CLEAR_RANDOM_ADDRESS:						QwikWrite("&C", param);		break;
		case BLE_CMD_ASSIGN_RESOLVABLE_RANDOM_ADDRESS:			QwikWrite("&R", param);		break;
		case BLE_CMD_START_ADVERTISING:							/*vvvv*/STR_ClearString(param);/*vvvv*/
		case BLE_CMD_START_ADVERTISING_ADV:						QwikWrite("A", param);		break;
		case BLE_CMD_BOND_CONNECTED_DEVICE:						QwikWrite("B", param);		break;
		case BLE_CMD_CONNECT_TO_ADDRESS:						QwikWrite("C", param);		break;
		case BLE_CMD_CONNECT_TO_PUBLIC_ADDRESS:					QwikWrite("C,0", param);	break;
		case BLE_CMD_CONNECT_TO_PRIVATE_ADDRESS:				QwikWrite("C,1", param);	break;
		case BLE_CMD_CONNECT_TO_STORED_DEVICE:					QwikWrite("C", param);		break;
		case BLE_CMD_START_SCANNING:							/*vvvv*/STR_ClearString(param);/*vvvv*/
		case BLE_CMD_START_SCANNING_ADV:						QwikWrite("F", param);		break;
		case BLE_CMD_INITIATE_UART_TRANSPARENT:					QwikWrite("I", param);		break;
		case BLE_CMD_DISCONNECT_ACTIVE_LINK:					QwikWrite("K,1", param);	break;
		case BLE_CMD_GET_SIGNAL_STRENGTH_OF_LAST_COMMUNICATION:	QwikWrite("M", param);		break;
		case BLE_CMD_ENTER_DORMANT_MODE:						QwikWrite("O,1", param);	break;
		case BLE_CMD_REBOOT:
			WriteUSART("R,1\r");
			ReadUSART(param);
		break;
		case BLE_CMD_REMOVE_EXISTING_BONDING:					QwikWrite("U", param);		break;
		case BLE_CMD_STOP_SCANNING:								QwikWrite("X", param);		break;
		case BLE_CMD_STOP_ADVERTISING:							QwikWrite("Y", param);		break;
		case BLE_CMD_CANCEL_CONNECTION_ATTEMPT:					QwikWrite("Z", param);		break;
		case BLE_CMD_LIST_BONDED_DEVICES:						QwikWrite("LB", param);		break;
		case BLE_CMD_SET_PRIVATE_CHARACTERISTIC:				QwikWrite("PC", param);		break;
		case BLE_CMD_SET_UUID:									QwikWrite("PS", param);		break;
		case BLE_CMD_CLEAR_ALL_SERVICE_SETTINGS:				QwikWrite("PZ", param);		break;
		case BLE_CMD_READ_CONTENT_OF_REMOTE_DEVICE:				QwikWrite("CHR", param);	break;
		case BLE_CMD_WRITE_CONTENT_OF_REMOTE_DEVICE:			QwikWrite("CHW", param);	break;
		case BLE_CMD_START_CLIENT_OPERATION:					QwikWrite("CI", param);		break;
		case BLE_CMD_READ_CONTENT_OF_SERVER_SERVICE:			QwikWrite("SHR", param);	break;
		case BLE_CMD_WRITE_CONTENT_OF_SERVER_SERVICE:			QwikWrite("SHW", param);	break;
		default: return USART_FAIL;
	}
	
	ReadUSART(param);
	// Check for no response or error
	if (STR_GetSize(param) == 0 || STR_Contains(param, "Err")) return USART_FAIL;
	else return USART_SUCCESS;
}