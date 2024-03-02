#include <utils.h>
#include <source_editor.h>
#include <registers.h>
#include <scroll_bar.h>
#include <input.h>
#include <breakpoints.h>

void MouseOver(App *app){
    if(app->breakpoints->conditions->opened || app->element_on || app->popup->opened) return;

    if(PointInRect(app->mouse.pos,app->step_button->rect) && !app->instruction_run){
        app->step_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->run_button->rect) && !app->instruction_run){
        app->run_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->stop_button->rect) && app->instruction_run){
        app->stop_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->reset_button->rect)){
        app->reset_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->irq_button->rect)){
        app->irq_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->nmi_button->rect)){
        app->nmi_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->seek_to_button->rect)){
        app->seek_to_button->mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(app->mouse.pos,app->memory_vertical_scroll_bar.scroll_rect) && app->memory_vertical_scroll_bar.valid){
        app->memory_vertical_scroll_bar.mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else if(PointInRect(app->mouse.pos,app->memory_rect)){
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else if(PointInRect(app->mouse.pos,app->stack_vertical_scroll_bar.scroll_rect) && app->stack_vertical_scroll_bar.valid){
        app->stack_vertical_scroll_bar.mouse_over = true;
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else if(PointInRect(app->mouse.pos,app->stack_rect)){
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else{
        app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
}

void MouseButtonDown(App *app){
    
    if(app->breakpoints->conditions->opened || app->popup->opened) return;

    if(app->step_button->mouse_over){
        app->step_button->mouse_pressed = true;
    }
    if(app->run_button->mouse_over){
        app->run_button->mouse_pressed = true;
    }
    else if(app->stop_button->mouse_over){
        app->stop_button->mouse_pressed = true;
    }
    else if(app->reset_button->mouse_over){
        app->reset_button->mouse_pressed  =true;
    }
    else if(app->irq_button->mouse_over){
        app->irq_button->mouse_pressed = true;
    }
    else if(app->nmi_button->mouse_over){
        app->nmi_button->mouse_pressed = true;
    }
    else if(app->seek_to_button->mouse_over){
        app->seek_to_button->mouse_pressed = true;
    }
    else if(app->memory_vertical_scroll_bar.mouse_over){
        app->memory_vertical_scroll_bar.on = true;
        app->memory_vertical_scroll_bar.mouse_diff = app->mouse.pos.y - app->memory_vertical_scroll_bar.scroll_rect.y;

        app->element_on = true;
    }
    else if(app->stack_vertical_scroll_bar.mouse_over){
        app->stack_vertical_scroll_bar.on = true;
        app->stack_vertical_scroll_bar.mouse_diff = app->mouse.pos.y - app->stack_vertical_scroll_bar.scroll_rect.y;

        app->element_on = true;
    }
}

void MouseButtonUp(App *app){
    if(app->breakpoints->conditions->opened || app->popup->opened) return;

    if(app->step_button->mouse_pressed){
        if(app->step_button->mouse_over){
            app->instruction_step = true;
            app->start_execution = app->current_time;
        }
        app->step_button->mouse_pressed = false;
    }
    if(app->run_button->mouse_pressed){
        if(app->run_button->mouse_over){
            app->instruction_run = true;
            app->start_execution = app->current_time;
        }
        app->run_button->mouse_pressed = false;
    }
    else if(app->stop_button->mouse_pressed){
        if(app->stop_button->mouse_over){
            app->instruction_run = false;
        }
        app->stop_button->mouse_pressed = false;
    }
    else if(app->reset_button->mouse_pressed){
        if(app->reset_button->mouse_over){
            CPU_6502_RES(app->cpu);
            app->instruction_step = false;
            app->instruction_run = false;
        }
        app->reset_button->mouse_pressed = false;
    }
    else if(app->irq_button->mouse_pressed){
        if(app->irq_button->mouse_over && !(app->cpu->status_register & CPU_6502_FLAG_I)){
            CPU_6502_IRQ(app->cpu);
            app->instruction_step = false;
            app->instruction_run = false;
        }
        app->irq_button->mouse_pressed = false;
    }
    else if(app->nmi_button->mouse_pressed){
        if(app->nmi_button->mouse_over){
            CPU_6502_NMI(app->cpu);
            app->instruction_step = false;
            app->instruction_run = false;
        }
        app->nmi_button->mouse_pressed = false;
    }
    else if(app->seek_to_button->mouse_pressed){
        if(app->seek_to_button->mouse_over){
            MemorySetOffsetAddress(app);
        }
        app->seek_to_button->mouse_pressed = false;;
    }
}


void DrawMemory(App *app){
    
    DrawFilledRectanglei(app->renderer,&app->memory_rect,BACKGROUND2_COLOR);

    RendererSetViewport(app->renderer,NULL,&app->memory_rect);

    uint32_t index = app->memory_start_index;
    uint32_t line = app->memory_start_line;
    uint8_t opcode = 0;
    uint32_t size = 0;

    while(index <= app->memory_end_index && line <= app->memory_end_line){

        opcode = READ_8_BITS(app->cpu,index);

        //Adddress
        swprintf(app->buffer,BUFFER_SIZE,L"%.4X:",index);
            
        DrawTextUint16f(
            app->renderer,
            app->atlas,
            app->buffer,
            (Vector2f){app->glyph_size.x * 0.0f,line * app->glyph_size.y + app->memory_vertical_scroll_bar.offset},
            (index == app->cpu->program_counter) ? SELECTED_COLOR : COLOR_ON
        );

        size = swprintf(app->buffer,BUFFER_SIZE,L"%.2X",opcode);

        if(app->cpu->instruction_table[opcode].bytes == 2){
            size += swprintf(app->buffer + size,BUFFER_SIZE - size,L" %.2X",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].bytes == 3){
            size += swprintf(app->buffer + size,BUFFER_SIZE - size,L" %.2X %.2X",READ_8_BITS(app->cpu,index + 1),READ_8_BITS(app->cpu,index + 2));
        }

        //Binary Instruction
        DrawTextUint16f(
            app->renderer,
            app->atlas,
            app->buffer,
            (Vector2f){app->glyph_size.x * 6.0f,line * app->glyph_size.y + app->memory_vertical_scroll_bar.offset},
            (index == app->cpu->program_counter) ? SELECTED_COLOR : COLOR_ON
        );

        //Assembly

        //Instruction
        swprintf(app->buffer,BUFFER_SIZE,L"%ls",app->cpu->instruction_table[opcode].name);

        DrawTextUint16f(
            app->renderer,
            app->atlas,
            app->buffer,
            (Vector2f){app->glyph_size.x * 20.0f,line * app->glyph_size.y + app->memory_vertical_scroll_bar.offset},
            (index == app->cpu->program_counter) ? SELECTED_COLOR : COLOR_ON
        );

        //Operand
        if(app->cpu->instruction_table[opcode].address == IME){
            swprintf(app->buffer,BUFFER_SIZE,L"#%.2X",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ABS){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.4X",READ_16_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ZP0){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.2X",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ABX){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.4X,X",READ_16_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ABY){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.4X,Y",READ_16_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ZPX){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.2X,X",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == ZPY){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.2X,Y",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == REL){
            swprintf(app->buffer,BUFFER_SIZE,L"$%.4X",(uint16_t)((index + 2) + (int8_t)READ_8_BITS(app->cpu,index + 1)));
        }
        else if(app->cpu->instruction_table[opcode].address == IND){
            swprintf(app->buffer,BUFFER_SIZE,L"($%.4X)",READ_16_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == IDX){
            swprintf(app->buffer,BUFFER_SIZE,L"($%.2X,X)",READ_8_BITS(app->cpu,index + 1));
        }
        else if(app->cpu->instruction_table[opcode].address == IDY){
            swprintf(app->buffer,BUFFER_SIZE - size,L"($%.2X,Y)",READ_8_BITS(app->cpu,index + 1));
        }

        if(app->cpu->instruction_table[opcode].address != IMP && app->cpu->instruction_table[opcode].address != ACU){
            DrawTextUint16f(
                app->renderer,
                app->atlas,
                app->buffer,
                (Vector2f){app->glyph_size.x * 24.0f,line * app->glyph_size.y + app->memory_vertical_scroll_bar.offset},
                (index == app->cpu->program_counter) ? SELECTED_COLOR : COLOR_ON
            );
        }

        line++;
        index += app->cpu->instruction_table[opcode].bytes;
    }

    RendererSetViewport(app->renderer,NULL,NULL);

    DrawScrollBar(&app->memory_vertical_scroll_bar,app->renderer);

    #ifdef DEBUG
    DrawRectanglei(app->renderer,&app->memory_rect,DEBUG_COLOR);
    #endif
}

void DrawStack(App *app){
    uint32_t index = app->stack_start_index;
    uint32_t line = app->stack_start_line;
    uint32_t size = 0;

    DrawLabel(app->renderer,app->stack_label,COLOR_ON);

    DrawFilledRectanglei(app->renderer,&app->stack_rect,BACKGROUND2_COLOR);

    RendererSetViewport(app->renderer,NULL,&app->stack_rect);

    while(index <= app->stack_end_index && line <= app->stack_end_line){

        size = swprintf(app->buffer,BUFFER_SIZE,L"%.4X:",index);
        
        for(int i=0; i<STACK_ROW_LENGTH; ++i){
            if(index + i > STACK_END) break;
            size += swprintf(app->buffer + size,BUFFER_SIZE - size,L" %.2X",READ_8_BITS(app->cpu,index + i));
        }

        DrawTextUint16f(app->renderer,app->atlas,app->buffer,(Vector2f){0.0f,line * app->glyph_size.y + app->stack_vertical_scroll_bar.offset},COLOR_ON);
        
        line++;
        index += STACK_ROW_LENGTH;
    }

    RendererSetViewport(app->renderer,NULL,NULL);

    DrawScrollBar(&app->stack_vertical_scroll_bar,app->renderer);

    #ifdef DEBUG
    DrawRectanglei(app->renderer,&app->stack_rect,DEBUG_COLOR);
    #endif
}


void AppConfigControlButtonsRect(App *app){
    app->step_button->rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = BORDER,
        .w = app->step_button->text_size.x,
        .h = app->step_button->text_size.y + SPACING_Y
    };
    app->step_button->pos = (Vector2i){
        app->step_button->rect.x + app->step_button->rect.w * 0.5f - app->step_button->text_size.x * 0.5f,
        app->step_button->rect.y + app->step_button->rect.h * 0.5f - app->step_button->text_size.y * 0.5f
    };


    app->run_button->rect = (Recti){
        .x = app->step_button->rect.x + app->step_button->rect.w + (BORDER * 2),
        .y = BORDER,
        .w = app->run_button->text_size.x,
        .h = app->run_button->text_size.y + SPACING_Y
    };
    app->run_button->pos = (Vector2i){
        app->run_button->rect.x + app->run_button->rect.w * 0.5f - app->run_button->text_size.x * 0.5f,
        app->run_button->rect.y + app->run_button->rect.h * 0.5f - app->run_button->text_size.y * 0.5f
    };


    app->stop_button->rect = (Recti){
        .x = app->run_button->rect.x + app->run_button->rect.w + (BORDER * 2),
        .y = BORDER,
        .w = app->stop_button->text_size.x,
        .h = app->stop_button->text_size.y + SPACING_Y
    };
    app->stop_button->pos = (Vector2i){
        .x = app->stop_button->rect.x + app->stop_button->rect.w * 0.5f - app->stop_button->text_size.x * 0.5f,
        .y = app->stop_button->rect.y + app->stop_button->rect.h * 0.5f - app->stop_button->text_size.y * 0.5f
    };


    app->reset_button->rect = (Recti){
        .x = app->stop_button->rect.x + app->stop_button->rect.w + (BORDER * 2),
        .y = BORDER,
        .w = app->reset_button->text_size.x,
        .h = app->reset_button->text_size.y + SPACING_Y
    };
    app->reset_button->pos = (Vector2i){
        .x = app->reset_button->rect.x + app->reset_button->rect.w * 0.5f - app->reset_button->text_size.x * 0.5f,
        .y = app->reset_button->rect.y + app->reset_button->rect.h * 0.5f - app->reset_button->text_size.y * 0.5f
    };

    
    app->irq_button->rect = (Recti){
        .x = app->reset_button->rect.x + app->reset_button->rect.w + (BORDER * 2),
        .y = BORDER,
        .w = app->irq_button->text_size.x,
        .h = app->irq_button->text_size.y + SPACING_Y
    };
    app->irq_button->pos = (Vector2i){
        .x = app->irq_button->rect.x + app->irq_button->rect.w * 0.5f - app->irq_button->text_size.x * 0.5f,
        .y = app->irq_button->rect.y + app->irq_button->rect.h * 0.5f - app->irq_button->text_size.y * 0.5f
    };

    
    app->nmi_button->rect = (Recti){
        .x = app->irq_button->rect.x + app->irq_button->rect.w + (BORDER * 2),
        .y = BORDER,
        .w = app->nmi_button->text_size.x,
        .h = app->nmi_button->text_size.y + SPACING_Y
    };
    app->nmi_button->pos = (Vector2i){
        .x = app->nmi_button->rect.x + app->nmi_button->rect.w * 0.5f - app->nmi_button->text_size.x * 0.5f,
        .y = app->nmi_button->rect.y + app->nmi_button->rect.h * 0.5f - app->nmi_button->text_size.y * 0.5f
    };
}

void AppConfigRect(App *app){

    PopUpConfigRect(app->popup);

    int stack_width = (app->screen_size.x - (BORDER * 4)) * (1.0f / 3.0f);
    
    if(stack_width < app->registers->label->text_size.x){
        stack_width = app->registers->label->text_size.x;
    }

    int memory_width = (app->screen_size.x - (BORDER * 4) - stack_width) * 0.5f;

    app->memory_rect = (Recti){
        .x = BORDER,
        .y = BORDER,
        .w = memory_width,
        .h = app->screen_size.y - BORDER * 2
    };

    app->memory_vertical_scroll_bar.bar_rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w - SCROLL_BAR_SIZE,
        .y = app->memory_rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = app->memory_rect.h
    };
    app->memory_vertical_scroll_bar.scroll_rect = (Rectf){
        .x = app->memory_vertical_scroll_bar.bar_rect.x,
        .y = app->memory_vertical_scroll_bar.bar_rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = 0
    };
    MemoryConfigVerticalScrollBar(app);

    AppConfigControlButtonsRect(app);

    RegistersConfiRect(app->registers);

    app->total_cicles_variable->name_rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->registers->pc->rect.y + app->registers->pc->rect.h + BORDER,
        .w = app->total_cicles_variable->name_size.x,
        .h = app->total_cicles_variable->name_size.y + SPACING_Y
    };
    app->total_cicles_variable->name_pos = (Vector2i){
        .x = app->total_cicles_variable->name_rect.x + app->total_cicles_variable->name_rect.w * 0.5f - app->total_cicles_variable->name_size.x * 0.5f,
        .y = app->total_cicles_variable->name_rect.y + app->total_cicles_variable->name_rect.h * 0.5f - app->total_cicles_variable->name_size.y * 0.5f
    };
    app->total_cicles_variable->value_pos = (Vector2i){
        .x = app->total_cicles_variable->name_rect.x + app->total_cicles_variable->name_rect.w + app->glyph_size.x,
        .y = app->total_cicles_variable->name_rect.y + app->total_cicles_variable->name_rect.h * 0.5f - app->glyph_size.y * 0.5f
    };


    app->total_instructions_variable->name_rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->total_cicles_variable->name_rect.y + app->total_cicles_variable->name_rect.h + BORDER,
        .w = app->total_instructions_variable->name_size.x,
        .h = app->total_instructions_variable->name_size.y + SPACING_Y
    };
    app->total_instructions_variable->name_pos = (Vector2i){
        .x = app->total_instructions_variable->name_rect.x + app->total_instructions_variable->name_rect.w * 0.5f - app->total_instructions_variable->name_size.x * 0.5f,
        .y = app->total_instructions_variable->name_rect.y + app->total_instructions_variable->name_rect.h * 0.5f - app->total_instructions_variable->name_size.y * 0.5f
    };
    app->total_instructions_variable->value_pos = (Vector2i){
        .x = app->total_instructions_variable->name_rect.x + app->total_instructions_variable->name_rect.w + app->glyph_size.x,
        .y = app->total_instructions_variable->name_rect.y + app->total_instructions_variable->name_rect.h * 0.5f - app->glyph_size.y * 0.5f
    };


    app->instruction_runtime_label->rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->total_instructions_variable->name_rect.y + app->total_instructions_variable->name_rect.h + BORDER,
        .w = app->instruction_runtime_label->text_size.x,
        .h = app->instruction_runtime_label->text_size.y + SPACING_Y
    };
    app->instruction_runtime_label->pos = (Vector2i){
        .x = app->instruction_runtime_label->rect.x + app->instruction_runtime_label->rect.w * 0.5f - app->instruction_runtime_label->text_size.x * 0.5f,
        .y = app->instruction_runtime_label->rect.y + app->instruction_runtime_label->rect.h * 0.5f - app->instruction_runtime_label->text_size.y * 0.5f
    };

    app->instruction_runtime_input->rect = (Recti){
        .x = app->instruction_runtime_label->rect.x + app->instruction_runtime_label->rect.w + app->glyph_size.x,
        .y = app->instruction_runtime_label->rect.y + app->instruction_runtime_label->rect.h * 0.5f - app->glyph_size.y * 0.5f,
        .w = app->glyph_size.x * app->instruction_runtime_input->max_length,
        .h = app->glyph_size.y
    };
    app->instruction_runtime_input->pos = (Vector2i){
        .x = app->instruction_runtime_input->rect.x,
        .y = app->instruction_runtime_input->rect.y
    };
    

    app->seek_to_button->rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->instruction_runtime_label->rect.y + app->instruction_runtime_label->rect.h + BORDER,
        .w = app->seek_to_button->text_size.x,
        .h = app->seek_to_button->text_size.y + SPACING_Y
    };
    app->seek_to_button->pos = (Vector2i){
        .x = app->seek_to_button->rect.x + app->seek_to_button->rect.w * 0.5f - app->seek_to_button->text_size.x * 0.5f,
        .y = app->seek_to_button->rect.y + app->seek_to_button->rect.h * 0.5f - app->seek_to_button->text_size.y * 0.5f
    };
    
    app->seek_to_input->rect = (Recti){
        .x = app->seek_to_button->rect.x + app->seek_to_button->rect.w + app->glyph_size.x,
        .y = app->seek_to_button->rect.y + app->seek_to_button->rect.h * 0.5f - app->glyph_size.y * 0.5f,
        .w = app->glyph_size.x * app->seek_to_input->max_length,
        .h = app->glyph_size.y
    };
    app->seek_to_input->pos = (Vector2i){
        .x = app->seek_to_input->rect.x,
        .y = app->seek_to_input->rect.y
    };

    
    app->stack_label->rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->seek_to_button->rect.y + app->seek_to_button->rect.h + BORDER,
        .w = app->stack_label->text_size.x,
        .h = app->stack_label->text_size.y + SPACING_Y
    };
    app->stack_label->pos = (Vector2i){
        .x = app->stack_label->rect.x + app->stack_label->rect.w * 0.5f - app->stack_label->text_size.x * 0.5f,
        .y = app->stack_label->rect.y + app->stack_label->rect.h * 0.5f - app->stack_label->text_size.y * 0.5f
    };

    app->stack_rect = (Recti){
        .x = app->memory_rect.x + app->memory_rect.w + BORDER,
        .y = app->stack_label->rect.y + app->stack_label->rect.h,
        .w = stack_width,
        .h = (app->screen_size.y - BORDER) - (app->stack_label->rect.y + app->stack_label->rect.h)
    };

    
    app->stack_vertical_scroll_bar.bar_rect = (Recti){
        .x = app->stack_rect.x + app->stack_rect.w - SCROLL_BAR_SIZE,
        .y = app->stack_rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = app->stack_rect.h
    };
    app->stack_vertical_scroll_bar.scroll_rect = (Rectf){
        .x = app->stack_vertical_scroll_bar.bar_rect.x,
        .y = app->stack_vertical_scroll_bar.bar_rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = 0.0f
    };
    StackConfigVerticalScrollBar(app);

    BreakpointsConfigRect(app->breakpoints);

    SourceEditorConfigRect(app->source_editor);
}

void AppInit(App *app){

    app->screen_size.x = SCREEN_MIN_WIDTH;
    app->screen_size.y = SCREEN_MIN_HEIGHT;

    app->window = SDL_CreateWindow("CPU_6502",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,app->screen_size.x,app->screen_size.y,SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_SetWindowMinimumSize(app->window,SCREEN_MIN_WIDTH,SCREEN_MIN_HEIGHT);

    app->renderer = CreateRenderer(app->window,false);

    RendererSetFillIncompletFormat(app->renderer,true);

    app->running = true;
    app->instruction_run = false;
    app->instruction_step = false;
    app->element_on = false;
    app->current_time = 0;
    app->start_execution = 0;
    app->current_input = NULL;

    app->font = OpenFont(L"./RobotoMono-Bold.ttf",FONT_HEIGHT);
    SizeText(app->font,L"A",&app->glyph_size.x,&app->glyph_size.y);

    app->atlas = CreateFontAtlas(app->renderer,app->font,FONT_HEIGHT,0x20,0xFF);

    app->cpu = malloc(sizeof(CPU_6502));
    CPU_6502_Init(app->cpu);

    app->cursors = CreateCursors();

    memset(&app->memory_vertical_scroll_bar,0,sizeof(Scroll_Bar));
    memset(&app->stack_vertical_scroll_bar,0,sizeof(Scroll_Bar));
    memset(&app->mouse,0,sizeof(Mouse));

    app->run_button = CreateButtonText(L"Run",app->atlas);
    app->stop_button = CreateButtonText(L"Stop",app->atlas);
    app->step_button = CreateButtonText(L"Step",app->atlas);
    app->reset_button = CreateButtonText(L"Reset",app->atlas);
    app->irq_button = CreateButtonText(L"IRQ",app->atlas);
    app->nmi_button = CreateButtonText(L"NMI",app->atlas);

    app->total_cicles_variable = CreateVariable(L"Total Cicles:",app->atlas);
    app->total_instructions_variable = CreateVariable(L"Total Instructions:",app->atlas);

    app->instruction_runtime_label = CreateLabel(L"Instruction Runtime:",app->atlas);
    app->instruction_runtime_input = CreateInput(4,FORMAT_DEC);
    app->instruction_runtime = 500;

    app->seek_to_button = CreateButtonText(L"Seek To",app->atlas);
    app->seek_to_input = CreateInput(4,FORMAT_HEX);
    app->address = 0;
    
    app->stack_label = CreateLabel(L"Stack (0100-01FF):",app->atlas);

    app->registers = malloc(sizeof(Registers));
    RegistersInit(app->registers,app);

    app->breakpoints = malloc(sizeof(Breakpoints));
    BreakpointsInit(app->breakpoints,app);

    app->source_editor = malloc(sizeof(Source_Editor));
    SourceEditorInit(app->source_editor,app);

    app->popup = CreatePopUp(app);
}

void AppDraw(App *app){
    //Step
    DrawButtonText(app->renderer,app->step_button,SELECTED_COLOR,(!app->instruction_run) ? COLOR_ON : COLOR_OFF);
    //Run
    DrawButtonText(app->renderer,app->run_button,SELECTED_COLOR,(!app->instruction_run) ? COLOR_ON : COLOR_OFF);
    //Stop
    DrawButtonText(app->renderer,app->stop_button,STOP_COLOR,(app->instruction_run) ? STOP_COLOR : COLOR_OFF);
    //Reset
    DrawButtonText(app->renderer,app->reset_button,SELECTED_COLOR,COLOR_ON);
    //IRQ
    DrawButtonText(app->renderer,app->irq_button,SELECTED_COLOR,COLOR_ON);
    //NMI
    DrawButtonText(app->renderer,app->nmi_button,SELECTED_COLOR,COLOR_ON);

    //Total Cicles
    swprintf(app->buffer,BUFFER_SIZE,L"%d",app->cpu->total_cicles);
    DrawVariable(app->renderer,app->total_cicles_variable,app->buffer,COLOR_ON);

    //Total Instructions
    swprintf(app->buffer,BUFFER_SIZE,L"%d",app->cpu->total_instructions);
    DrawVariable(app->renderer,app->total_instructions_variable,app->buffer,COLOR_ON);

    //Instruction Runtime
    DrawLabel(app->renderer,app->instruction_runtime_label,COLOR_ON);
    swprintf(app->buffer,BUFFER_SIZE,L"%d",app->instruction_runtime);
    DrawInput(app,app->instruction_runtime_input,app->buffer);

    //Seek To
    DrawButtonText(app->renderer,app->seek_to_button,SELECTED_COLOR,COLOR_ON);
    swprintf(app->buffer,BUFFER_SIZE,L"%.4X",app->address);
    DrawInput(app,app->seek_to_input,app->buffer);

    //Stack
    DrawStack(app);

    //Memory
    DrawMemory(app);
}

void AppEvent(App *app){
    
    MouseOver(app);

    if(app->event.type == SDL_QUIT){
        app->running = false;
    }
    else if(app->event.type == SDL_WINDOWEVENT && app->event.window.event == SDL_WINDOWEVENT_RESIZED){
        SDL_GetWindowSize(app->window,&app->screen_size.x,&app->screen_size.y);
        AppConfigRect(app);
    }
    else if(app->event.type == SDL_MOUSEWHEEL && !app->breakpoints->conditions->opened && !app->popup->opened){
        if(PointInRect(app->mouse.pos,app->memory_rect)){
            if(app->event.wheel.preciseY < 0.0f){
                //up content
                SetYScrollBar(&app->memory_vertical_scroll_bar,app->memory_vertical_scroll_bar.offset - (FONT_HEIGHT * 3.0f));
            }
            else if(app->event.wheel.preciseY > 0.0f){
                //down content
                SetYScrollBar(&app->memory_vertical_scroll_bar,app->memory_vertical_scroll_bar.offset + (FONT_HEIGHT * 3.0f));
            }
            MemorySetVisibleLines(app);
        }
        else if(PointInRect(app->mouse.pos,app->stack_rect)){
            if(app->event.wheel.preciseY < 0.0f){
                //up content
                SetYScrollBar(&app->stack_vertical_scroll_bar,app->stack_vertical_scroll_bar.offset - (FONT_HEIGHT * 3.0f));
            }
            else if(app->event.wheel.preciseY > 0.0f){
                //down content
                SetYScrollBar(&app->stack_vertical_scroll_bar,app->stack_vertical_scroll_bar.offset + (FONT_HEIGHT * 3.0f));
            }
            StackSetVisibleLines(app);
        }
    }
    else if(app->event.type == SDL_MOUSEBUTTONDOWN && app->event.button.button == SDL_BUTTON_LEFT){
        MouseButtonDown(app);
    }
    else if(app->event.type == SDL_MOUSEBUTTONUP && app->event.button.button == SDL_BUTTON_LEFT){
        MouseButtonUp(app);

        app->memory_vertical_scroll_bar.on = false;
        app->stack_vertical_scroll_bar.on = false;

        app->element_on = false;
    }
    else if(SDL_BUTTON(app->mouse.state) & SDL_BUTTON_LEFT){
        if(app->memory_vertical_scroll_bar.on){
            HandleVerticalScrollBar(&app->memory_vertical_scroll_bar,app->mouse);
            MemorySetVisibleLines(app);
        }
        else if(app->stack_vertical_scroll_bar.on){
            HandleVerticalScrollBar(&app->stack_vertical_scroll_bar,app->mouse);
            StackSetVisibleLines(app);
        }
    }
}

void AppFree(App *app){

    free(app->cpu);
    CloseFont(app->font);
    FontAtlasFree(app->atlas);
    CursorsFree(app->cursors);

    ButtonTextFree(app->step_button);
    ButtonTextFree(app->run_button);
    ButtonTextFree(app->stop_button);
    ButtonTextFree(app->reset_button);
    ButtonTextFree(app->irq_button);
    ButtonTextFree(app->nmi_button);

    RegistersFree(app->registers);
    BreakpointsFree(app->breakpoints);
    SourceEditorFree(app->source_editor);
    PopUpFree(app->popup);

    VariableFree(app->total_cicles_variable);
    VariableFree(app->total_instructions_variable);

    LabelFree(app->instruction_runtime_label);
    InputFree(app->instruction_runtime_input);

    ButtonTextFree(app->seek_to_button);
    InputFree(app->seek_to_input);

    LabelFree(app->stack_label);
    
    RendererFree(app->renderer);
    SDL_DestroyWindow(app->window);

    free(app);
}

int main(int n_args,char **args){
    SDL_Init(SDL_INIT_EVERYTHING);

    App *app = malloc(sizeof(App));

    AppInit(app);
    AppConfigRect(app);

    char title_buffer[BUFFER_SIZE];
    uint32_t frame_count = 0;
    uint32_t start_time = app->current_time;
    uint32_t delta_time = 0;

    SDL_StartTextInput();
    while(app->running){

        app->current_time = SDL_GetTicks();

        delta_time = app->current_time - start_time;
        frame_count++;
        if(delta_time >= 1000){
            float fps = frame_count / (delta_time / 1000.0f);
            snprintf(title_buffer,BUFFER_SIZE,"CPU 6502 Emulator - FPS: %.1f",fps);
            SDL_SetWindowTitle(app->window,title_buffer);
            frame_count = 0;
            start_time = app->current_time;
        }

        SDL_PollEvent(&app->event);
        
        app->mouse.state = SDL_GetMouseState(&app->mouse.pos.x,&app->mouse.pos.y);

        if(app->instruction_step){
            
            CPU_6502_Execute(app->cpu);
            
            MemoryConfigVerticalScrollBar(app);

            MemoryAjustVerticalScrollBarOffset(app);

            app->instruction_step = false;
        }
        else if(app->instruction_run && app->current_time - app->start_execution >= app->instruction_runtime){
            
            CPU_6502_Execute(app->cpu);
            
            MemoryConfigVerticalScrollBar(app);

            MemoryAjustVerticalScrollBarOffset(app);

            if(app->cpu->interrupt) app->instruction_run = false;
            
            CheckBreakpoints(app->breakpoints);

            app->start_execution = app->current_time;
        }

        ClearColor(app->renderer,BACKGROUND1_COLOR);

        //App
        AppEvent(app);
        AppDraw(app);

        //Inputs
        input_events(app);

        //Registers
        RegistersEvent(app->registers);
        RegistersDraw(app->registers);

        //Breakpoints
        BreakpointsEvent(app->breakpoints);
        BreakpointsDraw(app->breakpoints);

        //Source Editor
        SourceEditorEvent(app->source_editor);
        SourceEditorDraw(app->source_editor);

        if(app->popup->opened){
            PopUpEvent(app->popup);
            PopUpDraw(app->popup);
        }
        else if(app->breakpoints->conditions->opened){
            BreakpointConditionsEvents(app->breakpoints->conditions,app->event,app->mouse);
            DrawBreakpointConditions(app->renderer,app->breakpoints->conditions);
        }

        Flip(app->renderer);

        CursorsUpdate(app->cursors);
    }
    SDL_StopTextInput();

    AppFree(app);

    SDL_Quit();

    return 0;
}
