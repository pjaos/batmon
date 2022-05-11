# BATMON

## Overview
The batmon device connects to a Lithium ION battery (primarily aimed at Ebike batteries) and allows the user to 
trade off the lifetime of the battery against the maximum single charge. The life of a lithium ION cell 
is affected by many things, the maximum charge level of the cell, the temperature of the cell, the maximum 
discharge of the cell etc. The batmon device allows the user to limit the maximum charge voltage of the cell 
to reduce stresses on the battery chemistry.

For example if the maximum charge voltage of a Lithium ION cell is set to 4.1 
volts per cell rather than the maximum of 4.2 volts per cell then the number of charge/discharge cycles 
that the battery is capable of could be doubled. The trade off for this is that you will loose about 10 % 
of the range on a single charge. If set to 4.0 volts per cell then the number of charge/discharge cycles 
that the battery is capable of could be increased by four times. The trade off for this is that you will 
loose about 25 % of the range on a single charge. The batmon device allows the user to set this trade off 
by selecting the maximum charge voltage in five 0.05 volt steps from 4.2 to 4.0 volts.

The device also ensures that the battery is not charged when to cold or hot which also limit the batteries 
lifetime. Charging at temperatures of 0°C or below can also be dangerous.

As stated previously this project is primarily aimed at improving the lifetime of Ebike batteries although the hardware could be used for other batter voltages with slight tweaks to the software. The device provides a WiFi connection. This can connect to your home WiFi network or run in stand alone (AP) mode. The [Yview App](https://github.com/pjaos/yview/tree/master/gui) can be used (Android phone/tablet or a Windows, Linux PC) to access the state of the battery (charge voltage/current etc).

## System Requirements
An initial set of requirements for the system is shown below.

* Charge the battery to a maximum voltage of between 4.0 and 4.2 Volts, configurable by the user.
* Display the charge voltage, current and power.
* Turn off charging if the battery temperature is too cold (<= 0°C).
* Turn off charging if the battery temperature is too hot (> 45°C)g.

## Hardware.
The hardware design uses an ESP32 CPU which provides the WiFi connectivity. An ADS1115 I2C 16 bit ADC device is used to measure the battery voltage, charge current and battery temperature. A Mosfet is used to turn off the battery charge once it reaches the configured charge voltage.

![Top Level Schematic](images/top_schematic.png "Top Level Schematic")
![Top Level Schematic](images/psu_schematic.png "PSU Schematic")
![Top Level Schematic](images/mcu_schematic.png "MCU Schematic")
![PCB](images/pcb.png "PCB") ![PCB](images/pcb1.png "PCB Assembled")
![PCB](images/pcb2.png "PCB Assembled (Reduced Size, Side 1)")
![PCB](images/pcb3.png "PCB Assembled (Side 2)")
![PCB](images/pcb4.png "PCB Assembled (Inserting into case)")
![PCB](images/module1.png "Module Assembled (Side 2)")
![PCB](images/module2.png "Module Assembled (Side 1)")
![PCB](images/bat1.png "Module Installed in battery.")

## Software
The software for the device is written in C and uses the [Mongoose OS](https://mongoose-os.com/).



