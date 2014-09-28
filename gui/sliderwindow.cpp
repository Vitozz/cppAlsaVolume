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
		set_default_size(volumeSlider_->get_width(), volumeSlider_->get_width());
	}
	set_events(Gdk::LEAVE_NOTIFY_MASK);
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &SliderWindow::on_focus_out));
	set_border_width(0);
	set_keep_above(true);
}

SliderWindow::~SliderWindow()
{
	delete volumeSlider_;
}

void SliderWindow::setWindowPosition(const iconPosition &pos)
{
	if (!get_visible()) {
		const int wWidth = volumeSlider_->get_allocated_width();
		const int wHeight = volumeSlider_->get_allocated_height();
		const int wY = pos.trayAtTop_ ? pos.iconHeight_ + 4 : pos.screenHeight_ - wHeight - pos.iconHeight_ - 4;
		int wX;
		if (pos.geometryAvailable_) {
			wX = (wWidth > 1) ? pos.iconX_ - (wWidth/2 - pos.iconWidth_/2) : pos.iconX_ - pos.iconWidth_/2;
		}
		else{
			wX = (wWidth > 1) ? pos.iconX_ - wWidth/2 : pos.iconX_ - pos.iconWidth_;
		}
		show_all();
		this->move(wX, wY);
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
