/*
MIT License

Copyright (c) 2023 Rafiul

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*		Library Blocks for the Chipset and others	*/

#include <mega32.h>
#define F_CPU 8000000UL // 8Mhz Speed
#include <delay.h>



/*		Variables | Store Time	*/

// 86400 = 24 hours and 86399 = 23h 59m 59s
int totalSec, clockTime, stopWatchTime = 86400, 0, 0;
int monthMax, weekMax = 12, 7;

// Date associated with months
char monthNormal[monthMax] = { // Considering normal year
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        // Jan Feb Mar Apr May Jun July Aug Sep Oct Nov Dec
};
char monthLeap[monthMax] = { // Considering leap year
        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        // Jan Feb Mar Apr May Jun July Aug Sep Oct Nov Dec
};

// MonthCode
char monthCode[monthMax] = {
        0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5
        // Jan Feb Mar Apr May Jun July Aug Sep Oct Nov Dec        
};

// WeekCode
// 0, 1, 2, 3, 4, 5, 6
// Su, Mo, Tu, We, Th, Fr, Sa
char weekName_L1_part1[weekMax] = {
    0xB4, 0x6E, 0x81, 0x6D, 0x81, 0x8C, 0xB4
    // S, M, T, W, T, F, S
};
char weekName_L1_part2[weekMax] = {
    0x44, 0x90, 0x10, 0x28, 0x10, 0x44, 0x44
    // S, M, T, W, T, F, S
};
char weekName_L2_part1[weekMax] = {
    0x38, 0x38, 0x38, 0x18, 0x2C, 0x08, 0x18
    // u, o, u, e, h, r, a
};
char weekName_L2_part2[weekMax] = {
    0x00, 0x44, 0x00, 0x0C, 0x44, 0x44, 0x14
    // u, o, u, e, h, r, a
};



/*		Variables | Display Time	     */

// Arrange the 0 to 86400 time into Hour Min Sec format to display
char timeArr[6] = {
        0,	0, 0, 0, 0, 0
        // H2  H1  M2  M1  S2  S1
};

// HEX printing each number => 14 segment Display
// 16 bit [last bit not used] => 14 segment and extra one for DOT  [1 = active]
// 0xFC00, 0x6000, 0xD844, 0xF044, 0x6444, 0xB444, 0xBC44, 0xE000, 0xFC44, 0xF444

// part1 ==> PORTD
char num_part1[10] = {
    0xFC, 0x60, 0xD8, 0xF0, 0x64, 0xB4, 0xBC, 0xE0, 0xFC, 0xF4
};
// part2 ==> PORTB
char num_part2[10] = {
    0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x44, 0x44
};
char num_part2_dot[10] = {
    0x02, 0x02, 0x46, 0x46, 0x46, 0x46, 0x46, 0x02, 0x46, 0x46
};

// Chossing the right segment to display the variable
// 6x1 display [0 = active]
char seg_pos[6] = {
    0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB
};



/*      Fuctions         */

// Update the time
int updateTime(int timeV){
    if (timeV >= totalSec) {
        return timeV = 0;
    }
    timeV += 1:
    return timeV;
};

// Calculate week name from date
// ref: https://artofmemory.com/blog/how-to-calculate-the-day-of-the-week/
int weekName(int yearV, int monthV, int dayV) {
    // Year Code
    int yearVlast2D;
    yearVlast2D = yearV%100; // take last two digits
    int yearCode = (yearVlast2D + (yearVlast2D / 4)) % 7; // year code

    // Century Code
    int centuryCode;
    if (yearV >= 1700 && yearV < 1800) {
        centuryCode = 4;
    } else if (yearV >= 1800 && yearV < 1900) {
        centuryCode = 2;
    } else if (yearV >= 1900 && yearV < 2000) {
        centuryCode = 0;
    } else if (yearV >= 2000 && yearV < 2100) {
        centuryCode = 6;
    } else if (yearV >= 2100 && yearV < 2200) {
        centuryCode = 4;
    } else if (yearV >= 2200 && yearV < 2300) {
        centuryCode = 2;
    } else if (yearV >= 2300 && yearV < 2400) {
        centuryCode = 0;
    }

    // Leap Year Code
    int leapYearCode;
    if (yearV%4 == 0 && (yearV%100 != 0 || yearV%400 == 0)) {
        leapYearCode = 1;
    } else {
        leapYearCode = 0;
    }
    
    return (yearCode + monthCode[monthV-1] + centuryCode + dayV - leapYearCode) % 7;
};