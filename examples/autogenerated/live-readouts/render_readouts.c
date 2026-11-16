/* Real surfaceless EGL/GLES2 test. This is host rendering, not phone evidence. */
#include "seifert_renderer.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { WIDTH = 576, HEIGHT = 1152, READOUT_HEIGHT = 110 };
static unsigned char reference[WIDTH*READOUT_HEIGHT*4];
static unsigned char current[WIDTH*READOUT_HEIGHT*4];

static void read_display(unsigned char *pixels)
{
    seifert_renderer_draw();
    glReadPixels(0, HEIGHT-READOUT_HEIGHT, WIDTH, READOUT_HEIGHT,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    assert(glGetError() == GL_NO_ERROR);
}

static void save_image(const char *path)
{
    unsigned char *pixels = malloc(WIDTH*HEIGHT*4);
    assert(pixels != NULL);
    glReadPixels(0,0,WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    assert(glGetError() == GL_NO_ERROR);
    FILE *image = fopen(path,"wb");
    assert(image != NULL);
    fprintf(image,"P6\n%d %d\n255\n",WIDTH,HEIGHT);
    for (int y=HEIGHT-1;y>=0;--y) {
        for (int x=0;x<WIDTH;++x) {
            assert(fwrite(pixels+((size_t)y*WIDTH+(size_t)x)*4,1,3,image)==3);
        }
    }
    assert(fclose(image)==0);
    free(pixels);
}

int main(int argc,char **argv)
{
    PFNEGLGETPLATFORMDISPLAYEXTPROC platform =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    assert(platform != NULL);
    EGLDisplay display = platform(EGL_PLATFORM_SURFACELESS_MESA,EGL_DEFAULT_DISPLAY,NULL);
    assert(display != EGL_NO_DISPLAY && eglInitialize(display,NULL,NULL));
    assert(eglBindAPI(EGL_OPENGL_ES_API));
    const EGLint attributes[] = {EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,
        EGL_BLUE_SIZE,8,EGL_ALPHA_SIZE,8,EGL_DEPTH_SIZE,16,EGL_NONE};
    EGLConfig config;
    EGLint count;
    assert(eglChooseConfig(display,attributes,&config,1,&count) && count==1);
    const EGLint size[] = {EGL_WIDTH,WIDTH,EGL_HEIGHT,HEIGHT,EGL_NONE};
    const EGLint version[] = {EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
    EGLSurface surface = eglCreatePbufferSurface(display,config,size);
    EGLContext context = eglCreateContext(display,config,EGL_NO_CONTEXT,version);
    assert(surface!=EGL_NO_SURFACE && context!=EGL_NO_CONTEXT);
    assert(eglMakeCurrent(display,surface,surface,context));
    assert(seifert_renderer_start(WIDTH,HEIGHT,2));
    const float tau = 6.28318530717958647692f;
    const float zero[3] = {0,0,0};
    const float common_turn[3] = {tau,tau,tau};
    assert(seifert_renderer_set_angles(zero));
    read_display(reference);
    unsigned bright = 0;
    for (size_t pixel=0;pixel<sizeof(reference);pixel+=4) {
        if (reference[pixel]>100 || reference[pixel+1]>100) ++bright;
    }
    assert(bright>1000); /* glyphs are visibly drawn, not an empty region */
    assert(seifert_renderer_set_angles(common_turn));
    read_display(current);
    assert(!memcmp(reference,current,sizeof(reference)));
    const float upper_turn[3] = {tau,0,0};
    assert(seifert_renderer_set_angles(upper_turn));
    read_display(current);
    assert(memcmp(reference,current,sizeof(reference))!=0);
    unsigned char retained[sizeof(current)];
    memcpy(retained,current,sizeof(current));
    const float invalid[3] = {NAN,0,0};
    assert(!seifert_renderer_set_angles(invalid));
    read_display(current);
    assert(!memcmp(retained,current,sizeof(current)));
    seifert_renderer_stop();
    assert(seifert_renderer_start(WIDTH,HEIGHT,2));
    read_display(current);
    assert(!memcmp(retained,current,sizeof(current)));
    const float corner_turn[3] = {0,tau,0};
    assert(seifert_renderer_set_angles(corner_turn));
    read_display(current);
    assert(memcmp(retained,current,sizeof(current))!=0);
    if (argc==2) save_image(argv[1]);
    seifert_renderer_resize(1152,576);
    seifert_renderer_draw();
    assert(glGetError()==GL_NO_ERROR);
    seifert_renderer_stop();
    assert(eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT));
    assert(eglDestroyContext(display,context));
    assert(eglDestroySurface(display,surface));
    assert(eglTerminate(display));
    puts("PASS real GLES2: visible readouts, common-rotation invariance, live pixel changes, invalid input, surface recreation, resize");
    return 0;
}
