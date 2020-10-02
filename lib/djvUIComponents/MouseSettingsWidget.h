// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides a mouse settings widget.
            class MouseWidget : public IWidget
            {
                DJV_NON_COPYABLE(MouseWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                MouseWidget();

            public:
                static std::shared_ptr<MouseWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

                void setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>&) override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

