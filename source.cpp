// Project: Microcontrolled Takeout Robot

//Function declarations
void cct_init(void);
void delay(int);
void lcdinit(void);
void writecmd(int);
void writedata(char); 
void Return(void);
void write_dataa(unsigned char *disp);
int READ_SWITCHES(void);
int READ_SENSORS(void);
int get_key(void);
int get_sensor(void);
char* codetxt_to_ramtxt(const char* );
int auto_or_manual(void);
int move_or_configure(void);
int grip_or_flip(void);
int autofun(void);
int manual(void);
int move(void);
int configure(void);

//Static variables
static int grip=1;
static int flip=1;
static int cycle_delay=1;

//strings
const code unsigned char Const_Text1[] = "Grad Project";
const code unsigned char Const_Text2[] = "AmmarHameedNinab";
const code unsigned char Const_Text3[] = "I can only lift";
const code unsigned char Const_Text4[] = "weights < 1kg";
const code unsigned char Const_Text5[] = "That's heavy ay!";
const code unsigned char Const_Text6[] = "Piece of cake!";
const code unsigned char Const_Text7[] = "(+) Auto mode";
const code unsigned char Const_Text8[] = "(-) Manual mode";
const code unsigned char Const_Text9[] = "(Set) Settings";
const code unsigned char Const_Text10[] = "Auto mode on...";
const code unsigned char Const_Text11[] = "Manual mode on...";
const code unsigned char Const_Text12[] = "(+) Grip";
const code unsigned char Const_Text13[] = "(-) Flip";
const code unsigned char Const_Text14[] = "Time for 1 cycle";
const code unsigned char Const_Text15[] = " is:";
const code unsigned char Const_Text16[] = "Working";
const code unsigned char Const_Text17[] = "(+) Move";
const code unsigned char Const_Text18[] = "(-) Configure";
const code unsigned char Const_Text19[] = "You're in charge";
const code unsigned char Const_Text20[] = "Done!";
const code unsigned char Const_Text21[] = "0123456789";

/*******************
//Pin description

P1 is data bus
P2.0 is RS
P2.2 is E

*/
// ***********************************************************

// Main program

int main(void)
{
    int level=1;      //Initialize the first case
    cct_init();       // Make input and output pins as required
    lcdinit();        // Initilize LCD
    write_dataa(codetxt_to_ramtxt(Const_Text1));    //Intro text
    writecmd(0xC0);
    write_dataa(codetxt_to_ramtxt(Const_Text2));
    delay_ms(2000);
    while(1)                //Infinite loop
    {
        switch(level)       //Switch between modes
        {
            case 1: //auto or manual 
            {
                level=auto_or_manual();
            }
            break;
            case 2: //auto 
            {
                level=autofun();
            }
            break;
            case 3: //manual 
            {
                level=manual();     //manual function 
            }
            break;
            case 4:     //move or configure 
            {
                level=move_or_configure();
            }
            break;
            case 5: //move 
            {
                level=move();   //function of move
            }
            break;
            case 6: //configure 
            {
                 level=configure();     //function of configure
            }
            break;
            case 7: //grip or flip 
            {
                 level=grip_or_flip();
            }
            break;
        }
    }
}

void cct_init(void) {
    P0 = 0x00;      // for buzzer
    P1 = 0x00;      //Used as data port for LCD
    P2 = 0x00;      //Output port and used for RS and E
    P3 = 0xf0;      //Input port
    P2_4_bit=0;     //initiallize state of robot
    P2_5_bit=0;
}

char* codetxt_to_ramtxt(const char* ctxt) {     //Send text stored as code
    static char txt[20];                        //in the ROM to the LCD
    char i;
    for(i =0; txt[i] = ctxt[i]; i++);
    return txt; 
}

void delay(int a) {
    int i;
    for(i=0;i<a;i++);       //nulll statement
}

void cycle(int b) {         //used for cycle delays
    int i;
    for(i=0;i<b;i++)
        delay_ms(1000);
}

void writedata(char t) {        // sending data to the LCD
    P2_0_bit = 1;               // This is data
    P1 = t;                     // Data transfer
    P2_2_bit = 1;               // => E = 1
    delay(150);
    P2_2_bit = 0;               // => E = 0
    delay(150);
}

void write_dataa(unsigned char *disp) {                    // function to send string to LCD
    int x;
    for(x=0;disp[x]!=0;x++)
    {
        writedata(disp[x]);
    }
}


void writecmd(int z) {              //sending commands to LCD
    P2_0_bit = 0;       // This is command
    P1 = z;             //Data transfer
    P2_2_bit  = 1;      // => E = 1 
    delay(150); 
    P2_2_bit = 0;       // => E = 0
    delay(150); 
}

void lcdinit(void) {
///////////// Reset process from datasheet ///////// 
    delay(15000);
    writecmd(0x30);
    delay(4500);
    writecmd(0x30);
    delay(300);
    writecmd(0x30);
    delay(650);
/////////////////////////////////////////////////////
    writecmd(0x38);     //function set
    writecmd(0x0c);     //display on,cursor off,blink off
    writecmd(0x01);     //clear display
    writecmd(0x06);     //entry mode, set increment
}

int READ_SWITCHES(void) //Get state of push buttons 
{
    if(P3_4_bit == 0){
        while(P3_4_bit == 0);      //loop until debounce
        return 1;
    }
    if(P3_5_bit == 0){
        while(P3_5_bit == 0);
        return 2;
    }
    if(P3_6_bit == 0){
        while(P3_6_bit == 0);
        return 3;
    }
    if(P3_7_bit == 0){
        while(P3_7_bit == 0);
        return 4;
    }
    return 111;         // Means no key has been pressed
}

int READ_SENSORS(void) {    //Read sensors and send outputs accordingly
    if (P3_0_bit == 1) {    //photosensor indicates the existance of an
        cycle(cycle_delay); //object
        P2_4_bit=1;         //Grip
        cycle(1);
        if(P3_1_bit == 1) { //Strain gauges signal an overweight case
            P2_4_bit=0;     //Lose Grip
            cycle(1);
            return 1;       //return to the auto function
        }
        writecmd(0x01);
        write_dataa(codetxt_to_ramtxt(Const_Text16)); //"working"
        P2_5_bit=1;                         // Flip
        cycle(cycle_delay);
        P2_4_bit=0;                         //Lose Grip
        cycle((cycle_delay)/2);
        P2_5_bit=0;                         //Flip back
        return 2;                           //return to the auto function
    }
    if(P3_4_bit == 0) {                     //if 'set' is pressed
        while(P3_4_bit == 0);
        return 3;                           //go back to main menu
    }
    return 222;                             //by default
}

int get_key(void) {                 //get key from user
    int key = 111;                  //assume no key pressed
    while(key==111)                 //wait untill a key is pressed
        key = READ_SWITCHES();      //scan the keys again and again
    return key;
}

int get_sensor(void) {              //get key from user
    int sensor = 222;               //assume no sensor is triggered
    while(sensor==222)              //wait untill a key is pressed
        sensor = READ_SENSORS();    //scan the sensors again and again
    return sensor; 
}

int auto_or_manual(void) {
    int key;
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text7));
    writecmd(0xc0);
    write_dataa(codetxt_to_ramtxt(Const_Text8));
    key=get_key();
    switch(key)
    {
    case 1:
        return 1;
    break;
    case 2:
        return 2;
    break;
    case 3:
        return 3;
    break;
    }
}

int move_or_configure(void) {
    int key;
    writecmd(0x01); 
    write_dataa(codetxt_to_ramtxt(Const_Text17)); 
    writecmd(0xc0); 
    write_dataa(codetxt_to_ramtxt(Const_Text18)); 
    key=get_key();
    switch(key)
    {
    case 1: 
        return 1;
    break; 
    case 2:
        return 5; 
    break;
    case 3: 
        return 6;
    break;
    }
}

int grip_or_flip(void) {
    int key;
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text12));
    writecmd(0xc0);
    write_dataa(codetxt_to_ramtxt(Const_Text13));
    while(1)
    {
        key=get_key();
        switch(key)
        {
        case 1:
            return 1;
        break;
        case 2:
        {
            if(grip==1)         //the idea is to negate the state
            {
                p2_4_bit=1;
            grip=0;
            }
            else
            {
                p2_4_bit=0;
                grip=1;
            }
        }
        break;
        case 3:
        {
            if(flip==1)
            {
                p2_5_bit=1;
                flip=0;
            }
            else
            {
                p2_5_bit=0;
                flip=1;
            }
        }
        break;
        }
    }
}

int autofun(void)
{
    int sensor;
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text10)); 
    cycle(1);
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text3));
    writecmd(0xc0);
    write_dataa(codetxt_to_ramtxt(Const_Text4)); 
    cycle(2);
    while(1)
    {                        
        sensor = get_sensor(); 
        switch(sensor) {                //strain gauge
            case 1: 
            {
                 writecmd(0x01);
                 write_dataa(codetxt_to_ramtxt(Const_Text5));
            } 
            break; 
            case 2: 
            {
                 writecmd(0x01);
                 write_dataa(codetxt_to_ramtxt(Const_Text6));
            } 
            break; 
            case 3:
                return 1; 
            break;
        }
    }
}

int manual(void)
{
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text11)); 
    cycle(1);
    return 4; 
}

int move(void) {
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text19));
    cycle(1);
    return 7;
}

int configure(void)
{
    int key;
    writecmd(0x01);
    write_dataa(codetxt_to_ramtxt(Const_Text14));
    writecmd(0xc0);
    write_dataa(codetxt_to_ramtxt(Const_Text15));
    writecmd(0xc8);
    writedata(Const_Text21[cycle_delay]); //display the delay in the second row 
    while(1)
    {
        key=get_key(); 
        switch(key)
        {
            case 1: 
                return 1;
            break; 
            case 2:
                cycle_delay++;          //increase delay
                if(cycle_delay==10)     //if max value reached
                    cycle_delay=1;      //loop back
            break; 
            case 3:
                if(cycle_delay==1)      //if min value reached
                    cycle_delay=10;     //loop back
                cycle_delay--;          //decrease delay
            break;
            case 4: 
                writecmd(0x01);
                write_dataa(codetxt_to_ramtxt(Const_Text20));   //"Done"
                cycle(1);
                return 1; 
            break;
        }
        writecmd(0xc8);
        writedata(Const_Text21[cycle_delay]);
    }
}