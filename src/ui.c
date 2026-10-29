#include "ui.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * chart;
    lv_chart_series_t * series;
    lv_obj_t * arc;
    lv_obj_t * arc_value;
    lv_obj_t * bar;
    lv_obj_t * readout;
    lv_obj_t * slider;
    lv_obj_t * run_switch;
    int32_t sample;
} ui_state_t;

static ui_state_t ui;

static void apply_level(int32_t value)
{
    if(value < 0) value = 0;
    if(value > 100) value = 100;

    lv_arc_set_value(ui.arc, value);
    lv_bar_set_value(ui.bar, value, LV_ANIM_OFF);
    lv_label_set_text_fmt(ui.arc_value, "%d", (int)value);
    lv_label_set_text_fmt(ui.readout, "Signal   %d", (int)value);
}

static void tick_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    if(!lv_obj_has_state(ui.run_switch, LV_STATE_CHECKED)) return;

    ui.sample = (ui.sample + 4) % 360;
    int32_t center = lv_slider_get_value(ui.slider);
    int32_t wave = (lv_trigo_sin(ui.sample) * 28) / 32767;
    int32_t value = center + wave;

    if(value < 0) value = 0;
    if(value > 100) value = 100;

    lv_chart_set_next_value(ui.chart, ui.series, value);
    apply_level(value);
}

static void slider_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    if(lv_obj_has_state(ui.run_switch, LV_STATE_CHECKED)) return;
    apply_level(lv_slider_get_value(ui.slider));
}

static void reset_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    ui.sample = 0;
    lv_slider_set_value(ui.slider, 50, LV_ANIM_OFF);
    lv_chart_set_all_values(ui.chart, ui.series, 50);
    lv_chart_refresh(ui.chart);
    apply_level(50);
}

static lv_obj_t * make_card(lv_obj_t * parent, uint8_t grow)
{
    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_set_height(card, lv_pct(100));
    lv_obj_set_flex_grow(card, grow);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x1e293b), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_pad_all(card, 16, 0);
    lv_obj_set_style_pad_row(card, 8, 0);
    lv_obj_set_scrollable(card, false);
    return card;
}

static lv_obj_t * section_title(lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(0x94a3b8), 0);
    return label;
}

void ui_init(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x0b1220), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xe2e8f0), 0);

    /* Keep this off the screen itself so the mouse cursor stays out of the layout. */
    lv_obj_t * root = lv_obj_create(screen);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_radius(root, 0, 0);
    lv_obj_set_style_pad_all(root, 16, 0);
    lv_obj_set_style_pad_row(root, 12, 0);
    lv_obj_set_scrollable(root, false);

    lv_obj_t * header = lv_obj_create(root);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, 4, 0);
    lv_obj_set_scrollable(header, false);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "Display test");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    lv_obj_t * size_label = lv_label_create(header);
    lv_label_set_text(size_label, "800 x 480");
    lv_obj_set_style_text_color(size_label, lv_color_hex(0x38bdf8), 0);

    lv_obj_t * body = lv_obj_create(root);
    lv_obj_set_width(body, lv_pct(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_style_pad_all(body, 0, 0);
    lv_obj_set_style_pad_column(body, 12, 0);
    lv_obj_set_scrollable(body, false);

    lv_obj_t * chart_card = make_card(body, 3);
    section_title(chart_card, "WAVEFORM");
    ui.chart = lv_chart_create(chart_card);
    lv_obj_set_width(ui.chart, lv_pct(100));
    lv_obj_set_flex_grow(ui.chart, 1);
    lv_obj_set_style_bg_color(ui.chart, lv_color_hex(0x0f172a), 0);
    lv_obj_set_style_border_width(ui.chart, 0, 0);
    lv_chart_set_type(ui.chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui.chart, 80);
    lv_chart_set_axis_min_value(ui.chart, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(ui.chart, LV_CHART_AXIS_PRIMARY_Y, 100);
    lv_chart_set_div_line_count(ui.chart, 5, 8);
    ui.series = lv_chart_add_series(ui.chart, lv_color_hex(0x38bdf8), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_all_values(ui.chart, ui.series, 50);
    ui.readout = lv_label_create(chart_card);
    lv_label_set_text(ui.readout, "Signal   50");

    lv_obj_t * gauge_card = make_card(body, 2);
    lv_obj_set_flex_align(gauge_card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    section_title(gauge_card, "LEVEL");
    ui.arc = lv_arc_create(gauge_card);
    lv_obj_set_size(ui.arc, 150, 150);
    lv_obj_align(ui.arc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_rotation(ui.arc, 135);
    lv_arc_set_bg_angles(ui.arc, 0, 270);
    lv_arc_set_range(ui.arc, 0, 100);
    lv_arc_set_value(ui.arc, 50);
    lv_obj_remove_style(ui.arc, NULL, LV_PART_KNOB);
    lv_obj_set_clickable(ui.arc, false);
    lv_obj_set_style_arc_color(ui.arc, lv_color_hex(0x334155), LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui.arc, lv_color_hex(0x38bdf8), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui.arc, 14, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui.arc, 14, LV_PART_INDICATOR);
    ui.arc_value = lv_label_create(ui.arc);
    lv_label_set_text(ui.arc_value, "50");
    lv_obj_set_style_text_font(ui.arc_value, &lv_font_montserrat_20, 0);
    lv_obj_center(ui.arc_value);

    ui.bar = lv_bar_create(gauge_card);
    lv_obj_set_width(ui.bar, lv_pct(100));
    lv_obj_set_height(ui.bar, 12);
    lv_bar_set_range(ui.bar, 0, 100);
    lv_bar_set_value(ui.bar, 50, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui.bar, lv_color_hex(0x334155), LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui.bar, lv_color_hex(0x22c55e), LV_PART_INDICATOR);

    lv_obj_t * controls = make_card(body, 2);
    section_title(controls, "CONTROLS");

    lv_obj_t * run_row = lv_obj_create(controls);
    lv_obj_set_width(run_row, lv_pct(100));
    lv_obj_set_height(run_row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(run_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(run_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(run_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(run_row, 0, 0);
    lv_obj_set_style_pad_all(run_row, 0, 0);
    lv_obj_set_scrollable(run_row, false);
    lv_obj_t * run_label = lv_label_create(run_row);
    lv_label_set_text(run_label, "Run");
    ui.run_switch = lv_switch_create(run_row);
    lv_obj_add_state(ui.run_switch, LV_STATE_CHECKED);

    section_title(controls, "CENTER");
    ui.slider = lv_slider_create(controls);
    lv_obj_set_width(ui.slider, lv_pct(100));
    lv_slider_set_range(ui.slider, 0, 100);
    lv_slider_set_value(ui.slider, 50, LV_ANIM_OFF);
    lv_obj_add_event_cb(ui.slider, slider_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * reset = lv_button_create(controls);
    lv_obj_set_width(reset, lv_pct(100));
    lv_obj_set_height(reset, 40);
    lv_obj_add_event_cb(reset, reset_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * reset_label = lv_label_create(reset);
    lv_label_set_text(reset_label, "Reset");
    lv_obj_center(reset_label);

    lv_timer_create(tick_cb, 40, NULL);
}
