# PIC16-BASED-MEDICINE-REMINDER

Smart Medicine Reminder Box

This project is a Medicine Reminder System using PIC16F877A, DS3234 RTC, and 16x2 LCD.
It displays the current time on the LCD, and at predefined times it shows the medicine name for 30 seconds while blinking an LED.

🛠 Features

Real-time clock (RTC) using DS3234
16x2 LCD displays current time (HH:MM:SS) continuously
Pre-programmed medicine times trigger:
Medicine 1 → 14:50
Medicine 2 → 14:55
Medicine 3 → 15:00

Each medicine alert:

Displays medicine name from EEPROM
LED blinks for 30 seconds
LCD time keeps running (non-blocking code)

⚡ Hardware Used

PIC16F877A Microcontroller
DS3234 Real-Time Clock (RTC)
16x2 LCD Display (LM016L)
LEDs with resistors (for alerts)
Push buttons (optional for reset/acknowledge)
3V Battery (RTC backup)

🔌 Circuit Overview

RTC connected via SPI to PIC16F877A
LCD in 8-bit mode with control lines RS, RW, EN
LEDs on PORT pins (for alerts)
3V battery backup for DS3234

📋 How It Works

RTC continuously keeps time.
LCD shows Time: HH:MM:SS on the first line.
At scheduled times, system checks and:
Reads medicine name from EEPROM
Displays name on LCD second line
Turns ON LED for 30 seconds (blinking possible)
AftEr 30 seconds, LED turns off and LCD clears the message.
System continues running without stopping the timer.

🖥 Software

Developed in MPLAB X / XC8
Uses non-blocking code (no long delays)
RTC communication via SPI
EEPROM used to store medicine names

▶️ Simulation

Designed and tested in Proteus
Timer runs continuously, medicines alert at specific times

🚀 Future Enhancements

Add buzzer alert with LED
Use push button to confirm medicine taken
Expand to store multiple medicine schedules
Connect to Bluetooth / IoT for notifications

📌 Author: Aravindhan .G
📅 Year: 2025
