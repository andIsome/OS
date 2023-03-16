#include <stdarg.h>
#include <stdint.h>


#include "idt.h"
#include "pic.h"
#include "io_operations.h"

#include "memory.h"

#include "bitmapfont.h"

#define INTERRUPT_ATTRB __attribute__((interrupt))
#define PACK_ATTRB      __attribute__((packed))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define RUN_IN_QEMU 1
#define RUN_IN_VIRTUALBOX 0

#define RUN_STANDALONE (!RUN_IN_QEMU && !RUN_IN_VIRTUALBOX)

// from boot.asm
extern "C" {
    uint8_t BOOT_DISK;
    uint32_t CODE_SEGMENT, DATA_SEGMENT;
}

void shutdown(){
#if RUN_IN_VIRTUALBOX
outw(0x4004, 0x3400); // Virtual box
#elif RUN_IN_QEMU
outw(0x604, 0x2000); // QEMU
#else
#pragma message "WARNING NOT IMPLEMENTED"
#endif
}

int numberOfSetBits(uint32_t i){
     i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
     i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
     return (i * 0x01010101) >> 24;          // horizontal sum of bytes
}

void print_int_to_func(int val, void(prntstr)(const char*)){
    char buff[12];
    int size = sizeof(buff)-1;
    bool negative = false;

    if(val==0){return prntstr("0");}

    buff[size--] = '\0';

    if(val < 0) {
        negative = true;
        val*=-1;
    }

    while(val!=0){
        buff[size--] = (val % 10) + '0';
        val /= 10;
    }

    if(negative){
        buff[size--] = '-';
    }
    prntstr(buff + size + 1);
}

// Required buff size 11
void print_int_16_to_buff(uint32_t val, char* buff){
    const char* base16 = "0123456789ABCDEF";

    *buff++ = '0';
    *buff++ = 'x';

    int c = 7;
    while(val!=0){
        uint32_t digit = val % 0x10;
        val /= 0x10;
        *(buff+c--) = base16[digit];
    }

    while(c>=0){
        *(buff+c--) = '0';
    }

    *(buff+8) = '\0';
}

// Required buff size 12
int print_int_to_buff(int val, char* buff){
    int size = sizeof(buff)-1;
    bool negative = false;

    buff[size--] = '\0';

    if(val==0){
        buff[size--] = '0';
        return size + 1;
    }

    

    if(val < 0) {
        negative = true;
        val*=-1;
    }

    while(val!=0){
        buff[size--] = (val % 10) + '0';
        val /= 10;
    }

    if(negative){
        buff[size--] = '-';
    }
    return size + 1;
}

// Has to be macro. It requires a immediate value
#define TRIGGER_INTERRUPT(int_value) asm volatile("int %0" :: "i" (int_value))






#define SCANCODE_KEY_RELEASE_BIT 0x80

#define SCANCODE_KEY_ESC          01

#define SCANCODE_KEY_1            02
#define SCANCODE_KEY_2            03
#define SCANCODE_KEY_3            04
#define SCANCODE_KEY_4            05
#define SCANCODE_KEY_5            06
#define SCANCODE_KEY_6            07
#define SCANCODE_KEY_7            08
#define SCANCODE_KEY_8            09
#define SCANCODE_KEY_9            10
#define SCANCODE_KEY_0            11

#define SCANCODE_KEY_A            30
#define SCANCODE_KEY_D            32

#define SCANCODE_KEY_BACKSPACE    14

#define SCANCODE_KEY_ENTER        28
#define SCANCODE_KEY_SPACE        57
#define SCANCODE_KEY_CAPS_LOCK    58
#define SCANCODE_KEY_F1           59
#define SCANCODE_KEY_F2           60
#define SCANCODE_KEY_F3           61
#define SCANCODE_KEY_F4           62
#define SCANCODE_KEY_F5           63
#define SCANCODE_KEY_F6           64
#define SCANCODE_KEY_F7           65
#define SCANCODE_KEY_F8           66
#define SCANCODE_KEY_F9           67
#define SCANCODE_KEY_F10          68
#define SCANCODE_KEY_F11          87
#define SCANCODE_KEY_F12          88

#define SCANCODE_KEY_ARROW_UP     72
#define SCANCODE_KEY_ARROW_LEFT   75
#define SCANCODE_KEY_ARROW_RIGHT  77
#define SCANCODE_KEY_ARROW_DOWN   80





const uint32_t SCREEN_WIDTH  = 320;
const uint32_t SCREEN_HEIGHT = 200;

#define PIXEL_OFFSET(x,y) ((uint8_t*)(0xA0000 + (x)+(y)*(SCREEN_WIDTH)))

const int FONT_SIZE = 8;





#define COLOR_BLACK          0
#define COLOR_BLUE           1
#define COLOR_GREEN          2
#define COLOR_CYAN           3
#define COLOR_RED            4
#define COLOR_MAGENTA        5
#define COLOR_BROWN          6
#define COLOR_LIGHT_GRAY     7
#define COLOR_DARK_GRAY      8
#define COLOR_LIGHT_BLUE     9
#define COLOR_LIGHT_GREEN   10
#define COLOR_LIGHT_CYAN    11
#define COLOR_LIGHT_RED     12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW        14
#define COLOR_WHITE         15
// Grayscale 16-32


void writeChar(int xpos, int ypos, char c, uint8_t color){
    for(int y=0;y<FONT_SIZE;y++){
        uint8_t line = font8x8_basic[c][y];
        uint8_t it = 1;
        for(int x=0;x<FONT_SIZE;x++){
            if(line & it){
                *PIXEL_OFFSET(xpos+x,ypos+y) = color;
            }
            it <<= 1;
        }
    }
}

void writeString(int xpos, int ypos, const char* str, uint8_t color){
    int c=0;
    while(*str!=0){
        writeChar(xpos+c+1, ypos+1, *str, color+4); // Shadow with offset
        writeChar(xpos+c, ypos, *str, color);

        c += FONT_SIZE;
        str++;
    }
}

void writeInt(int xpos, int ypos, int val, uint8_t color, bool hex){
    char buff[12];
    int off;
    if(hex){
        off = 0;
        print_int_16_to_buff(val, buff);
    }else{
        off = print_int_to_buff(val, buff);
    }
    writeString(xpos, ypos, buff+off, color);
}







void dbg_putc(char c){
#if RUN_STANDALONE
    // No debug output
#else
    outb(0xE9, c);
#endif
}

void dbg_string(const char* str){
    char c;
    while(1){
        c = *str++;
        if(!c) break;
        dbg_putc(c);
    }
}

void dbg_int(int val){
    print_int_to_func(val, dbg_string);
}








uint32_t rand() { // RAND_MAX assumed to be 32767
    static uint32_t next = __LINE__;
    next = next * 1103515245 + 12345;
    return (uint32_t) (next / 65536) % 32768;
}


const int32_t NUM_TILES_PER_ROW = 10;
const int32_t NUM_ROWS = (SCREEN_HEIGHT/TETRAMINO_DIM+1);
const int32_t GAME_NUM_TILES = NUM_ROWS * NUM_TILES_PER_ROW;

// Grid where each tiles value is a index into a color palette. 0 means empty
#define EMPTY_SLOT 0
uint8_t gamestate[GAME_NUM_TILES];
#define GAME_MAP_INDEX(x,y) ((x)+(y)*NUM_TILES_PER_ROW)


typedef uint16_t tetramino_layout_t;

// Tetraminos are defined by a grid of 4 4bit rows
// The following 0x6440 represents a upsidedown L:
// _________________
// | 0 | 1 | 1 | 0 |
// | 0 | 1 | 0 | 0 |
// | 0 | 1 | 0 | 0 |
// | 0 | 0 | 0 | 0 |
// -----------------

// The array is divided into groups of 4. One for each clockwise rotation
const tetramino_layout_t tetraminos[]       = {
    0x0F00,0x2222,0x00F0,0x4444, 0x8E00,0x6440,0x0E20,0x44C0, 0x2E00,0x4460,0x0E80,0xC440, 0x6600,0x6600,0x6600,0x6600,
    0x6C00,0x4620,0x06C0,0x8C40, 0x4E00,0x4640,0x0E40,0x4C40, 0xC600,0x2640,0x0C60,0x4C80
};


int32_t level = 0;
int32_t score = 0;
int32_t currentTetraminoX=0, currentTetraminoY=0;
tetramino_layout_t currentTetraminoLayout = 0x0;
uint8_t currentTetraminoColorIndex = 2;

const int32_t TETRAMINO_DIM = 10;
const int32_t MARGIN_LEFT = (SCREEN_WIDTH-(10+2)*TETRAMINO_DIM)/2;

typedef struct{
    uint8_t main, light_shade, dark_shade;
} tetramino_color;

tetramino_color tetramino_color_palette[] = {
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}, // 'Invalid' color
    {COLOR_DARK_GRAY, COLOR_LIGHT_GRAY, 18}, // Borders of game

    // Acutal palette
    {COLOR_BLUE, COLOR_CYAN, COLOR_DARK_GRAY},
    {COLOR_GREEN, COLOR_LIGHT_GREEN, COLOR_LIGHT_GRAY},
    {COLOR_RED, COLOR_LIGHT_RED, COLOR_LIGHT_GRAY},
    {COLOR_WHITE+16+9, COLOR_WHITE+16+9+(uint8_t)16+(uint8_t)8, COLOR_BROWN}
};







// Interrupt with error code:
// void INTERRUPT_ATTRB handle_interrupt_N(struct interrupt_frame *frame, uint32_t ec)
// Interrupt with no error code
// void INTERRUPT_ATTRB handle_interrupt_N(struct interrupt_frame *frame)

struct interrupt_frame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
} PACK_ATTRB;

// Double Fault. When triggered abort execution
INTERRUPT_ATTRB
void handle_int_8(struct interrupt_frame *frame, uint32_t ec){
    dbg_string("\n\nDOUBLE FAULT!!!\n\tError code: ");
    dbg_int(ec);
    dbg_putc('\n');
    shutdown();

    disable_interrupts();
    asm volatile("hlt");
}


// IO Ports for Keyboard
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64


void drawPlayingField();
void moveTetraminoDown(bool soft);
void newRandomTetramino();
void rotateTetramino(bool right);
void moveSidewaysTetramino(bool right);
void resetGame();

INTERRUPT_ATTRB
void handle_int_keyboard(struct interrupt_frame* frame){

    pic_send_eoi(1);

    // Read scancode
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if(scancode == (SCANCODE_KEY_RELEASE_BIT | SCANCODE_KEY_ESC)){
        shutdown();
    }else if(scancode == SCANCODE_KEY_ARROW_DOWN){
        moveTetraminoDown(true);
        drawPlayingField();
    }else if(scancode == SCANCODE_KEY_ARROW_LEFT){
        rotateTetramino(false);
        drawPlayingField();
    }else if(scancode == SCANCODE_KEY_ARROW_RIGHT){
        rotateTetramino(true);
        drawPlayingField();
    }else if(scancode == SCANCODE_KEY_A){
        moveSidewaysTetramino(false);
        drawPlayingField();
    }else if(scancode == SCANCODE_KEY_D){
        moveSidewaysTetramino(true);
        drawPlayingField();
    }else if(scancode == SCANCODE_KEY_F2){
        resetGame();
        drawPlayingField();
    }
}




int removeFullLines(int acc){
    for(int y=NUM_ROWS-1;y>=0;y--){
        
        // Search for empty positions
        for(int x=0;x<NUM_TILES_PER_ROW;x++){
            if(gamestate[GAME_MAP_INDEX(x,y)] == EMPTY_SLOT){
                goto end; // Not empty so check next line
            }
        }

        // No empty positions, so remove row
        for(int x=0;x<NUM_TILES_PER_ROW;x++){
            gamestate[GAME_MAP_INDEX(x,y)] = EMPTY_SLOT;
        }

        // Move remaining lines one down
        for(;y>=1;y--){
            memcpy(&gamestate[GAME_MAP_INDEX(0,y)], &gamestate[GAME_MAP_INDEX(0,y-1)], NUM_TILES_PER_ROW);
        }

        // Recursive call to find other lines that are full
        // because there can be multiple
        return removeFullLines(acc+1);

        end: continue;
    }
    return acc;
}

bool validTetraminoPlacement(tetramino_layout_t tetramino, int x, int y){
    int mask = 0x8000;

    for(int yp = y;yp<y+4;yp++){
        for(int xp = x;xp<x+4;xp++){

            if(tetramino & mask){
                if(!(0<=xp && xp<NUM_TILES_PER_ROW)|| // Bounds checking
                        !(0<=yp && yp<NUM_ROWS) ||
                        gamestate[GAME_MAP_INDEX(xp, yp)]!=EMPTY_SLOT){ // Check if it is empty tile
                    return false;
                }
            }
            mask = mask >> 1;

        }
    }
    return true;
}

void setTetraminoInPlace(){
    uint16_t mask = 0x8000;

    for(int i=0;i<16;i++){
        if(mask & currentTetraminoLayout){
            int32_t index = GAME_MAP_INDEX(currentTetraminoX+i%4, currentTetraminoY+i/4);
            gamestate[index] = currentTetraminoColorIndex;
        }
        mask = mask >> 1;
    }

    int linesCleared = removeFullLines(0);
    newRandomTetramino();

    // Set max of 4 lines to not overflow
    linesCleared = min(linesCleared, 4);
    // Original Nintendo Scoring System
    const int multiplier[] = {0, 40, 100, 300, 1200};
    score += (level+1) * multiplier[linesCleared];

    if(!validTetraminoPlacement(currentTetraminoLayout, currentTetraminoX, currentTetraminoY)){
        resetGame();
    }
}

void moveTetraminoDown(bool addScore){
    bool tetraminoReachedBottom = true;

    // Check if it can move down
    if(validTetraminoPlacement(currentTetraminoLayout, currentTetraminoX, currentTetraminoY+1)){
        currentTetraminoY++;
        // Check if after moving down has reached the bottom
        if(validTetraminoPlacement(currentTetraminoLayout, currentTetraminoX, currentTetraminoY+1)){
            tetraminoReachedBottom = false;
        }
    }

    if(tetraminoReachedBottom){
        if(addScore){
            // Number of tiles in tetramino
            score += numberOfSetBits(currentTetraminoLayout);
        }
        setTetraminoInPlace();
    }
}

void moveSidewaysTetramino(bool right){
    int32_t dir = right ? 1 : -1;

    if(validTetraminoPlacement(currentTetraminoLayout, currentTetraminoX+dir, currentTetraminoY)){
        currentTetraminoX += dir;
    }
}

void rotateTetramino(bool right){
    int index = -1;
    // Get the index at what position the tetramino is in the array
    for(int i=0;i<sizeof(tetraminos)/sizeof(uint16_t);i++){
        if(tetraminos[i]==currentTetraminoLayout){
            index = i;
            break;
        }
    }

    // Error
    if(index==-1) return;

    uint16_t newTetramino;

    if(right){
        index++;

        if(index%4==0) // Wrap around
            index-=4;

        newTetramino = tetraminos[index];
    }else{
        if(index%4==0){
            index+=3;
        }else{
            index--;
        }
        newTetramino = tetraminos[index];
    }
    if(validTetraminoPlacement(newTetramino, currentTetraminoX, currentTetraminoY)){
        currentTetraminoLayout = newTetramino;
    }
}

void newRandomTetramino(){
    uint32_t randIndex = rand()%(sizeof(tetraminos)/sizeof(uint16_t));

    currentTetraminoY = 0;
    currentTetraminoX = rand()%(NUM_TILES_PER_ROW-4);
    currentTetraminoLayout = tetraminos[randIndex];
    currentTetraminoColorIndex = rand() % (sizeof(tetramino_color_palette)/sizeof(tetramino_color)-2) + 2;
}

void resetGame(){
    score = 0; level = 0;
    memset(gamestate, EMPTY_SLOT, GAME_NUM_TILES);

    drawGameBorders();
    newRandomTetramino();
}




void drawTile(int x, int y, tetramino_color color){
    for(int i=0;i<TETRAMINO_DIM;i++){

        const int yOff = i+y;
        if(!(0 <= yOff && yOff <= SCREEN_HEIGHT)) continue;

        for(int j=0;j<TETRAMINO_DIM;j++){
            const int xOff = x+j;
            
            if(!(0 <= xOff && xOff <= SCREEN_WIDTH )) continue; // Check bounds
            

            if(i==0 || j==0){ // First row/column use light shade color
                *PIXEL_OFFSET(xOff, yOff) = color.light_shade;
            }
            // Last row/column use dark shade color
            else if(i==TETRAMINO_DIM-1 || j == TETRAMINO_DIM-1){
                *PIXEL_OFFSET(xOff, yOff) = color.dark_shade;
            }
            else{ // Use main color
                *PIXEL_OFFSET(xOff, yOff) = color.main;
            }
        }
    }
}

void drawGameBorders(){
    for(int i=1;i<SCREEN_HEIGHT/TETRAMINO_DIM+2;i++){
        drawTile(
            MARGIN_LEFT,
            SCREEN_HEIGHT-i*TETRAMINO_DIM,
            tetramino_color_palette[1]);
        drawTile(
            SCREEN_WIDTH-MARGIN_LEFT-TETRAMINO_DIM,
            SCREEN_HEIGHT-i*TETRAMINO_DIM,
            tetramino_color_palette[1]);
    }
}

void drawPlayingField(){
    // Draw the playing field
    for(int y=0;y<SCREEN_HEIGHT/TETRAMINO_DIM+1;y++){
        for(int x=0;x<10;x++){
            uint8_t tile = gamestate[GAME_MAP_INDEX(x,SCREEN_HEIGHT/TETRAMINO_DIM-y)];
            drawTile(
                MARGIN_LEFT+(x+1)*TETRAMINO_DIM,
                SCREEN_HEIGHT-(y+1)*TETRAMINO_DIM,
                tetramino_color_palette[tile]);
        }
    }

    // Draw the currently falling tetramino
    int mask=0x8000;
    for(int i=0;i<16;i++){
        if(currentTetraminoLayout & mask){
            drawTile(
                MARGIN_LEFT+(currentTetraminoX+i%4+1)*TETRAMINO_DIM,
                SCREEN_HEIGHT-(NUM_ROWS-(currentTetraminoY+i/4))*TETRAMINO_DIM,
                tetramino_color_palette[currentTetraminoColorIndex]);
        }
        mask = mask >> 1;
    }
    // Set the screen part right of the playing field black e.g. clear screen
    for(int y=0;y<SCREEN_HEIGHT;y++){
        for(int x=SCREEN_WIDTH-MARGIN_LEFT;x<SCREEN_WIDTH;x++){
            *PIXEL_OFFSET(x,y) = COLOR_BLACK;
        }
    }
    // Print Level and Score
    writeString(SCREEN_WIDTH-MARGIN_LEFT+5, 30, "Level:", COLOR_WHITE);
    writeInt(SCREEN_WIDTH-MARGIN_LEFT+5+6*8, 30, level, COLOR_WHITE, false);
    writeString(SCREEN_WIDTH-MARGIN_LEFT+5, 60, "Score:", COLOR_WHITE);
    writeInt(SCREEN_WIDTH-MARGIN_LEFT+5+6*8, 60, score, COLOR_WHITE, false);
}

static void play_sound(uint32_t nFrequence) {
    return;
    uint32_t Div;
    uint8_t tmp;

    //Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (Div) );
    outb(0x42, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

void turn_sound_off(){
    return;
    uint8_t tmp = inb(0x61) & 0xFC;
 
 	outb(0x61, tmp);
}


#define NOTE_C0       16
#define NOTE_Cs0_Db0  17
#define NOTE_D0       18
#define NOTE_Ds0_Eb0  19
#define NOTE_E0       20
#define NOTE_F0       21
#define NOTE_Fs0_Gb0  23
#define NOTE_G0       24
#define NOTE_Gs0_Ab0  25
#define NOTE_A0       27
#define NOTE_As0_Bb0  29
#define NOTE_B0       30
#define NOTE_C1       32
#define NOTE_Cs1_Db1  34
#define NOTE_D1       36
#define NOTE_Ds1_Eb1  38
#define NOTE_E1       41
#define NOTE_F1       43
#define NOTE_Fs1_Gb1  46
#define NOTE_G1       49
#define NOTE_Gs1_Ab1  51
#define NOTE_A1       55
#define NOTE_As1_Bb1  58
#define NOTE_B1       61
#define NOTE_C2       65
#define NOTE_Cs2_Db2  69
#define NOTE_D2       73
#define NOTE_Ds2_Eb2  77
#define NOTE_E2       82
#define NOTE_F2       87
#define NOTE_Fs2_Gb2  92
#define NOTE_G2       98
#define NOTE_Gs2_Ab2  103
#define NOTE_A2       110
#define NOTE_As2_Bb2  116
#define NOTE_B2       123
#define NOTE_C3       130
#define NOTE_Cs3_Db3  138
#define NOTE_D3       146
#define NOTE_Ds3_Eb3  155
#define NOTE_E3       164
#define NOTE_F3       174
#define NOTE_Fs3_Gb3  185
#define NOTE_G3       196
#define NOTE_Gs3_Ab3  207
#define NOTE_A3       220
#define NOTE_As3_Bb3  233
#define NOTE_B3       246
#define NOTE_C4       261
#define NOTE_Cs4_Db4  277
#define NOTE_D4       293
#define NOTE_Ds4_Eb4  311
#define NOTE_E4       329
#define NOTE_F4       349
#define NOTE_Fs4_Gb4  369
#define NOTE_G4       392
#define NOTE_Gs4_Ab4  415
#define NOTE_A4       440
#define NOTE_As4_Bb4  466
#define NOTE_B4       493
#define NOTE_C5       523
#define NOTE_Cs5_Db5  554
#define NOTE_D5       587
#define NOTE_Ds5_Eb5  622
#define NOTE_E5       659
#define NOTE_F5       698
#define NOTE_Fs5_Gb5  739
#define NOTE_G5       783
#define NOTE_Gs5_Ab5  830
#define NOTE_A5       880
#define NOTE_As5_Bb5  932
#define NOTE_B5       987
#define NOTE_C6       1046
#define NOTE_Cs6_Db6  1108
#define NOTE_D6       1174
#define NOTE_Ds6_Eb6  1244
#define NOTE_E6       1318
#define NOTE_F6       1396
#define NOTE_Fs6_Gb6  1479
#define NOTE_G6       1567
#define NOTE_Gs6_Ab6  1661
#define NOTE_A6       1760
#define NOTE_As6_Bb6  1864
#define NOTE_B6       1975
#define NOTE_C7       2093
#define NOTE_Cs7_Db7  2217
#define NOTE_D7       2349
#define NOTE_Ds7_Eb7  2489
#define NOTE_E7       2637
#define NOTE_F7       2793
#define NOTE_Fs7_Gb7  2959
#define NOTE_G7       3135
#define NOTE_Gs7_Ab7  3322
#define NOTE_A7       3520
#define NOTE_As7_Bb7  3729
#define NOTE_B7       3951
#define NOTE_C8       4186
#define NOTE_Cs8_Db8  4434
#define NOTE_D8       4698
#define NOTE_Ds8_Eb8  4978
#define NOTE_E8       5274
#define NOTE_F8       5587
#define NOTE_Fs8_Gb8  5919
#define NOTE_G8       6271
#define NOTE_Gs8_Ab8  6644
#define NOTE_A8       7040
#define NOTE_As8_Bb8  7458
#define NOTE_B8       7902
// 192
// 312
uint32_t ticks = 0;

uint32_t sounds[1000];

uint32_t num_ticks = 0;

inline void addSixteenth(uint32_t val){
    sounds[num_ticks++] = val;
}

inline void addEigth(uint32_t val){
    sounds[num_ticks++] = val;
    sounds[num_ticks++] = val;
}

inline void addQuarter(uint32_t val){
    for(int i=0;i<4;i++){
        sounds[num_ticks++] = val;
    }
}

inline void addHalf(uint32_t val){
    for(int i=0;i<8;i++){
        sounds[num_ticks++] = val;
    }
}

inline void addWhole(uint32_t val){
    for(int i=0;i<16;i++){
        sounds[num_ticks++] = val;
    }
}


void addMusic(){
addQuarter(NOTE_E4);
addHalf(0);
addQuarter(NOTE_B3);
addHalf(0);
addQuarter(NOTE_C4);
addHalf(0);
addQuarter(NOTE_D4);
addQuarter(0);
addQuarter(NOTE_C4);
addQuarter(0);
addQuarter(NOTE_B3);
addQuarter(0);
addQuarter(NOTE_A3);
addQuarter(0);

addQuarter(NOTE_A3);
addEigth(0);
addQuarter(NOTE_C4);
addEigth(0);
addQuarter(NOTE_E4);
addEigth(0);
addQuarter(NOTE_D4);
addSixteenth(0);
addQuarter(NOTE_C4);
addSixteenth(0);
addQuarter(NOTE_B3);
addSixteenth(0);

addQuarter(NOTE_C4);
addSixteenth(0);
addQuarter(NOTE_D4);
addQuarter(NOTE_E4);
addQuarter(NOTE_C4);
addQuarter(NOTE_A3);
addQuarter(NOTE_A3);

addQuarter(NOTE_D4);
addQuarter(NOTE_F4);
addQuarter(NOTE_A4);
addQuarter(NOTE_G4);
addQuarter(NOTE_F4);
addQuarter(NOTE_E4);

addQuarter(NOTE_C4);
addQuarter(NOTE_E4);
addQuarter(NOTE_D4);
addQuarter(NOTE_C4);
addQuarter(NOTE_B3);

addQuarter(NOTE_B3);
addQuarter(NOTE_C4);
addQuarter(NOTE_D4);
addQuarter(NOTE_E4);
addQuarter(NOTE_C4);
addQuarter(NOTE_A4);
addQuarter(NOTE_A4);


addWhole(NOTE_E4);
addWhole(NOTE_C4);
addWhole(NOTE_D4);
addWhole(NOTE_B3);
addWhole(NOTE_C4);
addWhole(NOTE_A3);
addWhole(NOTE_Gs3_Ab3);//b
addWhole(NOTE_B3);
addWhole(NOTE_E4);
addWhole(NOTE_C4);
addWhole(NOTE_D4);
addWhole(NOTE_B3);
addWhole(NOTE_C4);
addWhole(NOTE_E4);
addWhole(NOTE_A4);
addWhole(NOTE_Gs4_Ab4);//b
addWhole(0);
}

uint8_t playFrame = 0;

void set_volume(uint8_t volume) {
    uint8_t speaker = inb(0x61);
    speaker &= 0xFC; // clear the two least significant bits
    speaker |= volume; // set the volume
    outb(0x61, speaker); // write the new value to the speaker port
}

bool other = false;

uint32_t downtick = 0;

INTERRUPT_ATTRB
void timer_interrupt_handler(struct interrupt_frame *frame){
    pic_send_eoi(0);

    downtick = (downtick+1) % 8;
    if(downtick == 7){
        moveTetraminoDown(true);
        drawPlayingField();
    }

    //if(playFrame>=4){
    //    if(playFrame>=20){
    //        playFrame = 0;
    //    }else{
    //        //turn_sound_off();
    //        set_volume(3-playFrame/6);
    //        play_sound(sounds[ticks]);
    //        playFrame++;
    //    }
    //    return;
    //}0x3a0x280x290x7b0x200x3a0x7c0x3a0x260x200x7d0x3b0x3a
//
    //playFrame++;

    //if(!other){
    //    other = true;
    //    return;
    //}
//
    //other = false;

    ticks = (ticks+1) % num_ticks;
    uint32_t freq = sounds[ticks];
    if(freq==0){
        turn_sound_off();
    }else{
        play_sound(freq);
    }
}

#define TIMER_FREQUENCY 1193182
#define INTERRUPT_FREQUENCY 16

// Define the timer counter value
#define TIMER_COUNTER_VALUE ((TIMER_FREQUENCY / INTERRUPT_FREQUENCY) - 1)

// Define the timer port addresses
#define TIMER_PORT_CONTROL 0x43
#define TIMER_PORT_CHANNEL0 0x40

// Define the interrupt number for the timer
#define INTERRUPT_NUMBER_TIMER 0x20

void setupPic(){
    uint32_t divisor = 1193180 / 60; // Set frequency to 60Hz

    // Set the PIT to generate interrupts at the desired frequency
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

extern "C" void main(){
    //clear_screen();
    //print_str(linux_dir_entry);


    dbg_string("Kernel main!\nBootet from disk: ");
    print_int_to_func((int)BOOT_DISK, dbg_string);
    dbg_putc('\n');

    dbg_string("Setting up IDT\n");
    disable_interrupts();

    addMusic();

    //setIdtEntry32(50, (void*)handle_int1, 8, IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    //enable_gate32(50);
    setIdtEntry32(0x8, (void*)handle_int_8, 8, IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    enable_gate32(0x8);

    setIdtEntry32(0x20, (void*)timer_interrupt_handler, 8, IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    enable_gate32(0x20);

    setIdtEntry32(0x21, (void*)handle_int_keyboard, 8, IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    enable_gate32(0x21);

    drawPlayingField();

    drawGameBorders();
    newRandomTetramino();
    drawPlayingField();

    load_idt32();

    setup_pic();
    //setupPic();
    outb(PIC1_DATA_PORT, 0xFC);


    enable_interrupts();
    dbg_string("Finished setting up IDT\n");
    dbg_string("testing interrupt\n");


    //TRIGGER_INTERRUPT(0x21);
    dbg_string("finished\n");

    play_sound(sounds[ticks]);
    

    while(1){asm volatile("hlt");}


    //writeString(41,11, "Hello World!", COLOR_DARK_GRAY);
    //writeString(40,10, "Hello World!", COLOR_WHITE);
    //writeInt(40, 19, 697811, COLOR_WHITE);



    //set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    //clear_screen();
    //char str1[] = "root@LocalPc:/mnt/etc$ ";
    //print_str(str1);
    //print_int(2147483647);
    //print_char('\n');
    //print_str(str1);
    //print_int((int)BOOT_DISK);
    ////print_int(-2147483648);
    //while(1){
    //    char c = getchar();

    //    print_char(c);

    //    if(c=='\n'){
    //        print_str(str1);
    //    }
    //}
    return;
}
