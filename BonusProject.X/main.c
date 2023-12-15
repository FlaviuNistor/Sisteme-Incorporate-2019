/*
 * File:   main.c
 * Author: Nistor Flaviu
 *
 * Created on December 7, 2023, 4:47 PM
 */

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is RESET)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Global variables declaration: auto initialized to default value by the 
 * IDE which calls the function that moves the values from the .text segment */
char x_start_point = 40;
char y_start_point = 3;
char tree_hight = 15;
char x_text_start = 0;
char y_text_start = 0;
char lights_style[] = "$";
char tree_style[] = "##############################";

/* Use keyword const to reduce the memory size needed for variables */
const char str[] = "HO HO HO, MERRY CHRISTMAS!!!";
/* Let's define some MACRO to make the code easy to be read */
#define gotoxy(x,y)          printf("\033[%d;%dH", (y), (x))
#define clear_screen()       printf("\e[1;1H\e[2J")
#define clear_line()         printf("\33[2K\r")
#define turn_off_cursor()    printf("\e[?25l")
#define turn_on_cursor()     printf("\e[?25lh")
#define set_color_black()    printf("\033[30m")
#define set_color_red()      printf("\033[31m")
#define set_color_green()    printf("\033[32m")
#define set_color_yellow()   printf("\033[33m")
#define set_color_blue()     printf("\033[34m")
#define set_color_magenta()  printf("\033[35m")
#define set_color_cyan()     printf("\033[36m")
#define set_color_white()    printf("\033[37m")


#define NR_OF_LIGHTS  33

/* Init the UART module */
void init_uart(void){
    BRGH = 0;  /* High Speed Mode */
    BRG16 = 1; /* 8bit */
    SPBRG = 25; /* 9600bPS @ 4MHz */
    TXSTA = 0x20; /* Enable Transmit */
    RCSTA |= 0x80;  /* Enable Serial Port */
    RCSTA |= 0x10;  /* Enable Receive */
}

/* Init the GPIOS module */
void init_gpio(void){
    ANSEL = 0x00;
    ANSELH = 0x00;  
    TRISC = 0xF0;
    PORTC = 0x0;
    TRISB &= 0x7F;
    TRISB |= 0x20;
}

/* Redefine the "weak" function putch so the real UART HW is used to print */
void putch(char byte)
{
    while(!TRMT); /* Wait for TX Buffer Empty */
    TXREG = byte;
}

void init_board(void){
    init_uart();
    init_gpio();
}

/* Define and empiric delay function based on the FOSC = 4MHz*/
void delay_ms (int ms){
    unsigned int i,j;
    
    for (i = 0; i < ms; i++){
        for (j=0 ; j<62; j++);
    }
}

/* Generate random colors for our tree lights */
void switch_to_random_color(void){
    /* Color codes for console: Black: 30; Red: 31; Green: 32; Yellow: 33; Blue: 34; Magenta: 35; Cyan: 36; White: 37*/
    int color;
    color = rand();
    /* Now lets do a modulo 7 to get only available colors*/
    color = color % 7;
    switch (color){
        case 0: set_color_yellow(); // if Black, use Yellow
            break;
        case 1: set_color_red(); 
            break;
        case 2: set_color_white(); // if Green, use White
            break;
        case 3: set_color_yellow(); 
            break;
        case 4: set_color_blue(); 
            break;
        case 5: set_color_magenta(); 
            break;
        case 6: set_color_cyan(); 
            break;
        case 7: set_color_white(); 
            break;
        default:
            break;
    }
}

/* Generate a green tree using "#" */
void create_christmas_tree(char x, char y, char hight){
    /*
                        *
                       /0\
                      /000\
                     /00000\
                    /0000000\
                       |||
                       |||
     */
    char x_point = 0;
    char y_point = 0;
    char i, j, offset;
    x_point = x;
    y_point = y;
    clear_screen();
    turn_off_cursor();
    gotoxy(x_point,y_start_point);
    set_color_yellow();
    printf("*");
    set_color_green();
    for (i = (y_point + 1), j = 1; i <= hight; i++, j = j+2){
        delay_ms(100);
        x_point = x_point - 1;
        gotoxy(x_point,i);
        printf("/");
        printf("%.*s", j, tree_style);    /*print n of the string characters*/ 
        printf("\\");
    }
    gotoxy(x_start_point-1, (hight + 1));
    printf("|||");
    gotoxy(x_start_point-1, (hight + 2));
    printf("|||");
    gotoxy(x_start_point-1, (hight + 3));
    printf("|||");
    offset = sizeof(str) / 2;
    gotoxy(x_start_point - offset, (hight + 5));
    set_color_red();
    x_text_start = x_start_point - offset;
    y_text_start = (hight + 5);
    printf("%s", str);
}

/* Generate a green tree using "#" */
void make_christmas_tree_black(char x, char y, char hight){
    char x_point = 0;
    char y_point = 0;
    char i, j;
    x_point = x;
    y_point = y;
    set_color_green();
    for (i = (y_point + 1), j = 1; i <= hight; i++, j = j+2){
        x_point = x_point - 1;
        gotoxy(x_point,i);
        printf("/");
        set_color_black();
        printf("%.*s", j, tree_style);    /*print n of the string characters*/ 
        set_color_green();
        printf("\\");
    }
}

/* Generate some tree lights with random colors */
void color_christmas_tree_lights(){
    char i;
    char color_coordonates[NR_OF_LIGHTS][2] = {{(x_start_point+1), (y_start_point+2)},  \
                                                {(x_start_point-1), (y_start_point+3)}, \
                                                {(x_start_point-2), (y_start_point+4)}, \
                                                {(x_start_point+2), (y_start_point+4)}, \
                                                {(x_start_point),   (y_start_point+5)}, \
                                                {(x_start_point-4), (y_start_point+6)}, \
                                                {(x_start_point+3), (y_start_point+6)}, \
                                                {(x_start_point-1), (y_start_point+6)}, \
                                                {(x_start_point-3), (y_start_point+7)}, \
                                                {(x_start_point+4), (y_start_point+7)}, \
                                                {(x_start_point+7), (y_start_point+8)}, \
                                                {(x_start_point+1), (y_start_point+8)}, \
                                                {(x_start_point-1), (y_start_point+8)}, \
                                                {(x_start_point-5), (y_start_point+8)}, \
                                                {(x_start_point+6), (y_start_point+9)}, \
                                                {(x_start_point),   (y_start_point+9)}, \
                                                {(x_start_point-6), (y_start_point+9)}, \
                                                {(x_start_point-8), (y_start_point+10)}, \
                                                {(x_start_point+7), (y_start_point+10)}, \
                                                {(x_start_point+2), (y_start_point+10)}, \
                                                {(x_start_point-3), (y_start_point+10)}, \
                                                {(x_start_point-10), (y_start_point+11)}, \
                                                {(x_start_point-7), (y_start_point+11)}, \
                                                {(x_start_point-2), (y_start_point+11)}, \
                                                {(x_start_point+3), (y_start_point+11)}, \
                                                {(x_start_point+6), (y_start_point+11)}, \
                                                {(x_start_point+10), (y_start_point+11)}, \
                                                {(x_start_point-9), (y_start_point+12)}, \
                                                {(x_start_point-5), (y_start_point+12)}, \
                                                {(x_start_point-1), (y_start_point+12)}, \
                                                {(x_start_point+1), (y_start_point+12)}, \
                                                {(x_start_point+5), (y_start_point+12)}, \
                                                {(x_start_point+8), (y_start_point+12)}};
    for (i = 0; i < NR_OF_LIGHTS; i++){
        gotoxy(color_coordonates[i][0], color_coordonates[i][1]);
        switch_to_random_color(); 
        printf("%s", lights_style);
    }
}


/* Make the text blink ON/OFF*/
void blink_text(char x, char y){
    /* Make the variable static so it will not lose the value between function calls*/
    static char cycle = 0;
    gotoxy(x, y);
    if (cycle) {
        set_color_red();
        cycle = 0;
    }
    else {
        set_color_black();
        cycle++;
    }
    printf("%s", str);
}

void ask_user_for_style(){
    char response = 0;
    char style =0;
    response  = RCREG;
    gotoxy(0, 0);
    printf("Please chose a number for the Christmas tree style from list: \n\r");
    printf("1: * style \n\r");
    printf("2: & style \n\r");
    printf("3: # style \n\r");
    RCIF = 0; /* clear flag in case a key was pressed before*/
    while (RCIF == 0) continue;
    response  = RCREG;
    delay_ms(200);
    RCIF = 0; /* clear flag*/
    
    if (response == '1' || response == '2' || response == '3'){
        printf("You chose: %c\n\r", response);
        switch (response){
            case '1': style = '*';
                break;
            case '2': style = '&';
                break;
            case '3': style = '#';
                break;
            default: style = '#';
                break;
        }
        memset(tree_style, style , (sizeof(tree_style) - 1));
    }
    else{
        printf("Default character # will be used.\n\r");
    }
    delay_ms(200);
    response = 0;
    printf("Please chose a number for the tree lights style from list:\n\r");
    printf("1: * style \n\r");
    printf("2: $ style \n\r");
    printf("3: o style \n\r");
    while (RCIF == 0) continue;
    response  = RCREG;
    delay_ms(200);
    RCIF = 0; /* clear flag*/
    if (response == '1' || response == '2' || response == '3'){
        printf("You chose: %c", response);
        switch (response){
            case '1': style = '*';
                break;
            case '2': style = '$';
                break;
            case '3': style = 'o';
                break;
            default: style = '#';
                break;
        }
        memset(lights_style, style , (sizeof(lights_style) - 1));
    }
    else{
        printf("Default character $ will be used.\n\r");
    }
    delay_ms(3000);
}

void toggle_leds(void){
    PORTC ^= 0x0F;
}

void main(void) {
    char response = 0;
    char dummy = 0;
    
    init_board();
    /* In case console color is set different by another run */
    clear_screen();
    set_color_white(); 
    
    ask_user_for_style();   
    create_christmas_tree(x_start_point, y_start_point, tree_hight);
    delay_ms(300);
    make_christmas_tree_black(x_start_point, y_start_point, tree_hight);
    
    while (1){
        color_christmas_tree_lights();
        toggle_leds();
        blink_text(x_text_start, y_text_start);
        delay_ms(50); 
    }
}
