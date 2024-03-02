#ifndef CPU_6502_EMULATOR_H_
#define CPU_6502_EMULATOR_H_

#include <CPU_6502.h>
#include <GPU/GPU.h>
#include <windows.h>

#define SCREEN_MIN_WIDTH 1100
#define SCREEN_MIN_HEIGHT 650

#define BUFFER_SIZE 256
#define FONT_HEIGHT 20
#define BORDER 10
#define SPACING_X 20
#define SPACING_Y 10
#define BREAKPOINT_SPACING_X 5
#define BREAKPOINT_SPACING_Y 5
#define INPUT_SPACING_X 5
#define INPUT_SPACING_Y 5

#define POPUP_MAX_LENGTH 40
#define POPUP_SPACING 2

#define GAP_EXPAND_SIZE 40

#define BLINK_TIME 600
#define MOVE_OFFSET_TIME 50

#define STACK_ROW_LENGTH 8
#define SOURCE_ROW_LENGTH 8

//Colors
#define BACKGROUND1_COLOR         (Color){15,15,15,255}
#define BACKGROUND2_COLOR         (Color){25,25,25,255}
#define BACKGROUND3_COLOR         (Color){20,20,20,255}

#define SCROLLBAR1_COLOR          (Color){150,150,150,50}
#define SCROLLBAR2_COLOR          (Color){255,255,255,100}
#define SCROLLBAR3_COLOR          (Color){255,255,255,150}

#define BORDER_COLOR              (Color){180,180,180,255}
#define SELECTED_COLOR            (Color){255,150,0,255}
#define COLOR_ON                  (Color){220,220,220,255}
#define COLOR_OFF                 (Color){80,80,80,255}
#define DEBUG_COLOR               (Color){0,255,0,255}
#define BREAKPOINT_SELECTED_COLOR (Color){20,20,20,255}
#define TEXT_SELECTED_COLOR       (Color){80,80,80,255}
#define CURSOR_COLOR              (Color){255,255,255,255}
#define STOP_COLOR                (Color){255,0,0,255}

//Flags
#define NONE 0x0000

//Breakpoint selected part flags
#define BREAKPOINT           0x0001
#define BREAKPOINT_CONDITION 0x0002
#define BREAKPOINT_INPUT     0x0004

//Breakpoint flags
#define INPUT_2BYTES       0x0002
#define INPUT_4BYTES       0x0004
#define INPUT_BYTES        (INPUT_2BYTES | INPUT_4BYTES)
#define USE_INPUT          INPUT_BYTES

#define SP_UNDER_OVER       (0x0008)
#define PC_BREAKPOINT       (0x0010 | INPUT_4BYTES)
#define ACCUMALATOR_EQUAL   (0x0020 | INPUT_2BYTES)
#define ACCUMALATOR_BIGGER  (0x0040 | INPUT_2BYTES)
#define ACCUMALATOR_SMALLER (0x0080 | INPUT_2BYTES)
#define X_EQUAL             (0x0100 | INPUT_2BYTES)
#define X_BIGGER            (0x0200 | INPUT_2BYTES)
#define X_SMALLER           (0x0400 | INPUT_2BYTES)
#define Y_EQUAL             (0x0800 | INPUT_2BYTES)
#define Y_BIGGER            (0x1000 | INPUT_2BYTES)
#define Y_SMALLER           (0x2000 | INPUT_2BYTES)

//Input value format flags
#define FORMAT_DEC 0x0001
#define FORMAT_HEX 0x0002

//Action flags
#define REDO 0x0001
#define UNDO 0x0002

#define ACTION_ADD_TEXT    0x0001
#define ACTION_ADD_LINE    0x0002
#define ACTION_DELETE_TEXT 0x0004
#define ACTION_DELETE_LINE 0x0008


#define PointInRect(point,rect) ((point).x >= (rect).x && (point).x < (rect).x + (rect).w && (point).y >= (rect).y && (point).y < (rect).y + (rect).h)

#define Vector2IsEqual(vector1,vector2) ((vector1).x == (vector2).x && (vector1).y == (vector2).y)


typedef struct _Scroll_Bar Scroll_Bar;
typedef struct _Cursors Cursors;
typedef struct _Cursor Cursor;
typedef struct _Mouse Mouse;

typedef struct _Breakpoint Breakpoint;
typedef struct _Action_Stack Action_Stack;
typedef struct _Line Line;

typedef struct _Input Input;
typedef struct _Label Label;
typedef struct _Button_Text Button_Text;
typedef struct _Flag Flag;
typedef struct _Variable Variable;

typedef struct _Text_Editor Text_Editor;
typedef struct _Breakpoint_Conditions Breakpoint_Conditions;
typedef struct _PopUp PopUp;
typedef struct _Breakpoints Breakpoints;
typedef struct _Registers Registers;
typedef struct _Source_Editor Source_Editor;
typedef struct _App App;

struct _Scroll_Bar{
    Recti bar_rect;
    Rectf scroll_rect;
    float offset;
    int length;
    float mouse_diff;
    bool on;
    bool mouse_over;
    bool visible;
    bool valid;
};

struct _Cursors{
    SDL_Cursor *arrow;
    SDL_Cursor *ibeam;
    SDL_Cursor *hand;
    SDL_SystemCursor current_cursor;
};

struct _Cursor{
    int time;
    bool visible;
    bool update;
    bool on;
    Vector2i pos;
    uint32_t time_move_offset;
};

struct _Mouse{
    Vector2i pos;
    Vector2i offset;
    uint32_t state;
};


struct _Breakpoint{
    Button_Text *condition_button;
    Input *value_input;
    Rectf rect;
    uint16_t condition;
    uint16_t value;
    bool mouse_over;
    struct _Breakpoint *previous;
    struct _Breakpoint *next; 
};

struct _Action_Stack{
    GapBuffer *gap_buffer;
    bool used;
    bool chained;
    bool text_selected;
    uint8_t action;
    Vector2i target_cursor;
    Vector2i origin_cursor;
    Vector2i start_modification;
    Vector2i end_modification;
    struct _Action_Stack *next;
};

struct _Line{
    GapBuffer *gap_buffer;
    int index;
    int start_selection;
    int end_selection;
    bool selected;
    struct _Line *next;
    struct _Line *previous;
};


struct _Input{
    Recti rect;
    Vector2i pos;
    GapBuffer *gap_buffer;
    uint32_t max_length;
    Cursor cursor;
    bool mouse_over;
    uint8_t format;
};

struct _Label{
    wchar_t *text;
    FontAtlas *atlas;
    Vector2i text_size;
    Recti rect;
    Vector2i pos;
};

struct _Button_Text{
    wchar_t *text;
    FontAtlas *atlas;
    Vector2i text_size;
    Recti rect;
    Vector2i pos;
    bool mouse_over;
    bool mouse_pressed;
};

struct _Flag{
    Recti rect;
    Vector2i pos;
    bool mouse_over;
    bool mouse_pressed;
};

struct _Variable{
    wchar_t *name;
    FontAtlas *atlas;
    Recti name_rect;
    Vector2i name_size;
    Vector2i name_pos;
    Vector2i value_pos;
};


struct _Text_Editor{
    Line *lines;
    Line *current_line;
    Line *start_visible_line;
    Line *end_visible_line;
    Action_Stack *undo;
    Action_Stack *redo;
    Cursor cursor;
    Recti rect;
    Scroll_Bar vertical_scroll_bar;
    Scroll_Bar horizontal_scroll_bar;
    Vector2i glyph_size;
    Vector2i start_selection;
    Vector2i end_selection;
    uint32_t number_of_lines;
    bool text_selected;
    bool selected;
};

struct _Breakpoint_Conditions{
    Recti rect;
    bool opened;
    Breakpoints *breakpoints;
    Button_Text *program_counter_breakpoint;
    Button_Text *stack_pointer_under_over;
    Button_Text *accumalator_equal;
    Button_Text *accumalator_bigger;
    Button_Text *accumalator_smaller;
    Button_Text *x_equal;
    Button_Text *x_bigger;
    Button_Text *x_smaller;
    Button_Text *y_equal;
    Button_Text *y_bigger;
    Button_Text *y_smaller;
};

struct _PopUp{
    App *app;
    Recti rect;
    Recti message_rect;
    Vector2i message_pos;
    wchar_t *message;
    Button_Text *ok_button;
    bool opened;
};

struct _Breakpoints{
    App *app;
    Label *label;
    Button_Text *new_button;
    Button_Text *delete_button;
    Breakpoint *breakpoints;
    Breakpoint *current_breakpoint;
    Breakpoint_Conditions *conditions;
    Recti rect;
    Scroll_Bar vertical_scroll_bar;

    void (*BreakpointsAjustVerticalScrollBarOffset)(Breakpoints *breakpoints);
    void (*BreakpointsConfigVerticalScrollBar)(Breakpoints *breakpoints);

    void (*ConfigBreakpointsRect)(Breakpoints *breakpoints);

    void (*PushBreakpoint)(Breakpoints *breakpoints);
    void (*RemoveBreakpoint)(Breakpoints *breakpoints);
};

struct _Registers{
    App *app;
    Label *label;
    Input *pc;
    Input *ar;
    Input *xr;
    Input *yr;
    Input *sr;
    Input *sp;
    Flag flag_n;
    Flag flag_v;
    Flag flag_u;
    Flag flag_i;
    Flag flag_b;
    Flag flag_d;
    Flag flag_z;
    Flag flag_c;
};

struct _Source_Editor{
    App *app;
    Text_Editor *editor;
    Label *label;
    Button_Text *load_file_button;
    Button_Text *load_memory_button;
    Button_Text *clear_memory_button;
    Label *start_address_label;
    Input *start_address_input;
    uint16_t start_address;
};

struct _App{
    SDL_Window *window;
    Renderer *renderer;
    CPU_6502 *cpu;
    Font *font;
    FontAtlas *atlas;
    Cursors *cursors;
    Input *current_input;
    
    SDL_Event event;
    wchar_t buffer[BUFFER_SIZE];

    Button_Text *step_button;
    Button_Text *run_button;
    Button_Text *stop_button;
    Button_Text *reset_button;
    Button_Text *irq_button;
    Button_Text *nmi_button;

    Registers *registers;
    Breakpoints *breakpoints;
    Source_Editor *source_editor;
    PopUp *popup;

    Variable *total_cicles_variable;
    Variable *total_instructions_variable;

    Label *instruction_runtime_label;
    Input *instruction_runtime_input;
    uint32_t instruction_runtime;

    Button_Text *seek_to_button;
    Input *seek_to_input;
    uint16_t address;

    Label *stack_label;
    Recti stack_rect;
    Scroll_Bar stack_vertical_scroll_bar;
    uint16_t stack_start_index;
    uint16_t stack_end_index;
    uint16_t stack_start_line;
    uint16_t stack_end_line;

    Recti memory_rect;
    Scroll_Bar memory_vertical_scroll_bar;
    uint16_t memory_start_index;
    uint16_t memory_end_index;
    uint16_t memory_start_line;
    uint16_t memory_end_line;

    Mouse mouse;
    uint32_t current_time;
    uint32_t start_execution;
    Vector2i screen_size;
    Vector2i glyph_size;
    bool running;
    bool instruction_step;
    bool instruction_run;
    bool element_on;
};

#endif
