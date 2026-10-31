#include "ui.h"
#include "lvgl/lvgl.h"

#include <math.h>

#define COL_BG        0x000000
#define COL_CARD      0x0C0C0E
#define COL_LINE      0x3A3A40
#define COL_LINE_SOFT 0x2A2A2E
#define COL_LINE_HI   0xE4E4E6
#define COL_DASH      0x6A6A70
#define COL_TEXT      0xF4F4F5
#define COL_MUTED     0x9A9AA0
#define COL_FAINT     0x8A8A90
#define COL_ICON      0xC8C8CC
#define COL_BLUE      0x7EA8C6
#define COL_YELLOW    0xF0C400
#define COL_PALE_BLUE 0xA8D0E8

static lv_obj_t * port_cards[4];
static lv_obj_t * port_num[4];
static lv_obj_t * port_name[4];
static lv_obj_t * status_page;
static lv_obj_t * test_page;
static int selected_port;
static bool showing_test;

static void flat(lv_obj_t * obj)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(COL_TEXT), 0);
    lv_obj_set_scrollable(obj, false);
    lv_obj_set_clickable(obj, false);
}

static lv_obj_t * hrow(lv_obj_t * parent, int gap)
{
    lv_obj_t * obj = lv_obj_create(parent);
    flat(obj);
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    lv_obj_set_width(obj, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(obj, gap, 0);
    return obj;
}

static lv_obj_t * icon_box(lv_obj_t * parent, int w, int h)
{
    lv_obj_t * box = lv_obj_create(parent);
    flat(box);
    lv_obj_set_size(box, w, h);
    return box;
}

static void stroke(lv_obj_t * obj, lv_color_t color, int width)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(obj, color, 0);
    lv_obj_set_style_border_width(obj, width, 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
}

static lv_obj_t * filled(lv_obj_t * parent, int x, int y, int w, int h, lv_color_t color)
{
    lv_obj_t * bar = lv_obj_create(parent);
    flat(bar);
    lv_obj_set_pos(bar, x, y);
    lv_obj_set_size(bar, w, h);
    lv_obj_set_style_bg_color(bar, color, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, 1, 0);
    return bar;
}

static lv_obj_t * make_ring(lv_obj_t * parent, int size, int border, lv_color_t color,
                            const char * text, const lv_font_t * font)
{
    lv_obj_t * ring = lv_obj_create(parent);
    flat(ring);
    lv_obj_set_size(ring, size, size);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    stroke(ring, color, border);

    if(text[0] != '\0') {
        lv_obj_t * label = lv_label_create(ring);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, color, 0);
        lv_obj_set_style_text_font(label, font, 0);
        lv_obj_center(label);
    }
    return ring;
}

static lv_obj_t * make_logo(lv_obj_t * parent)
{
    lv_obj_t * logo = lv_image_create(parent);
    lv_obj_set_size(logo, 34, 30);
    lv_image_set_src(logo, "A:assets/thermtest_logo.png");
    lv_image_set_inner_align(logo, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_set_clickable(logo, false);
    lv_obj_set_style_bg_opa(logo, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(logo, 0, 0);
    lv_obj_set_style_pad_all(logo, 0, 0);
    return logo;
}

static lv_obj_t * make_thermometer(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 14, 20);
    lv_obj_t * stem = lv_obj_create(box);
    flat(stem);
    lv_obj_set_size(stem, 6, 12);
    lv_obj_align(stem, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(stem, LV_RADIUS_CIRCLE, 0);
    stroke(stem, color, 2);

    lv_obj_t * bulb = lv_obj_create(box);
    flat(bulb);
    lv_obj_set_size(bulb, 10, 10);
    lv_obj_align(bulb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(bulb, LV_RADIUS_CIRCLE, 0);
    stroke(bulb, color, 2);

    lv_obj_t * mercury = lv_obj_create(box);
    flat(mercury);
    lv_obj_set_size(mercury, 2, 8);
    lv_obj_align(mercury, LV_ALIGN_BOTTOM_MID, 0, -3);
    lv_obj_set_style_bg_color(mercury, color, 0);
    lv_obj_set_style_bg_opa(mercury, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(mercury, LV_RADIUS_CIRCLE, 0);
    return box;
}

static lv_obj_t * make_device(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 22, 18);
    lv_obj_t * screen = lv_obj_create(box);
    flat(screen);
    lv_obj_set_size(screen, 20, 12);
    lv_obj_align(screen, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(screen, 2, 0);
    stroke(screen, color, 2);

    filled(box, 10, 12, 2, 3, color);
    filled(box, 5, 15, 12, 2, color);
    return box;
}

static lv_obj_t * make_nodes(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 22, 18);
    static const int pos[3][2] = {{7, 0}, {0, 12}, {14, 12}};
    for(int i = 0; i < 3; i++) {
        lv_obj_t * node = lv_obj_create(box);
        flat(node);
        lv_obj_set_size(node, 8, 6);
        lv_obj_set_pos(node, pos[i][0], pos[i][1]);
        lv_obj_set_style_radius(node, 1, 0);
        stroke(node, color, 1);
    }
    filled(box, 10, 6, 2, 4, color);
    filled(box, 4, 10, 14, 2, color);
    return box;
}

static void round_rect_point(float x, float y, float w, float h, float r, float dist, float * ox, float * oy)
{
    const float pi = 3.14159265f;
    const float half = pi * 0.5f;
    float horiz = w - 2.0f * r;
    float vert = h - 2.0f * r;
    if(horiz < 0) horiz = 0;
    if(vert < 0) vert = 0;
    float arc = half * r;
    float segs[8] = {horiz, arc, vert, arc, horiz, arc, vert, arc};
    float total = 2.0f * horiz + 2.0f * vert + 4.0f * arc;
    if(total <= 1.0f) {
        *ox = x;
        *oy = y;
        return;
    }
    while(dist < 0) dist += total;
    while(dist >= total) dist -= total;

    float acc = 0;
    int seg = 0;
    for(; seg < 7; seg++) {
        if(dist < acc + segs[seg]) break;
        acc += segs[seg];
    }
    float len = segs[seg];
    float t = (len > 0.0f) ? (dist - acc) / len : 0.0f;
    float cxr = x + w - r;
    float cxl = x + r;
    float cyt = y + r;
    float cyb = y + h - r;
    float a;

    switch(seg) {
        case 0: *ox = x + r + t * horiz; *oy = y; break;
        case 1:
            a = -half + t * half;
            *ox = cxr + cosf(a) * r;
            *oy = cyt + sinf(a) * r;
            break;
        case 2: *ox = x + w; *oy = y + r + t * vert; break;
        case 3:
            a = t * half;
            *ox = cxr + cosf(a) * r;
            *oy = cyb + sinf(a) * r;
            break;
        case 4: *ox = x + w - r - t * horiz; *oy = y + h; break;
        case 5:
            a = half + t * half;
            *ox = cxl + cosf(a) * r;
            *oy = cyb + sinf(a) * r;
            break;
        case 6: *ox = x; *oy = y + h - r - t * vert; break;
        default:
            a = pi + t * half;
            *ox = cxl + cosf(a) * r;
            *oy = cyt + sinf(a) * r;
            break;
    }
}

static void dashed_border_cb(lv_event_t * e)
{
    if(selected_port == 3) return;

    lv_obj_t * obj = lv_event_get_target(e);
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    const int stroke = 2;
    const int inset = 1;
    float x = (float)(coords.x1 + inset);
    float y = (float)(coords.y1 + inset);
    float w = (float)(lv_area_get_width(&coords) - inset * 2);
    float h = (float)(lv_area_get_height(&coords) - inset * 2);
    float r = (float)lv_obj_get_style_radius(obj, LV_PART_MAIN) - inset;
    if(w < 4 || h < 4) return;
    if(r < 2) r = 2;
    if(r > w / 2.0f) r = w / 2.0f;
    if(r > h / 2.0f) r = h / 2.0f;

    float horiz = w - 2.0f * r;
    float vert = h - 2.0f * r;
    float total = 2.0f * horiz + 2.0f * vert + 6.2831853f * r;
    const float dash = 5.0f;
    const float gap = 4.0f;
    const float step = 2.0f;

    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);
    dsc.color = lv_color_hex(COL_DASH);
    dsc.width = stroke;
    dsc.opa = LV_OPA_COVER;

    for(float d = 0; d < total; d += dash + gap) {
        float end = d + dash;
        if(end > total) end = total;
        float x0, y0;
        round_rect_point(x, y, w, h, r, d, &x0, &y0);
        for(float s = d + step; s <= end + 0.01f; s += step) {
            float ss = (s > end) ? end : s;
            float x1, y1;
            round_rect_point(x, y, w, h, r, ss, &x1, &y1);
            dsc.p1.x = x0;
            dsc.p1.y = y0;
            dsc.p2.x = x1;
            dsc.p2.y = y1;
            lv_draw_line(layer, &dsc);
            x0 = x1;
            y0 = y1;
            if(ss >= end) break;
        }
    }
}

static void style_method(int index, bool selected)
{
    bool empty = (index == 3);
    bool dashed = empty && !selected;
    lv_obj_t * card = port_cards[index];

    lv_obj_set_style_bg_color(card, lv_color_hex(COL_CARD), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 16, 0);
    if(dashed) {
        lv_obj_set_style_border_width(card, 0, 0);
    }
    else {
        lv_obj_set_style_border_width(card, 2, 0);
        lv_obj_set_style_border_color(card, lv_color_hex(selected ? COL_LINE_HI : COL_LINE), 0);
        lv_obj_set_style_border_opa(card, LV_OPA_COVER, 0);
    }

    lv_color_t text = lv_color_hex((empty && !selected) ? COL_FAINT : COL_TEXT);
    lv_obj_set_style_text_color(port_num[index], text, 0);
    lv_obj_set_style_text_color(port_name[index], text, 0);
}

static void select_port(int index)
{
    selected_port = index;
    for(int i = 0; i < 4; i++) style_method(i, i == index);
}

static void port_clicked(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    for(int i = 0; i < 4; i++) {
        if(port_cards[i] == target) select_port(i);
    }
}

static void add_info_row(lv_obj_t * parent, int kind, const char * name, const char * value, const char * note)
{
    lv_obj_t * line = lv_obj_create(parent);
    flat(line);
    lv_obj_set_width(line, lv_pct(100));
    lv_obj_set_height(line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(line, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(line, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(line, 10, 0);

    if(kind == 0) make_device(line, lv_color_hex(COL_ICON));
    else make_nodes(line, lv_color_hex(COL_ICON));

    lv_obj_t * label = lv_label_create(line);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_MUTED), 0);

    lv_obj_t * val = lv_label_create(line);
    lv_label_set_text(val, value);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(val, lv_color_hex(COL_TEXT), 0);

    if(note != NULL) {
        lv_obj_t * extra = lv_label_create(line);
        lv_label_set_text(extra, note);
        lv_obj_set_style_text_font(extra, &lv_font_montserrat_22, 0);
        lv_obj_set_style_text_color(extra, lv_color_hex(COL_MUTED), 0);
        lv_obj_set_style_margin_left(extra, -4, 0);
    }
}

static lv_obj_t * page_column(lv_obj_t * parent)
{
    lv_obj_t * page = lv_obj_create(parent);
    flat(page);
    lv_obj_set_width(page, lv_pct(100));
    lv_obj_set_flex_grow(page, 1);
    lv_obj_set_height(page, lv_pct(100));
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    return page;
}

static void show_panel_page(bool test)
{
    showing_test = test;
    lv_obj_set_hidden(status_page, test);
    lv_obj_set_hidden(test_page, !test);
}

static void panel_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    show_panel_page(!showing_test);
}

static lv_obj_t * add_value_row(lv_obj_t * parent, const char * name, const char * value, uint32_t value_color)
{
    lv_obj_t * line = lv_obj_create(parent);
    flat(line);
    lv_obj_set_width(line, lv_pct(100));
    lv_obj_set_height(line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(line, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(line, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(line, 14, 0);

    lv_obj_t * label = lv_label_create(line);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_MUTED), 0);

    lv_obj_t * val = lv_label_create(line);
    lv_label_set_text(val, value);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(val, lv_color_hex(value_color), 0);
    return line;
}

static void build_test_page(lv_obj_t * page)
{
    lv_obj_t * title = lv_label_create(page);
    lv_label_set_text(title, "Transient Test");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_44, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(COL_TEXT), 0);

    lv_obj_t * stage = add_value_row(page, "Stage:", "Heating", COL_YELLOW);
    lv_obj_set_style_margin_top(stage, 18, 0);

    lv_obj_t * elapsed = add_value_row(page, "Elapsed:", "03:42", COL_TEXT);
    lv_obj_set_style_margin_top(elapsed, 10, 0);

    lv_obj_t * progress = lv_obj_create(page);
    flat(progress);
    lv_obj_set_width(progress, lv_pct(100));
    lv_obj_set_height(progress, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(progress, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(progress, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(progress, 14, 0);
    lv_obj_set_style_margin_top(progress, 28, 0);

    lv_obj_t * bar = lv_bar_create(progress);
    lv_obj_set_height(bar, 16);
    lv_obj_set_flex_grow(bar, 1);
    lv_obj_set_clickable(bar, false);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 46, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x4A4A50), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x3C9AE6), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);

    lv_obj_t * percent = lv_label_create(progress);
    lv_label_set_text(percent, "46%");
    lv_obj_set_style_text_font(percent, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(percent, lv_color_hex(COL_TEXT), 0);

    lv_obj_t * note = lv_obj_create(page);
    flat(note);
    lv_obj_set_width(note, lv_pct(100));
    lv_obj_set_height(note, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(note, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(note, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(note, 12, 0);
    lv_obj_set_style_pad_all(note, 14, 0);
    lv_obj_set_style_margin_top(note, 18, 0);
    lv_obj_set_style_bg_color(note, lv_color_hex(0x161618), 0);
    lv_obj_set_style_bg_opa(note, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(note, 12, 0);
    lv_obj_set_style_border_width(note, 1, 0);
    lv_obj_set_style_border_color(note, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_border_opa(note, LV_OPA_COVER, 0);
    make_ring(note, 28, 2, lv_color_hex(COL_BLUE), "i", &lv_font_montserrat_16);

    lv_obj_t * note_text = lv_label_create(note);
    lv_label_set_text(note_text, "Cooling will begin automatically\nafter test.");
    lv_obj_set_style_text_font(note_text, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(note_text, lv_color_hex(COL_ICON), 0);
}

void ui_init(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(COL_BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(COL_TEXT), 0);
    lv_obj_set_style_text_font(screen, &lv_font_montserrat_14, 0);
    lv_obj_set_scrollable(screen, false);

    lv_obj_t * root = lv_obj_create(screen);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    flat(root);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * header = lv_obj_create(root);
    flat(header);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_ver(header, 10, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(header, 18, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_color(header, lv_color_hex(COL_LINE_SOFT), 0);
    lv_obj_set_style_border_opa(header, LV_OPA_COVER, 0);

    lv_obj_t * brand = hrow(header, 10);
    make_logo(brand);
    lv_obj_t * brand_name = lv_label_create(brand);
    lv_label_set_text(brand_name, "Thermtest");
    lv_obj_set_style_text_font(brand_name, &lv_font_montserrat_22, 0);
    lv_obj_t * model = lv_label_create(brand);
    lv_label_set_text(model, "MP-S");
    lv_obj_set_style_text_font(model, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(model, lv_color_hex(COL_ICON), 0);

    lv_obj_t * status_row = hrow(header, 22);

    lv_obj_t * ready = hrow(status_row, 8);
    make_ring(ready, 22, 2, lv_color_hex(COL_BLUE), LV_SYMBOL_OK, &lv_font_montserrat_12);
    lv_obj_t * ready_label = lv_label_create(ready);
    lv_label_set_text(ready_label, "System Ready");
    lv_obj_set_style_text_font(ready_label, &lv_font_montserrat_22, 0);

    lv_obj_t * temp = hrow(status_row, 8);
    make_thermometer(temp, lv_color_hex(COL_ICON));
    lv_obj_t * temp_label = lv_label_create(temp);
    lv_label_set_text(temp_label, "23.4 \xC2\xB0""C");
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_22, 0);

    lv_obj_t * gear = lv_label_create(status_row);
    lv_label_set_text(gear, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(gear, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(gear, lv_color_hex(COL_ICON), 0);

    lv_obj_t * body = lv_obj_create(root);
    flat(body);
    lv_obj_set_width(body, lv_pct(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_hor(body, 16, 0);
    lv_obj_set_style_pad_ver(body, 12, 0);
    lv_obj_set_style_pad_column(body, 14, 0);

    lv_obj_t * status = lv_obj_create(body);
    flat(status);
    lv_obj_set_style_bg_color(status, lv_color_hex(COL_CARD), 0);
    lv_obj_set_style_bg_opa(status, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(status, 16, 0);
    lv_obj_set_style_border_width(status, 1, 0);
    lv_obj_set_style_border_color(status, lv_color_hex(COL_LINE), 0);
    lv_obj_set_style_border_opa(status, LV_OPA_COVER, 0);
    lv_obj_set_height(status, lv_pct(100));
    lv_obj_set_flex_grow(status, 6);
    lv_obj_set_flex_flow(status, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(status, 20, 0);
    lv_obj_set_style_pad_row(status, 0, 0);
    lv_obj_set_clickable(status, true);
    lv_obj_add_event_cb(status, panel_clicked, LV_EVENT_CLICKED, NULL);

    status_page = page_column(status);
    lv_obj_t * kicker = lv_label_create(status_page);
    lv_label_set_text(kicker, "SYSTEM STATUS");
    lv_obj_set_style_text_font(kicker, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(kicker, lv_color_hex(COL_MUTED), 0);
    lv_obj_set_style_text_letter_space(kicker, 1, 0);

    lv_obj_t * hero = hrow(status_page, 16);
    lv_obj_set_style_margin_top(hero, 14, 0);
    make_ring(hero, 78, 3, lv_color_hex(COL_BLUE), LV_SYMBOL_OK, &lv_font_montserrat_32);

    lv_obj_t * words = lv_obj_create(hero);
    flat(words);
    lv_obj_set_size(words, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(words, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(words, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(words, 2, 0);
    lv_obj_t * idle = lv_label_create(words);
    lv_label_set_text(idle, "IDLE");
    lv_obj_set_style_text_font(idle, &lv_font_montserrat_44, 0);
    lv_obj_t * idle_sub = lv_label_create(words);
    lv_label_set_text(idle_sub, "System ready.");
    lv_obj_set_style_text_font(idle_sub, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(idle_sub, lv_color_hex(COL_MUTED), 0);

    lv_obj_t * spacer = lv_obj_create(status_page);
    flat(spacer);
    lv_obj_set_width(spacer, lv_pct(100));
    lv_obj_set_flex_grow(spacer, 1);

    lv_obj_t * rule = lv_obj_create(status_page);
    flat(rule);
    lv_obj_set_width(rule, lv_pct(100));
    lv_obj_set_height(rule, 1);
    lv_obj_set_style_bg_color(rule, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(rule, LV_OPA_20, 0);
    lv_obj_set_style_margin_bottom(rule, 16, 0);

    add_info_row(status_page, 0, "Instrument:", "MP-S", NULL);
    lv_obj_t * info_gap = lv_obj_create(status_page);
    flat(info_gap);
    lv_obj_set_height(info_gap, 12);
    lv_obj_set_width(info_gap, lv_pct(100));
    add_info_row(status_page, 1, "Available ports:", "4", "");
    lv_obj_set_style_margin_bottom(lv_obj_get_child(status_page, -1), 4, 0);

    test_page = page_column(status);
    build_test_page(test_page);
    show_panel_page(false);

    static const char * numbers[] = {"1", "2", "3", "4"};
    static const char * names[] = {"TPS", "MTPS", "THW", "Empty"};
    static const char * statuses[] = {"Active", "Monitoring", "", ""};
    static const uint32_t status_colors[] = {COL_YELLOW, COL_BLUE, COL_TEXT, COL_TEXT};

    lv_obj_t * ports = lv_obj_create(body);
    flat(ports);
    lv_obj_set_height(ports, lv_pct(100));
    lv_obj_set_flex_grow(ports, 4);
    lv_obj_set_flex_flow(ports, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(ports, 12, 0);

    for(int i = 0; i < 4; i++) {
        lv_obj_t * card = lv_obj_create(ports);
        flat(card);
        lv_obj_set_width(card, lv_pct(100));
        lv_obj_set_flex_grow(card, 1);
        lv_obj_set_flex_flow(card, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_hor(card, 18, 0);
        lv_obj_set_style_pad_column(card, 16, 0);
        lv_obj_set_clickable(card, true);
        lv_obj_add_event_cb(card, port_clicked, LV_EVENT_CLICKED, NULL);
        if(i == 3) lv_obj_add_event_cb(card, dashed_border_cb, LV_EVENT_DRAW_POST, NULL);
        port_cards[i] = card;

        lv_obj_t * num = lv_label_create(card);
        lv_label_set_text(num, numbers[i]);
        lv_obj_set_width(num, 20);
        lv_obj_set_style_text_font(num, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_align(num, LV_TEXT_ALIGN_LEFT, 0);
        port_num[i] = num;

        lv_obj_t * name = lv_label_create(card);
        lv_label_set_text(name, names[i]);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_28, 0);
        lv_obj_set_flex_grow(name, 1);
        port_name[i] = name;

        lv_obj_t * status = lv_label_create(card);
        lv_label_set_text(status, statuses[i]);
        lv_obj_set_style_text_font(status, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(status, lv_color_hex(status_colors[i]), 0);
    }
    select_port(0);
}
