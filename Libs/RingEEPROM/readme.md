
# RingEEPROM

<!-- Badges -->
[![Build Status][build-shield]][build-url]
[![GitHub release][release-shield]][release-url]
[![GitHub commits][commits-shield]][commits-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

## Hign Endurance EEEPROM
Atmega microcontrollers have a limited EEPROM write cycle of 100k. Using the library we can overcome this limit of 100k write cycles. Primarily I have developed the library for the Arduino platform. I will add the library to other platforms in the near future
## Problem Statement 
In many situation we need to write the EEPROM memory frequently.For an energy meter application, The energy parameter needs  to update frequently in EEPROM to persist the value for a long time. 
Let's Think about a situation. 
I need to write energy value in eeprom after 10s interval.
- total number of writes for one day would be 8640. 
- one month 259.2K 

Atmel AVR microcontroller such as atmega328 ensures EEPROM data reliability  upto 100k write/erase cycle. Oops!! That means we will cross that limit after 11 days. That is very sad. 

One more thing we need to clarify that 100k write/erase cycle for each cell, not for a whole EEPROM memory segment. Here is the point we are going to play with. 

# RingEEPROM Arduino Library

![Arduino EEPROM Endurance, EEPROM Ring buffer](/resources/parallel_o_buffer.png "Parallel  O Buffer | Source: AVR101: High Endurance EEPROM Storage")

For solving the eeprom wear out problem, I have developed an Arduino library  [RingEEPROM]().

## Solution
The idea is simple. As I cannot write/erase safely each cell more than 100k times. what if multiple cells is used for the same variable. To clarify more, Suppose I want to store a variable in EEPROM. What I will do is, at First I will write the variable in first location, second time I will write the variable in the second location.Third time I will write the variable in the third location, Fourth time I will write the variable in the first location. I am repeating the pattern after 3 locatons. Thus I am getting 3 times endurance for a single variable. 

I will store my value in different cells in each write cycle. So let's consider our buffer size i 8. I am planning to write a byte in EEPROM. As I am using 8 cells for a single byte. Now I get 8*100k = 800k write cycles. That's huge. The bigger the  buffer size is, the more write cycles I get. 

In this library, I will not store a single variable, I have developed the library such a way so that I can handle any size of buffer. 

So for saving value in eeprom, I need two types of buffer 
 1. Parameter Buffer : This is the intended value we want to store in EEPROM
 2. Status Buffer: This buffer keeps track of my current location in buffer.

For more information please go through the Microchip [application note](http://ww1.microchip.com/downloads/en/appnotes/doc2526.pdf)

License
----
MIT

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[build-shield]:https://travis-ci.com/shuvangkar/RingEEPROM.svg?branch=master
[build-url]: https://travis-ci.com/github/shuvangkar/RingEEPROM

[release-shield]: https://img.shields.io/github/release/shuvangkar/RingEEPROM.svg
[release-url]: https://github.com/shuvangkar/RingEEPROM

[commits-shield]: https://img.shields.io/github/commits-since/shuvangkar/RingEEPROM/v0.1.0
[commits-url]: https://img.shields.io/github/commits-since/shuvangkar/RingEEPROM/v0.1.0


[license-shield]: https://img.shields.io/github/license/shuvangkar/RingEEPROM
[license-url]: https://github.com/shuvangkar/RingEEPROM/blob/master/LICENSE.txt


[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/shuvangkar