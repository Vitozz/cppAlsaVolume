/*
 * sliderwindow.h
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
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

#ifndef SLIDERWINDOW_H
#define SLIDERWINDOW_H

#include "gtkmm/button.h"
#include "gtkmm/window.h"
#include "gtkmm/builder.h"
#include "gtkmm/scale.h"
#include "../tools/tools.h"

class SliderWindow : public Gtk::Window
{
public:
    SliderWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>&refGlade);
    ~SliderWindow() override;
    void setWindowPosition(const iconPosition& pos);
    void setVolumeValue(double value);
    //signal
    typedef sigc::signal<void, double> type_sliderwindow_signal;
    type_sliderwindow_signal signal_volume_changed();

private:
    void on_volume_slider();
    bool on_focus_out(GdkEventCrossing* event);
protected:
    type_sliderwindow_signal m_signal_volume_changed;
private:
    Gtk::Scale *volumeSlider_;
    double volumeValue_;
};

#endif // SLIDERWINDOW_H
