#include <text_editor.h>

bool SetClipboardText(wchar_t *text){
    if(!OpenClipboard(NULL)) return false;
    EmptyClipboard();
    HANDLE *handle_memory = GlobalAlloc(GHND,sizeof(wchar_t) * (wcslen(text) + 1));
    wchar_t *data = GlobalLock(handle_memory);
    if(!data) return false;
    wcscpy(data,text);
    GlobalUnlock(handle_memory);
    SetClipboardData(CF_UNICODETEXT,handle_memory);
    CloseClipboard();
    return true;
}

wchar_t* GetClipboardText(){
    if(!OpenClipboard(NULL)) return NULL;
    HANDLE *handle_clipboard = GetClipboardData(CF_UNICODETEXT);
    wchar_t *data = GlobalLock(handle_clipboard);
    if(!data) return NULL;
    wchar_t *text = wcsdup(data);
    GlobalUnlock(handle_clipboard);
    CloseClipboard();
    return text;
}

void FreeActionStack(Action_Stack *action_stack){
    Action_Stack *buffer;
    while(action_stack){
        buffer = action_stack->next;
        GapBufferFree(action_stack->gap_buffer);
        free(action_stack);
        action_stack = buffer;
    }
}

uint32_t GetSequenceNumberOfSpaces(GapBuffer *gap_buffer){
    if(!gap_buffer) return 0;
    uint32_t spaces = 0;
    wchar_t *ptr = gap_buffer->buffer;
    while(ptr < (wchar_t*)gap_buffer->buffer_end){
        if(ptr == (wchar_t*)gap_buffer->gap_start){
            ptr = gap_buffer->gap_end;
            if(ptr >= (wchar_t*)gap_buffer->buffer_end) break;
        }
        if(ptr[0] == L' ') spaces++;
        else               return 0;
        ptr++;
    }
    return spaces;
}


Vector2i TextEditor_GetPosInText(Text_Editor *text_editor,Mouse mouse){
    Vector2i position;
    position.x = (mouse.pos.x-text_editor->rect.x)-text_editor->horizontal_scroll_bar.offset;
    position.y = (mouse.pos.y-text_editor->rect.y)-text_editor->vertical_scroll_bar.offset;
    position.x = round(position.x / (float)text_editor->glyph_size.x);
    position.y = floor(position.y / (float)text_editor->glyph_size.y);
    if(position.x < 0) position.x = 0;
    if(position.y < 0) position.y = 0;
    return position;
}

void TextEditor_SetPosInText(Text_Editor *text_editor){
    int last_x = text_editor->cursor.pos.x;
    int last_y = text_editor->cursor.pos.y;

    if(text_editor->end_selection.y < text_editor->number_of_lines){
        if(text_editor->current_line->index != text_editor->end_selection.y){
            text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->end_selection.y);
        }
        uint32_t content_size = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
        text_editor->cursor.pos.x = (content_size < text_editor->end_selection.x) ? content_size : text_editor->end_selection.x;
        text_editor->cursor.pos.y = text_editor->current_line->index;
    }
    else{
        if(text_editor->current_line->index != text_editor->number_of_lines - 1){
            text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->number_of_lines - 1);
        }
        text_editor->cursor.pos.x = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
        text_editor->cursor.pos.y = text_editor->current_line->index;
    }

    if(last_x != text_editor->cursor.pos.x || last_y != text_editor->cursor.pos.y){
        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);
        text_editor->cursor.update = true;
    }
}

void TextEditor_DeselectLines(Text_Editor *text_editor){
    Line *line = text_editor->lines;
    while(line){
        line->selected = false;
        line->start_selection = -1;
        line->end_selection = -1;
        line = line->next;
    }
    text_editor->text_selected = false;
}

void TextEditor_SortLines(Text_Editor *text_editor){
    Line *line = text_editor->lines;
    uint32_t index = 0;
    while(line != NULL){
        line->index = index++;
        line = line->next;
    }
    text_editor->number_of_lines = index;
}

void TextEditor_FreeLines(Line *lines){
    Line *buffer_line;
    while(lines){
        buffer_line = lines->next;
        GapBufferFree(lines->gap_buffer);
        free(lines);
        lines = buffer_line;
    }
}

Line* TextEditor_GetLine(Line *lines,int index){
    Line *buffer_line = lines;
    while(buffer_line){
        if(buffer_line->index == index) return buffer_line;
        buffer_line = buffer_line->next;
    }
    return NULL;
}

Line* TextEditor_RemoveLine(Line *line){

    Line* buffer_line = line->previous;

    if(ContentSize(line->gap_buffer,wchar_t) > 0){
        GapBufferMoveGap(buffer_line->gap_buffer,wchar_t,ContentSize(buffer_line->gap_buffer,wchar_t));
        GapBufferInsertInto(buffer_line->gap_buffer,line->gap_buffer,wchar_t);
    }
    
    if(line->next){
        buffer_line->next = line->next;
        line->next->previous = buffer_line;
    }
    else{
        buffer_line->next = NULL;
    }

    GapBufferFree(line->gap_buffer);
    
    free(line);

    return buffer_line;
}

Line* TextEditor_PushLine(Line *line){
    
    Line *new_line = calloc(1,sizeof(Line));

    CreateGapBuffer(new_line->gap_buffer,wchar_t,GAP_EXPAND_SIZE);
    
    if(line){
        new_line->previous = line;
        if(line->next){
            new_line->next = line->next;
            line->next->previous = new_line;
        }
        line->next = new_line;
    }

    return new_line;
}

void TextEditor_SetVisibleLines(Text_Editor *text_editor){
    Line *line = text_editor->lines;
    text_editor->start_visible_line = NULL;
    text_editor->end_visible_line = NULL;
    while(line != NULL){
        if(line->index * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset > text_editor->rect.h){
            break;
        }
        else if((line->index + 1) * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset >=0.0f){
            
            if(!text_editor->start_visible_line) text_editor->start_visible_line = line;
            else                                 text_editor->end_visible_line = line;
        }
        line = line->next;
    }
}

void TextEditor_AddNewAction(Text_Editor *text_editor,uint8_t action_flag){
    Action_Stack *action = malloc(sizeof(Action_Stack));
    CreateGapBuffer(action->gap_buffer,wchar_t,GAP_EXPAND_SIZE);
    action->action = action_flag;
    action->text_selected = false;
    action->used = false;
    action->chained = false;
        
    action->origin_cursor = text_editor->cursor.pos;
    action->target_cursor = text_editor->cursor.pos;

    action->start_modification = text_editor->cursor.pos;
    action->end_modification = text_editor->cursor.pos;

    action->next = text_editor->undo;
    text_editor->undo = action;

    if(text_editor->redo){
        FreeActionStack(text_editor->redo);
        text_editor->redo = NULL;
    }
}

bool TextEditor_CursorYIsVisible(Text_Editor *text_editor){
    if(!text_editor->vertical_scroll_bar.valid){
        return true;
    }
    else if((text_editor->cursor.pos.y + 1.0f) * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset > text_editor->rect.h){
        return false;
    }
    else if(text_editor->cursor.pos.y * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset < 0){
        return false;
    }
    return true;
}

void TextEditor_Draw(Text_Editor *text_editor,Renderer *renderer,FontAtlas *atlas){

    DrawFilledRectanglei(renderer,&text_editor->rect,BACKGROUND2_COLOR);

    RendererSetViewport(renderer,NULL,&text_editor->rect);

    Line *line = text_editor->start_visible_line;
    int start,end;
    uint32_t content_size;

    while(line != NULL){

        content_size = ContentSize(line->gap_buffer,wchar_t);

        if(line->selected == true){

            if(line->start_selection == 0 && line->end_selection == 0){

                if(line->next && line->next->selected){start = 0;end = 1;}
                else                                  {start = 0;end = 0;}
            }
            else if(line->start_selection == content_size && line->end_selection == content_size){
                start = content_size;
                end = content_size+1;
            }
            else{
                start = line->start_selection;
                end = line->end_selection;
            }

            Rectf rect = {
                .x = (start * text_editor->glyph_size.x + 1.0f) + text_editor->horizontal_scroll_bar.offset,
                .y = (line->index * text_editor->glyph_size.y + 1.0f) + text_editor->vertical_scroll_bar.offset,
                .w = (end - start) * text_editor->glyph_size.x,
                .h = text_editor->glyph_size.y
            };

            DrawFilledRectanglef(renderer,&rect,TEXT_SELECTED_COLOR);
        }

        if(ContentSize(line->gap_buffer,wchar_t)){

            Vector2f pos = {
                .x = 1.0f + text_editor->horizontal_scroll_bar.offset,
                .y = (line->index * text_editor->glyph_size.y + 1.0f) + text_editor->vertical_scroll_bar.offset,
            };

            DrawTextByGapBufferUint16f(renderer,atlas,line->gap_buffer,pos,COLOR_ON);
        }

        if(line != text_editor->end_visible_line) line = line->next;
        else                                      break;
    }

    UpdateCursor(&text_editor->cursor,SDL_GetTicks());
    
    if(text_editor->cursor.visible && text_editor->selected){
        Vector2f point1 = {
            .x = text_editor->cursor.pos.x * text_editor->glyph_size.x + 1.0f + text_editor->horizontal_scroll_bar.offset,
            .y = text_editor->cursor.pos.y * text_editor->glyph_size.y + 1.0f + text_editor->vertical_scroll_bar.offset
        };

        Vector2f point2 = {
            .x = point1.x,
            .y = point1.y + text_editor->glyph_size.y - 1.0f
        };

        DrawLinef(renderer,point1,point2,CURSOR_COLOR);
    }

    RendererSetViewport(renderer,NULL,NULL);

    DrawScrollBar(&text_editor->vertical_scroll_bar,renderer);
    DrawScrollBar(&text_editor->horizontal_scroll_bar,renderer);

    #ifdef DEBUG
    DrawRectanglei(renderer,&text_editor->rect,DEBUG_COLOR);
    #endif
}

void TextEditor_Free(Text_Editor *text_editor){
    TextEditor_FreeLines(text_editor->lines);
    FreeActionStack(text_editor->undo);
    FreeActionStack(text_editor->redo);
    free(text_editor);
}


void TextEditor_AjusHorizontalScrollBarOffsetByCursor(Text_Editor *text_editor){
    
    if(!text_editor->horizontal_scroll_bar.valid) return;

    float length_diff = text_editor->horizontal_scroll_bar.length - text_editor->horizontal_scroll_bar.bar_rect.w;
    float scroll_length = text_editor->horizontal_scroll_bar.bar_rect.w - text_editor->horizontal_scroll_bar.scroll_rect.w;

    if((text_editor->cursor.pos.x + 1.0f) * text_editor->glyph_size.x + text_editor->horizontal_scroll_bar.offset > text_editor->horizontal_scroll_bar.bar_rect.w){

        float diff = (text_editor->cursor.pos.x + 1.0f) * text_editor->glyph_size.x + text_editor->horizontal_scroll_bar.offset - text_editor->horizontal_scroll_bar.bar_rect.w;
        text_editor->horizontal_scroll_bar.scroll_rect.x += diff / length_diff * scroll_length;
    }
    else if(text_editor->cursor.pos.x * text_editor->glyph_size.x + text_editor->horizontal_scroll_bar.offset < 0){

        float diff = text_editor->cursor.pos.x * text_editor->glyph_size.x + text_editor->horizontal_scroll_bar.offset;
        text_editor->horizontal_scroll_bar.scroll_rect.x += diff / length_diff * scroll_length;
    }

    AjustHorizontalScrollBar(&text_editor->horizontal_scroll_bar);
    SetHorizontalOffsetScrollBar(&text_editor->horizontal_scroll_bar);
}

void TextEditor_AjustVerticalScrollBarOffsetByCursor(Text_Editor *text_editor){
    
    if(!text_editor->vertical_scroll_bar.valid) return;

    float length_diff = text_editor->vertical_scroll_bar.length - text_editor->vertical_scroll_bar.bar_rect.h;
    float scroll_length = text_editor->vertical_scroll_bar.bar_rect.h - text_editor->vertical_scroll_bar.scroll_rect.h;

    if((text_editor->cursor.pos.y + 1.0f) * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset > text_editor->vertical_scroll_bar.bar_rect.h - SCROLL_BAR_SIZE){

        float diff = (text_editor->cursor.pos.y + 1.0f) * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset - (text_editor->vertical_scroll_bar.bar_rect.h - SCROLL_BAR_SIZE);
        text_editor->vertical_scroll_bar.scroll_rect.y +=  diff / length_diff * scroll_length;
    }
    else if(text_editor->cursor.pos.y * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset < 0.0f){

        float diff = text_editor->cursor.pos.y * text_editor->glyph_size.y + text_editor->vertical_scroll_bar.offset;
        text_editor->vertical_scroll_bar.scroll_rect.y +=  diff / length_diff * scroll_length;
    }

    SetVerticalOffsetScrollBar(&text_editor->vertical_scroll_bar);
}

void TextEditor_SetHorizontalScrollBarSize(Text_Editor *text_editor){

    text_editor->horizontal_scroll_bar.length = 0;

    Line *line = text_editor->start_visible_line;

    while(line != NULL){
        int width = ContentSize(line->gap_buffer,wchar_t) * text_editor->glyph_size.x + 2;

        if(width > text_editor->horizontal_scroll_bar.length){
            text_editor->horizontal_scroll_bar.length = width;
        }

        if(line != text_editor->end_visible_line) line = line->next;
        else                                      break;
    }

    text_editor->horizontal_scroll_bar.scroll_rect.w = GetScrollBarWidth(text_editor->horizontal_scroll_bar);

    if(text_editor->horizontal_scroll_bar.scroll_rect.w > 0.0f && text_editor->horizontal_scroll_bar.scroll_rect.w < text_editor->horizontal_scroll_bar.bar_rect.w){
        text_editor->horizontal_scroll_bar.valid = true;
        AjustHorizontalScrollBar(&text_editor->horizontal_scroll_bar);
        SetHorizontalOffsetScrollBar(&text_editor->horizontal_scroll_bar);
    }
    else{
        text_editor->horizontal_scroll_bar.valid = false;
        text_editor->horizontal_scroll_bar.scroll_rect.x = text_editor->horizontal_scroll_bar.bar_rect.x;
        text_editor->horizontal_scroll_bar.offset = 0.0f;
    }
}

void TextEditor_SetVerticalScrollBarSize(Text_Editor *text_editor){
    text_editor->vertical_scroll_bar.length = (text_editor->number_of_lines * text_editor->glyph_size.y) + (text_editor->rect.h - text_editor->glyph_size.y);

    text_editor->vertical_scroll_bar.scroll_rect.h = GetScrollBarHeight(text_editor->vertical_scroll_bar);

    if(text_editor->vertical_scroll_bar.scroll_rect.h > 0.0f && text_editor->vertical_scroll_bar.scroll_rect.h < text_editor->vertical_scroll_bar.bar_rect.h){
        text_editor->vertical_scroll_bar.valid = true;
        AjustVerticalScrollBar(&text_editor->vertical_scroll_bar);
        SetVerticalOffsetScrollBar(&text_editor->vertical_scroll_bar);
    }
    else{
        text_editor->vertical_scroll_bar.valid = false;
        text_editor->vertical_scroll_bar.scroll_rect.y = text_editor->vertical_scroll_bar.bar_rect.y;
        text_editor->vertical_scroll_bar.offset = 0.0f;
    }
}


static void RemoveSelectedTextInSingleLine(Text_Editor *text_editor){
    
    TextEditor_AddNewAction(text_editor,ACTION_ADD_TEXT);
    
    text_editor->undo->text_selected = true;
    text_editor->undo->used = true;

    text_editor->undo->start_modification = (Vector2i){text_editor->current_line->start_selection,text_editor->current_line->index};
    text_editor->undo->end_modification = (Vector2i){text_editor->current_line->end_selection,text_editor->current_line->index};

    wchar_t *content;
    GapBufferGetSubContent(text_editor->current_line->gap_buffer,wchar_t,text_editor->current_line->start_selection,text_editor->current_line->end_selection,content);

    if(content){
        GapBufferInsertValues(text_editor->undo->gap_buffer,wchar_t,content,wcslen(content));
        free(content);
    }

    GapBufferRemoveInInterval(text_editor->current_line->gap_buffer,wchar_t,text_editor->current_line->start_selection,text_editor->current_line->end_selection);
            
    text_editor->cursor.pos.x = text_editor->current_line->start_selection;
    text_editor->cursor.update = true;

    text_editor->text_selected = false;
    text_editor->current_line->selected = false;
    text_editor->current_line->start_selection = 0;
    text_editor->current_line->end_selection = 0;

    bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor); 
    
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
    
    if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

static void RemoveSelectedTextInMultLine(Text_Editor *text_editor,Line *line_selected,int start_y,int end_y){
    
    TextEditor_AddNewAction(text_editor,ACTION_ADD_TEXT);
    
    text_editor->undo->text_selected = true;
    text_editor->undo->used = true;

    text_editor->current_line = line_selected;

    uint32_t content_size = ContentSize(text_editor->current_line->gap_buffer,wchar_t);

    text_editor->cursor.pos.x = (text_editor->current_line->start_selection > content_size) ? content_size : text_editor->current_line->start_selection;
    text_editor->cursor.pos.y = text_editor->current_line->index;

    text_editor->undo->start_modification = text_editor->cursor.pos;

    text_editor->cursor.update = true;

    while(line_selected && line_selected->index <= end_y){

        if(line_selected->index == end_y || line_selected->next == NULL){
            uint32_t content_size = ContentSize(line_selected->gap_buffer,wchar_t);
            text_editor->undo->end_modification.x = (line_selected->end_selection > content_size) ? content_size : line_selected->end_selection;
            text_editor->undo->end_modification.y = line_selected->index;
        }

        if(ContentSize(line_selected->gap_buffer,wchar_t) > 0){

            if(line_selected->index == start_y || line_selected->index == end_y){
                wchar_t *content;
                GapBufferGetSubContent(line_selected->gap_buffer,wchar_t,line_selected->start_selection,line_selected->end_selection,content);
                
                if(content){
                    GapBufferInsertValues(text_editor->undo->gap_buffer,wchar_t,content,wcslen(content));
                    free(content);
                }

                GapBufferRemoveInInterval(line_selected->gap_buffer,wchar_t,line_selected->start_selection,line_selected->end_selection);
            }
            else{
                GapBufferInsertInto(text_editor->undo->gap_buffer,line_selected->gap_buffer,wchar_t);
                GapBufferClear(line_selected->gap_buffer);
            }
        }

        if(line_selected->index != end_y && line_selected->next != NULL){
            GapBufferInsertValue(text_editor->undo->gap_buffer,wchar_t,L'\n');
        }

        if(line_selected->index != start_y){
            line_selected = TextEditor_RemoveLine(line_selected);
        }

        line_selected = line_selected->next;
    }

    GapBufferInsertValue(text_editor->undo->gap_buffer,wchar_t,L'\0');

    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    TextEditor_SortLines(text_editor);

    TextEditor_DeselectLines(text_editor);

    TextEditor_SetVerticalScrollBarSize(text_editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

static void RemoveValue(Text_Editor *text_editor){
    wchar_t value;
    GapBufferGetValue(text_editor->current_line->gap_buffer,wchar_t,PresentValue,value);
                    
    GapBufferMoveGap(text_editor->undo->gap_buffer,wchar_t,0);
    GapBufferInsertValue(text_editor->undo->gap_buffer,wchar_t,value);
                    
    text_editor->undo->start_modification.x--;
                    
    GapBufferRemove(text_editor->current_line->gap_buffer,wchar_t,1);
    text_editor->cursor.pos.x--;
}

static void RemoveValueInLine(Text_Editor *text_editor){
    
    if(!text_editor->undo || text_editor->undo->used || text_editor->undo->action & ACTION_DELETE_TEXT || !Vector2IsEqual(text_editor->cursor.pos,text_editor->undo->start_modification)){
        TextEditor_AddNewAction(text_editor,ACTION_ADD_TEXT);
    }

    RemoveValue(text_editor);

    text_editor->cursor.update = true;

    bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

static void RemoveLine(Text_Editor *text_editor){
    TextEditor_AddNewAction(text_editor,ACTION_ADD_LINE);

    text_editor->undo->used = true;

    text_editor->cursor.pos.x = ContentSize(text_editor->current_line->previous->gap_buffer,wchar_t);
    text_editor->cursor.pos.y--;

    text_editor->undo->origin_cursor = text_editor->cursor.pos;
        
    text_editor->current_line = TextEditor_RemoveLine(text_editor->current_line);

    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    TextEditor_SortLines(text_editor);

    text_editor->cursor.update = true;

    TextEditor_SetVerticalScrollBarSize(text_editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

static bool InputAddNewAction(Text_Editor *text_editor,wchar_t character){
    
    if(
        !text_editor->undo ||
        text_editor->undo->used ||
        text_editor->undo->action & ACTION_ADD_TEXT ||
        !Vector2IsEqual(text_editor->cursor.pos,text_editor->undo->target_cursor)
    ) return true;

    uint32_t spaces = GetSequenceNumberOfSpaces(text_editor->undo->gap_buffer);

    if(character == L' ' && spaces == 0) return true;

    if(character != L' ' && spaces > 1) return true;

    return false;
}


void TextEditor_Input(Text_Editor *text_editor,wchar_t character){

    if(text_editor->text_selected && TextEditor_Backspace(text_editor) && text_editor->undo) text_editor->undo->chained = true;

    if(InputAddNewAction(text_editor,character)){
        
        bool chained = (text_editor->undo && text_editor->undo->action & ACTION_ADD_TEXT && text_editor->undo->chained) ? true : false;
        
        TextEditor_AddNewAction(text_editor,ACTION_DELETE_TEXT);
        
        text_editor->undo->chained = chained;
    }

    GapBufferInsertValue(text_editor->undo->gap_buffer,wchar_t,character);
    GapBufferInsertValue(text_editor->current_line->gap_buffer,wchar_t,character);
        
    text_editor->undo->end_modification.x++;
    text_editor->undo->target_cursor.x++;
        
    text_editor->cursor.pos.x++;

    text_editor->cursor.update = true;

    bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

void TextEditor_CTRL_C_Or_X(Text_Editor *text_editor,SDL_Event event){
    
    wchar_t *text = NULL;
    int start_y,end_y;

    if(text_editor->start_selection.y < text_editor->end_selection.y){
        start_y = text_editor->start_selection.y;
        end_y = text_editor->end_selection.y;
    }
    else{
        start_y = text_editor->end_selection.y;
        end_y = text_editor->start_selection.y;
    }

    Line *text_selected = TextEditor_GetLine(text_editor->lines,start_y);
           
    if((text_editor->end_selection.y == text_editor->start_selection.y || !text_selected->next) && text_editor->current_line->start_selection < ContentSize(text_editor->current_line->gap_buffer,wchar_t)){
        
        GapBufferGetSubContent(text_editor->current_line->gap_buffer,wchar_t,text_editor->current_line->start_selection,text_editor->current_line->end_selection,text);

        if(event.key.keysym.scancode == SDL_SCANCODE_X) RemoveSelectedTextInSingleLine(text_editor);
    }
    else if(text_editor->end_selection.y != text_editor->start_selection.y){
        
        wchar_t *buffer;

        GapBuffer *gap_buffer;
        CreateGapBuffer(gap_buffer,wchar_t,GAP_EXPAND_SIZE);
        
        if(event.key.keysym.scancode == SDL_SCANCODE_X){
            
            TextEditor_AddNewAction(text_editor,ACTION_ADD_TEXT);

            text_editor->undo->text_selected = true;
            text_editor->undo->chained = false;

            text_editor->current_line = text_selected;

            uint32_t content_size = ContentSize(text_selected->gap_buffer,wchar_t);
            
            text_editor->cursor.pos.x = (text_selected->start_selection > content_size) ? content_size : text_selected->start_selection;
            text_editor->cursor.pos.y = text_selected->index;

            text_editor->undo->start_modification = text_editor->cursor.pos;

            text_editor->cursor.update = true;
        }

        while((text_selected && text_selected->index <= end_y)){

            GapBufferGetSubContent(text_selected->gap_buffer,wchar_t,text_selected->start_selection,text_selected->end_selection,buffer);
            
            if(buffer){
                GapBufferInsertValues(gap_buffer,wchar_t,buffer,wcslen(buffer));
                free(buffer);
            }

            if(event.key.keysym.scancode == SDL_SCANCODE_X){

                if(text_selected->index == end_y || text_selected->next == NULL){
                    uint32_t content_size = ContentSize(text_selected->gap_buffer,wchar_t);
                    text_editor->undo->end_modification.x = (text_selected->end_selection > content_size) ? content_size : text_selected->end_selection;
                    text_editor->undo->end_modification.y = text_selected->index;
                }

                GapBufferRemoveInInterval(text_selected->gap_buffer,wchar_t,text_selected->start_selection,text_selected->end_selection);

                if(text_selected->index != start_y){
                    text_selected = TextEditor_RemoveLine(text_selected);
                }
            }

            if(text_selected->next && text_selected->next->selected) GapBufferInsertValue(gap_buffer,wchar_t,L'\n');

            text_selected = text_selected->next;
        }

        GapBufferInsertValue(gap_buffer,wchar_t,L'\0');

        if(event.key.keysym.scancode == SDL_SCANCODE_X){

            GapBufferInsertInto(text_editor->undo->gap_buffer,gap_buffer,wchar_t);

            GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

            TextEditor_SortLines(text_editor);

            TextEditor_DeselectLines(text_editor);

            TextEditor_SetVerticalScrollBarSize(text_editor);
            TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
            
            TextEditor_SetVisibleLines(text_editor);

            TextEditor_SetHorizontalScrollBarSize(text_editor);
            TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
        }

        GapBufferGetContent(gap_buffer,wchar_t,text);

        GapBufferFree(gap_buffer);
    }

    if(text){
        SetClipboardText(text);
        free(text);
    }
}

void TextEditor_CTRL_V(Text_Editor *text_editor){

    wchar_t *text = GetClipboardText();

    if(!text) return;

    uint32_t size_text = wcslen(text);

    bool chained = false;

    if(text_editor->text_selected && TextEditor_Backspace(text_editor) && text_editor->undo){
        chained = true;
        text_editor->undo->chained = chained;
    }

    TextEditor_AddNewAction(text_editor,ACTION_DELETE_TEXT);

    text_editor->undo->chained = chained;

    GapBufferInsertValues(text_editor->undo->gap_buffer,wchar_t,text,wcslen(text));
    GapBufferInsertValue(text_editor->undo->gap_buffer,wchar_t,L'\0');

    text_editor->undo->start_modification = text_editor->cursor.pos;

    wchar_t *rest;
    GapBufferGetContentAfterGap(text_editor->current_line->gap_buffer,wchar_t,rest);

    text_editor->current_line->gap_buffer->gap_end = text_editor->current_line->gap_buffer->buffer_end;

    GapBuffer *token;
    CreateGapBuffer(token,wchar_t,GAP_EXPAND_SIZE);

    for(int i=0; i<=size_text; ++i){

        if(text[i] == '\n' || text[i] == '\0'){

            GapBufferInsertInto(text_editor->current_line->gap_buffer,token,wchar_t);

            if(text[i] == '\n'){
                text_editor->current_line = TextEditor_PushLine(text_editor->current_line);
                text_editor->cursor.pos.y++;
            }

            GapBufferClear(token);
        }
        else if(text[i] == L' ' || !iswspace(text[i])){

            GapBufferInsertValue(token,wchar_t,text[i]);
        }
    }

    text_editor->cursor.pos.x = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
    text_editor->cursor.update = true;

    text_editor->undo->end_modification = text_editor->cursor.pos;
    text_editor->undo->target_cursor = text_editor->cursor.pos;

    if(rest){
        GapBufferInsertValues(text_editor->current_line->gap_buffer,wchar_t,rest,wcslen(rest));
        free(rest);
    }

    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    GapBufferFree(token);
    
    TextEditor_SortLines(text_editor);

    TextEditor_SetVerticalScrollBarSize(text_editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

void TextEditor_Return(Text_Editor *text_editor){

    if(text_editor->text_selected && TextEditor_Backspace(text_editor) && text_editor->undo){
        text_editor->undo->chained = true;
    }

    wchar_t *rest;
    GapBufferGetContentAfterGap(text_editor->current_line->gap_buffer,wchar_t,rest);

    text_editor->current_line->gap_buffer->gap_end = text_editor->current_line->gap_buffer->buffer_end;

    bool chained = (text_editor->undo && text_editor->undo->action & ACTION_ADD_TEXT && text_editor->undo->chained) ? true : false;
        
    TextEditor_AddNewAction(text_editor,ACTION_DELETE_LINE | ACTION_DELETE_TEXT);
        
    text_editor->undo->chained = chained;
    text_editor->undo->used = true;

    text_editor->current_line = TextEditor_PushLine(text_editor->current_line);
        
    text_editor->cursor.pos.x = 0;
    text_editor->cursor.pos.y++;
        
    if(rest){
        GapBufferInsertValues(text_editor->current_line->gap_buffer,wchar_t,rest,wcslen(rest));
        free(rest);
    }
        
    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    text_editor->undo->start_modification = (Vector2i){0,text_editor->cursor.pos.y};
    text_editor->undo->end_modification = text_editor->cursor.pos;

    text_editor->undo->target_cursor = text_editor->cursor.pos;

    text_editor->cursor.update = true;

    TextEditor_SortLines(text_editor);

    TextEditor_SetVerticalScrollBarSize(text_editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

bool TextEditor_Backspace(Text_Editor *text_editor){

    if(text_editor->text_selected){

        int start_y,end_y;

        if(text_editor->start_selection.y < text_editor->end_selection.y){
            start_y = text_editor->start_selection.y;
            end_y = text_editor->end_selection.y;
        }
        else{
            start_y = text_editor->end_selection.y;
            end_y = text_editor->start_selection.y;
        }

        Line *line_selected = TextEditor_GetLine(text_editor->lines,start_y);

        if((text_editor->end_selection.y == text_editor->start_selection.y || !line_selected->next) && text_editor->current_line->start_selection < ContentSize(text_editor->current_line->gap_buffer,wchar_t)){

            RemoveSelectedTextInSingleLine(text_editor);
            return true;
        }
        else if(text_editor->end_selection.y != text_editor->start_selection.y){

            RemoveSelectedTextInMultLine(text_editor,line_selected,start_y,end_y);
            return true;
        }
    }
    else if(text_editor->cursor.pos.x > 0){

        RemoveValueInLine(text_editor);
        return true;
    }
    else if(text_editor->current_line->previous){

        RemoveLine(text_editor);
        return true;
    }

    return false;
}


void TextEditor_DraggingMouse(Text_Editor *text_editor,Mouse mouse,uint32_t current_time){

    text_editor->end_selection = TextEditor_GetPosInText(text_editor,mouse);

    if(text_editor->text_selected){

        Recti text_rect = {
            text_editor->rect.x,
            text_editor->rect.y,
            text_editor->rect.w-SCROLL_BAR_SIZE,
            text_editor->rect.h
        };
        
        if(!PointInRect(mouse.pos,text_rect)){
            
            if(current_time - text_editor->cursor.time_move_offset >= MOVE_OFFSET_TIME){
                TextEditor_SetPosInText(text_editor);

                bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

                TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

                if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

                TextEditor_SetHorizontalScrollBarSize(text_editor);
                TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);

                text_editor->cursor.time_move_offset = current_time;
            }
            else{
                return;
            }
        }
        else{
            TextEditor_SetPosInText(text_editor);
        }
    }

    if((text_editor->text_selected || PointInRect(mouse.pos,text_editor->rect)) && 
       (text_editor->start_selection.x != text_editor->end_selection.x || text_editor->start_selection.y != text_editor->end_selection.y)){
        
        int start_y,end_y;

        if(text_editor->start_selection.y < text_editor->end_selection.y){
            start_y = text_editor->start_selection.y;
            end_y = text_editor->end_selection.y;
        }
        else{
            start_y = text_editor->end_selection.y;
            end_y = text_editor->start_selection.y;
        }

        Line *buffer_line = text_editor->lines;

        while(buffer_line){

            if(buffer_line->index >= start_y && buffer_line->index <= end_y){
                        
                text_editor->text_selected = true;
                buffer_line->selected = true;
                uint32_t content_size = ContentSize(buffer_line->gap_buffer,wchar_t);
      
                if(text_editor->end_selection.y > text_editor->start_selection.y){

                    if(buffer_line->index == text_editor->start_selection.y){

                        if(text_editor->start_selection.x > content_size){
                            buffer_line->start_selection = content_size;
                        }
                        else{
                            buffer_line->start_selection = text_editor->start_selection.x;
                        }
                    }
                    else{
                        buffer_line->start_selection = 0;
                    }
                                
                    if(text_editor->start_selection.y == buffer_line->index || text_editor->end_selection.y > buffer_line->index){
                        buffer_line->end_selection = content_size;
                    }
                    else{
                        buffer_line->end_selection = (text_editor->end_selection.x > content_size) ? content_size : text_editor->end_selection.x;
                    }
                }

                else if(text_editor->end_selection.y < text_editor->start_selection.y){
                                
                    if(buffer_line->index == text_editor->start_selection.y){
                        buffer_line->start_selection = 0;
                        buffer_line->end_selection = (text_editor->start_selection.x > content_size) ? content_size : text_editor->start_selection.x;
                    }
                    else{
                        buffer_line->end_selection = content_size;
                        buffer_line->start_selection = (buffer_line->index > text_editor->end_selection.y) ? 0 : text_editor->end_selection.x;
                        if(buffer_line->start_selection > content_size) buffer_line->start_selection = content_size;
                    }
                }
                        
                else if(text_editor->end_selection.y == text_editor->end_selection.y){
                        
                    if(text_editor->start_selection.x >= content_size && text_editor->end_selection.x >= content_size){
                        buffer_line->selected = false;
                    }
                    else if(text_editor->end_selection.x > text_editor->start_selection.x){
                        buffer_line->start_selection = (text_editor->start_selection.x > content_size) ? content_size : text_editor->start_selection.x;
                        buffer_line->end_selection = (text_editor->end_selection.x > content_size) ? content_size : text_editor->end_selection.x;
                    }
                    else{
                        buffer_line->start_selection = (text_editor->end_selection.x > content_size) ? content_size : text_editor->end_selection.x;
                        buffer_line->end_selection = (text_editor->start_selection.x > content_size) ? content_size : text_editor->start_selection.x;
                    }
                }
            }
            else{
                buffer_line->selected = false;
            }

            buffer_line = buffer_line->next;
        }
    }
}

void TextEditor_Left(Text_Editor *text_editor){

    if(text_editor->cursor.pos.x > 0){
        
        text_editor->cursor.pos.x--;
        GapBufferMoveGapLeft(text_editor->current_line->gap_buffer,wchar_t,1);
        
        bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);
        
        TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
        
        if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);
        
        TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
    }
    else if(text_editor->current_line->previous){
        
        text_editor->current_line = text_editor->current_line->previous;
        text_editor->cursor.pos.y--;
        text_editor->cursor.pos.x = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);
        
        bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);
        
        TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
        
        if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);
        
        TextEditor_SetHorizontalScrollBarSize(text_editor);
        TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
    }
    text_editor->cursor.update = true;
}

void TextEditor_Right(Text_Editor *text_editor){

    if(text_editor->cursor.pos.x == ContentSize(text_editor->current_line->gap_buffer,wchar_t) && text_editor->current_line->next){

        text_editor->current_line = text_editor->current_line->next;
        text_editor->cursor.pos.y++;
        text_editor->cursor.pos.x = 0;
        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,0);
        
        bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);
        
        TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
        
        if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);
        
        TextEditor_SetHorizontalScrollBarSize(text_editor);
        TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
    }
    else if(text_editor->cursor.pos.x < ContentSize(text_editor->current_line->gap_buffer,wchar_t)){
        
        text_editor->cursor.pos.x++;
        GapBufferMoveGapRight(text_editor->current_line->gap_buffer,wchar_t,1);

        bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

        TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);
        
        if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);
        
        TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
    }
    text_editor->cursor.update = true;
}

void TextEditor_Up(Text_Editor *text_editor){
    text_editor->current_line = text_editor->current_line->previous;

    uint32_t size_text = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
    text_editor->cursor.pos.x = (size_text < text_editor->cursor.pos.x) ? size_text : text_editor->cursor.pos.x;
    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    text_editor->cursor.pos.y--;
    text_editor->cursor.update = true;
    
    bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}

void TextEditor_Down(Text_Editor *text_editor){
    text_editor->current_line = text_editor->current_line->next;

    uint32_t size_text = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
    text_editor->cursor.pos.x = (size_text < text_editor->cursor.pos.x) ? size_text : text_editor->cursor.pos.x;
    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,text_editor->cursor.pos.x);

    text_editor->cursor.pos.y++;
    text_editor->cursor.update = true;

    bool cursor_is_visible = TextEditor_CursorYIsVisible(text_editor);

    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    if(!cursor_is_visible) TextEditor_SetVisibleLines(text_editor);

    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);
}


//undo and redo

static void ActionAddLine(Text_Editor *text_editor,Action_Stack *action_stack){
    
    text_editor->current_line = TextEditor_GetLine(text_editor->lines,action_stack->origin_cursor.y);

    text_editor->cursor.pos = action_stack->origin_cursor;
    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->origin_cursor.x);

    wchar_t *rest;
    GapBufferGetContentAfterGap(text_editor->current_line->gap_buffer,wchar_t,rest);

    text_editor->current_line->gap_buffer->gap_end = text_editor->current_line->gap_buffer->buffer_end;

    text_editor->current_line = TextEditor_PushLine(text_editor->current_line);

    if(rest){
        GapBufferInsertValues(text_editor->current_line->gap_buffer,wchar_t,rest,wcslen(rest));
        free(rest);
    }

    TextEditor_SortLines(text_editor);
    
    if(action_stack->action == ACTION_ADD_LINE){
    
        text_editor->cursor.pos = action_stack->target_cursor;
        
        if(text_editor->cursor.pos.y != text_editor->current_line->index){
            text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->cursor.pos.y);
        }
        
        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->target_cursor.x);
        
        text_editor->cursor.update = true;
    }
}

static void ActionDeleteLine(Text_Editor *text_editor,Action_Stack *action_stack){

    if(action_stack->start_modification.y != action_stack->end_modification.y){

        Line *buffer_line = text_editor->current_line;

        while(buffer_line && buffer_line->index <= action_stack->end_modification.y){
                
            if(buffer_line->index == action_stack->start_modification.y){
                GapBufferRemoveInInterval(buffer_line->gap_buffer,wchar_t,action_stack->start_modification.x,ContentSize(buffer_line->gap_buffer,wchar_t));
            }
            else{
                if(buffer_line->index == action_stack->end_modification.y){
                    GapBufferRemoveInInterval(buffer_line->gap_buffer,wchar_t,0,action_stack->end_modification.x);
                }
                else{
                    GapBufferClear(buffer_line->gap_buffer);
                }
                buffer_line = TextEditor_RemoveLine(buffer_line);
            }

            buffer_line = buffer_line->next;
        }

        TextEditor_SortLines(text_editor);
    }
    else if(ContentSize(text_editor->current_line->gap_buffer,wchar_t) > 0){
        GapBufferRemoveInInterval(text_editor->current_line->gap_buffer,wchar_t,action_stack->start_modification.x,action_stack->end_modification.x);
    }

    text_editor->cursor.pos = action_stack->start_modification;

    if(text_editor->cursor.pos.y != text_editor->current_line->index){
        text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->cursor.pos.y);
    }

    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->start_modification.x);

    text_editor->cursor.update = true;
}

static void ActionAddText(Text_Editor *text_editor,Action_Stack *action_stack){
    
    if(action_stack->start_modification.y != action_stack->end_modification.y){

        text_editor->cursor.pos = action_stack->start_modification;
        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->start_modification.x);

        wchar_t *rest;
        GapBufferGetContentAfterGap(text_editor->current_line->gap_buffer,wchar_t,rest);

        text_editor->current_line->gap_buffer->gap_end = text_editor->current_line->gap_buffer->buffer_end;

        text_editor->text_selected = true;

        GapBuffer *token;
        CreateGapBuffer(token,wchar_t,GAP_EXPAND_SIZE);

        int index = text_editor->current_line->index;

        wchar_t *ptr = action_stack->gap_buffer->buffer;

        while(ptr < (wchar_t*)action_stack->gap_buffer->buffer_end){

            if(ptr == (wchar_t*)action_stack->gap_buffer->gap_start){
                ptr = action_stack->gap_buffer->gap_end;
                if(ptr >= (wchar_t*)action_stack->gap_buffer->buffer_end) break;
            }

            if(*ptr == L'\0' || *ptr == L'\n'){

                GapBufferInsertInto(text_editor->current_line->gap_buffer,token,wchar_t);

                if(action_stack->text_selected){
                    
                    uint32_t content_size = ContentSize(text_editor->current_line->gap_buffer,wchar_t);
                    
                    if(index == action_stack->start_modification.y){
                        text_editor->current_line->start_selection = action_stack->start_modification.x;
                        text_editor->current_line->end_selection = content_size;
                        text_editor->current_line->selected = true;
                    }
                    else if(index == action_stack->end_modification.y){
                        text_editor->current_line->start_selection = 0;
                        text_editor->current_line->end_selection = action_stack->end_modification.x;
                        text_editor->current_line->selected = true;
                    }
                    else{
                        text_editor->current_line->start_selection = 0;
                        text_editor->current_line->end_selection = content_size;
                        text_editor->current_line->selected = true;
                    }
                }

                if(*ptr == '\n'){
                    text_editor->current_line = TextEditor_PushLine(text_editor->current_line);
                    text_editor->cursor.pos.x = 0;
                    text_editor->cursor.pos.y++;
                    ++index;
                }

                GapBufferClear(token);
            }
            else if(*ptr == L' ' || !iswspace(*ptr)){

                GapBufferInsertValue(token,wchar_t,*ptr);
            }
            ptr++;
        }

        if(rest){
            GapBufferInsertValues(text_editor->current_line->gap_buffer,wchar_t,rest,wcslen(rest));
            free(rest);
        }

        TextEditor_SortLines(text_editor);
    }
    else if(ContentSize(action_stack->gap_buffer,wchar_t) > 0){

        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->start_modification.x);
        GapBufferInsertInto(text_editor->current_line->gap_buffer,action_stack->gap_buffer,wchar_t);
        
        if(action_stack->text_selected){
            text_editor->current_line->start_selection = action_stack->start_modification.x;
            text_editor->current_line->end_selection = action_stack->end_modification.x;
            text_editor->current_line->selected = true;
            text_editor->text_selected = true;
        }
    }

    text_editor->cursor.pos = action_stack->target_cursor;

    if(text_editor->cursor.pos.y != text_editor->current_line->index){
        text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->cursor.pos.y);
    }

    GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->target_cursor.x);

    text_editor->cursor.update = true;
}

void TextEditor_UndoAndRedo(Text_Editor *text_editor,uint8_t type,bool callback){

    if((type == UNDO && !text_editor->undo) || (type == REDO && !text_editor->redo)) return;

    Action_Stack *action_stack = (type == UNDO) ? text_editor->undo : text_editor->redo;

    bool is_chained = false;

    if(!callback && action_stack->chained && action_stack->next && action_stack->next->chained){
        is_chained = true;
    }

    if(action_stack->action & ACTION_ADD_LINE){

        ActionAddLine(text_editor,action_stack);
    }
    else if(action_stack->start_modification.y != text_editor->current_line->index){
        text_editor->current_line = TextEditor_GetLine(text_editor->lines,action_stack->start_modification.y);
    }

    if(action_stack->action & ACTION_ADD_TEXT){

        action_stack->action &= ~ACTION_ADD_TEXT;
        action_stack->action |= ACTION_DELETE_TEXT;

        ActionAddText(text_editor,action_stack);
    }
    else if(action_stack->action & ACTION_DELETE_TEXT){

        action_stack->action &= ~ACTION_DELETE_TEXT;
        action_stack->action |= ACTION_ADD_TEXT;

        ActionDeleteLine(text_editor,action_stack);
    }

    if(action_stack->action & ACTION_ADD_LINE){
        
        action_stack->action &= ~ACTION_ADD_LINE;
        action_stack->action |= ACTION_DELETE_LINE;
    }
    else if(action_stack->action & ACTION_DELETE_LINE){

        action_stack->action &= ~ACTION_DELETE_LINE;
        action_stack->action |= ACTION_ADD_LINE;
        
        text_editor->current_line = TextEditor_RemoveLine(text_editor->current_line);

        TextEditor_SortLines(text_editor);

        text_editor->cursor.pos = action_stack->origin_cursor;

        if(text_editor->cursor.pos.y != text_editor->current_line->index){
            text_editor->current_line = TextEditor_GetLine(text_editor->lines,text_editor->cursor.pos.y);
        }

        GapBufferMoveGap(text_editor->current_line->gap_buffer,wchar_t,action_stack->origin_cursor.x);

        text_editor->cursor.update = true;
    }

    TextEditor_SetVerticalScrollBarSize(text_editor);
    TextEditor_AjustVerticalScrollBarOffsetByCursor(text_editor);

    TextEditor_SetVisibleLines(text_editor);
    
    TextEditor_SetHorizontalScrollBarSize(text_editor);
    TextEditor_AjusHorizontalScrollBarOffsetByCursor(text_editor);

    if(type == UNDO){
        action_stack->used = true;

        Action_Stack *buffer = text_editor->undo->next;
        text_editor->undo->next = text_editor->redo;
        text_editor->redo = text_editor->undo;
        text_editor->undo = buffer;
    }
    else if(type == REDO){

        Action_Stack *buffer = text_editor->redo->next;
        text_editor->redo->next = text_editor->undo;
        text_editor->undo = text_editor->redo;
        text_editor->redo = buffer;
    }

    if(is_chained) TextEditor_UndoAndRedo(text_editor,type,true);
}

