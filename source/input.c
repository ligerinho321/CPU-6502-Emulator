#include <input.h>

wchar_t* wcscasechr(wchar_t *string,wchar_t character){
    wchar_t upper_chr = towupper(character);
    while(*string){
        if(towupper(*string) == upper_chr) return string;
        string++;
    }
    return NULL;
}

void input_mouse_over(App *app){
    if(app->element_on) return;
    
    if(PointInRect(app->mouse.pos,app->registers->pc->rect)){
        app->registers->pc->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->registers->ar->rect)){
        app->registers->ar->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->registers->xr->rect)){
        app->registers->xr->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->registers->yr->rect)){
        app->registers->yr->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->registers->sr->rect)){
        app->registers->sr->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->registers->sp->rect)){
        app->registers->sp->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->instruction_runtime_input->rect)){
        app->instruction_runtime_input->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->seek_to_input->rect)){
        app->seek_to_input->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
    else if(PointInRect(app->mouse.pos,app->source_editor->start_address_input->rect)){
        app->source_editor->start_address_input->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
}

void input_mouse_button_down(App *app){
    
    if(app->current_input && app->current_input->mouse_over){
        set_pos_in_input(app);
    }
    else if(app->registers->pc->mouse_over){
        open_input(app,app->registers->pc,app->cpu->program_counter);
    }
    else if(app->registers->ar->mouse_over){
        open_input(app,app->registers->ar,app->cpu->accumalator);
    }
    else if(app->registers->xr->mouse_over){
        open_input(app,app->registers->xr,app->cpu->x_register);
    }
    else if(app->registers->yr->mouse_over){
        open_input(app,app->registers->yr,app->cpu->y_register);
    }
    else if(app->registers->sr->mouse_over){
        open_input(app,app->registers->sr,app->cpu->status_register);
    }
    else if(app->registers->sp->mouse_over){
        open_input(app,app->registers->sp,app->cpu->stack_pointer);
    }
    else if(app->instruction_runtime_input->mouse_over){
        open_input(app,app->instruction_runtime_input,app->instruction_runtime);
    }
    else if(app->seek_to_input->mouse_over){
        open_input(app,app->seek_to_input,app->address);
    }
    else if(app->source_editor->start_address_input->mouse_over){
        open_input(app,app->source_editor->start_address_input,app->source_editor->start_address);
    }
    else if(!app->breakpoints->current_breakpoint || !app->breakpoints->current_breakpoint->value_input || app->breakpoints->current_breakpoint->value_input != app->current_input){
        close_input(app);
    }
}


Vector2i set_pos_in_input(App *app){
    int x = 0;
    if(app->breakpoints->current_breakpoint && app->breakpoints->current_breakpoint->value_input && app->breakpoints->current_breakpoint->value_input == app->current_input){
        x = round((app->mouse.pos.x - app->breakpoints->rect.x - app->current_input->pos.x) / (float)app->glyph_size.x);
    }
    else{
        x = round((app->mouse.pos.x - app->current_input->pos.x) / (float)app->glyph_size.x);
    }

    uint32_t content_size = ContentSize(app->current_input->gap_buffer,wchar_t);
    if(x < 0){
        x = 0;
    }
    else if(x > content_size){
        x = content_size;
    }
    if(x != app->current_input->cursor.pos.x){
        app->current_input->cursor.update = true;
        GapBufferMoveGap(app->current_input->gap_buffer,wchar_t,x);
    }
    app->current_input->cursor.pos.x = x;
}

void open_input(App *app,Input *input,uint32_t value){
    if(app->current_input) close_input(app);
    if(!input->max_length) return;
    
    if(input->format & FORMAT_HEX){
        swprintf(app->buffer,BUFFER_SIZE,L"%.*X",input->max_length,value);
    }
    else if(input->format & FORMAT_DEC){
        swprintf(app->buffer,BUFFER_SIZE,L"%d",value);
    }

    GapBufferClear(input->gap_buffer);
    GapBufferInsertValues(input->gap_buffer,wchar_t,app->buffer,wcslen(app->buffer));

    input->cursor.pos.x = ContentSize(input->gap_buffer,wchar_t);
    input->cursor.update = true;
    app->current_input = input;
}

void close_input(App *app){
    if(!app->current_input) return;

    uint32_t content_size = ContentSize(app->current_input->gap_buffer,wchar_t);
    GapBufferMoveGap(app->current_input->gap_buffer,wchar_t,content_size);
    GapBufferInsertValue(app->current_input->gap_buffer,wchar_t,L'\0');
    long int value = 0;

    if(app->current_input->format & FORMAT_HEX){
        value = wcstol(app->current_input->gap_buffer->buffer,NULL,16);
    }
    else if(app->current_input->format & FORMAT_DEC){
        value = wcstol(app->current_input->gap_buffer->buffer,NULL,10);
    }

    if(app->current_input == app->registers->pc){
        app->cpu->program_counter = value;
    }
    else if(app->current_input == app->registers->ar){
        app->cpu->accumalator = value;
    }
    else if(app->current_input == app->registers->xr){
        app->cpu->x_register = value;
    }
    else if(app->current_input == app->registers->yr){
        app->cpu->y_register = value;
    }
    else if(app->current_input == app->registers->sr){
        app->cpu->status_register = value;
    }
    else if(app->current_input == app->registers->sp){
        app->cpu->stack_pointer = value;
        app->cpu->stack_pointer_word = STACK_START + value;
    }
    else if(app->current_input == app->seek_to_input){
        app->address = value;
    }
    else if(app->current_input == app->instruction_runtime_input){
        app->instruction_runtime = value;
    }
    else if(app->current_input == app->source_editor->start_address_input){
        app->source_editor->start_address = value;
    }
    else if(app->breakpoints->current_breakpoint && app->current_input == app->breakpoints->current_breakpoint->value_input){
        app->breakpoints->current_breakpoint->value = value;
    }

    app->current_input = NULL;
}

void input_events(App *app){
    if(app->breakpoints->conditions->opened || app->popup->opened) return;
    
    input_mouse_over(app);

    if(app->event.type == SDL_MOUSEBUTTONDOWN && app->event.button.button == SDL_BUTTON_LEFT){
        input_mouse_button_down(app);
    }
    else if(app->event.type == SDL_TEXTINPUT && app->current_input){
        static wchar_t wide_string[SDL_TEXTINPUTEVENT_TEXT_SIZE] = {0};

        if(MultiByteToWideChar(CP_UTF8,MB_COMPOSITE,app->event.text.text,-1,wide_string,SDL_TEXTINPUTEVENT_TEXT_SIZE) == 0){
            printf("Error in char to wchar converter: %d\n",GetLastError());
        }
        else{
            wchar_t *entry = NULL;
            
            if(app->current_input->format & FORMAT_HEX){
                entry = wcscasechr(L"0123456789ABCDEF",wide_string[0]);
            }
            else if(app->current_input->format & FORMAT_DEC){
                entry = wcscasechr(L"0123456789",wide_string[0]);
            }
            
            if(ContentSize(app->current_input->gap_buffer,wchar_t) < app->current_input->max_length && entry){
                
                wchar_t upper_chr = towupper(wide_string[0]);
                GapBufferInsertValue(app->current_input->gap_buffer,wchar_t,upper_chr);
                app->current_input->cursor.pos.x++;
                app->current_input->cursor.update = true;
            }
        }
    }
    else if(app->event.type == SDL_KEYDOWN && app->current_input){
        
        if(app->event.key.keysym.scancode == SDL_SCANCODE_RETURN){
            close_input(app);
        }
        if(app->event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && app->current_input->cursor.pos.x > 0){
            GapBufferRemove(app->current_input->gap_buffer,wchar_t,1);
            app->current_input->cursor.pos.x--;
            app->current_input->cursor.update = true;
        }
        if(app->event.key.keysym.scancode == SDL_SCANCODE_LEFT && app->current_input->cursor.pos.x > 0){
            GapBufferMoveGapLeft(app->current_input->gap_buffer,wchar_t,1);
            app->current_input->cursor.pos.x--;
            app->current_input->cursor.update = true;
        }
        else if(app->event.key.keysym.scancode == SDL_SCANCODE_RIGHT && app->current_input->cursor.pos.x < ContentSize(app->current_input->gap_buffer,wchar_t)){
            GapBufferMoveGapRight(app->current_input->gap_buffer,wchar_t,1);
            app->current_input->cursor.pos.x++;
            app->current_input->cursor.update = true;
        }
    }
}