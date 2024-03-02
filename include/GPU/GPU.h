#ifndef GPU_H_
#define GPU_H_

#define DLLExport __declspec(dllexport)

#include <SDL2/SDL.h>
#include "GapBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

typedef signed char      int8_t;
typedef signed short     int16_t;
typedef signed int       int32_t;
typedef signed long long int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef struct _FontAtlas FontAtlas;
typedef struct _Font      Font;
typedef struct _Texture   Texture;
typedef struct _Renderer  Renderer;


typedef enum _FlipFlag{
    FLIP_NONE       = 0x0000,
    FLIP_HORIZONTAL = 0x0001,
    FLIP_VERTICAL   = 0x0002,
}FlipFlag;

typedef enum _BlendModePreset{
    BLEND_NORMAL,
    BLEND_PREMULTIPLIED_ALPHA,
    BLEND_MULTIPLY,
    BLEND_ADD,
    BLEND_SUBTRACT,
    BLEND_MOD_ALPHA,
    BLEND_SET_ALPHA,
    BLEND_SET,
    BLEND_NORMAL_KEEP_ALPHA,
    BLEND_NORMAL_ADD_ALPHA,
    BLEND_NORMAL_FACTOR_ALPHA,
}BlendModePreset;

typedef enum _BlendFunction{
    FUNC_ZERO                = 0x0000,
    FUNC_ONE                 = 0x0001,
    FUNC_SRC_COLOR           = 0x0300,
    FUNC_ONE_MINUS_SRC_COLOR = 0x0301,
    FUNC_DST_COLOR           = 0x0306,
    FUNC_ONE_MINUS_DST_COLOR = 0x0307,
    FUNC_SRC_ALPHA           = 0x0302,
    FUNC_ONE_MINUS_SRC_ALPHA = 0x0303,
    FUNC_DST_ALPHA           = 0x0304,
    FUNC_ONE_MINUS_DST_ALPHA = 0x0305,
}BlendFunction;

typedef enum _BlendEquation{
    EQ_ADD              = 0x8006,
    EQ_SUBTRACT         = 0x800A,
    EQ_REVERSE_SUBTRACT = 0x800B,
    EQ_MIN              = 0x8007,
    EQ_MAX              = 0x8008,
}BlendEquation;

typedef enum _Filter{
    FILTER_NEAREST                = 0x2600,
    FILTER_LINEAR                 = 0x2601,
    FILTER_NEAREST_MIPMAP_NEAREST = 0x2700,
    FILTER_LINEAR_MIPMAP_NEAREST  = 0x2701,
    FILTER_NEAREST_MIPMAP_LINEAR  = 0x2702,
    FILTER_LINEAR_MIPMAP_LINEAR   = 0x2703,
}Filter;

typedef enum _Wrap{
    WRAP_REPEAT          = 0x2901,
    WRAP_MIRRORED_REPEAT = 0x8370,
    WRAP_CLAMP_TO_EDGE   = 0x812F,
}Wrap;

typedef enum PixelFormat{
    PIXEL_FORMAT_RED             = 0x1903,
    PIXEL_FORMAT_GREEN           = 0x1904,
    PIXEL_FORMAT_BLUE            = 0x1905,
    PIXEL_FORMAT_ALPHA           = 0x1906,
    PIXEL_FORMAT_LUMINANCE       = 0x1909,
    PIXEL_FORMAT_LUMINANCE_ALPHA = 0x190A,
    PIXEL_FORMAT_RG              = 0x8227,
    PIXEL_FORMAT_RGB             = 0x1907,
    PIXEL_FORMAT_RGBA            = 0x1908,
    PIXEL_FORMAT_BGB             = 0x80E0,
    PIXEL_FORMAT_BGBA            = 0x80E1,
}PixelFormat;

typedef struct _Color{
    uint8_t r,g,b,a;
}Color;

typedef struct _Rectf{
    float x,y,w,h;
}Rectf;

typedef struct _Recti{
    int x,y,w,h;
}Recti;

typedef struct _Vector2f{
    float x,y;
}Vector2f;

typedef struct _Vector2i{
    int x,y;
}Vector2i;

typedef struct _Vertex{
    Vector2f position;
    Color color;
    Vector2f texcoord;
}Vertex;

typedef struct _BlendMode{
    uint32_t srcRGB;
    uint32_t dstRGB;

    uint32_t srcAlpha;
    uint32_t dstAlpha;

    uint32_t modeRGB;
    uint32_t modeAlpha;
}BlendMode;


DLLExport BlendMode GetBlendModeByPreset(BlendModePreset preset);

DLLExport void SetBlendFunction(Renderer *renderer,Texture *target,BlendFunction srcRGB,BlendFunction dstRGB,BlendFunction srcAlpha,BlendFunction dstAlpha);

DLLExport void SetBlendEquation(Renderer *renderer,Texture *target,BlendEquation modeRGB,BlendEquation modeAlpha);

DLLExport void SetBlendModeByPreset(Renderer *renderer,Texture *target,BlendModePreset preset);

DLLExport void SetBlendMode(Renderer *renderer,Texture *target,BlendMode blend_mode);

DLLExport void SetBlending(Renderer *renderer,Texture *target,bool blending);


DLLExport void SetClip(Renderer *renderer,Texture *target,Recti *rect);

DLLExport void UnSetClip(Renderer *renderer,Texture *target);


DLLExport Texture* CreateTexture(Renderer *renderer,int width,int height,PixelFormat format,bool mipmap,bool framebuffer);

DLLExport Texture* CreateTextureFromTexture(Renderer *renderer,Texture *texture,bool mipmap,bool framebuffer);

DLLExport Texture* LoadTexture(Renderer *renderer,const uint16_t *fileName,bool mipmap,bool framebuffer);

DLLExport void TextureFree(Texture *texture);

DLLExport void SetTextureColor(Texture *texture,Color color);

DLLExport void SetTextureFilter(Texture *texture,Filter min_filter,Filter mag_filter);

DLLExport void SetTextureWrap(Texture *texture,Wrap wrap_x,Wrap wrap_y);

DLLExport void GenerateFrameBuffer(Renderer *renderer,Texture *texture);

DLLExport void GenearateMipmap(Renderer *renderer,Texture *texture);

DLLExport void LockTexture(Texture *texture,Recti *rect,void **pixels,int *pitch);

DLLExport void UnlockTexture(Texture *texture);

DLLExport void TextureSize(Texture *texture,int *width,int *height);

DLLExport void TextureFormat(Texture *texture,PixelFormat *format);

DLLExport bool TextureHasFrameBuffer(Texture *texture);

DLLExport bool TextureHasMipmap(Texture *texture);

DLLExport void TextureWrap(Texture *texture,Wrap *wrap_x,Wrap *wrap_y);

DLLExport void TextureFilter(Texture *texture,Filter *min_filter,Filter *mag_filter);



DLLExport Renderer* CreateRenderer(SDL_Window *window,bool vsync);

DLLExport void RendererFree(Renderer *renderer);

DLLExport void RendererSetTarget(Renderer *renderer,Texture *target);

DLLExport void RendererSetVsync(Renderer *renderer,bool value);

DLLExport void RendererSetFillIncompletFormat(Renderer *renderer,bool fill_incomplete_format);

DLLExport void RendererSetViewport(Renderer *renderer,Texture *target,Recti *rect);


DLLExport void FlushBlitBuffer(Renderer *renderer);

DLLExport void ClearColor(Renderer *renderer,Color color);

DLLExport void ClearRGBA(Renderer *renderer,uint8_t r,uint8_t g,uint8_t b,uint8_t a);

DLLExport void Flip(Renderer *renderer);



DLLExport void DrawFilledRectanglef(Renderer *renderer,Rectf *rect,Color color);

DLLExport void DrawFilledRectanglei(Renderer *renderer,Recti *rect,Color color);


DLLExport void DrawFilledRectanglesf(Renderer *renderer,Rectf *rects,uint32_t count,Color color);

DLLExport void DrawFilledRectanglesi(Renderer *renderer,Recti *rects,uint32_t count,Color color);


DLLExport void DrawRectanglef(Renderer *renderer,Rectf *rect,Color color);

DLLExport void DrawRectanglei(Renderer *renderer,Recti *rect,Color color);


DLLExport void DrawRectanglesf(Renderer *renderer,Rectf *rects,uint32_t count,Color color);

DLLExport void DrawRectanglesi(Renderer *renderer,Recti *rects,uint32_t count,Color color);


DLLExport void DrawFilledTrianglef(Renderer *renderer,Vector2f vertex1,Vector2f vertex2,Vector2f vertex3,Color color);

DLLExport void DrawFilledTrianglei(Renderer *renderer,Vector2i vertex1,Vector2i vertex2,Vector2i vertex3,Color color);


DLLExport void DrawTrianglef(Renderer *renderer,Vector2f vertex1,Vector2f vertex2,Vector2f vertex3,Color color);

DLLExport void DrawTrianglei(Renderer *renderer,Vector2i vertex1,Vector2i vertex2,Vector2i vertex3,Color color);


DLLExport void DrawFilledCirclef(Renderer *renderer,Vector2f center,float radius,Color color);

DLLExport void DrawFilledCirclei(Renderer *renderer,Vector2i center,int radius,Color color);


DLLExport void DrawCirclef(Renderer *renderer,Vector2f center,float radius,Color color);

DLLExport void DrawCirclei(Renderer *renderer,Vector2i center,int radius,Color color);


DLLExport void DrawFilledArcf(Renderer *renderer,Vector2f center,float radius,float startAngle,float endAngle,Color color);

DLLExport void DrawFilledArci(Renderer *renderer,Vector2i center,int radius,int startAngle,int endAngle,Color color);


DLLExport void DrawArcf(Renderer *renderer,Vector2f center,float radius,float startAngle,float endAngle,Color color);

DLLExport void DrawArci(Renderer *renderer,Vector2i center,int radius,int startAngle,int endAngle,Color color);


DLLExport void DrawPixelf(Renderer *renderer,Vector2f point,Color color);

DLLExport void DrawPixeli(Renderer *renderer,Vector2i point,Color color);


DLLExport void DrawLinef(Renderer *renderer,Vector2f point1,Vector2f point2,Color color);

DLLExport void DrawLinei(Renderer *renderer,Vector2i point1,Vector2i point2,Color color);


DLLExport void DrawLinesf(Renderer *renderer,Vector2f *points,uint32_t points_count,Color color);

DLLExport void DrawLinesi(Renderer *renderer,Vector2i *points,uint32_t points_count,Color color);


DLLExport void DrawPolygonf(Renderer *renderer,Vector2f *vertices,uint32_t vertices_count,Color color);

DLLExport void DrawPolygoni(Renderer *renderer,Vector2i *vertices,uint32_t vertices_count,Color color);


DLLExport void DrawFilledPolygonf(Renderer *renderer,Vector2f *vertices,uint32_t vertices_count,Color color);

DLLExport void DrawFilledPolygoni(Renderer *renderer,Vector2i *vertices,uint32_t vertices_count,Color color);


DLLExport void DrawTextUint16f(Renderer *renderer,FontAtlas *atlas,uint16_t *text,Vector2f pos,Color color);

DLLExport void DrawTextUint16i(Renderer *renderer,FontAtlas *atlas,uint16_t *text,Vector2i pos,Color color);


DLLExport void DrawTextByGapBufferUint16f(Renderer *renderer,FontAtlas *atlas,GapBuffer *gap_buffer,Vector2f pos,Color color);

DLLExport void DrawTextByGapBufferUint16i(Renderer *renderer,FontAtlas *atlas,GapBuffer *gap_buffer,Vector2i pos,Color color);


DLLExport void BlitColorf(Renderer *renderer,Texture *texture,Rectf *src_rect,Rectf *dst_rect,Color color);

DLLExport void BlitColori(Renderer *renderer,Texture *texture,Recti *src_rect,Recti *dst_rect,Color color);


DLLExport void Blitf(Renderer *renderer,Texture *texture,Rectf *src_rect,Rectf *dst_rect);

DLLExport void Bliti(Renderer *renderer,Texture *texture,Recti *src_rect,Recti *dst_rect);


DLLExport void BlitExf(Renderer *renderer,Texture *texture,Rectf *src_rect,Rectf *dst_rect,float angle,Vector2f *center,FlipFlag flags);

DLLExport void BlitExi(Renderer *renderer,Texture *texture,Recti *src_rect,Recti *dst_rect,float angle,Vector2i *center,FlipFlag flags);


DLLExport void Geometry(Renderer *renderer,Texture *texture,Vertex *vertices,uint32_t vertices_count,uint32_t *indices,uint32_t indices_count);



DLLExport bool GlyphIsProvided(Font *font,int char_code);

DLLExport void GlyphMetrics(Font *font,int char_code,int *advance,int *minx,int *maxx,int *miny,int *maxy);

DLLExport void SetFontKerning(Font *font,bool allowed);

DLLExport void SetFontHeight(Font *font,int height);

DLLExport Font* OpenFont(const uint16_t *fileName,int height);

DLLExport void CloseFont(Font *font);

DLLExport void SizeText(Font *font,const uint16_t *text,int *width,int *height);

DLLExport Texture* RenderGlyph(Renderer *renderer,Font *font,uint16_t char_code);

DLLExport Texture* RenderText(Renderer *renderer,Font *font,const uint16_t *text);

DLLExport FontAtlas* CreateFontAtlas(Renderer *renderer,Font *font,int height,int first_char,int num_chars);

DLLExport void SizeTextByFontAtlas(FontAtlas *atlas,uint16_t *text,int *width,int *height);

DLLExport void FontAtlasFree(FontAtlas *atlas);

#endif
