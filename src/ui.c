#include "ui.h"
#include "lvgl/lvgl.h"

#define COL_BG     0x10243C
#define COL_CARD   0x1B3356
#define COL_EDGE   0x2E5684
#define COL_TEXT   0xFFFFFF
#define COL_MUTED  0x93A8BE
#define COL_DIM    0xC5D3E2
#define COL_GREEN  0x62E06A
#define COL_BLUE   0x3CA8F0
#define COL_GOLD   0xE0D010
#define COL_ICON   0xE4EEF7

static lv_obj_t * port_cards[4];

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

static lv_obj_t * make_ring(lv_obj_t * parent, int size, int border, lv_color_t color,
                            const char * text, const lv_font_t * font)
{
    lv_obj_t * ring = lv_obj_create(parent);
    flat(ring);
    lv_obj_set_size(ring, size, size);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    stroke(ring, color, border);

    lv_obj_t * label = lv_label_create(ring);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_center(label);
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
    lv_obj_t * box = icon_box(parent, 18, 28);
    lv_obj_t * stem = lv_obj_create(box);
    flat(stem);
    lv_obj_set_size(stem, 8, 18);
    lv_obj_align(stem, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(stem, LV_RADIUS_CIRCLE, 0);
    stroke(stem, color, 2);

    lv_obj_t * bulb = lv_obj_create(box);
    flat(bulb);
    lv_obj_set_size(bulb, 14, 14);
    lv_obj_align(bulb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(bulb, LV_RADIUS_CIRCLE, 0);
    stroke(bulb, color, 2);

    lv_obj_t * mercury = lv_obj_create(box);
    flat(mercury);
    lv_obj_set_size(mercury, 4, 12);
    lv_obj_align(mercury, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_bg_color(mercury, color, 0);
    lv_obj_set_style_bg_opa(mercury, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(mercury, LV_RADIUS_CIRCLE, 0);
    return box;
}

static lv_obj_t * make_waves(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 32, 30);
    for(int i = 0; i < 3; i++) {
        lv_obj_t * arc = lv_arc_create(box);
        lv_obj_set_size(arc, 28, 14);
        lv_obj_set_pos(arc, 2, i * 9);
        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_obj_set_clickable(arc, false);
        lv_obj_set_style_pad_all(arc, 0, 0);
        lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(arc, 0, 0);
        lv_obj_set_style_arc_width(arc, 2, LV_PART_MAIN);
        lv_obj_set_style_arc_rounded(arc, true, LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc, color, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_arc_set_bg_angles(arc, 15, 165);
        lv_arc_set_angles(arc, 0, 0);
    }
    return box;
}

static lv_obj_t * make_probe(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 16, 34);
    lv_obj_t * head = lv_obj_create(box);
    flat(head);
    lv_obj_set_size(head, 12, 6);
    lv_obj_align(head, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(head, 2, 0);
    stroke(head, color, 2);

    lv_obj_t * shaft = lv_obj_create(box);
    flat(shaft);
    lv_obj_set_size(shaft, 4, 20);
    lv_obj_align(shaft, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_radius(shaft, LV_RADIUS_CIRCLE, 0);
    stroke(shaft, color, 2);

    lv_obj_t * tip = lv_obj_create(box);
    flat(tip);
    lv_obj_set_size(tip, 8, 8);
    lv_obj_align(tip, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(tip, LV_RADIUS_CIRCLE, 0);
    stroke(tip, color, 2);
    return box;
}

static lv_obj_t * make_empty(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * ring = make_ring(parent, 30, 2, color, "", &lv_font_montserrat_14);
    lv_obj_t * bar = lv_obj_create(ring);
    flat(bar);
    lv_obj_set_size(bar, 12, 2);
    lv_obj_set_style_bg_color(bar, color, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, 1, 0);
    lv_obj_center(bar);
    return ring;
}

static lv_obj_t * make_device(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 26, 22);
    lv_obj_t * screen = lv_obj_create(box);
    flat(screen);
    lv_obj_set_size(screen, 24, 14);
    lv_obj_align(screen, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(screen, 2, 0);
    stroke(screen, color, 2);

    lv_obj_t * stand = lv_obj_create(box);
    flat(stand);
    lv_obj_set_size(stand, 12, 2);
    lv_obj_align(stand, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(stand, color, 0);
    lv_obj_set_style_bg_opa(stand, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(stand, 1, 0);
    return box;
}

static lv_obj_t * make_nodes(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 26, 22);
    static const int pos[3][2] = {{9, 0}, {0, 13}, {17, 13}};
    for(int i = 0; i < 3; i++) {
        lv_obj_t * node = lv_obj_create(box);
        flat(node);
        lv_obj_set_size(node, 8, 8);
        lv_obj_set_pos(node, pos[i][0], pos[i][1]);
        lv_obj_set_style_radius(node, 2, 0);
        stroke(node, color, 2);
    }
    return box;
}

static lv_obj_t * make_bars(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * box = icon_box(parent, 22, 18);
    static const int heights[3] = {8, 16, 11};
    for(int i = 0; i < 3; i++) {
        lv_obj_t * bar = lv_obj_create(box);
        flat(bar);
        lv_obj_set_size(bar, 4, heights[i]);
        lv_obj_align(bar, LV_ALIGN_BOTTOM_LEFT, i * 7, 0);
        lv_obj_set_style_bg_color(bar, color, 0);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(bar, 1, 0);
    }
    return box;
}

static void card_chrome(lv_obj_t * card, bool selected)
{
    lv_obj_set_style_bg_color(card, lv_color_hex(COL_CARD), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_border_width(card, 3, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(selected ? COL_GOLD : COL_EDGE), 0);
    lv_obj_set_style_border_opa(card, LV_OPA_COVER, 0);
}

static void select_port(int index)
{
    for(int i = 0; i < 4; i++) card_chrome(port_cards[i], i == index);
}

static void port_clicked(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    for(int i = 0; i < 4; i++) {
        if(port_cards[i] == target) select_port(i);
    }
}

static lv_obj_t * port_icon(lv_obj_t * parent, int kind, lv_color_t color)
{
    lv_obj_t * slot = icon_box(parent, 40, 36);
    lv_obj_t * icon = NULL;
    if(kind == 0) icon = make_thermometer(slot, color);
    else if(kind == 1) icon = make_waves(slot, color);
    else if(kind == 2) icon = make_probe(slot, color);
    else icon = make_empty(slot, color);
    lv_obj_center(icon);
    return slot;
}

static void add_info_row(lv_obj_t * parent, int kind, const char * name, const char * value)
{
    lv_obj_t * line = lv_obj_create(parent);
    flat(line);
    lv_obj_set_width(line, lv_pct(100));
    lv_obj_set_height(line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(line, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(line, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(line, 12, 0);

    if(kind == 0) make_device(line, lv_color_hex(COL_DIM));
    else make_nodes(line, lv_color_hex(COL_DIM));

    lv_obj_t * label = lv_label_create(line);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_MUTED), 0);

    lv_obj_t * val = lv_label_create(line);
    lv_label_set_text(val, value);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(val, lv_color_hex(COL_TEXT), 0);
    lv_obj_set_style_margin_left(val, 18, 0);
}

static lv_obj_t * nav_item(lv_obj_t * parent, int kind, const char * text)
{
    lv_color_t color = lv_color_hex(COL_DIM);
    lv_obj_t * col = lv_obj_create(parent);
    flat(col);
    lv_obj_set_size(col, 88, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(col, 4, 0);

    if(kind == 0) {
        lv_obj_t * gear = lv_label_create(col);
        lv_label_set_text(gear, LV_SYMBOL_SETTINGS);
        lv_obj_set_style_text_font(gear, &lv_font_montserrat_22, 0);
        lv_obj_set_style_text_color(gear, color, 0);
    }
    else if(kind == 1) {
        make_ring(col, 24, 2, color, "i", &lv_font_montserrat_14);
    }
    else {
        make_bars(col, color);
    }

    lv_obj_t * label = lv_label_create(col);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, color, 0);
    return col;
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
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(header, lv_color_hex(COL_CARD), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_style_pad_ver(header, 10, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header, 2, 0);
    lv_obj_set_style_border_color(header, lv_color_hex(COL_EDGE), 0);
    lv_obj_set_style_border_opa(header, LV_OPA_COVER, 0);

    #define HEADER_FONT &lv_font_montserrat_20
    lv_obj_t * brand = hrow(header, 8);
    make_logo(brand);
    lv_obj_t * brand_name = lv_label_create(brand);
    lv_label_set_text(brand_name, "Thermtest");
    lv_obj_set_style_text_font(brand_name, HEADER_FONT, 0);
    lv_obj_t * model = lv_label_create(brand);
    lv_label_set_text(model, "MP-1");
    lv_obj_set_style_text_font(model, HEADER_FONT, 0);
    lv_obj_set_style_text_color(model, lv_color_hex(COL_DIM), 0);
    lv_obj_set_style_margin_left(model, 14, 0);

    lv_obj_t * ready = hrow(header, 8);
    make_ring(ready, 22, 2, lv_color_hex(COL_GREEN), LV_SYMBOL_OK, &lv_font_montserrat_12);
    lv_obj_t * ready_label = lv_label_create(ready);
    lv_label_set_text(ready_label, "System Ready");
    lv_obj_set_style_text_font(ready_label, HEADER_FONT, 0);

    lv_obj_t * temp = hrow(header, 8);
    make_thermometer(temp, lv_color_hex(COL_ICON));
    lv_obj_t * temp_label = lv_label_create(temp);
    lv_label_set_text(temp_label, "23.4 \xC2\xB0""C");
    lv_obj_set_style_text_font(temp_label, HEADER_FONT, 0);

    lv_obj_t * clock = hrow(header, 12);
    lv_obj_t * usb = lv_label_create(clock);
    lv_label_set_text(usb, LV_SYMBOL_USB);
    lv_obj_set_style_text_font(usb, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(usb, lv_color_hex(COL_DIM), 0);
    lv_obj_t * time_label = lv_label_create(clock);
    lv_label_set_text(time_label, "10:27 AM");
    lv_obj_set_style_text_font(time_label, HEADER_FONT, 0);

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
    card_chrome(status, false);
    lv_obj_set_height(status, lv_pct(100));
    lv_obj_set_flex_grow(status, 8);
    lv_obj_set_flex_flow(status, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(status, 22, 0);
    lv_obj_set_style_pad_row(status, 8, 0);

    lv_obj_t * kicker = lv_label_create(status);
    lv_label_set_text(kicker, "SYSTEM STATUS");
    lv_obj_set_style_text_font(kicker, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(kicker, lv_color_hex(COL_MUTED), 0);
    lv_obj_set_style_text_letter_space(kicker, 1, 0);

    lv_obj_t * hero = hrow(status, 18);
    lv_obj_set_style_margin_top(hero, 8, 0);
    make_ring(hero, 78, 3, lv_color_hex(COL_BLUE), LV_SYMBOL_OK, &lv_font_montserrat_32);

    lv_obj_t * words = lv_obj_create(hero);
    flat(words);
    lv_obj_set_size(words, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(words, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(words, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(words, 2, 0);
    lv_obj_t * idle = lv_label_create(words);
    lv_label_set_text(idle, "IDLE");
    lv_obj_set_style_text_font(idle, &lv_font_montserrat_48, 0);
    lv_obj_t * idle_sub = lv_label_create(words);
    lv_label_set_text(idle_sub, "System ready.");
    lv_obj_set_style_text_font(idle_sub, &lv_font_montserrat_30, 0);
    lv_obj_set_style_text_color(idle_sub, lv_color_hex(COL_MUTED), 0);

    lv_obj_t * spacer = lv_obj_create(status);
    flat(spacer);
    lv_obj_set_width(spacer, lv_pct(100));
    lv_obj_set_flex_grow(spacer, 1);

    lv_obj_t * rule = lv_obj_create(status);
    flat(rule);
    lv_obj_set_width(rule, lv_pct(100));
    lv_obj_set_height(rule, 1);
    lv_obj_set_style_bg_color(rule, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(rule, LV_OPA_20, 0);
    lv_obj_set_style_margin_bottom(rule, 16, 0);

    add_info_row(status, 0, "Instrument:", "MP-1");
    lv_obj_t * info_gap = lv_obj_create(status);
    flat(info_gap);
    lv_obj_set_height(info_gap, 14);
    lv_obj_set_width(info_gap, lv_pct(100));
    add_info_row(status, 1, "Available ports:", "4");
    lv_obj_set_style_margin_bottom(lv_obj_get_child(status, -1), 6, 0);

    static const char * names[] = {"Port 1", "Port 2", "Port 3", "Port 4"};
    static const char * kinds[] = {"RTSC", "TPS-TP", "TLS-100", "Empty"};

    lv_obj_t * ports = lv_obj_create(body);
    flat(ports);
    lv_obj_set_height(ports, lv_pct(100));
    lv_obj_set_flex_grow(ports, 4);
    lv_obj_set_flex_flow(ports, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(ports, 10, 0);

    for(int i = 0; i < 4; i++) {
        lv_obj_t * card = lv_obj_create(ports);
        flat(card);
        card_chrome(card, i == 0);
        lv_obj_set_width(card, lv_pct(100));
        lv_obj_set_flex_grow(card, 1);
        lv_obj_set_flex_flow(card, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_hor(card, 14, 0);
        lv_obj_set_style_pad_column(card, 8, 0);
        lv_obj_set_clickable(card, true);
        lv_obj_add_event_cb(card, port_clicked, LV_EVENT_CLICKED, NULL);
        port_cards[i] = card;

        port_icon(card, i, lv_color_hex(COL_ICON));

        lv_obj_t * text = lv_obj_create(card);
        flat(text);
        lv_obj_set_height(text, LV_SIZE_CONTENT);
        lv_obj_set_flex_grow(text, 1);
        lv_obj_set_flex_flow(text, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(text, 2, 0);

        lv_obj_t * title = lv_label_create(text);
        lv_label_set_text(title, names[i]);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);

        lv_obj_t * kind = lv_label_create(text);
        lv_label_set_text(kind, kinds[i]);
        lv_obj_set_style_text_font(kind, &lv_font_montserrat_22, 0);
        lv_obj_set_style_text_color(kind, lv_color_hex(COL_MUTED), 0);

        lv_obj_t * chevron = lv_label_create(card);
        lv_label_set_text(chevron, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_font(chevron, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(chevron, lv_color_hex(COL_DIM), 0);
    }

    lv_obj_t * footer = lv_obj_create(root);
    flat(footer);
    lv_obj_set_width(footer, lv_pct(100));
    lv_obj_set_height(footer, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(footer, lv_color_hex(COL_CARD), 0);
    lv_obj_set_style_bg_opa(footer, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(footer, 16, 0);
    lv_obj_set_style_pad_ver(footer, 8, 0);
    lv_obj_set_style_border_side(footer, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_width(footer, 2, 0);
    lv_obj_set_style_border_color(footer, lv_color_hex(COL_EDGE), 0);
    lv_obj_set_style_border_opa(footer, LV_OPA_COVER, 0);

    nav_item(footer, 0, "Settings");
    nav_item(footer, 1, "Info");
    nav_item(footer, 2, "Data");
}
