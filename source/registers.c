#include <registers.h>

void RegistersInit(Registers *registers,App *app){
    registers->app = app;

    registers->label = CreateLabel(L"PC    AR XR YR SR SP  N V - B D I Z C",app->atlas);

    registers->pc = CreateInput(4,FORMAT_HEX);
    registers->ar = CreateInput(2,FORMAT_HEX);
    registers->xr = CreateInput(2,FORMAT_HEX);
    registers->yr = CreateInput(2,FORMAT_HEX);
    registers->sr = CreateInput(2,FORMAT_HEX);
    registers->sp = CreateInput(2,FORMAT_HEX);

    memset(&registers->flag_n,0,sizeof(Flag));
    memset(&registers->flag_v,0,sizeof(Flag));
    memset(&registers->flag_u,0,sizeof(Flag));
    memset(&registers->flag_b,0,sizeof(Flag));
    memset(&registers->flag_d,0,sizeof(Flag));
    memset(&registers->flag_i,0,sizeof(Flag));
    memset(&registers->flag_z,0,sizeof(Flag));
    memset(&registers->flag_c,0,sizeof(Flag));
}

static void StatusFlagConfigRect(Registers *registers){
    
    registers->flag_n.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 22),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_n.pos = (Vector2i){
        .x = registers->flag_n.rect.x,
        .y = registers->flag_n.rect.y
    };

    registers->flag_v.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 24),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_v.pos = (Vector2i){
        .x = registers->flag_v.rect.x,
        .y = registers->flag_v.rect.y
    };

    registers->flag_u.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 26),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_u.pos = (Vector2i){
        .x = registers->flag_u.rect.x,
        .y = registers->flag_u.rect.y
    };


    registers->flag_b.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 28),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_b.pos = (Vector2i){
        .x = registers->flag_b.rect.x,
        .y = registers->flag_b.rect.y
    };


    registers->flag_d.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 30),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_d.pos = (Vector2i){
        .x = registers->flag_d.rect.x,
        .y = registers->flag_d.rect.y
    };


    registers->flag_i.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 32),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_i.pos = (Vector2i){
        .x = registers->flag_i.rect.x,
        .y = registers->flag_i.rect.y
    };


    registers->flag_z.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 34),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_z.pos = (Vector2i){
        .x = registers->flag_z.rect.x,
        .y = registers->flag_z.rect.y
    };


    registers->flag_c.rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 36),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x,
        .h = registers->app->glyph_size.y
    };
    registers->flag_c.pos = (Vector2i){
        .x = registers->flag_c.rect.x,
        .y = registers->flag_c.rect.y
    };
}

void RegistersConfiRect(Registers *registers){

    registers->label->rect = (Recti){
        .x = registers->app->memory_rect.x + registers->app->memory_rect.w + BORDER,
        .y = registers->app->run_button->rect.y + registers->app->run_button->rect.h + BORDER,
        .w = registers->label->text_size.x,
        .h = registers->label->text_size.y + SPACING_Y,
    };
    registers->label->pos = (Vector2i){
        .x = registers->label->rect.x + registers->label->rect.w * 0.5f - registers->label->text_size.x * 0.5f,
        .y = registers->label->rect.y + registers->label->rect.h * 0.5f - registers->label->text_size.y * 0.5f
    };

    
    registers->pc->rect = (Recti){
        .x = registers->label->rect.x,
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->pc->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->pc->pos = (Vector2i){
        .x = registers->pc->rect.x,
        .y = registers->pc->rect.y
    };

    
    registers->ar->rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 6),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->ar->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->ar->pos = (Vector2i){
        registers->ar->rect.x,
        registers->ar->rect.y
    };

    
    registers->xr->rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 9),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->xr->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->xr->pos = (Vector2i){
        .x = registers->xr->rect.x,
        .y = registers->xr->rect.y
    };

    
    registers->yr->rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 12),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->yr->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->yr->pos = (Vector2i){
        .x = registers->yr->rect.x,
        .y = registers->yr->rect.y
    };

    
    registers->sr->rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 15),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->sr->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->sr->pos = (Vector2i){
        .x = registers->sr->rect.x,
        .y = registers->sr->rect.y
    };

    
    registers->sp->rect = (Recti){
        .x = registers->label->rect.x + (registers->app->glyph_size.x * 18),
        .y = registers->label->rect.y + registers->label->rect.h,
        .w = registers->app->glyph_size.x * registers->sp->max_length,
        .h = registers->app->glyph_size.y
    };
    registers->sp->pos = (Vector2i){
        .x = registers->sp->rect.x,
        .y = registers->sp->rect.y
    };

    StatusFlagConfigRect(registers);
}

static void RegistersDrawFlag(Renderer *renderer,Flag *flag,FontAtlas *atlas,wchar_t *value){
    #ifdef DEBUG
    DrawRectanglei(renderer,&flag->rect,DEBUG_COLOR);
    #endif
    if(flag->mouse_over){
        DrawTextUint16i(renderer,atlas,value,flag->pos,SELECTED_COLOR);
        flag->mouse_over = false;
    }
    else{
        DrawTextUint16i(renderer,atlas,value,flag->pos,COLOR_ON);
    }
}

void RegistersDraw(Registers *registers){
    Renderer *renderer = registers->app->renderer;
    FontAtlas *atlas = registers->app->atlas;
    CPU_6502 *cpu = registers->app->cpu;

    //Label
    DrawLabel(renderer,registers->label,COLOR_ON);

    //PC
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.4X",cpu->program_counter);
    DrawInput(registers->app,registers->pc,registers->app->buffer);
    //AR
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.2X",cpu->accumalator);
    DrawInput(registers->app,registers->ar,registers->app->buffer);
    //XR
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.2X",cpu->x_register);
    DrawInput(registers->app,registers->xr,registers->app->buffer);
    //YR
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.2X",cpu->y_register);
    DrawInput(registers->app,registers->yr,registers->app->buffer);
    //SR
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.2X",cpu->status_register);
    DrawInput(registers->app,registers->sr,registers->app->buffer);
    //SP
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.2X",cpu->stack_pointer);
    DrawInput(registers->app,registers->sp,registers->app->buffer);

    //N
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_N) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_n,atlas,registers->app->buffer);
    //V
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_V) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_v,atlas,registers->app->buffer);
    //Ignored
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_U) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_u,atlas,registers->app->buffer);
    //B
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_B) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_b,atlas,registers->app->buffer);
    //D
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_D) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_d,atlas,registers->app->buffer);
    //I
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_I) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_i,atlas,registers->app->buffer);
    //Z
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_Z) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_z,atlas,registers->app->buffer);
    //C
    swprintf(registers->app->buffer,BUFFER_SIZE,L"%.1d",(cpu->status_register & CPU_6502_FLAG_C) ? 1 : 0);
    RegistersDrawFlag(renderer,&registers->flag_c,atlas,registers->app->buffer);
}

static void RegistersMouseOver(Registers *registers){
    if(registers->app->element_on) return;
    
    Vector2i position = registers->app->mouse.pos;

    if(PointInRect(position,registers->flag_n.rect)){
        registers->flag_n.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_v.rect)){
        registers->flag_v.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_u.rect)){
        registers->flag_u.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_b.rect)){
        registers->flag_b.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_d.rect)){
        registers->flag_d.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_i.rect)){
        registers->flag_i.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_z.rect)){
        registers->flag_z.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(position,registers->flag_c.rect)){
        registers->flag_c.mouse_over = true;
        registers->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
}

static void RegistersMouseDown(Registers *registers){
    
    if(registers->flag_n.mouse_over){
        registers->flag_n.mouse_pressed = true;
    }
    else if(registers->flag_v.mouse_over){
        registers->flag_v.mouse_pressed = true;
    }
    else if(registers->flag_u.mouse_over){
        registers->flag_u.mouse_pressed = true;
    }
    else if(registers->flag_b.mouse_over){
        registers->flag_b.mouse_pressed = true;
    }
    else if(registers->flag_d.mouse_over){
        registers->flag_d.mouse_pressed = true;
    }
    else if(registers->flag_i.mouse_over){
        registers->flag_i.mouse_pressed = true;
    }
    else if(registers->flag_z.mouse_over){
        registers->flag_z.mouse_pressed = true;
    }
    else if(registers->flag_c.mouse_over){
        registers->flag_c.mouse_pressed = true;
    }
}

static void RegistersMouseUp(Registers *registers){
    CPU_6502 *cpu = registers->app->cpu;

    if(registers->flag_n.mouse_pressed){
        if(registers->flag_n.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_N,!(cpu->status_register & CPU_6502_FLAG_N));
        }
        registers->flag_n.mouse_pressed = false;
    }
    else if(registers->flag_v.mouse_pressed){
        if(registers->flag_v.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_V,!(cpu->status_register & CPU_6502_FLAG_V));
        }
        registers->flag_v.mouse_pressed = false;
    }
    else if(registers->flag_u.mouse_pressed){
        if(registers->flag_u.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_U,!(cpu->status_register & CPU_6502_FLAG_U));
        }
        registers->flag_u.mouse_pressed = false;
    }
    else if(registers->flag_b.mouse_pressed){
        if(registers->flag_b.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_B,!(cpu->status_register & CPU_6502_FLAG_B));
        }
        registers->flag_b.mouse_pressed = false;
    }
    else if(registers->flag_d.mouse_pressed){
        if(registers->flag_d.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_D,!(cpu->status_register & CPU_6502_FLAG_D));
        }
        registers->flag_d.mouse_pressed = false;
    }
    else if(registers->flag_i.mouse_pressed){
        if(registers->flag_i.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_I,!(cpu->status_register & CPU_6502_FLAG_I));
        }
        registers->flag_i.mouse_pressed = false;
    }
    else if(registers->flag_z.mouse_pressed){
        if(registers->flag_z.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_Z,!(cpu->status_register & CPU_6502_FLAG_Z));
        }
        registers->flag_z.mouse_pressed = false;
    }
    else if(registers->flag_c.mouse_pressed){
        if(registers->flag_c.mouse_over){
            SET_FLAG(cpu,CPU_6502_FLAG_C,!(cpu->status_register & CPU_6502_FLAG_C));
        }
        registers->flag_c.mouse_pressed = false;
    }
}

void RegistersEvent(Registers *registers){
    if(registers->app->breakpoints->conditions->opened || registers->app->popup->opened) return;
    
    RegistersMouseOver(registers);

    if(registers->app->event.type == SDL_MOUSEBUTTONDOWN && registers->app->event.button.button == SDL_BUTTON_LEFT){
        RegistersMouseDown(registers);
    }
    else if(registers->app->event.type == SDL_MOUSEBUTTONUP && registers->app->event.button.button == SDL_BUTTON_LEFT){
        RegistersMouseUp(registers);
    }
}

void RegistersFree(Registers *registers){
    LabelFree(registers->label);

    InputFree(registers->pc);
    InputFree(registers->ar);
    InputFree(registers->xr);
    InputFree(registers->yr);
    InputFree(registers->sr);
    InputFree(registers->sp);
    
    free(registers);
}