#include <breakpoints.h>

void CheckBreakpoints(Breakpoints *breakpoints){

    if(!breakpoints->breakpoints || !breakpoints->app->instruction_run) return;

    CPU_6502 *cpu = breakpoints->app->cpu;
    Breakpoint *breakpoint = breakpoints->breakpoints;

    while(breakpoint != NULL){

        if(breakpoint->condition == SP_UNDER_OVER){
            if(cpu->stack_pointer_word < STACK_START){
                OpenPopUp(breakpoints->app->popup,L"A stack overflow occurred");
                breakpoints->app->instruction_run = false;
                break;
            }
            else if(cpu->stack_pointer_word > STACK_END){
                OpenPopUp(breakpoints->app->popup,L"A stack underflow occurred");
                breakpoints->app->instruction_run = false;
                break;
            }
        }
        else if(breakpoint->condition == PC_BREAKPOINT && cpu->program_counter == breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"Hit breakpoint at %.4X",cpu->program_counter);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == ACCUMALATOR_EQUAL && cpu->accumalator == (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"The accumulator is equal to %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == ACCUMALATOR_BIGGER && cpu->accumalator > (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"The accumulator is bigger than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == ACCUMALATOR_SMALLER && cpu->accumalator < (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"The accumulator is smaller than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == X_EQUAL && cpu->x_register == (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"X is equal to %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == X_BIGGER && cpu->x_register > (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"X is bigger than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == X_SMALLER && cpu->x_register < (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"X is smaller than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == Y_EQUAL && cpu->y_register == (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"Y is equal to %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == Y_BIGGER && cpu->y_register > (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"Y is bigger than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }
        else if(breakpoint->condition == Y_SMALLER && cpu->y_register < (uint8_t)breakpoint->value){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"Y is smaller than %.2X",breakpoint->value);
            OpenPopUp(breakpoints->app->popup,breakpoints->app->buffer);            
            breakpoints->app->instruction_run = false;
            break;
        }

        breakpoint = breakpoint->next;
    }
}

void BreakpointsAjustVerticalScrollBarOffset(Breakpoints *breakpoints){
    
    if(!breakpoints->vertical_scroll_bar.valid || !breakpoints->current_breakpoint) return;

    float length_diff = breakpoints->vertical_scroll_bar.length - breakpoints->vertical_scroll_bar.bar_rect.h;
    float scroll_length = breakpoints->vertical_scroll_bar.bar_rect.h - breakpoints->vertical_scroll_bar.scroll_rect.h;

    if(breakpoints->current_breakpoint->rect.y + breakpoints->current_breakpoint->rect.h > breakpoints->rect.h){

        float diff = (breakpoints->current_breakpoint->rect.y + breakpoints->current_breakpoint->rect.h) - breakpoints->rect.h;
        breakpoints->vertical_scroll_bar.scroll_rect.y +=  diff / length_diff * scroll_length;
    }
    else if(breakpoints->current_breakpoint->rect.y  < 0){

        float diff = breakpoints->current_breakpoint->rect.y;
        breakpoints->vertical_scroll_bar.scroll_rect.y +=  diff / length_diff * scroll_length;
    }

    SetVerticalOffsetScrollBar(&breakpoints->vertical_scroll_bar);
}

void BreakpointsConfigVerticalScrollBar(Breakpoints *breakpoints){
    Breakpoint *breakpoint = breakpoints->breakpoints;
    uint32_t count = 0;
    
    while(breakpoint){count++;breakpoint = breakpoint->next;}
    
    breakpoints->vertical_scroll_bar.length = count * (breakpoints->app->glyph_size.y + BREAKPOINT_SPACING_Y * 4);
    
    breakpoints->vertical_scroll_bar.scroll_rect.h = GetScrollBarHeight(breakpoints->vertical_scroll_bar);
    
    if(breakpoints->vertical_scroll_bar.scroll_rect.h > 0.0f && breakpoints->vertical_scroll_bar.scroll_rect.h < breakpoints->vertical_scroll_bar.bar_rect.h){
        breakpoints->vertical_scroll_bar.valid = true;
        SetYScrollBar(&breakpoints->vertical_scroll_bar,breakpoints->vertical_scroll_bar.offset);
    }
    else{
        breakpoints->vertical_scroll_bar.valid = false;
        breakpoints->vertical_scroll_bar.offset = 0.0f;
        breakpoints->vertical_scroll_bar.scroll_rect.y = breakpoints->vertical_scroll_bar.bar_rect.y;
    }
}


void ConfigBreakpointsRect(Breakpoints *breakpoints){

    Breakpoint *breakpoint = breakpoints->breakpoints;
    while(breakpoint){
        
        breakpoint->rect = (Rectf){
            .x = 0,
            .y = (breakpoint->previous) ? breakpoint->previous->rect.y + breakpoint->previous->rect.h : breakpoints->vertical_scroll_bar.offset,
            .w = breakpoints->rect.w,
            .h = breakpoints->app->glyph_size.y + BREAKPOINT_SPACING_Y * 4
        };

        breakpoint->condition_button->rect = (Recti){
            .x = breakpoint->rect.x + BREAKPOINT_SPACING_X,
            .y = breakpoint->rect.y + breakpoint->rect.h * 0.5f - (breakpoint->condition_button->text_size.y + BREAKPOINT_SPACING_Y * 2) * 0.5f,
            .w = breakpoints->conditions->rect.w,
            .h = breakpoint->condition_button->text_size.y + BREAKPOINT_SPACING_Y * 2
        };
        breakpoint->condition_button->pos = (Vector2i){
            .x = breakpoint->condition_button->rect.x + BORDER,
            .y = breakpoint->condition_button->rect.y + breakpoint->condition_button->rect.h * 0.5f - breakpoint->condition_button->text_size.y * 0.5f
        };

        if(breakpoint->condition & USE_INPUT){

            breakpoint->value_input->rect = (Recti){
                .x = breakpoint->condition_button->rect.x + breakpoint->condition_button->rect.w + BREAKPOINT_SPACING_X,
                .y = breakpoint->rect.y + breakpoint->rect.h * 0.5f - (breakpoints->app->glyph_size.y + INPUT_SPACING_Y * 2) * 0.5f,
                .w = breakpoints->app->glyph_size.x * breakpoint->value_input->max_length + (INPUT_SPACING_X * 2),
                .h = breakpoints->app->glyph_size.y + (INPUT_SPACING_Y * 2)
            };
            breakpoint->value_input->pos = (Vector2i){
                .x = breakpoint->value_input->rect.x + INPUT_SPACING_X,
                .y = breakpoint->value_input->rect.y + INPUT_SPACING_Y
            };
        }

        breakpoint = breakpoint->next;
    }
}


void PushBreakpoint(Breakpoints *breakpoints){
    Breakpoint *breakpoint = malloc(sizeof(Breakpoint));
    breakpoint->condition = PC_BREAKPOINT;
    breakpoint->value = 0x0000;
    breakpoint->mouse_over = false;
    breakpoint->condition_button = CreateButtonText(L"PC Breakpoint",breakpoints->app->atlas);
    breakpoint->value_input = CreateInput(4,FORMAT_HEX);
    breakpoint->rect = (Rectf){0};
    breakpoint->previous = NULL;
    breakpoint->next = NULL;

    if(!breakpoints->breakpoints){
        breakpoints->breakpoints = breakpoint;
    }
    else{
        Breakpoint *buffer = breakpoints->breakpoints;
        while(buffer->next){buffer = buffer->next;}
        buffer->next = breakpoint;
        breakpoint->previous = buffer;
    }

    BreakpointsConfigVerticalScrollBar(breakpoints);
    ConfigBreakpointsRect(breakpoints);
}

void RemoveBreakpoint(Breakpoints *breakpoints){
    if(!breakpoints->current_breakpoint) return;

    Breakpoint *breakpoint = breakpoints->current_breakpoint;
    
    breakpoints->current_breakpoint = NULL;

    if(breakpoint->previous){
        Breakpoint *previous = breakpoint->previous;
        previous->next = breakpoint->next;

        if(breakpoint->next){
            Breakpoint *next = breakpoint->next;
            next->previous = breakpoint->previous;
        }
    }
    else{
        breakpoints->breakpoints = breakpoint->next;
        if(breakpoints->breakpoints){
            breakpoints->breakpoints->previous = NULL;
        }
    }

    if(breakpoint->value_input == breakpoints->app->current_input){
        breakpoints->app->current_input = NULL;
    }
    
    ButtonTextFree(breakpoint->condition_button);
    InputFree(breakpoint->value_input);
    free(breakpoint);

    BreakpointsConfigVerticalScrollBar(breakpoints);
    ConfigBreakpointsRect(breakpoints);
}


void BreakpointsInit(Breakpoints *breakpoints,App *app){
    breakpoints->app = app;
    breakpoints->label = CreateLabel(L"Breakpoints:",app->atlas);
    breakpoints->new_button = CreateButtonText(L"New",app->atlas);
    breakpoints->delete_button = CreateButtonText(L"Delete",app->atlas);
    breakpoints->breakpoints = NULL;
    breakpoints->current_breakpoint = NULL;
    memset(&breakpoints->vertical_scroll_bar,0,sizeof(Scroll_Bar));

    BreakpointConditionsInit(breakpoints);

    breakpoints->BreakpointsAjustVerticalScrollBarOffset = BreakpointsAjustVerticalScrollBarOffset;
    breakpoints->BreakpointsConfigVerticalScrollBar = BreakpointsConfigVerticalScrollBar;

    breakpoints->ConfigBreakpointsRect = ConfigBreakpointsRect;

    breakpoints->PushBreakpoint = PushBreakpoint;
    breakpoints->RemoveBreakpoint = RemoveBreakpoint;
}

void BreakpointsConfigRect(Breakpoints *breakpoints){
    
    breakpoints->label->rect = (Recti){
        .x = breakpoints->app->stack_rect.x + breakpoints->app->stack_rect.w + BORDER,
        .y = BORDER,
        .w = breakpoints->label->text_size.x,
        .h = breakpoints->label->text_size.y + SPACING_Y
    };
    breakpoints->label->pos = (Vector2i){
        .x = breakpoints->label->rect.x + breakpoints->label->rect.w * 0.5f - breakpoints->label->text_size.x * 0.5f,
        .y = breakpoints->label->rect.y + breakpoints->label->rect.h * 0.5f - breakpoints->label->text_size.y * 0.5f
    };

    breakpoints->delete_button->rect = (Recti){
        .x = (breakpoints->app->screen_size.x - BORDER) - breakpoints->delete_button->text_size.x,
        .y = BORDER,
        .w = breakpoints->delete_button->text_size.x,
        .h = breakpoints->delete_button->text_size.y + SPACING_Y
    };
    breakpoints->delete_button->pos = (Vector2i){
        .x = breakpoints->delete_button->rect.x + breakpoints->delete_button->rect.w * 0.5f - breakpoints->delete_button->text_size.x * 0.5f,
        .y = breakpoints->delete_button->rect.y + breakpoints->delete_button->rect.h * 0.5f - breakpoints->delete_button->text_size.y * 0.5f
    };

    breakpoints->new_button->rect = (Recti){
        .x = breakpoints->delete_button->rect.x - (BORDER * 2) - breakpoints->new_button->text_size.x,
        .y = BORDER,
        .w = breakpoints->new_button->text_size.x,
        .h = breakpoints->new_button->text_size.y + SPACING_Y
    };
    breakpoints->new_button->pos = (Vector2i){
        .x = breakpoints->new_button->rect.x + breakpoints->new_button->rect.w * 0.5f - breakpoints->new_button->text_size.x * 0.5f,
        .y = breakpoints->new_button->rect.y + breakpoints->new_button->rect.h * 0.5f - breakpoints->new_button->text_size.y * 0.5f
    };

    //(screen_height - border * 3) * 0.4f - breakpoints->label->rect.h
    breakpoints->rect = (Recti){
        .x = breakpoints->app->stack_rect.x + breakpoints->app->stack_rect.w + BORDER,
        .y = breakpoints->label->rect.y + breakpoints->label->rect.h,
        .w = (breakpoints->app->screen_size.x - BORDER) - (breakpoints->app->stack_rect.x + breakpoints->app->stack_rect.w + BORDER),
        .h = ((breakpoints->app->screen_size.y - (BORDER * 3.0f)) * 0.4f) - breakpoints->label->rect.h
    };

    breakpoints->vertical_scroll_bar.bar_rect = (Recti){
        .x = breakpoints->rect.x + breakpoints->rect.w - SCROLL_BAR_SIZE,
        .y = breakpoints->rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = breakpoints->rect.h
    };

    breakpoints->vertical_scroll_bar.scroll_rect = (Rectf){
        .x = breakpoints->vertical_scroll_bar.bar_rect.x,
        .y = breakpoints->vertical_scroll_bar.bar_rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = 0.0f
    };

    BreakpointsConfigVerticalScrollBar(breakpoints);

    ConfigBreakpointsRect(breakpoints);
}

void BreakpointsDraw(Breakpoints *breakpoints){
    Renderer *renderer = breakpoints->app->renderer;
    FontAtlas *atlas = breakpoints->app->atlas;

    //Label
    DrawLabel(renderer,breakpoints->label,COLOR_ON);

    //New
    DrawButtonText(renderer,breakpoints->new_button,SELECTED_COLOR,COLOR_ON);

    //Delete
    DrawButtonText(renderer,breakpoints->delete_button,SELECTED_COLOR,COLOR_ON);

    //Rect
    DrawFilledRectanglei(renderer,&breakpoints->rect,BACKGROUND2_COLOR);
    
    //Breakpoints
    RendererSetViewport(renderer,NULL,&breakpoints->rect);

    Breakpoint *breakpoint = breakpoints->breakpoints;

    while(breakpoint != NULL){

        if(breakpoint != breakpoints->current_breakpoint){
            if(breakpoint->mouse_over){
                DrawFilledRectanglef(renderer,&breakpoint->rect,BREAKPOINT_SELECTED_COLOR);
            }

            if(breakpoint->condition & USE_INPUT){
                swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"%ls %.*X",breakpoint->condition_button->text,breakpoint->condition & INPUT_BYTES,breakpoint->value);
                DrawTextUint16i(renderer,atlas,breakpoints->app->buffer,breakpoint->condition_button->pos,COLOR_ON);
            }
            else{
                DrawTextUint16i(renderer,atlas,breakpoint->condition_button->text,breakpoint->condition_button->pos,COLOR_ON);
            }

            breakpoint->mouse_over = false;
        }

        breakpoint = breakpoint->next;
    }

    if(breakpoints->current_breakpoint){
        
        breakpoint = breakpoints->current_breakpoint;

        DrawFilledRectanglef(renderer,&breakpoint->rect,BREAKPOINT_SELECTED_COLOR);
        DrawRectanglef(renderer,&breakpoint->rect,BORDER_COLOR);

        DrawRectanglei(renderer,&breakpoint->condition_button->rect,BORDER_COLOR);

        if(breakpoint->condition_button->mouse_over){
            DrawTextUint16i(renderer,atlas,breakpoint->condition_button->text,breakpoint->condition_button->pos,SELECTED_COLOR);
            breakpoint->condition_button->mouse_over = false;
        }
        else{
            DrawTextUint16i(renderer,atlas,breakpoint->condition_button->text,breakpoint->condition_button->pos,COLOR_ON);
        }
            
        if(breakpoint->condition & INPUT_BYTES){
            swprintf(breakpoints->app->buffer,BUFFER_SIZE,L"%.*X",breakpoint->value_input->max_length,breakpoint->value);
                
            DrawRectanglei(renderer,&breakpoint->value_input->rect,BORDER_COLOR);

            if(breakpoint->value_input == breakpoints->app->current_input){
                    
                UpdateCursor(&breakpoint->value_input->cursor,breakpoints->app->current_time);
                    
                if(breakpoint->value_input->cursor.visible){
                    Vector2i point1 = {
                        .x = breakpoint->value_input->pos.x + breakpoint->value_input->cursor.pos.x * breakpoints->app->glyph_size.x,
                        .y = breakpoint->value_input->rect.y + breakpoint->value_input->rect.h * 0.5f - breakpoints->app->glyph_size.y * 0.5f
                    };
                    Vector2i point2 = {
                        .x = point1.x,
                        .y = point1.y + breakpoints->app->glyph_size.y
                    };
                    DrawLinei(renderer,point1,point2,CURSOR_COLOR);
                }
                    
                DrawTextByGapBufferUint16i(renderer,atlas,breakpoint->value_input->gap_buffer,breakpoint->value_input->pos,COLOR_ON);
            }
            else{
                if(breakpoint->value_input->mouse_over){
                    DrawTextUint16i(renderer,atlas,breakpoints->app->buffer,breakpoint->value_input->pos,SELECTED_COLOR);
                }
                else{
                    DrawTextUint16i(renderer,atlas,breakpoints->app->buffer,breakpoint->value_input->pos,COLOR_ON);
                }
            }

            breakpoint->value_input->mouse_over = false;
        }

        breakpoint->mouse_over = false;
    }

    RendererSetViewport(renderer,NULL,NULL);

    //Vertical Scroll Bar
    DrawScrollBar(&breakpoints->vertical_scroll_bar,renderer);
}


static void BreakpointsMouseOver(Breakpoints *breakpoints){
    if(breakpoints->app->element_on) return;

    Vector2i position = breakpoints->app->mouse.pos;

    if(PointInRect(position,breakpoints->new_button->rect)){
        breakpoints->new_button->mouse_over = true;
        breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,breakpoints->delete_button->rect)){
        breakpoints->delete_button->mouse_over = true;
        breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,breakpoints->rect)){
        
        if(PointInRect(position,breakpoints->vertical_scroll_bar.scroll_rect) && breakpoints->vertical_scroll_bar.valid){
            breakpoints->vertical_scroll_bar.mouse_over = true;
            breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
        }
        else{
            Breakpoint *breakpoint = breakpoints->breakpoints;
            Vector2i relative_position = {
                position.x - breakpoints->rect.x,
                position.y - breakpoints->rect.y
            };
            
            while(breakpoint){
                
                if(!PointInRect(relative_position,breakpoint->rect)){
                    breakpoint = breakpoint->next;
                    continue;
                }
                
                breakpoint->mouse_over = true;

                if(breakpoint == breakpoints->current_breakpoint){
                        
                    if(PointInRect(relative_position,breakpoint->condition_button->rect)){
                        breakpoint->condition_button->mouse_over = true;
                        breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
                    }
                    else if(breakpoint->condition & USE_INPUT && PointInRect(relative_position,breakpoint->value_input->rect)){
                        breakpoint->value_input->mouse_over = true;
                        breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
                    }
                    else{
                        breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
                    }
                }
                else{
                    breakpoints->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
                }

                break;
            }
        }
    }
}

static void BreakpointsMouseDown(Breakpoints *breakpoints){
    
    if(breakpoints->new_button->mouse_over){
        breakpoints->new_button->mouse_pressed = true;
    }
    else if(breakpoints->delete_button->mouse_over){
        breakpoints->delete_button->mouse_pressed = true;
    }
    else if(PointInRect(breakpoints->app->mouse.pos,breakpoints->rect)){
        
        if(breakpoints->vertical_scroll_bar.mouse_over){
            
            breakpoints->vertical_scroll_bar.on = true;
            breakpoints->vertical_scroll_bar.mouse_diff = breakpoints->app->mouse.pos.y - breakpoints->vertical_scroll_bar.scroll_rect.y;

            breakpoints->app->element_on = true;
        }
        else{
            Breakpoint *breakpoint = breakpoints->breakpoints;
            Breakpoint *breakpoint_selected = NULL;
            uint32_t selected_part = NONE;
            
            while(breakpoint){
                
                if(!breakpoint->mouse_over){
                    breakpoint = breakpoint->next;
                    continue;
                }
                
                breakpoint_selected = breakpoint;

                if(breakpoint == breakpoints->current_breakpoint){
                        
                    if(breakpoint->condition_button->mouse_over){
                        selected_part = BREAKPOINT_CONDITION;
                    }else if(breakpoint->condition & USE_INPUT && breakpoint->value_input->mouse_over){
                        selected_part = BREAKPOINT_INPUT;
                    }
                    else{
                        selected_part = BREAKPOINT;
                    }
                }
                
                break;
            }
            
            if(
                breakpoints->current_breakpoint && 
                breakpoints->current_breakpoint->condition & USE_INPUT && 
                breakpoints->current_breakpoint->value_input == breakpoints->app->current_input && 
                (breakpoints->current_breakpoint != breakpoint_selected || selected_part != BREAKPOINT_INPUT)
            ){
                close_input(breakpoints->app);
            }
            
            if(selected_part & BREAKPOINT_CONDITION){

                if(!breakpoints->conditions->opened){
                    breakpoints->app->event.type = SDL_FIRSTEVENT;
                    
                    breakpoints->conditions->opened = true;
                    
                    BreakpointsAjustVerticalScrollBarOffset(breakpoints);
                    ConfigBreakpointsRect(breakpoints);

                    breakpoints->conditions->rect.x = breakpoints->rect.x + breakpoint_selected->condition_button->rect.x;
                    breakpoints->conditions->rect.y = breakpoints->rect.y + breakpoint_selected->condition_button->rect.y + breakpoint_selected->condition_button->rect.h - 1;
                }
                else{
                    breakpoints->conditions->opened = false;
                }
            }
            else if(selected_part & BREAKPOINT_INPUT){
                if(breakpoint_selected->value_input != breakpoints->app->current_input){
                    open_input(breakpoints->app,breakpoint_selected->value_input,breakpoint_selected->value);
                }
                else{
                    set_pos_in_input(breakpoints->app);
                }
            }

            breakpoints->current_breakpoint = breakpoint_selected;
        }
    }
    else{
        if(breakpoints->current_breakpoint && breakpoints->current_breakpoint->condition & USE_INPUT && breakpoints->current_breakpoint->value_input == breakpoints->app->current_input){
            close_input(breakpoints->app);
        }
        breakpoints->current_breakpoint = NULL;
    }
}

static void BreakpointsMouseUp(Breakpoints *breakpoints){
    if(breakpoints->new_button->mouse_pressed){
        if(breakpoints->new_button->mouse_over){
            PushBreakpoint(breakpoints);
        }
        breakpoints->new_button->mouse_pressed = false;
    }
    else if(breakpoints->delete_button->mouse_pressed){
        if(breakpoints->delete_button->mouse_over){
            RemoveBreakpoint(breakpoints);
        }
        breakpoints->delete_button->mouse_pressed = false;
    }
}

void BreakpointsEvent(Breakpoints *breakpoints){
    if(breakpoints->conditions->opened || breakpoints->app->instruction_run || breakpoints->app->popup->opened) return;

    SDL_Event event = breakpoints->app->event;

    BreakpointsMouseOver(breakpoints);

    if(event.type == SDL_MOUSEWHEEL && PointInRect(breakpoints->app->mouse.pos,breakpoints->rect) && !breakpoints->conditions->opened){
        
        if(event.wheel.preciseY < 0.0f){
            //up content
            SetYScrollBar(&breakpoints->vertical_scroll_bar,breakpoints->vertical_scroll_bar.offset - (breakpoints->app->glyph_size.y + BREAKPOINT_SPACING_Y * 4));
        }
        else if(event.wheel.preciseY > 0.0f){
            //down content
            SetYScrollBar(&breakpoints->vertical_scroll_bar,breakpoints->vertical_scroll_bar.offset + (breakpoints->app->glyph_size.y + BREAKPOINT_SPACING_Y * 4));
        }
        ConfigBreakpointsRect(breakpoints);
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN && SDL_BUTTON_LEFT){
        BreakpointsMouseDown(breakpoints);
    }
    else if(event.type == SDL_MOUSEBUTTONUP && SDL_BUTTON_LEFT){
        BreakpointsMouseUp(breakpoints);

        breakpoints->vertical_scroll_bar.on = false;
    }
    else if(SDL_BUTTON(breakpoints->app->mouse.state) & SDL_BUTTON_LEFT && breakpoints->vertical_scroll_bar.on){
        HandleVerticalScrollBar(&breakpoints->vertical_scroll_bar,breakpoints->app->mouse);
        ConfigBreakpointsRect(breakpoints);
    }
}

void BreakpointsFree(Breakpoints *breakpoints){
    LabelFree(breakpoints->label);

    ButtonTextFree(breakpoints->new_button);
    ButtonTextFree(breakpoints->delete_button);

    BreakpointConditionsFree(breakpoints->conditions);

    Breakpoint *breakpoint = breakpoints->breakpoints;
    while(breakpoint != NULL){
        ButtonTextFree(breakpoint->condition_button);
        InputFree(breakpoint->value_input);
        if(breakpoint->value_input == breakpoints->app->current_input){
            breakpoints->app->current_input = NULL;
        }
        breakpoint = breakpoint->next;
    }

    free(breakpoints);
}