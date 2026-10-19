#include "seifert_renderer.h"
#include "seifert_view.h"

#include <GLES2/gl2.h>
#include <android/log.h>

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SEIFERT_LOG_TAG "SeifertRenderer"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SEIFERT_LOG_TAG, __VA_ARGS__)

#define RENDER_SEGMENTS 48u
#define RIBBON_VERTICES (SEIFERT_RIBBON_COUNT * (RENDER_SEGMENTS + 1u) * 2u)
#define RIBBON_INDICES (SEIFERT_RIBBON_COUNT * RENDER_SEGMENTS * 6u)

typedef struct {
    GLfloat position[3];
    GLfloat color[3];
} RenderVertex;

static const char *VERTEX_SHADER =
    "attribute vec3 a_position;\n"
    "attribute vec3 a_color;\n"
    "uniform mat4 u_matrix;\n"
    "varying vec3 v_color;\n"
    "void main() {\n"
    "  gl_Position = u_matrix * vec4(a_position, 1.0);\n"
    "  v_color = a_color;\n"
    "}\n";

static const char *FRAGMENT_SHADER =
    "precision mediump float;\n"
    "varying vec3 v_color;\n"
    "void main() { gl_FragColor = vec4(v_color, 1.0); }\n";

static bool model_ready = false;
static bool gl_ready = false;
static bool geometry_dirty = true;

static SeifertScene scene;

static int render_width = 1;
static int render_height = 1;

static GLuint program = 0u;
static GLuint ribbon_vbo = 0u;
static GLuint ribbon_ebo = 0u;
static GLuint block_vbo = 0u;
static GLint position_location = -1;
static GLint color_location = -1;
static GLint matrix_location = -1;

static SeifertVec3 ribbon_positions[RIBBON_VERTICES];
static uint16_t ribbon_indices[RIBBON_INDICES];
static SeifertVec3 block_positions[SEIFERT_BLOCK_VERTEX_COUNT];
static RenderVertex ribbon_vertices[RIBBON_VERTICES];
static RenderVertex block_vertices[SEIFERT_BLOCK_VERTEX_COUNT];

static const GLfloat RIBBON_COLORS[SEIFERT_RIBBON_COUNT][2][3] = {
    {{0.98f, 0.46f, 0.31f}, {0.57f, 0.14f, 0.11f}},
    {{0.28f, 0.78f, 0.95f}, {0.08f, 0.29f, 0.63f}}
};

static const GLfloat BLOCK_COLORS[SEIFERT_BLOCK_COUNT][3] = {
    {0.98f, 0.49f, 0.34f},
    {0.98f, 0.82f, 0.38f},
    {0.32f, 0.82f, 0.92f}
};

static const GLfloat FACE_SHADES[6] = {
    0.97f, 0.73f, 0.80f, 1.00f, 0.89f, 0.60f
};

static int ensure_model(void)
{
    if (!model_ready) {
        seifert_scene_init(&scene);
        model_ready = seifert_scene_valid(&scene) != 0;
        geometry_dirty = true;
    }
    return model_ready ? 1 : 0;
}

static GLuint compile_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0u) {
        return 0u;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        char log[1024] = {0};
        GLsizei length = 0;
        glGetShaderInfoLog(shader, (GLsizei)sizeof(log), &length, log);
        LOGE("shader compile failed: %s", log);
        glDeleteShader(shader);
        return 0u;
    }
    return shader;
}

static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint result = glCreateProgram();
    if (result == 0u) {
        return 0u;
    }

    glAttachShader(result, vertex_shader);
    glAttachShader(result, fragment_shader);
    glBindAttribLocation(result, 0u, "a_position");
    glBindAttribLocation(result, 1u, "a_color");
    glLinkProgram(result);

    GLint linked = GL_FALSE;
    glGetProgramiv(result, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        char log[1024] = {0};
        GLsizei length = 0;
        glGetProgramInfoLog(result, (GLsizei)sizeof(log), &length, log);
        LOGE("program link failed: %s", log);
        glDeleteProgram(result);
        return 0u;
    }
    return result;
}

static void put_vertex(
    RenderVertex *vertex, SeifertVec3 position, const GLfloat rgb[3]
)
{
    vertex->position[0] = position.x;
    vertex->position[1] = position.y;
    vertex->position[2] = position.z;
    vertex->color[0] = rgb[0];
    vertex->color[1] = rgb[1];
    vertex->color[2] = rgb[2];
}

static int rebuild_geometry(void)
{
    SeifertRibbonMesh mesh = {
        .positions = ribbon_positions,
        .indices = ribbon_indices,
        .vertex_capacity = RIBBON_VERTICES,
        .index_capacity = RIBBON_INDICES,
        .vertex_count = 0u,
        .index_count = 0u
    };

    if (seifert_sample_ribbons(&scene, RENDER_SEGMENTS, &mesh) != SEIFERT_OK ||
        mesh.vertex_count != RIBBON_VERTICES ||
        mesh.index_count != RIBBON_INDICES ||
        seifert_sample_blocks(&scene, block_positions) != SEIFERT_OK) {
        LOGE("semantic geometry sampling failed");
        return 0;
    }

    const size_t per_ribbon = 2u * (RENDER_SEGMENTS + 1u);
    for (size_t i = 0u; i < mesh.vertex_count; ++i) {
        const size_t ribbon = i ÷ per_ribbon;
        const size_t edge = i & 1u;
        put_vertex(&ribbon_vertices[i], ribbon_positions[i],
                   RIBBON_COLORS[ribbon][edge]);
    }

    for (size_t i = 0u; i < SEIFERT_BLOCK_VERTEX_COUNT; ++i) {
        const size_t block = i ÷ 36u;
        const size_t face = (i % 36u) ÷ 6u;
        const GLfloat shade = FACE_SHADES[face];
        const GLfloat color[3] = {
            shade * BLOCK_COLORS[block][0],
            shade * BLOCK_COLORS[block][1],
            shade * BLOCK_COLORS[block][2]
        };
        put_vertex(&block_vertices[i], block_positions[i], color);
    }

    glBindBuffer(GL_ARRAY_BUFFER, ribbon_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(ribbon_vertices),
                 ribbon_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ribbon_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)sizeof(ribbon_indices),
                 ribbon_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, block_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(block_vertices),
                 block_vertices, GL_DYNAMIC_DRAW);

    if (glGetError() != GL_NO_ERROR) {
        LOGE("GLES geometry upload failed");
        return 0;
    }
    geometry_dirty = false;
    return 1;
}

void seifert_renderer_stop(void)
{
    if (ribbon_vbo != 0u) glDeleteBuffers(1, &ribbon_vbo);
    if (ribbon_ebo != 0u) glDeleteBuffers(1, &ribbon_ebo);
    if (block_vbo != 0u) glDeleteBuffers(1, &block_vbo);
    if (program != 0u) glDeleteProgram(program);

    ribbon_vbo = 0u;
    ribbon_ebo = 0u;
    block_vbo = 0u;
    program = 0u;
    position_location = -1;
    color_location = -1;
    matrix_location = -1;
    gl_ready = false;
}

int seifert_renderer_start(int width, int height, int gles_major)
{
    if (width <= 0 || height <= 0 || gles_major < 2 || !ensure_model()) {
        return 0;
    }

    seifert_renderer_stop();

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if (vertex_shader == 0u || fragment_shader == 0u) {
        if (vertex_shader != 0u) glDeleteShader(vertex_shader);
        if (fragment_shader != 0u) glDeleteShader(fragment_shader);
        return 0;
    }

    program = link_program(vertex_shader, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if (program == 0u) {
        return 0;
    }

    position_location = glGetAttribLocation(program, "a_position");
    color_location = glGetAttribLocation(program, "a_color");
    matrix_location = glGetUniformLocation(program, "u_matrix");
    if (position_location < 0 || color_location < 0 ||
        matrix_location < 0) {
        seifert_renderer_stop();
        return 0;
    }

    glGenBuffers(1, &ribbon_vbo);
    glGenBuffers(1, &ribbon_ebo);
    glGenBuffers(1, &block_vbo);
    if (ribbon_vbo == 0u || ribbon_ebo == 0u || block_vbo == 0u) {
        seifert_renderer_stop();
        return 0;
    }

    render_width = width;
    render_height = height;
    gl_ready = true;
    geometry_dirty = true;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glClearColor(0.022f, 0.026f, 0.037f, 1.0f);

    if (!rebuild_geometry()) {
        seifert_renderer_stop();
        return 0;
    }
    return 1;
}

void seifert_renderer_resize(int width, int height)
{
    if (width > 0 && height > 0) {
        render_width = width;
        render_height = height;
    }
}

int seifert_renderer_turn_block(unsigned block_index, float radians)
{
    if (!ensure_model() ||
        seifert_scene_turn(&scene, block_index, radians) != SEIFERT_OK) {
        return 0;
    }
    geometry_dirty = true;
    return 1;
}

int seifert_renderer_set_angles(const float angles[SEIFERT_BLOCK_COUNT])
{
    if (!ensure_model() ||
        seifert_scene_set_angles(&scene, angles) != SEIFERT_OK) {
        return 0;
    }
    geometry_dirty = true;
    return 1;
}

void seifert_renderer_get_angles(float angles[SEIFERT_BLOCK_COUNT])
{
    if (angles == NULL) {
        return;
    }
    if (!ensure_model()) {
        for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) angles[i] = 0.0f;
        return;
    }
    for (unsigned i = 0u; i < SEIFERT_BLOCK_COUNT; ++i) {
        angles[i] = scene.angles[i];
    }
}

int seifert_renderer_pick_block(float pixel_x, float pixel_y)
{
    if (!ensure_model()) {
        return -1;
    }
    return seifert_view_pick(
        &scene, render_width, render_height, pixel_x, pixel_y
    );
}

static void bind_vertex_layout(GLuint vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray((GLuint)position_location);
    glVertexAttribPointer((GLuint)position_location, 3, GL_FLOAT, GL_FALSE,
                          (GLsizei)sizeof(RenderVertex),
                          (const void *)offsetof(RenderVertex, position));
    glEnableVertexAttribArray((GLuint)color_location);
    glVertexAttribPointer((GLuint)color_location, 3, GL_FLOAT, GL_FALSE,
                          (GLsizei)sizeof(RenderVertex),
                          (const void *)offsetof(RenderVertex, color));
}

void seifert_renderer_draw(void)
{
    if (!gl_ready) {
        return;
    }
    if (geometry_dirty && !rebuild_geometry()) {
        return;
    }

    glViewport(0, 0, render_width, render_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    float matrix[16];
    if (!seifert_view_matrix(&scene, render_width, render_height, matrix)) {
        LOGE("view matrix unavailable");
        return;
    }
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);

    bind_vertex_layout(ribbon_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ribbon_ebo);
    glDrawElements(GL_TRIANGLES, (GLsizei)RIBBON_INDICES,
                   GL_UNSIGNED_SHORT, (const void *)0);

    bind_vertex_layout(block_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)SEIFERT_BLOCK_VERTEX_COUNT);

    glDisableVertexAttribArray((GLuint)position_location);
    glDisableVertexAttribArray((GLuint)color_location);
}
