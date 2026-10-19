#include <EGL/egl.h>
#include <android/input.h>
#include <android/log.h>
#include <android/native_window.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif
#include <android_native_app_glue.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "seifert_renderer.h"
#include "seifert_view.h"

#define SEIFERT_LOG_TAG "SeifertNative"
#define SEIFERT_LOG(...) __android_log_print(ANDROID_LOG_INFO, SEIFERT_LOG_TAG, __VA_ARGS__)
#define SEIFERT_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SEIFERT_LOG_TAG, __VA_ARGS__)

typedef struct {
    float angles[SEIFERT_BLOCK_COUNT];
} SeifertSavedState;

typedef struct {
    struct android_app *app;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    int width;
    int height;
    bool renderer_started;
    bool redraw;
    bool focused;

    SeifertGrab grab;
} SeifertAndroidState;

static void release_grab(SeifertAndroidState *state)
{
    seifert_grab_reset(&state->grab);
}

static int short_side(const SeifertAndroidState *state)
{
    if (state->width <= 0 || state->height <= 0) {
        return 1;
    }
    return state->width < state->height ? state->width : state->height;
}

static int pointer_index_for_id(
    AInputEvent *event,
    int32_t pointer_id
)
{
    const size_t count = AMotionEvent_getPointerCount(event);
    for (size_t index = 0u; index < count; ++index) {
        if (AMotionEvent_getPointerId(event, index) == pointer_id) {
            return (int)index;
        }
    }
    return -1;
}

static void stop_surface(SeifertAndroidState *state)
{
    if (state->renderer_started) {
        seifert_renderer_stop();
        state->renderer_started = false;
    }

    if (state->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(
            state->display,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT
        );

        if (state->context != EGL_NO_CONTEXT) {
            eglDestroyContext(state->display, state->context);
        }
        if (state->surface != EGL_NO_SURFACE) {
            eglDestroySurface(state->display, state->surface);
        }

        eglTerminate(state->display);
    }

    state->display = EGL_NO_DISPLAY;
    state->surface = EGL_NO_SURFACE;
    state->context = EGL_NO_CONTEXT;
    state->width = 0;
    state->height = 0;
    state->redraw = false;
    release_grab(state);
}

static bool start_surface(SeifertAndroidState *state)
{
    if (state->app->window == NULL) {
        return false;
    }

    float retained_angles[SEIFERT_BLOCK_COUNT];
    seifert_renderer_get_angles(retained_angles);
    stop_surface(state);
    (void)seifert_renderer_set_angles(retained_angles);

    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (state->display == EGL_NO_DISPLAY) {
        return false;
    }

    if (eglInitialize(state->display, NULL, NULL) != EGL_TRUE) {
        stop_surface(state);
        return false;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
        stop_surface(state);
        return false;
    }

    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLConfig config = NULL;
    EGLint config_count = 0;
    if (eglChooseConfig(
            state->display,
            config_attributes,
            &config,
            1,
            &config_count
        ) != EGL_TRUE || config_count != 1) {
        stop_surface(state);
        return false;
    }

    EGLint native_format = 0;
    if (eglGetConfigAttrib(
            state->display,
            config,
            EGL_NATIVE_VISUAL_ID,
            &native_format
        ) != EGL_TRUE) {
        stop_surface(state);
        return false;
    }

    ANativeWindow_setBuffersGeometry(
        state->app->window,
        0,
        0,
        native_format
    );

    const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    state->context = eglCreateContext(
        state->display,
        config,
        EGL_NO_CONTEXT,
        context_attributes
    );
    if (state->context == EGL_NO_CONTEXT) {
        stop_surface(state);
        return false;
    }

    state->surface = eglCreateWindowSurface(
        state->display,
        config,
        state->app->window,
        NULL
    );
    if (state->surface == EGL_NO_SURFACE) {
        stop_surface(state);
        return false;
    }

    if (eglMakeCurrent(
            state->display,
            state->surface,
            state->surface,
            state->context
        ) != EGL_TRUE) {
        stop_surface(state);
        return false;
    }

    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(state->display, state->surface, EGL_WIDTH, &width);
    eglQuerySurface(state->display, state->surface, EGL_HEIGHT, &height);

    if (width <= 0 || height <= 0 ||
        !seifert_renderer_start(width, height, 2)) {
        stop_surface(state);
        return false;
    }

    state->width = width;
    state->height = height;
    state->renderer_started = true;
    state->redraw = true;

    (void)eglSwapInterval(state->display, 1);

    SEIFERT_LOG("EGL ready: %dx%d GLES2", width, height);
    return true;
}

static void resize_surface(SeifertAndroidState *state)
{
    if (!state->renderer_started || state->surface == EGL_NO_SURFACE) {
        return;
    }

    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(state->display, state->surface, EGL_WIDTH, &width);
    eglQuerySurface(state->display, state->surface, EGL_HEIGHT, &height);

    if (width > 0 && height > 0) {
        state->width = width;
        state->height = height;
        seifert_renderer_resize(width, height);
        state->redraw = true;
        release_grab(state);
    }
}

static void save_state(SeifertAndroidState *state)
{
    SeifertSavedState *saved = malloc(sizeof(*saved));
    if (saved == NULL) {
        return;
    }

    seifert_renderer_get_angles(saved->angles);
    state->app->savedState = saved;
    state->app->savedStateSize = sizeof(*saved);
}

static void handle_command(
    struct android_app *app,
    int32_t command
)
{
    SeifertAndroidState *state =
        (SeifertAndroidState *)app->userData;

    switch (command) {
        case APP_CMD_INIT_WINDOW:
            (void)start_surface(state);
            break;

        case APP_CMD_TERM_WINDOW:
            stop_surface(state);
            break;

        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_CONFIG_CHANGED:
        case APP_CMD_CONTENT_RECT_CHANGED:
            resize_surface(state);
            break;

        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_RESUME:
            state->focused = true;
            state->redraw = true;
            break;

        case APP_CMD_LOST_FOCUS:
        case APP_CMD_PAUSE:
            state->focused = false;
            release_grab(state);
            break;

        case APP_CMD_SAVE_STATE:
            save_state(state);
            break;

        default:
            break;
    }
}

static int32_t handle_input(
    struct android_app *app,
    AInputEvent *event
)
{
    SeifertAndroidState *state =
        (SeifertAndroidState *)app->userData;

    if (!state->focused ||
        !state->renderer_started ||
        AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
        return 0;
    }

    const int32_t action = AMotionEvent_getAction(event);
    const int32_t masked = action & AMOTION_EVENT_ACTION_MASK;
    const int32_t pointer_index =
        (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
        AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

    if (masked == AMOTION_EVENT_ACTION_CANCEL) {
        release_grab(state);
        return 1;
    }

    if (masked == AMOTION_EVENT_ACTION_DOWN ||
        masked == AMOTION_EVENT_ACTION_POINTER_DOWN) {
        if (state->grab.active_pointer_id >= 0) {
            return 1;
        }

        const size_t count = AMotionEvent_getPointerCount(event);
        if (pointer_index < 0 ||
            (size_t)pointer_index >= count) {
            return 1;
        }

        const float x =
            AMotionEvent_getX(event, (size_t)pointer_index);
        const float y =
            AMotionEvent_getY(event, (size_t)pointer_index);

        const int block = seifert_renderer_pick_block(x, y);
        const int32_t pointer_id = AMotionEvent_getPointerId(
            event, (size_t)pointer_index
        );
        if (!seifert_grab_begin(&state->grab, block, pointer_id, x)) {
            return 0;
        }
        return 1;
    }

    if (masked == AMOTION_EVENT_ACTION_MOVE &&
        state->grab.active_pointer_id >= 0) {
        const int index = pointer_index_for_id(
            event, state->grab.active_pointer_id
        );
        if (index < 0) {
            release_grab(state);
            return 1;
        }

        const float x = AMotionEvent_getX(event, (size_t)index);
        float delta_angle = 0.0f;
        if (seifert_grab_move(
                &state->grab,
                state->grab.active_pointer_id,
                x,
                short_side(state),
                &delta_angle
            ) &&
            fabsf(delta_angle) > 0.0f &&
            seifert_renderer_turn_block(
                (unsigned)state->grab.active_block, delta_angle
            )) {
            state->redraw = true;
        }

        return 1;
    }

    if ((masked == AMOTION_EVENT_ACTION_UP ||
         masked == AMOTION_EVENT_ACTION_POINTER_UP) &&
        state->grab.active_pointer_id >= 0) {
        const size_t count = AMotionEvent_getPointerCount(event);
        if (pointer_index >= 0 &&
            (size_t)pointer_index < count) {
            const int32_t released_id =
                AMotionEvent_getPointerId(
                    event,
                    (size_t)pointer_index
                );
            (void)seifert_grab_end(&state->grab, released_id);
        }
        return 1;
    }

    return 0;
}

void android_main(struct android_app *app)
{
    SEIFERT_LOG("native entry");

    SeifertAndroidState state;
    memset(&state, 0, sizeof(state));

    state.app = app;
    state.display = EGL_NO_DISPLAY;
    state.surface = EGL_NO_SURFACE;
    state.context = EGL_NO_CONTEXT;
    seifert_grab_reset(&state.grab);
    state.focused = true;

    app->userData = &state;
    app->onAppCmd = handle_command;
    app->onInputEvent = handle_input;

    if (app->savedState != NULL &&
        app->savedStateSize == sizeof(SeifertSavedState)) {
        const SeifertSavedState *saved =
            (const SeifertSavedState *)app->savedState;
        (void)seifert_renderer_set_angles(saved->angles);
    }

    for (;;) {
        int events = 0;
        struct android_poll_source *source = NULL;
        const int timeout =
            state.renderer_started && state.redraw ? 0 : -1;

        int ident = 0;
        while ((ident = ALooper_pollOnce(
                    timeout,
                    NULL,
                    &events,
                    (void **)&source
                )) >= 0) {
            (void)ident;

            if (source != NULL) {
                source->process(app, source);
            }

            if (app->destroyRequested != 0) {
                stop_surface(&state);
                return;
            }

            if (state.renderer_started && state.redraw) {
                break;
            }
        }

        if (app->destroyRequested != 0) {
            stop_surface(&state);
            return;
        }

        if (state.renderer_started && state.redraw) {
            seifert_renderer_draw();

            if (eglSwapBuffers(
                    state.display,
                    state.surface
                ) != EGL_TRUE) {
                SEIFERT_LOGE(
                    "eglSwapBuffers failed: 0x%x",
                    eglGetError()
                );
                stop_surface(&state);
                continue;
            }

            state.redraw = false;
        }
    }
}
