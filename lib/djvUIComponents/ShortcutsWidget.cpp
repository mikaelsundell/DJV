// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListWidget.h>
#include <djvUI/Overlay.h>
#include <djvUI/PopupLayout.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutData.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

#include <djvCore/String.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace
        {
            class KeyPressWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(KeyPressWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                KeyPressWidget();

            public:
                ~KeyPressWidget() override;

                static std::shared_ptr<KeyPressWidget> create(const std::shared_ptr<System::Context>&);

                void setShortcut(const UI::ShortcutData&);
                void setCallback(const std::function<void(const UI::ShortcutData&)>&);

                bool acceptFocus(UI::TextFocusDirection) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                UI::ShortcutData _shortcut;
                std::shared_ptr<UI::Text::Label> _label;
                std::function<void(const UI::ShortcutData&)> _callback;
                System::Event::PointerID _pressedID = System::Event::invalidID;
            };

            void KeyPressWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djvUIComponents::KeyPressWidget");
                setPointerEnabled(true);

                _label = UI::Text::Label::create(context);
                _label->setTextHAlign(UI::TextHAlign::Left);
                _label->setMargin(UI::MetricsRole::MarginSmall);
                _label->setBackgroundColorRole(UI::ColorRole::Trough);
                addChild(_label);

                _widgetUpdate();
            }

            KeyPressWidget::KeyPressWidget()
            {}

            KeyPressWidget::~KeyPressWidget()
            {}

            std::shared_ptr<KeyPressWidget> KeyPressWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<KeyPressWidget>(new KeyPressWidget);
                out->_init(context);
                return out;
            }

            void KeyPressWidget::setShortcut(const UI::ShortcutData& value)
            {
                if (value == _shortcut)
                    return;
                _shortcut = value;
                _widgetUpdate();
            }

            void KeyPressWidget::setCallback(const std::function<void(const UI::ShortcutData&)>& value)
            {
                _callback = value;
            }

            bool KeyPressWidget::acceptFocus(UI::TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void KeyPressWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                glm::vec2 size = _label->getMinimumSize();
                size += btf * 2.F;
                _setMinimumSize(size);
            }

            void KeyPressWidget::_layoutEvent(System::Event::Layout&)
            {
                const auto& style = _getStyle();
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();
                const Math::BBox2f g2 = g.margin(-btf);
                _label->setGeometry(g2);
            }

            void KeyPressWidget::_paintEvent(System::Event::Paint&)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();
                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                    UI::drawBorder(render, g, btf);
                }
                else
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Border));
                    UI::drawBorder(render, g.margin(-b), b);
                }
            }

            void KeyPressWidget::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                if (!event.isRejected())
                {
                    event.accept();
                }
            }

            void KeyPressWidget::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                event.accept();
            }

            void KeyPressWidget::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                event.accept();
            }

            void KeyPressWidget::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                if (_pressedID || !isEnabled(true))
                    return;
                event.accept();
                takeTextFocus();
            }

            void KeyPressWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == _pressedID)
                {
                    event.accept();
                    _pressedID = System::Event::invalidID;
                }
            }

            void KeyPressWidget::_keyPressEvent(System::Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                if (auto context = getContext().lock())
                {
                    if (!event.isAccepted() && hasTextFocus())
                    {
                        switch (event.getKey())
                        {
                        case 0:
                        case GLFW_KEY_ENTER:
                        case GLFW_KEY_TAB:
                        case GLFW_KEY_CAPS_LOCK:
                        case GLFW_KEY_SCROLL_LOCK:
                        case GLFW_KEY_NUM_LOCK:
                        case GLFW_KEY_LEFT_SHIFT:
                        case GLFW_KEY_LEFT_CONTROL:
                        case GLFW_KEY_LEFT_ALT:
                        case GLFW_KEY_LEFT_SUPER:
                        case GLFW_KEY_RIGHT_SHIFT:
                        case GLFW_KEY_RIGHT_CONTROL:
                        case GLFW_KEY_RIGHT_ALT:
                        case GLFW_KEY_RIGHT_SUPER:
                        case GLFW_KEY_MENU:
                            break;
                        case GLFW_KEY_ESCAPE:
                            event.accept();
                            releaseTextFocus();
                            break;
                        default:
                            event.accept();
                            _shortcut.key = event.getKey();
                            _shortcut.modifiers = event.getKeyModifiers();
                            _widgetUpdate();
                            if (_callback)
                            {
                                _callback(_shortcut);
                            }
                            break;
                        }
                    }
                }
            }

            void KeyPressWidget::_textFocusEvent(System::Event::TextFocus& event)
            {
                event.accept();
                _redraw();
            }

            void KeyPressWidget::_textFocusLostEvent(System::Event::TextFocusLost& event)
            {
                event.accept();
                _redraw();
            }

            void KeyPressWidget::_initEvent(System::Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void KeyPressWidget::_widgetUpdate()
            {
                const auto& textSystem = _getTextSystem();
                _label->setText(getText(_shortcut, textSystem));
            }

        } // namespace

        struct ShortcutsWidget::Private
        {
            Private(ShortcutsWidget& p) :
                p(p)
            {}

            ShortcutsWidget& p;

            UI::ShortcutDataMap shortcuts;
            std::vector<std::string> shortcutNames;
            std::map<std::string, bool> shortcutsCollisions;
            int currentShortcut = -1;

            std::shared_ptr<SearchBox> searchBox;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<KeyPressWidget> keyPressWidgets[2];
            std::shared_ptr<UI::ToolButton> clearButtons[2];
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::Window> window;

            std::function<void(const UI::ShortcutDataMap&)> shortcutsCallback;

            void setCurrentPrimary(const UI::ShortcutData&);
            void setCurrentSecondary(const UI::ShortcutData&);

            void shortcutsUpdate();
            void itemsUpdate();
            void currentItemUpdate();
        };

        void ShortcutsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UIComponents::ShortcutsWidget");

            p.searchBox = SearchBox::create(context);

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->addChild(p.listWidget);

            for (size_t i = 0; i < 2; ++i)
            {
                p.keyPressWidgets[i] = KeyPressWidget::create(context);
                p.clearButtons[i] = UI::ToolButton::create(context);
                p.clearButtons[i]->setIcon("djvIconClearSmall");
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.searchBox);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.keyPressWidgets[0]);
            hLayout->setStretch(p.keyPressWidgets[0]);
            hLayout->addChild(p.clearButtons[0]);
            hLayout->addChild(p.keyPressWidgets[1]);
            hLayout->setStretch(p.keyPressWidgets[1]);
            hLayout->addChild(p.clearButtons[1]);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            p.listWidget->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->keyPressWidgets[0]->takeTextFocus();
                    }
                });
            p.listWidget->setRadioCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentShortcut = value;
                        widget->_p->currentItemUpdate();
                    }
                });

            p.keyPressWidgets[0]->setCallback(
                [weak](const UI::ShortcutData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setCurrentPrimary(value);
                    }
                });
            p.keyPressWidgets[1]->setCallback(
                [weak](const UI::ShortcutData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setCurrentSecondary(value);
                    }
                });

            p.clearButtons[0]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setCurrentPrimary(UI::ShortcutData());
                    }
                });
            p.clearButtons[1]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setCurrentSecondary(UI::ShortcutData());
                    }
                });
        }

        ShortcutsWidget::ShortcutsWidget() :
            _p(new Private(*this))
        {}

        std::shared_ptr<ShortcutsWidget> ShortcutsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ShortcutsWidget>(new ShortcutsWidget);
            out->_init(context);
            return out;
        }

        void ShortcutsWidget::setShortcuts(const UI::ShortcutDataMap& value)
        {
            if (value == _p->shortcuts)
                return;
            _p->shortcuts = value;
            _p->currentShortcut = Math::clamp(_p->currentShortcut, 0, static_cast<int>(_p->shortcuts.size()) - 1);
            _p->shortcutsUpdate();
            _p->itemsUpdate();
            _p->currentItemUpdate();
        }

        void ShortcutsWidget::setShortcutsCallback(const std::function<void(const UI::ShortcutDataMap&)>& value)
        {
            _p->shortcutsCallback = value;
        }

        void ShortcutsWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.layout->getMinimumSize() + getMargin().getSize(style));
        }

        void ShortcutsWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void ShortcutsWidget::_initEvent(System::Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.searchBox->setTooltip(_getText(DJV_TEXT("shortcut_search")));
                p.keyPressWidgets[0]->setTooltip(_getText(DJV_TEXT("shortcut_primary_tooltip")));
                p.keyPressWidgets[1]->setTooltip(_getText(DJV_TEXT("shortcut_secondary_tooltip")));
                p.clearButtons[0]->setTooltip(_getText(DJV_TEXT("shortcut_primary_clear_tooltip")));
                p.clearButtons[1]->setTooltip(_getText(DJV_TEXT("shortcut_secondary_clear_tooltip")));
                p.itemsUpdate();
            }
        }

        void ShortcutsWidget::Private::setCurrentPrimary(const UI::ShortcutData& value)
        {
            if (currentShortcut >= 0 && currentShortcut < static_cast<int>(shortcutNames.size()))
            {
                auto i = shortcuts.find(shortcutNames[currentShortcut]);
                if (i != shortcuts.end())
                {
                    i->second.primary = value;
                    shortcutsUpdate();
                    itemsUpdate();
                    currentItemUpdate();
                    shortcutsCallback(shortcuts);
                }
            }
        }

        void ShortcutsWidget::Private::setCurrentSecondary(const UI::ShortcutData& value)
        {
            if (currentShortcut >= 0 && currentShortcut < static_cast<int>(shortcutNames.size()))
            {
                auto i = shortcuts.find(shortcutNames[currentShortcut]);
                if (i != shortcuts.end())
                {
                    i->second.secondary = value;
                    shortcutsUpdate();
                    itemsUpdate();
                    currentItemUpdate();
                    shortcutsCallback(shortcuts);
                }
            }
        }

        void ShortcutsWidget::Private::shortcutsUpdate()
        {
            std::map<UI::ShortcutData, std::set<std::string> > taken;
            for (const auto& i : shortcuts)
            {
                if (i.second.primary.isValid())
                {
                    taken[i.second.primary].insert(i.first);
                }
                if (i.second.secondary.isValid())
                {
                    taken[i.second.secondary].insert(i.first);
                }
            }
            shortcutsCollisions.clear();
            for (const auto& i : taken)
            {
                for (const auto& j : i.second)
                {
                    shortcutsCollisions[j] |= i.second.size() > 1;
                }
            }
        }

        void ShortcutsWidget::Private::itemsUpdate()
        {
            shortcutNames.clear();
            std::vector<UI::ListItem> items;
            const auto& textSystem = p._getTextSystem();
            for (const auto& i : shortcuts)
            {
                shortcutNames.push_back(i.first);
                std::vector<std::string> list;
                if (i.second.primary.isValid())
                {
                    list.push_back(UI::getText(i.second.primary.key, i.second.primary.modifiers, textSystem));
                }
                if (i.second.secondary.isValid())
                {
                    list.push_back(UI::getText(i.second.secondary.key, i.second.secondary.modifiers, textSystem));
                }
                const auto j = shortcutsCollisions.find(i.first);
                const bool collision = j != shortcutsCollisions.end() && j->second;
                UI::ListItem item;
                item.text = p._getText(i.first);
                item.rightText = String::join(list, ", ");
                item.rightIcon = collision ? "djvIconWarningSmall" : std::string();
                items.emplace_back(item);
            }
            listWidget->setItems(items);
            listWidget->setChecked(currentShortcut);
        }

        void ShortcutsWidget::Private::currentItemUpdate()
        {
            UI::ShortcutDataPair shortcut;
            if (currentShortcut >= 0 && currentShortcut < static_cast<int>(shortcutNames.size()))
            {
                const auto i = shortcuts.find(shortcutNames[currentShortcut]);
                if (i != shortcuts.end())
                {
                    shortcut = i->second;
                }
            }
            keyPressWidgets[0]->setShortcut(shortcut.primary);
            keyPressWidgets[1]->setShortcut(shortcut.secondary);
        }

    } // namespace UIComponents
} // namespace djv

