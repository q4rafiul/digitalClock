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

/*        Library Blocks for the Chipset and others    */

#include <mega324.h>
#define F_CPU 8000000UL // 8Mhz Speed
#include <delay.h>
#include <stdbool.h>




/*        Variables | Store Time    */

// Time Variables
int timeArray[3] = {
    0, 0, 0
};
bool timeAM;
bool timeActive = true;

// Date Variables
int yearV = 2023;
int leapYearCode = 0;
int monthV = 1; // monthV -> 1 to 12
int dayV = 1;

int monthMax = 12;
int weekMax = 7;

// StopWatch Variables
int stopWArray[3] = {
    0, 0, 0
};
bool stopWActive = false;

// Button Value
int buttonArray[4] = {
    0, 0, 0
    // Edit State Change
};
// State = 0 -> time, 1 -> stopW, 2 -> timer and 3 -> alarm
// Edit = 0 and 1
// Change = 0 and 1


// Display Variables
int segP;


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




/*        Numbers & Letters | Display         */

// 16 bit [last bit not used] => 14 segment and extra one for DOT  [1 = active]
// 0xFC00, 0x6000, 0xD844, 0xF044, 0x6444, 0xB444, 0xBC44, 0xE000, 0xFC44, 0xF444
char num_part1[10] = {
    0xFC, 0x60, 0xD8, 0xF0, 0x64, 0xB4, 0xBC, 0xE0, 0xFC, 0xF4
};
char num_part2[10] = {
    0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x44, 0x44
};
char num_part2_dot[10] = {
    0x02, 0x02, 0x46, 0x46, 0x46, 0x46, 0x46, 0x02, 0x46, 0x46
};

// WeekCode => 0, 1, 2, 3, 4, 5, 6
char weekName_Big_part1[weekMax] = {
    0xB4, 0x6E, 0x81, 0x6D, 0x81, 0x8C, 0xB4
    // S, M, T, W, T, F, S
};
char weekName_Big_part2[weekMax] = {
    0x44, 0x90, 0x10, 0x28, 0x10, 0x44, 0x44
    // S, M, T, W, T, F, S
};
char weekName_Small_part1[weekMax] = {
    0x38, 0x38, 0x38, 0x18, 0x2C, 0x08, 0x18
    // u, o, u, e, h, r, a
};
char weekName_Small_part2[weekMax] = {
    0x00, 0x44, 0x00, 0x0C, 0x44, 0x44, 0x14
    // u, o, u, e, h, r, a
};

// Chossing the right segment to display the variable
// 6x1 display [0 = active]
char seg_pos[6] = {
    0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB
};




/*      Numbers & Letters | Display      */

// DateDisplay Arrays
char dateDisplay_part1[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
char dateDisplay_part2[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// TimeDisplay Arrays
char timeDisplay_part1[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
char timeDisplay_part2[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// StopWatchDisplay Arrays
char stopWDisplay_part1[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
char stopWDisplay_part2[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




/*      Fuctions         */

// Week Name Value Calculator [range 0 to 6]
// formula ref: https://artofmemory.com/blog/how-to-calculate-the-day-of-the-week/
int weekValueFunction() {
    // Year Code
    int yearVlast2D, yearCode, centuryCode;
    yearVlast2D = yearV%100; // take last two digits
    yearCode = (yearVlast2D + (yearVlast2D / 4)) % 7; // year code

    // Century Code
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

    return (yearCode + monthCode[monthV-1] + centuryCode + dayV - leapYearCode) % 7;
};


// Hour, Minute and Second Update
void hmsUpdate(int *array) {
    // if second crosses 59
    if (array[2] > 59) {
        array[2] = 0;
        array[1] += 1;
    }
    // if minute crosses 59
    if (array[1] > 59) {
        array[1] = 0;
        array[0] += 1;
    }
    // if hour crosses 23
    if (array[1] > 23) {
        array[1] = 0;
    }
    
    // For just switching case
    // hour reverses
    if (array[0] < 0) {
        array[0] = 23;
    }
    // second reverses
    if (array[2] < 0) {
        array[2] = 59;
    }
    // second reverses
    if (array[1] < 0) {
        array[1] = 59;
    }
};


// Time Update
void timeUpdate() {        
    hmsUpdate(timeArray);

    // if hour crosses 23
    if (timeArray[0] > 23) {
        timeArray[0] = 0;
        dayV += 1;
    }

    // Leap Year Code
    if (yearV%400 == 0 && yearV%100 == 0) {
        leapYearCode = 1;
    } else if (yearV%4 == 0 &&  yearV%100 != 0) {
        leapYearCode = 1;
    } else {
        leapYearCode = 0;
    }

    // if day crosses current months last day
    if (leapYearCode == 1 && monthV == 2) {
        if (dayV > monthNormal[monthV-1]) {
            dayV = 0;
            monthV += 1;
        }
    } else {
        if (dayV > monthLeap[monthV-1]) {
            dayV = 0;
            monthV += 1;
        }
    }
    // if month crosses 12
    if (monthV > 12) {
        monthV = 0;
        yearV += 1;
    }
};


// Arrange Date for Display
void timeArrangeFunction(char *p1, char *p2) {
    int timeHset;
    if (timeArray[0] >= 12) {
        timeAM = false;
        if (timeArray[0] > 12) {
            timeHset = timeArray[0] - 12;
        } else {
            timeHset = timeArray[0];
        }
    } else {
        timeAM = true;
        if (timeArray[0] == 0) {
            timeHset = timeArray[0]+12;    
        } else {
            timeHset = timeArray[0];
        }
    }

    p1[0] = num_part1[timeHset/10];
    p2[0] = num_part2[timeHset/10];
    
    p1[1] = num_part1[timeHset%10];
    p2[1] = num_part2_dot[timeHset%10];

    p1[2] = num_part1[timeArray[1]/10];
    p2[2] = num_part2[timeArray[1]/10];
    
    p1[3] = num_part1[timeArray[1]%10];
    p2[3] = num_part2_dot[timeArray[1]%10];

    if (timeAM) {
        // letter A
        p1[4] = 0xEC;
        p2[4] = 0x44;
    } else {
        // letter P
        p1[4] = 0xCC;
        p2[4] = 0x44;
    }
    // letter M
    p1[5] = 0x6E;
    p2[5] = 0x90;
};


// Arrange Date for Display
void dateArrangeFunction(char *p1, char *p2) {
    p1[0] = num_part1[dayV/10];
    p2[0] = num_part2[dayV/10];
    
    p1[1] = num_part1[dayV%10];
    p2[1] = num_part2_dot[dayV%10];

    p1[2] = num_part1[monthV/10];
    p2[2] = num_part2[monthV/10];
    
    p1[3] = num_part1[monthV%10];
    p2[3] = num_part2_dot[monthV%10];

    p1[4] = weekName_Big_part1[weekValueFunction()];
    p2[4] = weekName_Big_part2[weekValueFunction()];
    
    p1[5] = weekName_Small_part1[weekValueFunction()];
    p2[5] = weekName_Small_part2[weekValueFunction()];
};


// Arrange Date for Display
void stopWArrangeFunction(char *p1, char *p2) {
    p1[0] = num_part1[stopWArray[0]/10];
    p2[0] = num_part2[stopWArray[0]/10];
    
    p1[1] = num_part1[stopWArray[0]%10];
    p2[1] = num_part2_dot[stopWArray[0]%10];

    p1[2] = num_part1[stopWArray[1]/10];
    p2[2] = num_part2[stopWArray[1]/10];
    
    p1[3] = num_part1[stopWArray[1]%10];
    p2[3] = num_part2_dot[stopWArray[1]%10];

    p1[4] = num_part1[stopWArray[2]/10];
    p2[4] = num_part2[stopWArray[2]/10];
    
    p1[5] = num_part1[stopWArray[2]%10];
    p2[5] = num_part2[stopWArray[2]%10];
};


// Display of all type
void display(char *p1, char *p2) {
    for (segP = 0; segP < 6; segP++) {
        // Selecting the Segment to display the value
        PORTA = seg_pos[segP];

        PORTD = p1[segP];
        PORTB = p2[segP];
        delay_ms(3);
    }
};




/*      Main Loop       */

void main(void)
{
    // R/W = 0/1 initial -> 0
    DDRA = 0xFF; // First 6 bit as output
    DDRB = 0xFF; // First 7 bit as output
    DDRC = 0x00; // First 4 bit as input
    DDRD = 0xFF; // All bit as output

    while(1) {
        /* Action Buttons */
        
        // CHANGE Button
        if ((PINC & (1<<4)) && !(buttonArray[1] == 0 && buttonArray[0] == 0)) {
            buttonArray[2] += 1;
            if (buttonArray[2] > 1) {
                buttonArray[2] = 0;
            }
        }
        // EDIT Button
        if (PINC & (1<<7)) {
            buttonArray[0] += 1;
            if (buttonArray[0] > 1) {
                buttonArray[0] = 0;
            }
        }
        // UP Button
        if (buttonArray[0] ==0 && (PINC & (1<<6))) {
            buttonArray[1] += 1;
            buttonArray[0] = 0; // reset edit button value to 0
            buttonArray[2] = 0; // reset change button value to 0
        }
        // DOWN Button
        if (buttonArray[0] ==0 && (PINC & (1<<5))) {
            buttonArray[1] -= 1;
            buttonArray[0] = 0; // reset edit button value to 0
            buttonArray[2] = 0; // reset change button value to 0
        }
        if (buttonArray[1] > 3) {
            buttonArray[1] = 0;
        } else if (buttonArray[1] < 0) {
            buttonArray[1] = 3;
        }


        /* Conditions */
        // Always update time just off when it's in edit mode
        if (timeActive) {
            timeArray[1] += 1;
            timeUpdate();
        }


        /* Features */

        // Time's Feature
        if (buttonArray[1] == 0 && buttonArray[0] == 0) {
            timeActive = true;

            if (PINC & (1<<4)) {
                dateArrangeFunction(dateDisplay_part1, dateDisplay_part2);
                display(dateDisplay_part1, dateDisplay_part2);
            } else {
                timeArrangeFunction(timeDisplay_part1, timeDisplay_part2);
                display(timeDisplay_part1, timeDisplay_part2);
            }
        } else if (buttonArray[1] == 0 && buttonArray[0] == 1) {
            timeActive = false;

            // minute change
            if (buttonArray[2] == 0) {
                if (PINC & (1<<6)) {
                    timeArray[1] += 1;
                }
                if (PINC & (1<<5)) {
                    timeArray[1] -= 1;
                }
            }
            // hour change
            if (buttonArray[2] == 1) {
                if (PINC & (1<<6)) {
                    timeArray[0] += 1;
                }
                if (PINC & (1<<5)) {
                    timeArray[0] -= 1;
                }
            }
            hmsUpdate(timeArray);
            timeArrangeFunction(timeDisplay_part1, timeDisplay_part2);
            display(timeDisplay_part1, timeDisplay_part2);
        }


        // StopWatch Feature
        if (buttonArray[1] == 1) {
            // Start and Stop
            if (buttonArray[2] == 0) {
                stopWActive = false;
            } else if (buttonArray[2] == 1) {
                stopWActive = true;
            }

            // Reset
            if (buttonArray[0] == 1) {
                stopWActive = false;
                stopWArray[0] = 0;
                stopWArray[1] = 0;
                stopWArray[2] = 0;
            }

            if (stopWActive) {
                stopWArray[2] += 1;
                hmsUpdate(stopWArray);
                stopWArrangeFunction(stopWDisplay_part1, stopWDisplay_part2);
                display(stopWDisplay_part1, stopWDisplay_part2);
            } else {
                stopWArrangeFunction(stopWDisplay_part1, stopWDisplay_part2);
                display(stopWDisplay_part1, stopWDisplay_part2);
            }
        }
    }
};