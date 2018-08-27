/*
Watchdog is personal Bluetooth range sensor that alarms users when they are out of range.
    	Copyright (C) 2018  Mouser Electronics

   	This program is free software: you can redistribute it and/or modify
  	 it under the terms of the GNU General Public License as published by
   	the Free Software Foundation, either version 3 of the License, or
    	any later version.

    	This program is distributed in the hope that it will be useful,
    	but WITHOUT ANY WARRANTY; without even the implied warranty of
    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    	GNU General Public License for more details.

    	You should have received a copy of the GNU General Public License
    	along with this program.  If not, see <http://www.gnu.org/licenses/>.

		Project BOM:
		https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=de635072d5
		Note: The BOM list only the materials for a single band. It is
		recommended that you get multiple bands for testing and debugging.
		
		Watchdog can only connect to a single, hard-coded band with a known
		Bluetooth address. You will need to change the address in "usart.h"
		at BLE_DEVICE_ADDTESS and BLE_PERTNER_ADDRESS for this band's address
		and the the band to connect to respectively.
		
		Some design choices may seem a bit off because we were originally
		planning more features than we were able to complete and test in
		the time we were given. Some of the features that were planned are
		listed in the project article in the "Expanded Features" section.

      
*/

#include "watchdog.h"

/// Function Declarations

// Main
int main(void);

/* NOTE: It might matter if the vibrator sets off the accelerometer. You should turn off or reduce the sensitivity of the accelerometer while the device in vibrating */ // Or only worry about 

/// Main

int main(void)
{
	BandInit();
	
	while (1) BandMain();
	
	return 0;
}





