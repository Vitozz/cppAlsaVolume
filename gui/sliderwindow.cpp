/*
 * sliderwindow.cpp
 * Copyright (C) 2012-2025 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sliderwindow.h"
#include <iostream>

#define SLIDER_HEIGHT 120
#define SLIDER_MIN_WIDTH 20

SliderWindow::SliderWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
    : Gtk::Window(cobject),
      volumeSlider_(nullptr)
{
    volumeValue_ = 0;
    refGlade->get_widget("volume_slider", volumeSlider_);
    if (volumeSlider_) {
        volumeSlider_->signal_value_changed().connect(sigc::mem_fun(*this, &SliderWindow::on_volume_slider));
        set_default_size(volumeSlider_->get_width(), volumeSlider_->get_width());
    }
    add_events(Gdk::LEAVE_NOTIFY_MASK);
    signal_leave_notify_event().connect(sigc::mem_fun(*this, &SliderWindow::on_focus_out));
    set_border_width(0);

    int sliderWidth = 0;
#ifndef IS_GTK_2
    sliderWidth = volumeSlider_->get_allocated_width();
#else
    sliderWidth = volumeSlider_->get_width();
#endif
    if (sliderWidth < SLIDER_MIN_WIDTH) {
        set_size_request(SLIDER_MIN_WIDTH, SLIDER_HEIGHT);
    }
    set_keep_above(true);
}

SliderWindow::~SliderWindow()
{
    delete volumeSlider_;
}

void SliderWindow::setWindowPosition(const iconPosition &pos)
{
    if (!get_visible()) {
#ifndef IS_GTK_2
        const int wWidth = volumeSlider_->get_allocated_width();
        const int wHeight = volumeSlider_->get_allocated_height();
#else
        const int wWidth = volumeSlider_->get_width();
        const int wHeight = SLIDER_HEIGHT;
#endif
#ifdef IS_DEBUG
        std::cout << "Screen height = " << pos.screenHeight_ << std::endl;
        std::cout << "At top = " << pos.trayAtTop_ << std::endl;
        std::cout << "wHeight = " << wHeight << std::endl;
        std::cout << "iconHeight = " << pos.iconHeight_ << std::endl;
#endif
        const int wY = pos.trayAtTop_ ? pos.iconHeight_ + 4 : pos.screenHeight_ - wHeight - pos.iconHeight_ - 4;
        int wX;
        if (pos.geometryAvailable_) {
#ifdef IS_DEBUG
            std::cout << "Geometry available" << std::endl;
            std::cout << "wY = " << wY << std::endl;
#endif
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
    if ((event->type == GDK_LEAVE_NOTIFY)
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
