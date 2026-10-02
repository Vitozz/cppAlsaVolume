/*
 * sliderwindow.cpp
 * Copyright (C) 2012-2026 Vitaly Tonkacheyev
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

SliderWindow::SliderWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refGlade) :
    Gtk::Window(cobject), volumeSlider_(nullptr)
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
    sliderWidth     = volumeSlider_->get_allocated_width();
    if (sliderWidth < SLIDER_MIN_WIDTH) {
        set_size_request(SLIDER_MIN_WIDTH, SLIDER_HEIGHT);
    }
    set_keep_above(true);
}

SliderWindow::~SliderWindow() { delete volumeSlider_; }

void SliderWindow::setWindowPosition(const iconPosition &pos)
{
    if (!get_visible()) {
        show_all();
        const int wWidth  = this->get_allocated_width();
        const int wHeight = this->get_allocated_height();

        int       wX     = 0;
        int       wY     = 0;
        const int offset = 6;

        // get icon position according to screen edges
        bool isTop    = pos.iconY_ < pos.screenHeight_ / 4;
        bool isBottom = pos.iconY_ > (pos.screenHeight_ * 3 / 4);
        bool isLeft   = pos.iconX_ < pos.screenWidth_ / 4;
        bool isRight  = pos.iconX_ > (pos.screenWidth_ * 3 / 4);

        if (!isTop && !isBottom && !isLeft && !isRight) {
            isBottom = !pos.trayAtTop_;
            isTop    = pos.trayAtTop_;
        }

#ifdef IS_DEBUG
        std::cout << "Screen height = " << pos.screenHeight_ << std::endl;
        std::cout << "wHeight = " << wHeight << std::endl;
        std::cout << "wWidth = " << wWidth << std::endl;
        std::cout << "iconHeight = " << pos.iconHeight_ << std::endl;
        std::cout << "iconWidth = " << pos.iconWidth_ << std::endl;
        std::cout << "iconX = " << pos.iconX_ << std::endl;
        std::cout << "iconY = " << pos.iconY_ << std::endl;
        std::cout << "Geom = " << pos.geometryAvailable_ << std::endl;
#endif

        if (isTop) {
            // TOP panel:
            // If icon geometry exists, position the window directly below the icon (or below the entire panel).
            // The key point is that wY starts below the bottom edge of the icon/panel.
            int panelBottomEdge = pos.geometryAvailable_ ? (pos.iconY_ + pos.iconHeight_) : (pos.iconY_);
            wY                  = panelBottomEdge + offset;

            // Center horizontally relative to the icon/click, but prevent it from going off-screen
            int anchorX = pos.geometryAvailable_ ? (pos.iconX_ + pos.iconWidth_ / 2) : pos.iconX_;
            wX          = anchorX - (wWidth / 2);
#ifdef IS_DEBUG
            std::cout << "Panel at top, wY=" << wY << " wX=" << wX << std::endl;
#endif
        } else if (isBottom) {
            // Panel at the BOTTOM:
            // The window must be positioned ABOVE the panel / icon.
            // Window's top edge = icon's top edge (or panel's top edge) minus window height.
            int panelTopEdge = pos.geometryAvailable_ ? pos.iconY_ : pos.iconY_;
            // If geometryAvailable_ is false, pos.iconY_ is the mouse click Y-coordinate (top of the panel minus a
            // couple of pixels)
            wY = panelTopEdge - wHeight - offset;

            int anchorX = pos.geometryAvailable_ ? (pos.iconX_ + pos.iconWidth_ / 2) : pos.iconX_;
            wX          = anchorX - (wWidth / 2);
#ifdef IS_DEBUG
            std::cout << "Panel at bottom, wY=" << wY << " wX=" << wX << std::endl;
#endif
        } else if (isLeft) {
            // LEFT panel: window to the right of the panel/icon
            int panelRightEdge = pos.geometryAvailable_ ? (pos.iconX_ + pos.iconWidth_) : pos.iconX_;
            wX                 = panelRightEdge + offset;

            int anchorY = pos.geometryAvailable_ ? (pos.iconY_ + pos.iconHeight_ / 2) : pos.iconY_;
            wY          = anchorY - (wHeight / 2);
#ifdef IS_DEBUG
            std::cout << "Panel at left, wY=" << wY << " wX=" << wX << std::endl;
#endif
        } else if (isRight) {
            // RIGHT panel: window to the left of the panel/icon
            int panelLeftEdge = pos.geometryAvailable_ ? pos.iconX_ : pos.iconX_;
            wX                = panelLeftEdge - wWidth - offset;

            int anchorY = pos.geometryAvailable_ ? (pos.iconY_ + pos.iconHeight_ / 2) : pos.iconY_;
            wY          = anchorY - (wHeight / 2);
#ifdef IS_DEBUG
            std::cout << "Panel at right, wY=" << wY << " wX=" << wX << std::endl;
#endif
        } else {
            // Default (bottom of screen)
            wX = pos.iconX_ - (wWidth / 2);
            wY = pos.screenHeight_ - wHeight - 50;
        }

        // --- Smart constraint (to prevent the window from sticking tightly to screen edges or going off-screen) ---
        if (wX < offset) {
            wX = offset;
        }
        if (pos.screenWidth_ > 0 && wX + wWidth > pos.screenWidth_ - offset) {
            wX = pos.screenWidth_ - wWidth - offset;
        }
        if (wY < offset) {
            wY = offset;
        }
        if (pos.screenHeight_ > 0 && wY + wHeight > pos.screenHeight_ - offset) {
            wY = pos.screenHeight_ - wHeight - offset;
        }

#ifdef IS_DEBUG
        std::cout << "Final wX = " << wX << ", wY = " << wY << std::endl;
#endif

        this->move(wX, wY);
    } else {
        hide();
    }
}

void SliderWindow::on_volume_slider()
{
    volumeValue_ = volumeSlider_->get_value();
    m_signal_volume_changed(volumeValue_);
}

bool SliderWindow::on_focus_out(GdkEventCrossing *event)
{
    if ((event->type == GDK_LEAVE_NOTIFY)
        && (event->x < 0 || event->x >= get_width() || event->y < 0 || event->y >= get_height())) {
        hide();
    }
    return false;
}

void SliderWindow::setVolumeValue(double value)
{
    volumeValue_ = value;
    volumeSlider_->set_value(value);
}

SliderWindow::type_sliderwindow_signal SliderWindow::signal_volume_changed() { return m_signal_volume_changed; }
