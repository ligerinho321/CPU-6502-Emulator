#include <utils.h>

void UpdateCursor(Cursor *cursor,uint32_t current_time){
    if(cursor->update){
        cursor->update = false;
        cursor->visible = true;
        cursor->time = current_time;
    }
    if(current_time - cursor->time >= BLINK_TIME){
        cursor->visible = !cursor->visible;
        cursor->time = current_time;
    }
}


void MemorySetVisibleLines(App *app){
    uint32_t index = 0;
    uint32_t line = 0;
    uint8_t opcode = 0;

    app->memory_start_index = (uint16_t)-1;
    app->memory_end_index = (uint16_t)-1;
    app->memory_start_line = (uint16_t)-1;
    app->memory_end_line = (uint16_t)-1;

    while(index < MEMORY_SIZE){

        opcode = READ_8_BITS(app->cpu,index);

        if(line * app->glyph_size.y + app->memory_vertical_scroll_bar.offset >= app->memory_rect.h){
            
            break;
        }
        else if((line+1) * app->glyph_size.y + app->memory_vertical_scroll_bar.offset >= 0.0f){
            
            if(app->memory_start_index == (uint16_t)-1 && app->memory_start_line == (uint16_t)-1){
                
                app->memory_start_index = index;
                app->memory_start_line = line;
            }
            else{
                
                app->memory_end_index = index;
                app->memory_end_line = line;
            }
        }

        line++;
        index += app->cpu->instruction_table[opcode].bytes;
    }
}

void MemoryConfigVerticalScrollBar(App *app){
    uint32_t index = 0;
    uint32_t line = 0;
    uint8_t opcode = 0;

    while(index < MEMORY_SIZE && line < MEMORY_SIZE){
        opcode = READ_8_BITS(app->cpu,index);
        line++;
        index += app->cpu->instruction_table[opcode].bytes;
    }

    app->memory_vertical_scroll_bar.length = line * app->glyph_size.y;

    app->memory_vertical_scroll_bar.scroll_rect.h = GetScrollBarHeight(app->memory_vertical_scroll_bar);

    if(app->memory_vertical_scroll_bar.scroll_rect.h > 0.0f && app->memory_vertical_scroll_bar.scroll_rect.h < app->memory_vertical_scroll_bar.bar_rect.h){
        app->memory_vertical_scroll_bar.valid = true;
        SetYScrollBar(&app->memory_vertical_scroll_bar,app->memory_vertical_scroll_bar.offset);
    }
    else{
        app->memory_vertical_scroll_bar.valid = false;
        app->memory_vertical_scroll_bar.offset = 0.0f;
        app->memory_vertical_scroll_bar.scroll_rect.y = app->memory_vertical_scroll_bar.bar_rect.y;
    }

    MemorySetVisibleLines(app);
}

void MemorySetOffsetAddress(App *app){
    uint32_t index = 0;
    uint32_t line = 0;
    uint8_t opcode = 0;
    int address_length = 0;

    while(index < MEMORY_SIZE && line < MEMORY_SIZE){
        opcode = READ_8_BITS(app->cpu,index);
        if(app->address >= index && app->address < index + app->cpu->instruction_table[opcode].bytes){
            address_length = line * app->glyph_size.y;
            break;
        }
        line++;
        index += app->cpu->instruction_table[opcode].bytes;
    }

    SetYScrollBar(&app->memory_vertical_scroll_bar,-1.0f * address_length);
    MemorySetVisibleLines(app);
}

void MemoryAjustVerticalScrollBarOffset(App *app){
    uint32_t index = 0;
    uint32_t line = 0;
    uint8_t opcode = 0;
    int position = 0;

    while(index < MEMORY_SIZE && line < MEMORY_SIZE){
        opcode = READ_8_BITS(app->cpu,index);
        if(app->cpu->program_counter >= index && app->cpu->program_counter < index + app->cpu->instruction_table[opcode].bytes){
            position = line * app->glyph_size.y;
            break;
        }

        line++;
        index += app->cpu->instruction_table[opcode].bytes;
    }

    float length_diff = app->memory_vertical_scroll_bar.length - app->memory_vertical_scroll_bar.bar_rect.h;
    float scroll_length = app->memory_vertical_scroll_bar.bar_rect.h - app->memory_vertical_scroll_bar.scroll_rect.h;

    if(position + app->memory_vertical_scroll_bar.offset < 0.0f){
        float diff = position + app->memory_vertical_scroll_bar.offset;
        app->memory_vertical_scroll_bar.scroll_rect.y += diff / length_diff * scroll_length;

        SetVerticalOffsetScrollBar(&app->memory_vertical_scroll_bar);
        if(diff <= -app->glyph_size.y) MemorySetVisibleLines(app);
    }
    else if(position + app->glyph_size.y + app->memory_vertical_scroll_bar.offset > app->memory_rect.h){
        float diff = position + app->glyph_size.y + app->memory_vertical_scroll_bar.offset - app->memory_rect.h;
        app->memory_vertical_scroll_bar.scroll_rect.y += diff / length_diff * scroll_length;

        SetVerticalOffsetScrollBar(&app->memory_vertical_scroll_bar);
        if(diff >= app->glyph_size.y) MemorySetVisibleLines(app);
    }
}


void StackSetVisibleLines(App *app){
    uint16_t index = STACK_START;
    uint16_t line = 0;

    app->stack_start_index = (uint16_t)-1;
    app->stack_end_index = (uint16_t)-1;
    app->stack_start_line = (uint16_t)-1;
    app->stack_end_line = (uint16_t)-1;

    while(index <= STACK_END){
        if(line * app->glyph_size.y + app->stack_vertical_scroll_bar.offset >= app->stack_rect.h){
            break;
        }
        else if((line + 1) * app->glyph_size.y + app->stack_vertical_scroll_bar.offset > 0.0f){
            if(app->stack_start_index == (uint16_t)-1 && app->stack_start_line == (uint16_t)-1){
                app->stack_start_index = index;
                app->stack_start_line = line;
            }
            else{
                app->stack_end_index = index;
                app->stack_end_line = line;
            }
        }

        line++;
        index += STACK_ROW_LENGTH;
    }   
}

void StackConfigVerticalScrollBar(App *app){

    app->stack_vertical_scroll_bar.length = ((STACK_LENGTH / STACK_ROW_LENGTH) + 1) * FONT_HEIGHT;

    app->stack_vertical_scroll_bar.scroll_rect.h = GetScrollBarHeight(app->stack_vertical_scroll_bar);

    if(app->stack_vertical_scroll_bar.scroll_rect.y > 0.0f && app->stack_vertical_scroll_bar.scroll_rect.h < app->stack_vertical_scroll_bar.bar_rect.h){
        app->stack_vertical_scroll_bar.valid = true;
        SetYScrollBar(&app->stack_vertical_scroll_bar,app->stack_vertical_scroll_bar.offset);
    }
    else{
        app->stack_vertical_scroll_bar.valid = false;
        app->stack_vertical_scroll_bar.offset = 0.0f;
        app->stack_vertical_scroll_bar.scroll_rect.y = app->stack_vertical_scroll_bar.bar_rect.y;
    }

    StackSetVisibleLines(app);
}


Cursors* CreateCursors(){
    Cursors *cursors = malloc(sizeof(Cursors));
    cursors->arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    cursors->ibeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    cursors->hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    return cursors;
};

void CursorsUpdate(Cursors *cursors){
    SDL_Cursor *ptr = SDL_GetCursor();
    if(cursors->current_cursor == SDL_SYSTEM_CURSOR_ARROW && ptr != cursors->arrow){
        SDL_SetCursor(cursors->arrow);
    }
    else if(cursors->current_cursor == SDL_SYSTEM_CURSOR_IBEAM && ptr != cursors->ibeam){
        SDL_SetCursor(cursors->ibeam);
    }
    else if(cursors->current_cursor == SDL_SYSTEM_CURSOR_HAND && ptr != cursors->hand){
        SDL_SetCursor(cursors->hand);
    }
}

void CursorsFree(Cursors *cursors){
    SDL_FreeCursor(cursors->arrow);
    SDL_FreeCursor(cursors->ibeam);
    SDL_FreeCursor(cursors->hand);
    free(cursors);
}


Label* CreateLabel(wchar_t *text,FontAtlas *atlas){
    Label *label = malloc(sizeof(Label));
    label->atlas = atlas;
    label->text = wcsdup(text);
    SizeTextByFontAtlas(atlas,label->text,&label->text_size.x,&label->text_size.y);
    label->pos = (Vector2i){0};
    label->rect = (Recti){0};
    return label;
}

void DrawLabel(Renderer *renderer,Label *label,Color color){
    if(!renderer || !label) return;
    #ifdef DEBUG
    DrawRectanglei(renderer,&label->rect,DEBUG_COLOR);
    #endif
    DrawTextUint16i(renderer,label->atlas,label->text,label->pos,color);
}

void LabelFree(Label *label){
    if(!label) return;
    free(label->text);
    free(label);
}


Button_Text* CreateButtonText(wchar_t *text,FontAtlas *atlas){
    Button_Text *button_text = malloc(sizeof(Button_Text));
    button_text->text = wcsdup(text);
    button_text->atlas = atlas;
    SizeTextByFontAtlas(atlas,button_text->text,&button_text->text_size.x,&button_text->text_size.y);
    button_text->rect = (Recti){0};
    button_text->pos = (Vector2i){0};
    button_text->mouse_over = false;
    button_text->mouse_pressed = false;
    return button_text;
}

void ButtonTextFree(Button_Text *button_text){
    if(!button_text) return;
    free(button_text->text);
    free(button_text);
}

void UpdateButtonText(Button_Text *button_text,wchar_t *new_text){
    free(button_text->text);
    button_text->text = wcsdup(new_text);
    SizeTextByFontAtlas(button_text->atlas,button_text->text,&button_text->text_size.x,&button_text->text_size.y);
}

void DrawButtonText(Renderer *renderer,Button_Text *button_text,Color mouse_over_color,Color color){
    if(!renderer || !button_text) return;
    #ifdef DEBUG
    DrawRectanglei(renderer,&button_text->rect,DEBUG_COLOR);
    #endif
    if(button_text->mouse_over){
        DrawTextUint16i(renderer,button_text->atlas,button_text->text,button_text->pos,mouse_over_color);
        button_text->mouse_over = false;
    }
    else{
        DrawTextUint16i(renderer,button_text->atlas,button_text->text,button_text->pos,color);
    }
}


Input* CreateInput(uint32_t max_length,uint8_t format){
    Input *input = malloc(sizeof(Input));
    input->max_length = max_length;
    CreateGapBuffer(input->gap_buffer,wchar_t,max_length);
    memset(&input->cursor,0,sizeof(Cursor));
    input->cursor.on = true;
    input->rect = (Recti){0};
    input->format = format;
    return input;
}

void InputFree(Input *input){
    GapBufferFree(input->gap_buffer);
    free(input);
}

void DrawInput(App *app,Input *input,wchar_t *text){

    if(input == app->current_input){

        DrawFilledRectanglei(app->renderer,&input->rect,BACKGROUND2_COLOR);

        UpdateCursor(&input->cursor,app->current_time);

        if(input->cursor.visible){
            Vector2i point1 = {
                .x = input->pos.x + input->cursor.pos.x * app->glyph_size.x,
                .y = input->rect.y + input->rect.h * 0.5f - app->glyph_size.y * 0.5f
            };
            Vector2i point2 = {
                .x = point1.x,
                .y = point1.y + app->glyph_size.y
            };

            DrawLinei(app->renderer,point1,point2,CURSOR_COLOR);
        }

        DrawTextByGapBufferUint16i(app->renderer,app->atlas,input->gap_buffer,input->pos,COLOR_ON);
    }
    else{
        if(input->mouse_over){
            DrawTextUint16i(app->renderer,app->atlas,text,input->pos,SELECTED_COLOR);
        }
        else{
            DrawTextUint16i(app->renderer,app->atlas,text,input->pos,COLOR_ON);
        }
    }
    input->mouse_over = false;

    #ifdef DEBUG
    DrawRectanglei(app->renderer,&input->rect,DEBUG_COLOR);
    #endif
}


Variable* CreateVariable(wchar_t *text,FontAtlas *atlas){
    Variable *variable = malloc(sizeof(Variable));
    variable->name = wcsdup(text);
    variable->atlas = atlas;
    SizeTextByFontAtlas(atlas,text,&variable->name_size.x,&variable->name_size.y);
    variable->name_rect = (Recti){0};
    variable->name_pos = (Vector2i){0};
    variable->value_pos = (Vector2i){0};
    return variable;
}

void VariableFree(Variable *variable){
    if(!variable) return;
    free(variable->name);
    free(variable);
}

void DrawVariable(Renderer *renderer,Variable *variable,wchar_t *value,Color color){
    #ifdef DEBUG
    DrawRectanglei(renderer,&variable->name_rect,DEBUG_COLOR);
    #endif
    //name
    DrawTextUint16i(renderer,variable->atlas,variable->name,variable->name_pos,color);
    //value
    DrawTextUint16i(renderer,variable->atlas,value,variable->value_pos,color);
}


PopUp* CreatePopUp(App *app){
    PopUp *popup = malloc(sizeof(PopUp));
    popup->app = app;
    popup->rect = (Recti){0};
    popup->message_rect = (Recti){0};
    popup->message_pos = (Vector2i){0};
    popup->message = NULL;
    popup->ok_button = CreateButtonText(L"Ok",app->atlas);
    popup->opened = false;
    return popup;
}

void PopUpConfigRect(PopUp *popup){
    int message_width = (POPUP_MAX_LENGTH + POPUP_SPACING) * popup->app->glyph_size.x;
    int button_width = popup->ok_button->text_size.x + (POPUP_SPACING * popup->app->glyph_size.x);
    int width = message_width + button_width;
    int height = popup->app->glyph_size.y + SPACING_Y;

    popup->rect = (Recti){
        .x = popup->app->screen_size.x * 0.5f - width * 0.5f,
        .y = popup->app->screen_size.y * 0.5f - height * 0.5f,
        .w = width,
        .h = height
    };

    popup->message_rect = (Recti){
        .x = popup->rect.x,
        .y = popup->rect.y,
        .w = message_width,
        .h = height
    };
    popup->message_pos = (Vector2i){
     .x = popup->message_rect.x + (POPUP_SPACING * popup->app->glyph_size.x),
     .y = popup->message_rect.y + (SPACING_Y * 0.5f)
    };

    popup->ok_button->rect = (Recti){
        .x = popup->message_rect.x + popup->message_rect.w,
        .y = popup->rect.y,
        .w = button_width,
        .h = height
    };
    popup->ok_button->pos = (Vector2i){
        .x = popup->ok_button->rect.x + popup->ok_button->rect.w * 0.5f - popup->ok_button->text_size.x * 0.5f,
        .y = popup->ok_button->rect.y + popup->ok_button->rect.h * 0.5f - popup->ok_button->text_size.y * 0.5f
    };
}

void PopUpDraw(PopUp *popup){
    DrawFilledRectanglei(popup->app->renderer,&popup->rect,BACKGROUND3_COLOR);
    DrawRectanglei(popup->app->renderer,&popup->rect,BORDER_COLOR);

    DrawTextUint16i(popup->app->renderer,popup->app->atlas,popup->message,popup->message_pos,COLOR_ON);

    DrawButtonText(popup->app->renderer,popup->ok_button,SELECTED_COLOR,COLOR_ON);

    #ifdef DEBUG
    DrawRectanglei(popup->app->renderer,&popup->rect,DEBUG_COLOR);
    DrawRectanglei(popup->app->renderer,&popup->message_rect,DEBUG_COLOR);
    DrawRectanglei(popup->app->renderer,&popup->ok_button->rect,DEBUG_COLOR);
    #endif
}

void PopUpEvent(PopUp *popup){
    if(popup->app->breakpoints->conditions->opened) return;

    if(PointInRect(popup->app->mouse.pos,popup->ok_button->rect)){
        popup->ok_button->mouse_over = true;
        popup->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else{
        popup->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }

    if(popup->app->event.type == SDL_MOUSEBUTTONDOWN && popup->app->event.button.button == SDL_BUTTON_LEFT){
        if(popup->ok_button->mouse_over){
            popup->ok_button->mouse_pressed = true;
        }
    }
    else if(popup->app->event.type == SDL_MOUSEBUTTONUP && popup->app->event.button.button == SDL_BUTTON_LEFT){
        if(popup->ok_button->mouse_pressed){
            if(popup->ok_button->mouse_over){
                popup->opened = false;
            }
            popup->ok_button->mouse_pressed = false;
        }
    }
}

void OpenPopUp(PopUp *popup,wchar_t *message){
    if(popup->opened || !message || wcslen(message) > POPUP_MAX_LENGTH) return;
    popup->opened = true;
    popup->message = wcsdup(message);
}

void PopUpFree(PopUp *popup){
    ButtonTextFree(popup->ok_button);
    free(popup->message);
    free(popup);
}


static void BreakpointSetCondition(Breakpoint_Conditions *breakpoint_conditions,uint32_t condition,wchar_t *text){
    Breakpoint *breakpoint = breakpoint_conditions->breakpoints->current_breakpoint;

    if(breakpoint->condition != condition){
        
        breakpoint->condition = condition;
        breakpoint->value = 0x0000;
        UpdateButtonText(breakpoint->condition_button,text);
        breakpoint->value_input->max_length = condition & INPUT_BYTES;
        breakpoint_conditions->breakpoints->ConfigBreakpointsRect(breakpoint_conditions->breakpoints);
    }

    breakpoint_conditions->opened = false;
}

static void BreakpointConditionsMouseOver(Breakpoint_Conditions *breakpoint_conditions,Mouse mouse){
    
    if(PointInRect(mouse.pos,breakpoint_conditions->rect)){
        breakpoint_conditions->breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else{
        breakpoint_conditions->breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }

    Vector2i position = {
        .x = mouse.pos.x - breakpoint_conditions->rect.x,
        .y = mouse.pos.y - breakpoint_conditions->rect.y
    };

    if(PointInRect(position,breakpoint_conditions->program_counter_breakpoint->rect)){
        breakpoint_conditions->program_counter_breakpoint->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->stack_pointer_under_over->rect)){
        breakpoint_conditions->stack_pointer_under_over->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->accumalator_equal->rect)){
        breakpoint_conditions->accumalator_equal->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->accumalator_bigger->rect)){
        breakpoint_conditions->accumalator_bigger->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->accumalator_smaller->rect)){
        breakpoint_conditions->accumalator_smaller->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->x_equal->rect)){
        breakpoint_conditions->x_equal->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->x_bigger->rect)){
        breakpoint_conditions->x_bigger->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->x_smaller->rect)){
        breakpoint_conditions->x_smaller->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->y_equal->rect)){
        breakpoint_conditions->y_equal->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->y_bigger->rect)){
        breakpoint_conditions->y_bigger->mouse_over = true;
    }
    else if(PointInRect(position,breakpoint_conditions->y_smaller->rect)){
        breakpoint_conditions->y_smaller->mouse_over = true;
    }
}

static void BreakpointConditionsMousePressed(Breakpoint_Conditions *breakpoint_conditions){
    
    if(breakpoint_conditions->program_counter_breakpoint->mouse_over){
        breakpoint_conditions->program_counter_breakpoint->mouse_pressed = true;
    }
    else if(breakpoint_conditions->stack_pointer_under_over->mouse_over){
        breakpoint_conditions->stack_pointer_under_over->mouse_pressed = true;
    }
    else if(breakpoint_conditions->accumalator_equal->mouse_over){
        breakpoint_conditions->accumalator_equal->mouse_pressed = true;
    }
    else if(breakpoint_conditions->accumalator_bigger->mouse_over){
        breakpoint_conditions->accumalator_bigger->mouse_pressed = true;
    }
    else if(breakpoint_conditions->accumalator_smaller->mouse_over){
        breakpoint_conditions->accumalator_smaller->mouse_pressed = true;
    }
    else if(breakpoint_conditions->x_equal->mouse_over){
        breakpoint_conditions->x_equal->mouse_pressed = true;
    }
    else if(breakpoint_conditions->x_bigger->mouse_over){
        breakpoint_conditions->x_bigger->mouse_pressed = true;
    }
    else if(breakpoint_conditions->x_smaller->mouse_over){
        breakpoint_conditions->x_smaller->mouse_pressed = true;
    }
    else if(breakpoint_conditions->y_equal->mouse_over){
        breakpoint_conditions->y_equal->mouse_pressed = true;
    }
    else if(breakpoint_conditions->y_bigger->mouse_over){
        breakpoint_conditions->y_bigger->mouse_pressed = true;
    }
    else if(breakpoint_conditions->y_smaller->mouse_over){
        breakpoint_conditions->y_smaller->mouse_pressed = true;
    }
}

static void BreakpointConditionsMouseUp(Breakpoint_Conditions *breakpoint_conditions){
    if(breakpoint_conditions->program_counter_breakpoint->mouse_pressed){
        
        if(breakpoint_conditions->program_counter_breakpoint->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,PC_BREAKPOINT,breakpoint_conditions->program_counter_breakpoint->text);
        }
        breakpoint_conditions->program_counter_breakpoint->mouse_pressed = false;
    }
    else if(breakpoint_conditions->stack_pointer_under_over->mouse_pressed){
        
        if(breakpoint_conditions->stack_pointer_under_over->mouse_pressed){
            BreakpointSetCondition(breakpoint_conditions,SP_UNDER_OVER,breakpoint_conditions->stack_pointer_under_over->text);
        }
        breakpoint_conditions->stack_pointer_under_over->mouse_pressed = false;
    }
    else if(breakpoint_conditions->accumalator_equal->mouse_pressed){

        if(breakpoint_conditions->accumalator_equal->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,ACCUMALATOR_EQUAL,breakpoint_conditions->accumalator_equal->text);
        }
        breakpoint_conditions->accumalator_equal->mouse_pressed = false;
    }
    else if(breakpoint_conditions->accumalator_bigger->mouse_pressed){

        if(breakpoint_conditions->accumalator_bigger->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,ACCUMALATOR_BIGGER,breakpoint_conditions->accumalator_bigger->text);
        }
        breakpoint_conditions->accumalator_bigger->mouse_pressed = false;
    }
    else if(breakpoint_conditions->accumalator_smaller->mouse_pressed){

        if(breakpoint_conditions->accumalator_smaller->mouse_over){
           BreakpointSetCondition(breakpoint_conditions,ACCUMALATOR_SMALLER,breakpoint_conditions->accumalator_smaller->text);
        }
        breakpoint_conditions->accumalator_smaller->mouse_pressed = false;
    }
    else if(breakpoint_conditions->x_equal->mouse_pressed){

        if(breakpoint_conditions->x_equal->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,X_EQUAL,breakpoint_conditions->x_equal->text);
        }
        breakpoint_conditions->x_equal->mouse_pressed = false;
    }
    else if(breakpoint_conditions->x_bigger->mouse_pressed){

        if(breakpoint_conditions->x_bigger->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,X_BIGGER,breakpoint_conditions->x_bigger->text);
        }
        breakpoint_conditions->x_bigger->mouse_pressed = false;
    }
    else if(breakpoint_conditions->x_smaller->mouse_pressed){

        if(breakpoint_conditions->x_smaller->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,X_SMALLER,breakpoint_conditions->x_smaller->text);
        }
        breakpoint_conditions->x_smaller->mouse_pressed = false;
    }
    else if(breakpoint_conditions->y_equal->mouse_pressed){

        if(breakpoint_conditions->y_equal->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,Y_EQUAL,breakpoint_conditions->y_equal->text);
        }
        breakpoint_conditions->y_equal->mouse_pressed = false;
    }
    else if(breakpoint_conditions->y_bigger->mouse_pressed){

        if(breakpoint_conditions->y_bigger->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,Y_BIGGER,breakpoint_conditions->y_bigger->text);
        }
        breakpoint_conditions->y_bigger->mouse_pressed = false;
    }
    else if(breakpoint_conditions->y_smaller->mouse_pressed){

        if(breakpoint_conditions->y_smaller->mouse_over){
            BreakpointSetCondition(breakpoint_conditions,Y_SMALLER,breakpoint_conditions->y_smaller->text);
        }
        breakpoint_conditions->y_smaller->mouse_pressed = false;
    }
}

static void ConfigBreakpointConditionsRect(Breakpoint_Conditions *breakpoint_conditions){
    breakpoint_conditions->rect = (Recti){
        .x = 0,
        .y = 0,
        .w = breakpoint_conditions->stack_pointer_under_over->text_size.x + BORDER * 2,
        .h = (breakpoint_conditions->stack_pointer_under_over->text_size.y + SPACING_Y) * 11
    };

    //Program Counter Breakpoint
    breakpoint_conditions->program_counter_breakpoint->rect = (Recti){
        .x = 0,
        .y = 0,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->program_counter_breakpoint->text_size.y + SPACING_Y
    };
    breakpoint_conditions->program_counter_breakpoint->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->program_counter_breakpoint->rect.y + breakpoint_conditions->program_counter_breakpoint->rect.h * 0.5f - breakpoint_conditions->program_counter_breakpoint->text_size.y * 0.5f
    };

    //Stack Pointer under over
    breakpoint_conditions->stack_pointer_under_over->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->program_counter_breakpoint->rect.y + breakpoint_conditions->program_counter_breakpoint->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->stack_pointer_under_over->text_size.y + SPACING_Y
    };
    breakpoint_conditions->stack_pointer_under_over->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->stack_pointer_under_over->rect.y + breakpoint_conditions->stack_pointer_under_over->rect.h * 0.5f - breakpoint_conditions->stack_pointer_under_over->text_size.y * 0.5f
    };

    //Acumalator ==
    breakpoint_conditions->accumalator_equal->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->stack_pointer_under_over->rect.y + breakpoint_conditions->stack_pointer_under_over->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->accumalator_equal->text_size.y + SPACING_Y
    };
    breakpoint_conditions->accumalator_equal->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->accumalator_equal->rect.y + breakpoint_conditions->accumalator_equal->rect.h * 0.5f - breakpoint_conditions->accumalator_equal->text_size.y * 0.5f
    };

    //Acumalator >
    breakpoint_conditions->accumalator_bigger->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->accumalator_equal->rect.y + breakpoint_conditions->accumalator_equal->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->accumalator_bigger->text_size.y + SPACING_Y
    };
    breakpoint_conditions->accumalator_bigger->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->accumalator_bigger->rect.y + breakpoint_conditions->accumalator_bigger->rect.h * 0.5f - breakpoint_conditions->accumalator_bigger->text_size.y * 0.5f
    };

    //Acumalator <
    breakpoint_conditions->accumalator_smaller->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->accumalator_bigger->rect.y + breakpoint_conditions->accumalator_bigger->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->accumalator_smaller->text_size.y + SPACING_Y
    };
    breakpoint_conditions->accumalator_smaller->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->accumalator_smaller->rect.y + breakpoint_conditions->accumalator_smaller->rect.h * 0.5f - breakpoint_conditions->accumalator_smaller->text_size.y * 0.5f
    };

    //X ==
    breakpoint_conditions->x_equal->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->accumalator_smaller->rect.y + breakpoint_conditions->accumalator_smaller->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->x_equal->text_size.y + SPACING_Y
    };
    breakpoint_conditions->x_equal->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->x_equal->rect.y + breakpoint_conditions->x_equal->rect.h * 0.5f - breakpoint_conditions->x_equal->text_size.y * 0.5f
    };

    //X >
    breakpoint_conditions->x_bigger->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->x_equal->rect.y + breakpoint_conditions->x_equal->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->x_bigger->text_size.y + SPACING_Y
    };
    breakpoint_conditions->x_bigger->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->x_bigger->rect.y + breakpoint_conditions->x_bigger->rect.h * 0.5f - breakpoint_conditions->x_bigger->text_size.y * 0.5f
    };

    //X <
    breakpoint_conditions->x_smaller->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->x_bigger->rect.y + breakpoint_conditions->x_bigger->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->x_smaller->text_size.y + SPACING_Y
    };
    breakpoint_conditions->x_smaller->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->x_smaller->rect.y + breakpoint_conditions->x_smaller->rect.h * 0.5f - breakpoint_conditions->x_smaller->text_size.y * 0.5f
    };

    //Y ==
    breakpoint_conditions->y_equal->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->x_smaller->rect.y + breakpoint_conditions->x_smaller->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->y_equal->text_size.y + SPACING_Y
    };
    breakpoint_conditions->y_equal->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->y_equal->rect.y + breakpoint_conditions->y_equal->rect.h * 0.5f - breakpoint_conditions->y_equal->text_size.y * 0.5f
    };

    //Y >
    breakpoint_conditions->y_bigger->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->y_equal->rect.y + breakpoint_conditions->y_equal->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->y_bigger->text_size.y + SPACING_Y
    };
    breakpoint_conditions->y_bigger->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->y_bigger->rect.y + breakpoint_conditions->y_bigger->rect.h * 0.5f - breakpoint_conditions->y_bigger->text_size.y * 0.5f
    };

    //Y <
    breakpoint_conditions->y_smaller->rect = (Recti){
        .x = 0,
        .y = breakpoint_conditions->y_bigger->rect.y + breakpoint_conditions->y_bigger->rect.h,
        .w = breakpoint_conditions->rect.w,
        .h = breakpoint_conditions->y_smaller->text_size.y + SPACING_Y
    };
    breakpoint_conditions->y_smaller->pos = (Vector2i){
        .x = BORDER,
        .y = breakpoint_conditions->y_smaller->rect.y + breakpoint_conditions->y_smaller->rect.h * 0.5f - breakpoint_conditions->y_smaller->text_size.y * 0.5f
    };
}

void BreakpointConditionsInit(Breakpoints *breakpoints){
    Breakpoint_Conditions *breakpoint_conditions = malloc(sizeof(Breakpoint_Conditions));
    
    breakpoint_conditions->rect = (Recti){0};
    breakpoint_conditions->opened = false;
    breakpoint_conditions->breakpoints = breakpoints;

    breakpoint_conditions->program_counter_breakpoint = CreateButtonText(L"PC Breakpoint",breakpoints->app->atlas);

    breakpoint_conditions->stack_pointer_under_over = CreateButtonText(L"SP Under/Overflow",breakpoints->app->atlas);
    
    breakpoint_conditions->accumalator_equal = CreateButtonText(L"Accumalator ==",breakpoints->app->atlas);
    breakpoint_conditions->accumalator_bigger = CreateButtonText(L"Accumalator >",breakpoints->app->atlas);
    breakpoint_conditions->accumalator_smaller = CreateButtonText(L"Accumalator <",breakpoints->app->atlas);
    
    breakpoint_conditions->x_equal = CreateButtonText(L"X ==",breakpoints->app->atlas);
    breakpoint_conditions->x_bigger = CreateButtonText(L"X >",breakpoints->app->atlas);
    breakpoint_conditions->x_smaller = CreateButtonText(L"X <",breakpoints->app->atlas);

    breakpoint_conditions->y_equal = CreateButtonText(L"Y ==",breakpoints->app->atlas);
    breakpoint_conditions->y_bigger = CreateButtonText(L"Y >",breakpoints->app->atlas);
    breakpoint_conditions->y_smaller = CreateButtonText(L"Y <",breakpoints->app->atlas);

    ConfigBreakpointConditionsRect(breakpoint_conditions);

    breakpoints->conditions = breakpoint_conditions;
}

void BreakpointConditionsEvents(Breakpoint_Conditions *breakpoint_conditions,SDL_Event event,Mouse mouse){
    if(!breakpoint_conditions->opened) return;

    BreakpointConditionsMouseOver(breakpoint_conditions,mouse);

    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
        
        if(!PointInRect(mouse.pos,breakpoint_conditions->rect)){
            breakpoint_conditions->opened = false;
            return;
        }

        BreakpointConditionsMousePressed(breakpoint_conditions);
    }
    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT){
        BreakpointConditionsMouseUp(breakpoint_conditions);
    }
}

void DrawBreakpointConditions(Renderer *renderer,Breakpoint_Conditions *breakpoint_conditions){
    RendererSetViewport(renderer,NULL,&breakpoint_conditions->rect);
    
    DrawFilledRectanglei(renderer,NULL,BACKGROUND3_COLOR);
    DrawRectanglei(renderer,NULL,BORDER_COLOR);

    //Program Counter Breakpoint
    DrawButtonText(renderer,breakpoint_conditions->program_counter_breakpoint,SELECTED_COLOR,COLOR_ON);
    //Stack Pointer Under/Overflow
    DrawButtonText(renderer,breakpoint_conditions->stack_pointer_under_over,SELECTED_COLOR,COLOR_ON);
    //Acumalator ==
    DrawButtonText(renderer,breakpoint_conditions->accumalator_equal,SELECTED_COLOR,COLOR_ON);
    //Acumalator >
    DrawButtonText(renderer,breakpoint_conditions->accumalator_bigger,SELECTED_COLOR,COLOR_ON);
    //Acumalator <
    DrawButtonText(renderer,breakpoint_conditions->accumalator_smaller,SELECTED_COLOR,COLOR_ON);
    //X ==
    DrawButtonText(renderer,breakpoint_conditions->x_equal,SELECTED_COLOR,COLOR_ON);
    //X >
    DrawButtonText(renderer,breakpoint_conditions->x_bigger,SELECTED_COLOR,COLOR_ON);
    //X <
    DrawButtonText(renderer,breakpoint_conditions->x_smaller,SELECTED_COLOR,COLOR_ON);
    //Y ==
    DrawButtonText(renderer,breakpoint_conditions->y_equal,SELECTED_COLOR,COLOR_ON);
    //Y >
    DrawButtonText(renderer,breakpoint_conditions->y_bigger,SELECTED_COLOR,COLOR_ON);
    //Y <
    DrawButtonText(renderer,breakpoint_conditions->y_smaller,SELECTED_COLOR,COLOR_ON);

    RendererSetViewport(renderer,NULL,NULL);
}

void BreakpointConditionsFree(Breakpoint_Conditions *breakpoint_conditions){
    ButtonTextFree(breakpoint_conditions->program_counter_breakpoint);

    ButtonTextFree(breakpoint_conditions->stack_pointer_under_over);

    ButtonTextFree(breakpoint_conditions->accumalator_equal);
    ButtonTextFree(breakpoint_conditions->accumalator_bigger);
    ButtonTextFree(breakpoint_conditions->accumalator_smaller);
    
    ButtonTextFree(breakpoint_conditions->x_equal);
    ButtonTextFree(breakpoint_conditions->x_bigger);
    ButtonTextFree(breakpoint_conditions->x_smaller);

    ButtonTextFree(breakpoint_conditions->y_equal);
    ButtonTextFree(breakpoint_conditions->y_bigger);
    ButtonTextFree(breakpoint_conditions->y_smaller);

    free(breakpoint_conditions);
}