#include <scroll_bar.h>

float GetScrollBarHeight(Scroll_Bar scroll_bar){
    float height = scroll_bar.bar_rect.h / (scroll_bar.length / (float)scroll_bar.bar_rect.h);
    return ((height < SCROLL_BAR_MIN_SIZE) ? SCROLL_BAR_MIN_SIZE : height);
}

float GetScrollBarWidth(Scroll_Bar scroll_bar){
    float width = scroll_bar.bar_rect.w / (scroll_bar.length / (float)scroll_bar.bar_rect.w);
    return ((width < SCROLL_BAR_MIN_SIZE) ? SCROLL_BAR_MIN_SIZE : width);
}


void SetVerticalOffsetScrollBar(Scroll_Bar *scroll_bar){
    if(!scroll_bar->valid) return;
    float length_diff = scroll_bar->length - scroll_bar->bar_rect.h;
    float scroll_length = scroll_bar->bar_rect.h - scroll_bar->scroll_rect.h;
    scroll_bar->offset = round(length_diff / scroll_length * (scroll_bar->scroll_rect.y - scroll_bar->bar_rect.y) * -1.0f);
}

void SetHorizontalOffsetScrollBar(Scroll_Bar *scroll_bar){
    if(!scroll_bar->valid) return;
    float length_diff = scroll_bar->length - scroll_bar->bar_rect.w;
    float scroll_length = scroll_bar->bar_rect.w - scroll_bar->scroll_rect.w;
    scroll_bar->offset = round(length_diff / scroll_length * (scroll_bar->scroll_rect.x - scroll_bar->bar_rect.x) * -1.0f);
}


void AjustVerticalScrollBar(Scroll_Bar *scroll_bar){
    if(scroll_bar->scroll_rect.y < scroll_bar->bar_rect.y){
        scroll_bar->scroll_rect.y = scroll_bar->bar_rect.y;
    }
    else if(scroll_bar->scroll_rect.y+scroll_bar->scroll_rect.h > scroll_bar->bar_rect.y+scroll_bar->bar_rect.h){
        scroll_bar->scroll_rect.y = (scroll_bar->bar_rect.y+scroll_bar->bar_rect.h)-scroll_bar->scroll_rect.h;
    }
}

void AjustHorizontalScrollBar(Scroll_Bar *scroll_bar){
    if(scroll_bar->scroll_rect.x < scroll_bar->bar_rect.x){
        scroll_bar->scroll_rect.x = scroll_bar->bar_rect.x;
    }
    else if(scroll_bar->scroll_rect.x+scroll_bar->scroll_rect.w > scroll_bar->bar_rect.x+scroll_bar->bar_rect.w){
        scroll_bar->scroll_rect.x = (scroll_bar->bar_rect.x+scroll_bar->bar_rect.w)-scroll_bar->scroll_rect.w;
    }
}


void SetYScrollBar(Scroll_Bar *scroll_bar,float offset){
    if(!scroll_bar->valid) return;
    float length_diff = scroll_bar->length - scroll_bar->bar_rect.h;
    float scroll_length = scroll_bar->bar_rect.h - scroll_bar->scroll_rect.h;
    scroll_bar->scroll_rect.y = scroll_bar->bar_rect.y + (-offset / length_diff * scroll_length);
    AjustVerticalScrollBar(scroll_bar);
    SetVerticalOffsetScrollBar(scroll_bar);
}

void SetXScrollBar(Scroll_Bar *scroll_bar,float offset){
    if(!scroll_bar->valid) return;
    float length_diff = scroll_bar->length - scroll_bar->bar_rect.w;
    float scroll_length = scroll_bar->bar_rect.w - scroll_bar->scroll_rect.w;
    scroll_bar->scroll_rect.x = scroll_bar->bar_rect.x + (-offset / length_diff * scroll_length);
    AjustHorizontalScrollBar(scroll_bar);
    SetHorizontalOffsetScrollBar(scroll_bar);
}


void HandleVerticalScrollBar(Scroll_Bar *scroll_bar,Mouse mouse){
    if(!scroll_bar->valid) return;
    scroll_bar->scroll_rect.y = mouse.pos.y - scroll_bar->mouse_diff;
    AjustVerticalScrollBar(scroll_bar);
    SetVerticalOffsetScrollBar(scroll_bar);
}

void HandleHorizontalScrollBar(Scroll_Bar *scroll_bar,Mouse mouse){
    if(!scroll_bar->valid) return;
    scroll_bar->scroll_rect.x = mouse.pos.x - scroll_bar->mouse_diff;
    AjustHorizontalScrollBar(scroll_bar);
    SetHorizontalOffsetScrollBar(scroll_bar);
}


void DrawScrollBar(Scroll_Bar *scroll_bar,Renderer *renderer){

    #ifdef DEBUG
    DrawRectanglei(renderer,&scroll_bar->bar_rect,DEBUG_COLOR);
    #endif
    
    if(!scroll_bar->valid) return;

    if(scroll_bar->on){
        DrawFilledRectanglef(renderer,&scroll_bar->scroll_rect,SCROLLBAR3_COLOR);
    }
    else if(scroll_bar->mouse_over){
        DrawFilledRectanglef(renderer,&scroll_bar->scroll_rect,SCROLLBAR2_COLOR);
        scroll_bar->mouse_over = false;
    }
    else{
        DrawFilledRectanglef(renderer,&scroll_bar->scroll_rect,SCROLLBAR1_COLOR);
    }
}