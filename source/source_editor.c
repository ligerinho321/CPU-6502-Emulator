#include <source_editor.h>

static void SourceEditorConfigVerticalScrollBar(Source_Editor *source_editor){
    
    source_editor->editor->vertical_scroll_bar.bar_rect = (Recti){
        .x = source_editor->editor->rect.x + source_editor->editor->rect.w - SCROLL_BAR_SIZE,
        .y = source_editor->editor->rect.y,
        .w = SCROLL_BAR_SIZE,
        .h = source_editor->editor->rect.h
    };

    source_editor->editor->vertical_scroll_bar.scroll_rect = (Rectf){
        .x = source_editor->editor->vertical_scroll_bar.bar_rect.x,
        .y = source_editor->editor->vertical_scroll_bar.bar_rect.y,
        .w = source_editor->editor->vertical_scroll_bar.bar_rect.w,
        .h = GetScrollBarHeight(source_editor->editor->vertical_scroll_bar)
    };

    if(source_editor->editor->vertical_scroll_bar.scroll_rect.h > 0.0f && source_editor->editor->vertical_scroll_bar.scroll_rect.h < source_editor->editor->vertical_scroll_bar.bar_rect.h){
        source_editor->editor->vertical_scroll_bar.valid = true;
        SetYScrollBar(&source_editor->editor->vertical_scroll_bar,source_editor->editor->vertical_scroll_bar.offset);
    }
    else{
        source_editor->editor->vertical_scroll_bar.valid = false;
        source_editor->editor->vertical_scroll_bar.offset = 0.0f;
        source_editor->editor->vertical_scroll_bar.scroll_rect.y = source_editor->editor->vertical_scroll_bar.bar_rect.y;
    }

    TextEditor_SetVisibleLines(source_editor->editor);
}

static void SourceEditorConfigHorizontalScrollBar(Source_Editor *source_editor){
    source_editor->editor->horizontal_scroll_bar.bar_rect = (Recti){
        .x = source_editor->editor->rect.x,
        .y = source_editor->editor->rect.y + source_editor->editor->rect.h - SCROLL_BAR_SIZE,
        .w = source_editor->editor->rect.w - SCROLL_BAR_SIZE,
        .h = SCROLL_BAR_SIZE
    };

    source_editor->editor->horizontal_scroll_bar.scroll_rect = (Rectf){
        .x = source_editor->editor->horizontal_scroll_bar.bar_rect.x,
        .y = source_editor->editor->horizontal_scroll_bar.bar_rect.y,
        .w = GetScrollBarWidth(source_editor->editor->horizontal_scroll_bar),
        .h = source_editor->editor->horizontal_scroll_bar.bar_rect.h
    };

    if(source_editor->editor->horizontal_scroll_bar.scroll_rect.w > 0.0f && source_editor->editor->horizontal_scroll_bar.scroll_rect.w < source_editor->editor->horizontal_scroll_bar.bar_rect.w){
        source_editor->editor->horizontal_scroll_bar.valid = true;
        SetXScrollBar(&source_editor->editor->horizontal_scroll_bar,source_editor->editor->horizontal_scroll_bar.offset);

    }
    else{
        source_editor->editor->horizontal_scroll_bar.valid = false;
        source_editor->editor->horizontal_scroll_bar.offset = 0.0f;
        source_editor->editor->horizontal_scroll_bar.scroll_rect.x = source_editor->editor->horizontal_scroll_bar.bar_rect.x;
    }
}


static void LoadFile(Source_Editor *source_editor,wchar_t *path){
    FILE *file = _wfopen(path,L"rb");
    if(!file){fprintf(stderr,"Error: %s\n",strerror(errno));return;}

    fseek(file,0,SEEK_END);
    long int size = ftell(file);
    fseek(file,0,SEEK_SET);

    uint8_t *content = malloc(size * sizeof(uint8_t));
    if(!content){fprintf(stderr,"Error: %s\n",strerror(errno));return;}

    fread(content,sizeof(uint8_t),size,file);

    fclose(file);

    Line *line = source_editor->editor->lines;
    GapBufferClear(line->gap_buffer);
    TextEditor_FreeLines(line->next);
    line->next = NULL;

    source_editor->editor->current_line = line;

    for(long int i=0; i<size; ++i){
        if(i > 0 && i % 8 == 0) line = TextEditor_PushLine(line);

        if(i % 8 == 0){
            swprintf(source_editor->app->buffer,BUFFER_SIZE,L"%.2X",content[i]);
            GapBufferInsertValues(line->gap_buffer,wchar_t,source_editor->app->buffer,2);
        }
        else{
            swprintf(source_editor->app->buffer,BUFFER_SIZE,L" %.2X",content[i]);
            GapBufferInsertValues(line->gap_buffer,wchar_t,source_editor->app->buffer,3);
        }
    }

    free(content);

    TextEditor_SortLines(source_editor->editor);

    TextEditor_SetVerticalScrollBarSize(source_editor->editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(source_editor->editor);

    TextEditor_SetVisibleLines(source_editor->editor);

    TextEditor_SetHorizontalScrollBarSize(source_editor->editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(source_editor->editor);
}

static void LoadMemory(Source_Editor *source_editor){
    GapBuffer *gap_buffer;
    CreateGapBuffer(gap_buffer,uint8_t,GAP_EXPAND_SIZE * source_editor->editor->number_of_lines);

    Line *line = source_editor->editor->lines;
    wchar_t *ptr = NULL;
    wchar_t value[3] = {0};
    uint8_t index = 0;

    while(line != NULL){

        ptr = line->gap_buffer->buffer;

        while(ptr < (wchar_t*)line->gap_buffer->buffer_end){

            if(ptr == (wchar_t*)line->gap_buffer->gap_start){
                ptr = line->gap_buffer->gap_end;
                if(ptr >= (wchar_t*)line->gap_buffer->buffer_end) break;
            }

            if(!iswspace(ptr[0])) value[index++] = ptr[0];

            if(index >= 2 || (!line->next && ptr+1 == (wchar_t*)line->gap_buffer->buffer_end)){
                GapBufferInsertValue(gap_buffer,uint8_t,wcstol(value,NULL,16));
                index = 0;
                memset(value,0,sizeof(wchar_t) * 3);
            }  
            ptr++;
        }
        line = line->next;
    }

    CPU_6502_LoadProgram(source_editor->app->cpu,gap_buffer->buffer,ContentSize(gap_buffer,uint8_t),source_editor->start_address);
    MemoryConfigVerticalScrollBar(source_editor->app);
    
    OpenPopUp(source_editor->app->popup,L"Memory loaded");
    
    GapBufferFree(gap_buffer);
}

static void ClearMemory(Source_Editor *source_editor){
    memset(source_editor->app->cpu->memory,0,sizeof(uint8_t) * MEMORY_SIZE);
    CPU_6502_RES(source_editor->app->cpu);
    source_editor->app->instruction_run = false;
    source_editor->app->instruction_step = false;
    MemoryConfigVerticalScrollBar(source_editor->app);
}


static void SourceEditorMouseOver(Source_Editor *source_editor){
    if(source_editor->app->element_on) return;

    if(PointInRect(source_editor->app->mouse.pos,source_editor->load_file_button->rect)){
        source_editor->load_file_button->mouse_over = true;
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->load_memory_button->rect)){
        source_editor->load_memory_button->mouse_over = true;
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->clear_memory_button->rect)){
        source_editor->clear_memory_button->mouse_over = true;
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_HAND;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->editor->vertical_scroll_bar.scroll_rect) && source_editor->editor->vertical_scroll_bar.valid){
        source_editor->editor->vertical_scroll_bar.mouse_over = true;
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->editor->horizontal_scroll_bar.scroll_rect) && source_editor->editor->horizontal_scroll_bar.valid){
        source_editor->editor->horizontal_scroll_bar.mouse_over = true;
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_ARROW;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->editor->rect)){
        source_editor->app->cursors->current_cursor = SDL_SYSTEM_CURSOR_IBEAM;
    }
}

static void SourceEditorMouseDown(Source_Editor *source_editor){

    if(source_editor->load_file_button->mouse_over){
        source_editor->load_file_button->mouse_pressed = true;
    }
    else if(source_editor->load_memory_button->mouse_over){
        source_editor->load_memory_button->mouse_pressed = true;
    }
    else if(source_editor->clear_memory_button->mouse_over){
        source_editor->clear_memory_button->mouse_pressed = true;
    }
    else if(source_editor->editor->vertical_scroll_bar.mouse_over){
        source_editor->editor->vertical_scroll_bar.on = true;
        source_editor->editor->vertical_scroll_bar.mouse_diff = source_editor->app->mouse.pos.y - source_editor->editor->vertical_scroll_bar.scroll_rect.y;

        source_editor->app->element_on = true;
    }
    else if(source_editor->editor->horizontal_scroll_bar.mouse_over){
        source_editor->editor->horizontal_scroll_bar.on = true;
        source_editor->editor->horizontal_scroll_bar.mouse_diff = source_editor->app->mouse.pos.x - source_editor->editor->horizontal_scroll_bar.scroll_rect.x;

        source_editor->app->element_on = true;
    }
    else if(PointInRect(source_editor->app->mouse.pos,source_editor->editor->rect)){

        if(!source_editor->editor->selected){
            source_editor->editor->selected = true;
            source_editor->editor->cursor.update = true;
        }
        
        source_editor->editor->cursor.on = true;

        source_editor->app->element_on = true;

        if(source_editor->editor->text_selected) TextEditor_DeselectLines(source_editor->editor);

        source_editor->editor->start_selection = source_editor->editor->end_selection = TextEditor_GetPosInText(source_editor->editor,source_editor->app->mouse);
        
        TextEditor_SetPosInText(source_editor->editor);
    }
    else{
        source_editor->editor->selected = false;
        source_editor->editor->cursor.on = false;
    }
}

static void SourceEditorMouseUp(Source_Editor *source_editor){
    
    if(source_editor->load_file_button->mouse_pressed){
        if(source_editor->load_file_button->mouse_over){
            
            wchar_t file_path[MAX_PATH] = {0};
            
            OPENFILENAMEW open_file_name;
            memset(&open_file_name,0,sizeof(OPENFILENAME));
            
            open_file_name.lStructSize = sizeof(OPENFILENAME);
            open_file_name.lpstrFilter = L"Todos os Arquivos\0*.*\0";
            open_file_name.nFilterIndex = 1;
            open_file_name.lpstrFile = file_path;
            open_file_name.nMaxFile = MAX_PATH;
            open_file_name.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
            
            if(GetOpenFileNameW(&open_file_name)){
                LoadFile(source_editor,file_path);
            }
            else{
                printf("User aborted action in open file dialog box\n");
            }
        }
        source_editor->load_file_button->mouse_pressed = false;
    }
    else if(source_editor->load_memory_button->mouse_pressed){
        if(source_editor->load_memory_button->mouse_over){
            LoadMemory(source_editor);
        }
        source_editor->load_memory_button->mouse_pressed = false;
    }
    else if(source_editor->clear_memory_button->mouse_pressed){
        if(source_editor->clear_memory_button->mouse_over){
            ClearMemory(source_editor);
        }
        source_editor->clear_memory_button->mouse_pressed = false;
    }
}


void SourceEditorInit(Source_Editor *source_editor,App *app){
    source_editor->app = app;

    source_editor->label = CreateLabel(L"Source:",app->atlas);
    source_editor->load_file_button = CreateButtonText(L"Load File",app->atlas);
    source_editor->load_memory_button = CreateButtonText(L"Load Memory",app->atlas);
    source_editor->clear_memory_button = CreateButtonText(L"Clear Memory",app->atlas);

    source_editor->start_address_label = CreateLabel(L"Start Address:",app->atlas);
    source_editor->start_address_input = CreateInput(4,FORMAT_HEX);
    source_editor->start_address = 0x0000;

    source_editor->editor = malloc(sizeof(Text_Editor));

    source_editor->editor->lines = TextEditor_PushLine(NULL);
    source_editor->editor->current_line = source_editor->editor->lines;

    source_editor->editor->start_visible_line = source_editor->editor->current_line;
    source_editor->editor->end_visible_line = source_editor->editor->current_line;

    source_editor->editor->undo = NULL;
    source_editor->editor->redo = NULL;

    source_editor->editor->number_of_lines = 1;
    source_editor->editor->start_selection = (Vector2i){0};
    source_editor->editor->end_selection = (Vector2i){0};
    source_editor->editor->glyph_size = source_editor->app->glyph_size;
    source_editor->editor->text_selected = false;
    source_editor->editor->selected = false;

    memset(&source_editor->editor->vertical_scroll_bar,0,sizeof(Scroll_Bar));
    memset(&source_editor->editor->horizontal_scroll_bar,0,sizeof(Scroll_Bar));
    memset(&source_editor->editor->cursor,0,sizeof(Cursor));
}

void SourceEditorConfigRect(Source_Editor *source_editor){
    
    source_editor->label->rect = (Recti){
        .x = source_editor->app->stack_rect.x + source_editor->app->stack_rect.w + BORDER,
        .y = source_editor->app->breakpoints->rect.y + source_editor->app->breakpoints->rect.h + BORDER,
        .w = source_editor->label->text_size.x,
        .h = source_editor->label->text_size.y + SPACING_Y
    };

    source_editor->label->pos = (Vector2i){
        .x = source_editor->label->rect.x + source_editor->label->rect.w * 0.5f - source_editor->label->text_size.x * 0.5f,
        .y = source_editor->label->rect.y + source_editor->label->rect.h * 0.5f - source_editor->label->text_size.y * 0.5f
    };

    //((screen_height - (border * 3.0f)) * 0.6f) - ((glyph_size.y + SPACING_Y) * 3.0f) - (border * 2.0f)
    source_editor->editor->rect = (Recti){
        .x = source_editor->app->stack_rect.x + source_editor->app->stack_rect.w + BORDER,
        .y = source_editor->label->rect.y + source_editor->label->rect.h,
        .w = (source_editor->app->screen_size.x - BORDER) - (source_editor->app->stack_rect.x + source_editor->app->stack_rect.w + BORDER),
        .h = ((source_editor->app->screen_size.y - (BORDER * 3.0f)) * 0.6f) - ((source_editor->app->glyph_size.y + SPACING_Y) * 3.0f) - (BORDER * 2.0f)
    };

    SourceEditorConfigVerticalScrollBar(source_editor);
    SourceEditorConfigHorizontalScrollBar(source_editor);

    source_editor->load_file_button->rect = (Recti){
        .x = source_editor->app->stack_rect.x + source_editor->app->stack_rect.w + BORDER,
        .y = source_editor->editor->rect.y + source_editor->editor->rect.h + BORDER,
        .w = source_editor->load_file_button->text_size.x,
        .h = source_editor->load_file_button->text_size.y + SPACING_Y
    };
    source_editor->load_file_button->pos = (Vector2i){
        .x = source_editor->load_file_button->rect.x + source_editor->load_file_button->rect.w * 0.5f - source_editor->load_file_button->text_size.x * 0.5f,
        .y = source_editor->load_file_button->rect.y + source_editor->load_file_button->rect.h * 0.5f - source_editor->load_file_button->text_size.y * 0.5f
    };

    source_editor->load_memory_button->rect = (Recti){
        .x = source_editor->load_file_button->rect.x + source_editor->load_file_button->rect.w + (BORDER * 2),
        .y = source_editor->editor->rect.y + source_editor->editor->rect.h + BORDER,
        .w = source_editor->load_memory_button->text_size.x,
        .h = source_editor->load_memory_button->text_size.y + SPACING_Y
    };
    source_editor->load_memory_button->pos = (Vector2i){
        .x = source_editor->load_memory_button->rect.x + source_editor->load_memory_button->rect.w * 0.5f - source_editor->load_memory_button->text_size.x * 0.5f,
        .y = source_editor->load_memory_button->rect.y + source_editor->load_memory_button->rect.h * 0.5f - source_editor->load_memory_button->text_size.y * 0.5f
    };

    source_editor->clear_memory_button->rect = (Recti){
        .x = source_editor->load_memory_button->rect.x + source_editor->load_memory_button->rect.w + (BORDER * 2),
        .y = source_editor->editor->rect.y + source_editor->editor->rect.h + BORDER,
        .w = source_editor->clear_memory_button->text_size.x,
        .h = source_editor->clear_memory_button->text_size.y + SPACING_Y
    };
    source_editor->clear_memory_button->pos = (Vector2i){
        .x = source_editor->clear_memory_button->rect.x + source_editor->clear_memory_button->rect.w * 0.5f - source_editor->clear_memory_button->text_size.x * 0.5f,
        .y = source_editor->clear_memory_button->rect.y + source_editor->clear_memory_button->rect.h * 0.5f - source_editor->clear_memory_button->text_size.y * 0.5f
    };

    source_editor->start_address_label->rect = (Recti){
        .x = source_editor->app->stack_rect.x + source_editor->app->stack_rect.w + BORDER,
        .y = source_editor->load_file_button->rect.y + source_editor->load_file_button->rect.h + BORDER,
        .w = source_editor->start_address_label->text_size.x,
        .h = source_editor->start_address_label->text_size.y + SPACING_Y
    };
    source_editor->start_address_label->pos = (Vector2i){
        .x = source_editor->start_address_label->rect.x + source_editor->start_address_label->rect.w * 0.5f - source_editor->start_address_label->text_size.x * 0.5f,
        .y = source_editor->start_address_label->rect.y + source_editor->start_address_label->rect.h * 0.5f - source_editor->start_address_label->text_size.y * 0.5f
    };

    source_editor->start_address_input->rect = (Recti){
        .x = source_editor->start_address_label->rect.x + source_editor->start_address_label->rect.w + source_editor->app->glyph_size.x,
        .y = source_editor->start_address_label->rect.y + source_editor->start_address_label->rect.h * 0.5f - source_editor->app->glyph_size.y * 0.5f,
        .w = source_editor->start_address_input->max_length * source_editor->app->glyph_size.x,
        .h = source_editor->app->glyph_size.y
    };
    source_editor->start_address_input->pos = (Vector2i){
        .x = source_editor->start_address_input->rect.x,
        .y = source_editor->start_address_input->rect.y
    };
}

void SourceEditorDraw(Source_Editor *source_editor){

    DrawLabel(source_editor->app->renderer,source_editor->label,COLOR_ON);

    TextEditor_Draw(source_editor->editor,source_editor->app->renderer,source_editor->app->atlas);

    //Load File
    DrawButtonText(source_editor->app->renderer,source_editor->load_file_button,SELECTED_COLOR,COLOR_ON);
    //Load Memory
    DrawButtonText(source_editor->app->renderer,source_editor->load_memory_button,SELECTED_COLOR,COLOR_ON);
    //Clear Memory
    DrawButtonText(source_editor->app->renderer,source_editor->clear_memory_button,SELECTED_COLOR,COLOR_ON);

    //Start Address
    DrawLabel(source_editor->app->renderer,source_editor->start_address_label,COLOR_ON);
    swprintf(source_editor->app->buffer,BUFFER_SIZE,L"%.4X",source_editor->start_address);
    DrawInput(source_editor->app,source_editor->start_address_input,source_editor->app->buffer);
}

void SourceEditorEvent(Source_Editor *source_editor){

    if(source_editor->app->breakpoints->conditions->opened || source_editor->app->popup->opened) return;
    
    SDL_Event event = source_editor->app->event;

    SourceEditorMouseOver(source_editor);

    if(event.type == SDL_MOUSEWHEEL && PointInRect(source_editor->app->mouse.pos,source_editor->editor->rect) && source_editor->editor->vertical_scroll_bar.valid){
        if(event.wheel.preciseY < 0.0f){
            //up
            SetYScrollBar(&source_editor->editor->vertical_scroll_bar,source_editor->editor->vertical_scroll_bar.offset - (source_editor->editor->glyph_size.y * 3));
        }
        else if(event.wheel.preciseY > 0.0f){
            //down
            SetYScrollBar(&source_editor->editor->vertical_scroll_bar,source_editor->editor->vertical_scroll_bar.offset + (source_editor->editor->glyph_size.y * 3));
        }
        TextEditor_SetVisibleLines(source_editor->editor);
        TextEditor_SetHorizontalScrollBarSize(source_editor->editor);
    }
    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
        
        SourceEditorMouseDown(source_editor);
    }
    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT){
        
        SourceEditorMouseUp(source_editor);

        source_editor->editor->cursor.on = false;

        source_editor->editor->vertical_scroll_bar.on = false;
        source_editor->editor->horizontal_scroll_bar.on = false;
    }
    else if(SDL_BUTTON(source_editor->app->mouse.state) & SDL_BUTTON_LEFT){
        
        if(source_editor->editor->cursor.on){
            TextEditor_DraggingMouse(source_editor->editor,source_editor->app->mouse,SDL_GetTicks());
        }
        else if(source_editor->editor->vertical_scroll_bar.on){
            HandleVerticalScrollBar(&source_editor->editor->vertical_scroll_bar,source_editor->app->mouse);
            TextEditor_SetVisibleLines(source_editor->editor);
        }
        else if(source_editor->editor->horizontal_scroll_bar.on){
            HandleHorizontalScrollBar(&source_editor->editor->horizontal_scroll_bar,source_editor->app->mouse);
        }
    }
    else if(event.type == SDL_TEXTINPUT && source_editor->app->source_editor->editor->selected){

        static wchar_t wide_string[SDL_TEXTINPUTEVENT_TEXT_SIZE] = {0};

        if(MultiByteToWideChar(CP_UTF8,MB_COMPOSITE,event.text.text,-1,wide_string,SDL_TEXTINPUTEVENT_TEXT_SIZE) == 0){
            printf("Error in char to wchar converter: %d\n",GetLastError());
        }
        else{
            TextEditor_Input(source_editor->editor,wide_string[0]);
        }
    }
    else if(event.type == SDL_KEYDOWN && source_editor->editor->selected){
        
        if((event.key.keysym.scancode == SDL_SCANCODE_C  || event.key.keysym.scancode == SDL_SCANCODE_X) && SDL_GetModState() & KMOD_CTRL){
            TextEditor_CTRL_C_Or_X(source_editor->editor,event);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_V && SDL_GetModState() & KMOD_CTRL){
            TextEditor_CTRL_V(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_Z && SDL_GetModState() & KMOD_CTRL && source_editor->editor->undo){
            if(source_editor->editor->text_selected) TextEditor_DeselectLines(source_editor->editor);
            TextEditor_UndoAndRedo(source_editor->editor,UNDO,false);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_Y && SDL_GetModState() & KMOD_CTRL && source_editor->editor->redo){
            if(source_editor->editor->text_selected) TextEditor_DeselectLines(source_editor->editor);
            TextEditor_UndoAndRedo(source_editor->editor,REDO,false);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
            TextEditor_Backspace(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_RETURN){
            TextEditor_Return(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_UP && source_editor->editor->current_line->previous){
            TextEditor_Up(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN && source_editor->editor->current_line->next){
            TextEditor_Down(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_LEFT){
            TextEditor_Left(source_editor->editor);
        }
        else if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT){
            TextEditor_Right(source_editor->editor);
        }
    }
}

void SourceEditorFree(Source_Editor *source_editor){
    TextEditor_Free(source_editor->editor);

    LabelFree(source_editor->label);

    ButtonTextFree(source_editor->load_file_button);
    ButtonTextFree(source_editor->load_memory_button);
    ButtonTextFree(source_editor->clear_memory_button);

    LabelFree(source_editor->start_address_label);
    InputFree(source_editor->start_address_input);
    
    free(source_editor);
}
