//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvViewApp/MDIWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class SubWidget : public UI::MDI::IWidget
            {
                DJV_NON_COPYABLE(SubWidget);

            protected:
                void _init(const std::shared_ptr<Media>& , Context*);
                SubWidget()
                {}

            public:
                static std::shared_ptr<SubWidget> create(const std::shared_ptr<Media> &, Context*);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::ToolButton> _maximizeButton;
                std::shared_ptr<UI::ToolButton> _closeButton;
                std::shared_ptr<UI::HorizontalLayout> _titleBar;
                std::shared_ptr<MediaWidget> _mediaWidget;
                std::shared_ptr<UI::Border> _border;
            };

            void SubWidget::_init(const std::shared_ptr<Media>& media, Context* context)
            {
                IWidget::_init(context);

                _titleLabel = UI::Label::create(context);
                _titleLabel->setText(Core::FileSystem::Path(media->getFileName()).getFileName());
                _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                _titleLabel->setMargin(UI::MetricsRole::Margin);

                _maximizeButton = UI::ToolButton::create(context);
                _maximizeButton->setIcon("djvIconViewLibSDI");
                _maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

                _closeButton = UI::ToolButton::create(context);
                _closeButton->setIcon("djvIconClose");
                _closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

                _titleBar = UI::HorizontalLayout::create(context);
                _titleBar->setBackgroundRole(UI::ColorRole::Header);
                _titleBar->setSpacing(UI::MetricsRole::None);
                _titleBar->addChild(_titleLabel);
                _titleBar->setStretch(_titleLabel, UI::RowStretch::Expand);
                _titleBar->addChild(_maximizeButton);
                _titleBar->addChild(_closeButton);

                _mediaWidget = MediaWidget::create(context);
                _mediaWidget->setMedia(media);

                auto layout = UI::VerticalLayout::create(context);
                layout->setBackgroundRole(UI::ColorRole::Background);
                layout->setSpacing(UI::MetricsRole::None);
                layout->addChild(_titleBar);
                layout->addChild(_mediaWidget);
                layout->setStretch(_mediaWidget, UI::RowStretch::Expand);

                _border = UI::Border::create(context);
                _border->setMargin(UI::MetricsRole::Handle);
                _border->addChild(layout);
                addChild(_border);

                _maximizeButton->setClickedCallback(
                    [media, context]
                {
                    auto fileSystem = context->getSystemT<FileSystem>();
                    auto windowSystem = context->getSystemT<WindowSystem>();
                    fileSystem->setCurrentMedia(media);
                    windowSystem->setWindowMode(WindowMode::SDI);
                });

                _closeButton->setClickedCallback(
                    [media, context]
                {
                    auto fileSystem = context->getSystemT<FileSystem>();
                    fileSystem->close(media);
                });
            }

            std::shared_ptr<SubWidget> SubWidget::create(const std::shared_ptr<Media>& media, Context* context)
            {
                auto out = std::shared_ptr<SubWidget>(new SubWidget);
                out->_init(media, context);
                return out;
            }

            void SubWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_border->getMinimumSize());
            }

            void SubWidget::_layoutEvent(Event::Layout&)
            {
                _border->setGeometry(getGeometry());
            }

        } // namespace

        struct MDIWidget::Private
        {
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::map<std::shared_ptr<Media>, std::shared_ptr<SubWidget> > subWidgets;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > openedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > closedObserver;
        };

        void MDIWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            p.canvas = UI::MDI::Canvas::create(context);
            addChild(p.canvas);

            auto weak = std::weak_ptr<MDIWidget>(std::dynamic_pointer_cast<MDIWidget>(shared_from_this()));
            auto fileSystem = context->getSystemT<FileSystem>();
            p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                fileSystem->observeCurrentMedia(),
                [weak](const std::shared_ptr<Media>& value)
            {
                if (auto widget = weak.lock())
                {
                    const auto i = widget->_p->subWidgets.find(value);
                    if (i != widget->_p->subWidgets.end())
                    {
                        i->second->moveToFront();
                    }
                }
            });

            p.openedObserver = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                fileSystem->observeOpened(),
                [weak, context](const std::pair<std::shared_ptr<Media>, glm::vec2>& value)
            {
                if (value.first)
                {
                    if (auto widget = weak.lock())
                    {
                        auto subWidget = SubWidget::create(value.first, context);
                        widget->_p->canvas->addChild(subWidget);
                        widget->_p->canvas->setWidgetPos(subWidget, value.second);
                        widget->_p->subWidgets[value.first] = subWidget;
                    }
                }
            });

            p.closedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                fileSystem->observeClosed(),
                [weak](const std::shared_ptr<Media>& value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->subWidgets.find(value);
                        if (i != widget->_p->subWidgets.end())
                        {
                            widget->_p->canvas->removeChild(i->second);
                            widget->_p->subWidgets.erase(i);
                        }
                    }
                }
            });
        }

        MDIWidget::MDIWidget() :
            _p(new Private)
        {}

        MDIWidget::~MDIWidget()
        {}

        std::shared_ptr<MDIWidget> MDIWidget::create(Context* context)
        {
            auto out = std::shared_ptr<MDIWidget>(new MDIWidget);
            out->_init(context);
            return out;
        }

        void MDIWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->canvas->getMinimumSize());
        }

        void MDIWidget::_layoutEvent(Event::Layout&)
        {
            _p->canvas->setGeometry(getGeometry());
        }

        void MDIWidget::_localeEvent(Event::Locale& event)
        {
            DJV_PRIVATE_PTR();
        }

    } // namespace ViewApp
} // namespace djv

