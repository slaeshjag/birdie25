/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 *
 * This file is part of Muon.
 *
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui.h"

UI_WIDGET *ui_widget_create_button(UI_WIDGET *child) {
	UI_WIDGET *widget;
	if((widget = malloc(sizeof(UI_WIDGET))) == NULL)
		return NULL;
	if((widget->properties = malloc(sizeof(struct UI_BUTTON_PROPERTIES))) == NULL) {
		free(widget);
		return NULL;
	}
	if((widget->event_handler = malloc(sizeof(UI_EVENT_HANDLER))) == NULL) {
		free(widget->properties);
		free(widget);
		return NULL;
	}
	widget->event_handler->handlers = NULL;
	widget->event_handler->add = ui_event_add;
	widget->event_handler->remove = ui_event_remove;
	widget->event_handler->send = ui_event_send;
	widget->event_handler->add(widget, ui_button_event_click, UI_EVENT_TYPE_MOUSE);
	widget->event_handler->add(widget, ui_button_event_key, UI_EVENT_TYPE_KEYBOARD);

	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	p->child = child;
	p->activated = 0;
	p->border = d_render_line_new(8, 1);
	p->active_border = d_render_line_new(4, 1);
	widget->destroy = ui_widget_destroy_button;
	widget->set_prop = ui_button_set_prop;
	widget->get_prop = ui_button_get_prop;
	widget->resize = ui_button_resize;
	widget->request_size = ui_button_request_size;
	widget->render = ui_button_render;

	widget->x = widget->y = widget->w = widget->h = 0;
	widget->enabled = 1;

	return widget;
}

UI_WIDGET *ui_widget_create_button_text(DARNIT_FONT *font, const char *text) {
	UI_WIDGET *widget, *label;
	label = ui_widget_create_label(font, text);
	widget = ui_widget_create_button(label);
	widget->destroy = ui_widget_destroy_button_recursive;
	return widget;
}

UI_WIDGET *ui_widget_create_button_image() {
	return NULL;
}

void *ui_widget_destroy_button(UI_WIDGET *widget) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	d_render_line_free(p->border);
	d_render_line_free(p->active_border);
	return ui_widget_destroy(widget);
}

void *ui_widget_destroy_button_recursive(UI_WIDGET *widget) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	p->child->destroy(p->child);
	return ui_widget_destroy_button(widget);
}

void ui_button_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(!widget->enabled)
		return;
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	if(e->keyboard->keysym != KEY(RETURN) && e->keyboard->keysym != KEY(KP_ENTER) && e->keyboard->keysym != KEY(SPACE))
		return;
	switch(type) {
		case UI_EVENT_TYPE_KEYBOARD_PRESS:
			p->activated = 1;
			break;
		case UI_EVENT_TYPE_KEYBOARD_RELEASE:
			p->activated = 0;
			break;
	}
}

void ui_button_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(!widget->enabled)
		return;
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	switch(type) {
		case UI_EVENT_TYPE_MOUSE_PRESS:
			if(e->mouse->buttons == UI_EVENT_MOUSE_BUTTON_LEFT)
				ui_selected_widget = widget;
			p->activated = 1;
			break;
		case UI_EVENT_TYPE_MOUSE_RELEASE:
			if(((e->mouse->buttons)&UI_EVENT_MOUSE_BUTTON_LEFT) == UI_EVENT_MOUSE_BUTTON_LEFT || !p->activated)
				break;
			UI_EVENT ee;
			/*UI_EVENT_UI e_u={};
			ee.ui=&e_u;*/
			ee.mouse = &ui_e_m_prev;
			widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, &ee);
		case UI_EVENT_TYPE_MOUSE_LEAVE:
			p->activated = 0;
			break;
	}
}

void ui_button_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	UI_WIDGET *old_child;
	switch(prop) {
		case UI_BUTTON_PROP_CHILD:
			old_child = p->child;
			p->child = value.p;
			p->child->resize(p->child, old_child->x, old_child->y, old_child->w, old_child->h);
			if(widget->destroy == ui_widget_destroy_button_recursive) {
				old_child->destroy(old_child);
				widget->destroy = ui_widget_destroy_button;
			}
			break;
		case UI_BUTTON_PROP_ACTIVATED:
			p->activated = value.i;
			break;
		case UI_BUTTON_PROP_BORDER:
			break;
		case UI_BUTTON_PROP_ACTIVE_BORDER:
			break;
	}
}

UI_PROPERTY_VALUE ui_button_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	UI_PROPERTY_VALUE v = {.p = NULL};
	switch(prop) {
		case UI_BUTTON_PROP_CHILD:
			v.p = p->child;
			break;
		case UI_BUTTON_PROP_ACTIVATED:
			v.i = p->activated;
			break;
		case UI_BUTTON_PROP_BORDER:
			v.p = p->border;
			break;
		case UI_BUTTON_PROP_ACTIVE_BORDER:
			v.p = p->active_border;
			break;
	}
	return v;
}

void ui_button_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	widget->x = x;
	widget->y = y;
	widget->w = w;
	widget->h = h;
	p->child->resize(p->child, x + 2 + ui_padding, y + 2 + ui_padding, w - 4 - ui_padding * 2, h - 4 - ui_padding * 2);

	d_render_line_move(p->border, 0, x, y, x + w - 1, y);
	d_render_line_move(p->border, 1, x, y + h, x + w - 1, y + h);
	d_render_line_move(p->border, 2, x, y, x, y + h - 1);
	d_render_line_move(p->border, 3, x + w, y, x + w, y + h - 1);
	d_render_line_move(p->border, 4, x + 2, y + 2, x + w - 3, y + 2);
	d_render_line_move(p->border, 5, x + 2, y + h - 2, x + w - 3, y + h - 2);
	d_render_line_move(p->border, 6, x + 2, y + 2, x + 2, y + h - 3);
	d_render_line_move(p->border, 7, x + w - 2, y + 2, x + w - 2, y + h - 3);

	d_render_line_move(p->active_border, 0, x, y + 1, x + w - 1, y + 1);
	d_render_line_move(p->active_border, 1, x, y + h - 1, x + w - 1, y + h - 1);
	d_render_line_move(p->active_border, 2, x + 1, y, x + 1, y + h - 1);
	d_render_line_move(p->active_border, 3, x + w - 1, y, x + w - 1, y + h - 1);
}

void ui_button_request_size(UI_WIDGET *widget, int *w, int *h) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	p->child->request_size(p->child, w, h);
	(*w) += 8 + ui_padding * 2;
	(*h) += 8 + ui_padding * 2;
}

void ui_button_render(UI_WIDGET *widget) {
	struct UI_BUTTON_PROPERTIES *p = widget->properties;
	p->child->render(p->child);
	d_render_line_draw(p->border, 8);
	if(p->activated)
		d_render_line_draw(p->active_border, 4);
}
