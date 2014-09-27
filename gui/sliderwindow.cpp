/*
 * sliderwindow.cpp
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "sliderwindow.h"
#include <iostream>

SliderWindow::SliderWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject)
{
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
	volumeSlider_ = 0;
	volumeValue_ = 0;
	builder->get_widget("volume_slider", volumeSlider_);
	if (volumeSlider_) {
		volumeSlider_->signal_value_changed().connect(sigc::mem_fun(*this, &SliderWindow::on_volume_slider));
	}
	set_events(Gdk::LEAVE_NOTIFY_MASK);
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &SliderWindow::on_focus_out));
	set_keep_above(true);
}

SliderWindow::~SliderWindow()
{
	delete volumeSlider_;
}

void SliderWindow::setWindowPosition(int x_, int y_, int height_, int width_)
{
	if (!get_visible()) {
		int wX = 0;
		int wY = 0;
		const int wWidth = get_width();
		const int wHeight = get_height();
		if (y_ <= 200) { //check tray up/down position
			wY = y_ + height_ + 2;
		}
		else {
			wY = y_ - wHeight - 4;
		}
		if (wWidth > 1) {//at first run window widht = 1
			wX = (x_ + width_/2) - wWidth/2;
		}
		else {
			wX = x_;
		}
		move(wX,wY);
		show_all();
	}
	else {
		hide();
	}
}

void SliderWindow::on_volume_slider()
{
	volumeValue_ = volumeSlider_->get_value();
	m_signal_volume_changed(volumeValue_);
}

bool SliderWindow::on_focus_out(GdkEventCrossing* event)
{
	if ((event->type |(GDK_LEAVE_NOTIFY == 0))
			&& (event->x < 0
			  ||event->x >= get_width()
			  ||event->y < 0
			  ||event->y >= get_height())){
		hide();
	}
	return false;
}

void SliderWindow::setVolumeValue(double value)
{
	volumeValue_ = value;
	volumeSlider_->set_value(value);
}

SliderWindow::type_sliderwindow_signal SliderWindow::signal_volume_changed()
{
	return m_signal_volume_changed;
}
