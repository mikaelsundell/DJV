// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewSettings.h>

#include <djvViewApp/ViewData.h>

#include <djvMath/BBox.h>
#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewSettings::Private
        {
            std::map<std::string, bool> bellowsState;
            std::shared_ptr<Observer::ValueSubject<ViewLock> > lock;
            std::shared_ptr<Observer::ValueSubject<GridOptions> > gridOptions;
            std::shared_ptr<Observer::ValueSubject<HUDOptions> > hudOptions;
            std::shared_ptr<Observer::ValueSubject<ViewBackgroundOptions> > backgroundOptions;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void ViewSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ViewSettings", context);

            DJV_PRIVATE_PTR();
            p.lock = Observer::ValueSubject<ViewLock>::create(ViewLock::Frame);
            p.gridOptions = Observer::ValueSubject<GridOptions>::create();
            p.hudOptions = Observer::ValueSubject<HUDOptions>::create();
            p.backgroundOptions = Observer::ValueSubject<ViewBackgroundOptions>::create();
            _load();
        }

        ViewSettings::ViewSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ViewSettings> ViewSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewSettings>(new ViewSettings);
            out->_init(context);
            return out;
        }

        const std::map<std::string, bool> ViewSettings::getBellowsState() const
        {
            return _p->bellowsState;
        }

        void ViewSettings::setBellowsState(const std::map<std::string, bool>& value)
        {
            _p->bellowsState = value;
        }

        std::shared_ptr<Observer::IValueSubject<ViewLock> > ViewSettings::observeLock() const
        {
            return _p->lock;
        }

        void ViewSettings::setLock(ViewLock value)
        {
            _p->lock->setIfChanged(value);
        }

        std::shared_ptr<Core::Observer::IValueSubject<GridOptions> > ViewSettings::observeGridOptions() const
        {
            return _p->gridOptions;
        }

        std::shared_ptr<Core::Observer::IValueSubject<HUDOptions> > ViewSettings::observeHUDOptions() const
        {
            return _p->hudOptions;
        }

        std::shared_ptr<Observer::IValueSubject<ViewBackgroundOptions> > ViewSettings::observeBackgroundOptions() const
        {
            return _p->backgroundOptions;
        }

        void ViewSettings::setGridOptions(const GridOptions& value)
        {
            _p->gridOptions->setIfChanged(value);
        }

        void ViewSettings::setHUDOptions(const HUDOptions& value)
        {
            _p->hudOptions->setIfChanged(value);
        }

        void ViewSettings::setBackgroundOptions(const ViewBackgroundOptions& value)
        {
            _p->backgroundOptions->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& ViewSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ViewSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ViewSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("BellowsState", value, p.bellowsState);
                UI::Settings::read("Lock", value, p.lock);
                UI::Settings::read("GridOptions", value, p.gridOptions);
                UI::Settings::read("HUDOptions", value, p.hudOptions);
                UI::Settings::read("BackgroundOptions", value, p.backgroundOptions);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ViewSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("BellowsState", p.bellowsState, out, allocator);
            UI::Settings::write("Lock", p.lock->get(), out, allocator);
            UI::Settings::write("GridOptions", p.gridOptions->get(), out, allocator);
            UI::Settings::write("HUDOptions", p.hudOptions->get(), out, allocator);
            UI::Settings::write("BackgroundOptions", p.backgroundOptions->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

