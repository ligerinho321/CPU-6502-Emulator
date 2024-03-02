#ifndef GAP_BUFFER_H_
#define GAP_BUFFER_H_

typedef struct _GapBuffer{
    void *buffer;
    void *buffer_end;
    void *gap_start;
    void *gap_end;
    unsigned int expand_size;
}GapBuffer;

#define PreviousValue (-1)
#define PresentValue  (-2)
#define NextValue     (-3)

#define GapSize(gap_buffer,type) ((type*)(gap_buffer)->gap_end - (type*)(gap_buffer)->gap_start)

#define BufferSize(gap_buffer,type) ((type*)(gap_buffer)->buffer_end - (type*)(gap_buffer)->buffer)

#define ContentSize(gap_buffer,type) (BufferSize(gap_buffer,type) - GapSize(gap_buffer,type))

#define GapStartOffset(gap_buffer,type) ((type*)(gap_buffer)->gap_start - (type*)(gap_buffer)->buffer)
#define GapEndOffset(gap_buffer,type) ((type*)(gap_buffer)->gap_end - (type*)(gap_buffer)->buffer)

#define CreateGapBuffer(gap_buffer,type,expand)                       \
do{                                                                   \
    (gap_buffer) = malloc(sizeof(GapBuffer));                         \
    (gap_buffer)->buffer = malloc(sizeof(type) * (expand));           \
    (gap_buffer)->buffer_end = (type*)(gap_buffer)->buffer + (expand);\
    (gap_buffer)->gap_start = (gap_buffer)->buffer;                   \
    (gap_buffer)->gap_end = (gap_buffer)->buffer_end;                 \
    (gap_buffer)->expand_size = expand;                               \
}while(0)

#define ExpandGapBuffer(gap_buffer,type,size)                                                                       \
do{                                                                                                                 \
    unsigned int old_size = BufferSize(gap_buffer,type);                                                            \
    unsigned int new_size = old_size + (size);                                                                      \
    int gap_start_offset = GapStartOffset(gap_buffer,type);                                                         \
    int gap_end_offset = GapEndOffset(gap_buffer,type);                                                             \
    (gap_buffer)->buffer = realloc((gap_buffer)->buffer,new_size * sizeof(type));                                   \
    (gap_buffer)->gap_start = (type*)(gap_buffer)->buffer + gap_start_offset;                                       \
    (gap_buffer)->gap_end = (type*)(gap_buffer)->buffer + gap_end_offset;                                           \
    (gap_buffer)->buffer_end = (type*)(gap_buffer)->buffer + new_size;                                              \
    memmove((type*)(gap_buffer)->gap_end + (size),(gap_buffer)->gap_end,(old_size - gap_end_offset) * sizeof(type));\
    (gap_buffer)->gap_end = (type*)(gap_buffer)->gap_end + (size);                                                  \
}while(0)

#define GapBufferInsertValue(gap_buffer,type,value)                                              \
do{                                                                                              \
    if((type*)(gap_buffer)->gap_start >= (type*)(gap_buffer)->gap_end){                          \
        ExpandGapBuffer(gap_buffer,type,(gap_buffer)->expand_size);                              \
    }                                                                                            \
    ((type*)(gap_buffer)->gap_start)[0] = value;                                                 \
    (gap_buffer)->gap_start = (type*)(gap_buffer)->gap_start + 1;                                \
}while(0)

#define GapBufferInsertValues(gap_buffer,type,values,length)                                             \
do{                                                                                                      \
    if((length) > GapSize(gap_buffer,type)){                                                             \
        ExpandGapBuffer(gap_buffer,type,(length) - GapSize(gap_buffer,type) + (gap_buffer)->expand_size);\
    }                                                                                                    \
    memmove((gap_buffer)->gap_start,values,(length) * sizeof(type));                                     \
    (gap_buffer)->gap_start = (type*)(gap_buffer)->gap_start + (length);                                 \
}while(0)

#define GapBufferInsertInto(dst,src,type)                     \
do{                                                           \
    type *ptr = (src)->buffer;                                \
    while(ptr < (type*)(src)->buffer_end){                    \
        if(ptr == (type*)(src)->gap_start){                   \
            ptr = (src)->gap_end;                             \
            if(ptr >= (type*)(src)->buffer_end) break;        \
        }                                                     \
        if((type*)(dst)->gap_start >= (type*)(dst)->gap_end){ \
            ExpandGapBuffer(dst,type,(dst)->expand_size);     \
        }                                                     \
        ((type*)(dst)->gap_start)[0] = *(ptr++);              \
        (dst)->gap_start = (type*)(dst)->gap_start + 1;       \
    }                                                         \
}while(0)

#define GapBufferRemove(gap_buffer,type,length)                                                                                                        \
do{                                                                                                                                                    \
    (gap_buffer)->gap_start = (type*)(gap_buffer)->gap_start - ((length < GapStartOffset(gap_buffer,type)) ? length : GapStartOffset(gap_buffer,type));\
}while(0)

#define GapBufferRemoveInInterval(gap_buffer,type,start,end) \
do{                                                          \
    int content_size = ContentSize(gap_buffer,type);         \
    int s = start;                                           \
    int e = end;                                             \
                                                             \
    if(content_size <= 0) break;                             \
                                                             \
    if(s > content_size) s = content_size;                   \
    else if(s < 0)       s = 0;                              \
                                                             \
    if(e > content_size) e = content_size;                   \
    else if(e < 0)       e = 0;                              \
                                                             \
    if(s > e){                                               \
        int t = s;                                           \
        s = e;                                               \
        e = t;                                               \
    }                                                        \
                                                             \
    GapBufferMoveGap(gap_buffer,type,e);                     \
    GapBufferRemove(gap_buffer,type,e - s);                  \
}while(0)

#define GapBufferMoveGapLeft(gap_buffer,type,distanci)                                   \
do{                                                                                      \
    if((distanci) > 0 && (distanci) <= GapStartOffset(gap_buffer,type)){                 \
        (gap_buffer)->gap_start = (type*)(gap_buffer)->gap_start - (distanci);           \
        (gap_buffer)->gap_end = (type*)(gap_buffer)->gap_end - (distanci);               \
        memmove((gap_buffer)->gap_end,(gap_buffer)->gap_start,(distanci) * sizeof(type));\
    }                                                                                    \
}while(0)

#define GapBufferMoveGapRight(gap_buffer,type,distanci)                                                \
do{                                                                                                    \
    if((distanci) > 0 && (distanci) <= (type*)(gap_buffer)->buffer_end - (type*)(gap_buffer)->gap_end){\
        memmove((gap_buffer)->gap_start,(gap_buffer)->gap_end,(distanci) * sizeof(type));              \
        (gap_buffer)->gap_start = (type*)(gap_buffer)->gap_start + (distanci);                         \
        (gap_buffer)->gap_end = (type*)(gap_buffer)->gap_end + (distanci);                             \
    }                                                                                                  \
}while(0)

#define GapBufferMoveGap(gap_buffer,type,offset)                        \
do{                                                                     \
    int gap_start_offset = GapStartOffset(gap_buffer,type);             \
    int gap_end_offset = GapEndOffset(gap_buffer,type);                 \
    int content_size = ContentSize(gap_buffer,type);                    \
    int o = offset;                                                     \
                                                                        \
    if(o == gap_start_offset || content_size <= 0) break;               \
                                                                        \
    if(o > content_size) o = content_size;                              \
    else if(o < 0)       o = 0;                                         \
                                                                        \
    if(o < gap_start_offset){                                           \
        GapBufferMoveGapLeft(gap_buffer,type,gap_start_offset - o);     \
    }                                                                   \
    else{                                                               \
        o += GapSize(gap_buffer,type);                                  \
        GapBufferMoveGapRight(gap_buffer,type,o - gap_end_offset);      \
    }                                                                   \
}while(0)

#define GapBufferGetContent(gap_buffer,type,result)                 \
do{                                                                 \
    result = NULL;                                                  \
    int content_size = ContentSize(gap_buffer,type);                \
                                                                    \
    if(content_size <= 0) break;                                    \
                                                                    \
    result = malloc(sizeof(type) * (content_size + 1));             \
                                                                    \
    type *ptr_content = result;                                     \
    type *ptr_buffer = (gap_buffer)->buffer;                        \
                                                                    \
    while(ptr_buffer < (type*)(gap_buffer)->buffer_end){            \
        if(ptr_buffer == (type*)(gap_buffer)->gap_start){           \
            ptr_buffer = (gap_buffer)->gap_end;                     \
            if(ptr_buffer >= (type*)(gap_buffer)->buffer_end) break;\
        }                                                           \
        *(ptr_content++) = *(ptr_buffer++);                         \
    }                                                               \
    *ptr_content = 0;                                               \
}while(0)

#define GapBufferGetContentAfterGap(gap_buffer,type,result)                                    \
do{                                                                                            \
    result = NULL;                                                                             \
    int content_size = (type*)(gap_buffer)->buffer_end - (type*)(gap_buffer)->gap_end;         \
    if(content_size <= 0) break;                                                               \
    result = malloc(sizeof(type) * (content_size + 1));                                        \
    memmove(result,(gap_buffer)->gap_end,sizeof(type) * content_size);                         \
    (result)[content_size] = '\0';                                                             \
}while(0)

#define GapBufferGetSubContent(gap_buffer,type,start,end,result)    \
do{                                                                 \
    result = NULL;                                                  \
    int content_size = ContentSize(gap_buffer,type);                \
    int gap_start_offset = GapStartOffset(gap_buffer,type);         \
    int gap_size = GapSize(gap_buffer,type);                        \
    int s = start;                                                  \
    int e = end;                                                    \
                                                                    \
    if(content_size <= 0) break;                                    \
                                                                    \
    if(s > content_size)           s = content_size;                \
    else if(s >= gap_start_offset) s += gap_size;                   \
                                                                    \
    if(e > content_size)           e = content_size;                \
    else if(e >= gap_start_offset) e += gap_size;                   \
                                                                    \
    if(s > e){                                                      \
        int t = s;                                                  \
        s = e;                                                      \
        e = t;                                                      \
    }                                                               \
                                                                    \
    int sub_content_size = e - s;                                   \
                                                                    \
    if(sub_content_size <= 0) break;                                \
                                                                    \
    result = malloc(sizeof(type) * (sub_content_size + 1));         \
                                                                    \
    type *ptr_content = result;                                     \
    type *ptr_start = (type*)(gap_buffer)->buffer + s;              \
    type *ptr_end = (type*)(gap_buffer)->buffer + e;                \
                                                                    \
    while(ptr_start < ptr_end){                                     \
        if(ptr_start == (type*)(gap_buffer)->gap_start){            \
            ptr_start = (type*)(gap_buffer)->gap_end;               \
            if(ptr_start >= ptr_end) break;                         \
        }                                                           \
        *(ptr_content++) = *(ptr_start++);                          \
    }                                                               \
    *ptr_content = 0;                                               \
}while(0)

#define GapBufferGetValue(gap_buffer,type,index,value)                  \
do{                                                                     \
    value = 0;                                                          \
    int i = index;                                                      \
    int content_size = ContentSize(gap_buffer,type);                    \
                                                                        \
    if(content_size <= 0 || i > content_size) break;                    \
                                                                        \
    int buffer_size = BufferSize(gap_buffer,type);                      \
    int gap_start_offset = GapStartOffset(gap_buffer,type);             \
    int gap_end_offset = GapEndOffset(gap_buffer,type);                 \
                                                                        \
    if(i == PreviousValue && gap_start_offset > 1){                     \
        value = ((type*)(gap_buffer)->gap_start)[-2];                   \
    }                                                                   \
    else if(i == PresentValue && gap_start_offset > 0){                 \
        value = ((type*)(gap_buffer)->gap_start)[-1];                   \
    }                                                                   \
    else if(i == NextValue && gap_end_offset < buffer_size){            \
        value = ((type*)(gap_buffer)->gap_end)[0];                      \
    }                                                                   \
    else if(i >= 0){                                                    \
        if(i >= gap_start_offset) i += GapSize(gap_buffer,type);        \
        value = ((type*)(gap_buffer)->buffer)[i];                       \
    }                                                                   \
}while(0)

#define GapBufferClear(gap_buffer)                   \
do{                                                  \
    (gap_buffer)->gap_start = (gap_buffer)->buffer;  \
    (gap_buffer)->gap_end = (gap_buffer)->buffer_end;\
}while(0)

#define GapBufferFree(gap_buffer)  \
do{                                \
    if(gap_buffer != NULL){        \
        free((gap_buffer)->buffer);\
        free(gap_buffer);          \
    }                              \
}while(0)

#endif