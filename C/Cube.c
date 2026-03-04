#include "gaspardos_user_x86.h"


/*
** cube3d_nolibc.c
**
** Zéro libc. Zéro malloc. Zéro float depuis math.h.
** Seule API externe :
**   void gaspardapi_put_pixel(uint32_t x, uint32_t y, uint32_t color);
**
** Tout est calculé à la main :
**   - sqrt / sin / cos / tan via séries de Taylor
**   - z-buffer statique sur la stack/BSS
**   - Bresenham pour les lignes
*/



/* ─── dimensions écran ───────────────────────────────────────────── */

#define SCREEN_W  (1024 -1 )
#define SCREEN_H  (768 -1)

/* ─── z-buffer statique (float-like : on stocke 1/z * 65536, fixe) ─ */
/* On utilise uint16_t pour économiser la mémoire.
   0 = infini loin, 65535 = très proche.
   inv_z_scaled = (1.0 / z) * Z_SCALE  avec z > 0               */

#define Z_SCALE 4096

static uint16_t *zbuf = NULL;

/* ─── maths sans libc ────────────────────────────────────────────── */

/* Valeur absolue entière */
static int iabs(int x) { return x < 0 ? -x : x; }

/* Float maison : on travaille en virgule fixe Q16.16
   mais pour la 3D on préfère rester en float soft.
   On utilise float du compilateur (pas de libm) — les opérations
   +,-,*,/ sont intrinsèques au compilateur C, seules sin/cos/sqrt
   nécessitent libm. On les réimplémente ci-dessous.             */

#define PI    3.14159265358979f
#define PI2   6.28318530717959f
#define HALF_PI 1.57079632679490f

/* sqrt via méthode de Newton-Raphson */
static float my_sqrt(float x)
{
    if (x <= 0.0f) return 0.0f;
    float r = x > 1.0f ? x * 0.5f : 1.0f;
    /* 8 itérations suffisent pour la précision nécessaire */
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    r = (r + x / r) * 0.5f;
    return r;
}

/* Ramener l'angle dans [-PI, PI] */
static float wrap_angle(float a)
{
    while (a >  PI) a -= PI2;
    while (a < -PI) a += PI2;
    return a;
}

/* sin via série de Taylor : sin(x) = x - x³/6 + x⁵/120 - x⁷/5040 + ...
   Converge bien sur [-PI, PI].                                      */
static float my_sin(float x)
{
    x = wrap_angle(x);
    float x2  = x * x;
    float x3  = x2 * x;
    float x5  = x3 * x2;
    float x7  = x5 * x2;
    float x9  = x7 * x2;
    float x11 = x9 * x2;
    return x
         - x3  * 0.16666666667f
         + x5  * 0.00833333333f
         - x7  * 0.00019841270f
         + x9  * 0.00000275573f
         - x11 * 0.00000002505f;
}

static float my_cos(float x) { return my_sin(x + HALF_PI); }

/* tan = sin/cos  (évite la division si cos ≈ 0) */
static float my_tan(float x)
{
    float c = my_cos(x);
    if (c < 0.0001f && c > -0.0001f) c = 0.0001f;
    return my_sin(x) / c;
}

/* ─── Vec3 ───────────────────────────────────────────────────────── */

typedef struct { float x, y, z; } Vec3;

static Vec3 v3_add  (Vec3 a, Vec3 b) { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 v3_sub  (Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 v3_scale(Vec3 v, float s){ return (Vec3){v.x*s,   v.y*s,   v.z*s  }; }

static float v3_dot(Vec3 a, Vec3 b)  { return a.x*b.x + a.y*b.y + a.z*b.z; }

static Vec3 v3_cross(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

static Vec3 v3_norm(Vec3 v) {
    float l = my_sqrt(v3_dot(v, v));
    if (l < 1e-6f) return (Vec3){0,0,0};
    return v3_scale(v, 1.0f / l);
}

/* Rotations élémentaires */
static Vec3 v3_rot_x(Vec3 v, float a) {
    float c = my_cos(a), s = my_sin(a);
    return (Vec3){ v.x, v.y*c - v.z*s, v.y*s + v.z*c };
}
static Vec3 v3_rot_y(Vec3 v, float a) {
    float c = my_cos(a), s = my_sin(a);
    return (Vec3){ v.x*c + v.z*s, v.y, -v.x*s + v.z*c };
}
static Vec3 v3_rot_z(Vec3 v, float a) {
    float c = my_cos(a), s = my_sin(a);
    return (Vec3){ v.x*c - v.y*s, v.x*s + v.y*c, v.z };
}

/* ─── Couleur ────────────────────────────────────────────────────── */

/* Packer ARGB/XRGB 0xRRGGBB */
static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

/* ─── Z-buffer + put_pixel ───────────────────────────────────────── */

static void zbuf_clear(void)
{
    for (int y = 0; y < SCREEN_H; y++)
        for (int x = 0; x < SCREEN_W; x++)
            zbuf[y+x] = 0;  /* 0 = rien devant */
}

/* Dessine un pixel si plus proche (inv_z plus grand = plus proche) */
static void put(int x, int y, float z, uint32_t color)
{
    if ((uint32_t)x >= (uint32_t)SCREEN_W) return;  /* cast unsigned = test [0,W[ */
    if ((uint32_t)y >= (uint32_t)SCREEN_H) return;

    /* z > 0 ici (distance caméra). On stocke 1/z pour le test. */
    uint16_t inv_z = (z < 0.0001f) ? 65535u : (uint16_t)(Z_SCALE / z);
    /* Plus z est petit (proche), plus inv_z est grand → écrase */
    if (inv_z <= zbuf[y+x]) return;
    zbuf[y+x] = inv_z;
    gaspardapi_put_pixel((uint32_t)x, (uint32_t)y, color);
}

/* ─── Projection perspective ─────────────────────────────────────── */

#define FOV_DEG  70.0f
#define FOV_RAD  (FOV_DEG * PI / 180.0f)
#define NEAR     0.1f

/*
**  fov_factor = 1 / tan(fov/2)
**  x_screen = fov_factor * v.x / (-v.z)   → NDC [-1,1]
**  y_screen = fov_factor * v.y / (-v.z * aspect)
**  pixel_x  = (x_screen + 1) * 0.5 * W
*/

static int project(Vec3 v, int *px, int *py, float *out_z)
{
    if (v.z >= -NEAR) return 0;   /* derrière la caméra */

    float inv_tan_half_fov = 1.0f / my_tan(FOV_RAD * 0.5f);
    float aspect = (float)SCREEN_W / (float)SCREEN_H;
    float nz     = -v.z;          /* positif */

    float xn = (inv_tan_half_fov * v.x) / (nz * aspect);
    float yn = (inv_tan_half_fov * v.y) / nz;

    *px    = (int)((xn + 1.0f) * 0.5f * SCREEN_W);
    *py    = (int)((1.0f - (yn + 1.0f) * 0.5f) * SCREEN_H);
    *out_z = nz;
    return 1;
}

/* ─── Ligne de Bresenham avec interpolation de z ─────────────────── */

static void draw_line(int x0, int y0, float z0,
                      int x1, int y1, float z1,
                      uint32_t color)
{
    int dx  =  iabs(x1 - x0);
    int dy  = -iabs(y1 - y0);
    int sx  = x0 < x1 ? 1 : -1;
    int sy  = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int len = dx > -dy ? dx : -dy;
    if (len == 0) len = 1;
    int step = 0;

    while (1) {
        float t = (float)step / (float)len;
        float z = z0 + t * (z1 - z0);
        put(x0, y0, z, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
        step++;
    }
}

/* ─── Cube ───────────────────────────────────────────────────────── */

/*
**  Sommets en coordonnées locales centrées sur l'origine.
**
**       7 ──── 6
**      /|     /|
**     3 ──── 2 |
**     | 4 ───| 5
**     |/     |/
**     0 ──── 1
*/

#define N_VERTS  8
#define N_EDGES 12

static const Vec3 CUBE_V[N_VERTS] = {
    {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f},
    { 0.5f, 0.5f,-0.5f}, {-0.5f,0.5f,-0.5f},
    {-0.5f,-0.5f, 0.5f}, {0.5f,-0.5f, 0.5f},
    { 0.5f, 0.5f, 0.5f}, {-0.5f,0.5f, 0.5f},
};

static const int CUBE_E[N_EDGES][2] = {
    {0,1},{1,2},{2,3},{3,0},  /* face arrière */
    {4,5},{5,6},{6,7},{7,4},  /* face avant   */
    {0,4},{1,5},{2,6},{3,7},  /* traverses    */
};

typedef struct {
    Vec3     position;   /* translation (espace caméra) */
    Vec3     rotation;   /* Euler XYZ en radians        */
    float    scale;
    uint32_t color;
} Cube;

static void cube_draw(const Cube *c)
{
    /* Transformer les 8 sommets */
    Vec3 world[N_VERTS];
    for (int i = 0; i < N_VERTS; i++) {
        Vec3 v = v3_scale(CUBE_V[i], c->scale);
        v = v3_rot_x(v, c->rotation.x);
        v = v3_rot_y(v, c->rotation.y);
        v = v3_rot_z(v, c->rotation.z);
        v = v3_add(v, c->position);
        world[i] = v;
    }

    /* Projeter et rasteriser les 12 arêtes */
    for (int e = 0; e < N_EDGES; e++) {
        int ia = CUBE_E[e][0];
        int ib = CUBE_E[e][1];
        int ax, ay, bx, by;
        float az, bz;
        if (!project(world[ia], &ax, &ay, &az)) continue;
        if (!project(world[ib], &bx, &by, &bz)) continue;
        draw_line(ax, ay, az, bx, by, bz, c->color);
    }
}

/* ─── Fond dégradé ───────────────────────────────────────────────── */

static void draw_background(void)
{
    for (int y = 0; y < SCREEN_H; y++) {
        /* Dégradé vertical bleu nuit → noir */
        uint8_t b = (uint8_t)(30 - (y * 30) / SCREEN_H);
        uint8_t g = (uint8_t)( 5 - (y *  5) / SCREEN_H);
        uint32_t color = rgb(0, g, b);
        for (int x = 0; x < SCREEN_W; x++)
            gaspardapi_put_pixel((uint32_t)x, (uint32_t)y, color);
    }
}

/* ─── Police bitmap 5×7 (sans libc) ─────────────────────────────── */

static const uint8_t FONT[96][7] = {
    /* ASCII 32 = ' ' */
    [0]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /* '0'=48 → idx 16 */
    [16] = {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
    [17] = {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    [18] = {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F},
    [19] = {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},
    [20] = {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
    [21] = {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    [22] = {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
    [23] = {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    [24] = {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
    [25] = {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    /* 'A'=65 → idx 33 */
    [33] = {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11},
    [34] = {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
    [35] = {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
    [36] = {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C},
    [37] = {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},
    [38] = {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    [39] = {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},
    [40] = {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
    [41] = {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
    [42] = {0x07,0x02,0x02,0x02,0x02,0x12,0x0C},
    [43] = {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
    [44] = {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    [45] = {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},
    [46] = {0x11,0x19,0x15,0x13,0x11,0x11,0x11},
    [47] = {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
    [48] = {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
    [49] = {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},
    [50] = {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    [51] = {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E},
    [52] = {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
    [53] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
    [54] = {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
    [55] = {0x11,0x11,0x15,0x15,0x15,0x1B,0x11},
    [56] = {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    [57] = {0x11,0x11,0x11,0x0A,0x04,0x04,0x04},
    [58] = {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F},
    /* '-' = 45 → idx 13 */
    [13] = {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    /* '.' = 46 → idx 14 */
    [14] = {0x00,0x00,0x00,0x00,0x00,0x04,0x00},
    /* ':' = 58 → idx 26 */
    [26] = {0x00,0x04,0x00,0x00,0x04,0x00,0x00},
};

static void draw_char(int cx, int cy, char ch, int scale, uint32_t col)
{
    int idx = (int)ch - 32;
    if (idx < 0 || idx >= 96) return;

    for (int row = 0; row < 7; row++) {
        uint8_t bits = FONT[idx][row];
        for (int bit = 4; bit >= 0; bit--) {
            if (bits & (1 << bit)) {
                int px = cx + (4 - bit) * scale;
                int py = cy + row * scale;
                for (int sy = 0; sy < scale; sy++)
                    for (int sx = 0; sx < scale; sx++)
                        gaspardapi_put_pixel(
                            (uint32_t)(px + sx),
                            (uint32_t)(py + sy),
                            col);
            }
        }
    }
}

static void draw_str(int x, int y, const char *s, int scale, uint32_t col)
{
    for (int i = 0; s[i]; i++)
        draw_char(x + i * (5 * scale + scale), y, s[i], scale, col);
}

/* ─── Point d'entrée ─────────────────────────────────────────────── */

/*
** Appeler cette fonction depuis votre boucle principale.
** angle : en radians, incrémenter à chaque frame pour animer.
*/
void scene_render(float angle)
{
    zbuf_clear();
    draw_background();

    /* ── Cube central ── */
    Cube c0 = {
        .position = {  0.0f,  0.0f, -4.5f },
        .rotation = {  angle * 0.7f, angle, angle * 0.3f },
        .scale    = 2.0f,
        .color    = rgb(255, 255, 255),   /* cyan */
    };
    cube_draw(&c0);

    /* ── Cube haut-gauche ── */
    Cube c1 = {
        .position = { -2.8f,  1.3f, -7.0f },
        .rotation = {  angle * 1.2f, angle * 0.5f, 0.3f },
        .scale    = 1.2f,
        .color    = rgb(255, 140, 40),   /* orange */
    };
    cube_draw(&c1);

    /* ── Cube bas-droite ── */
    Cube c2 = {
        .position = {  3.0f, -1.2f, -8.0f },
        .rotation = {  0.2f, angle * 1.5f, angle * 0.8f },
        .scale    = 1.4f,
        .color    = rgb(100, 255, 120),  /* vert */
    };
    cube_draw(&c2);

    /* ── HUD texte ── */
    draw_str(20, 20, "CUBE3D",                2, rgb(255, 220,  50));
    draw_str(20, 42, "NO LIBC - VEC3 - PERSP",1, rgb(200, 200, 200));
    draw_str(20, 52, "GASPARDAPI PUT PIXEL",  1, rgb(200, 200, 200));
}


void gmain() {



    gaspardapi_init_video();
    zbuf = gaspardapi_alloc(4096 /*MAX*/);
    scene_render(0.55f);   /* angle fixe pour le test */



}