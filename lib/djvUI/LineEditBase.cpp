// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/LineEditBase.h>

#include <djvUI/EventSystem.h>
#include <djvUI/ShortcutData.h>
#include <djvUI/Style.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <codecvt>
#include <locale>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Text
        {
            namespace
            {
                const Time::Duration cursorTimeout = std::chrono::milliseconds(500);

            } // namespace

            struct LineEditBase::Private
            {
                std::shared_ptr<Render2D::Font::FontSystem> fontSystem;

                std::string text;
                std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32Convert;
                std::basic_string<djv_char_t> utf32;

                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                Render2D::Font::Metrics fontMetrics;
                std::future<Render2D::Font::Metrics> fontMetricsFuture;

                ColorRole textColorRole = ColorRole::Foreground;

                MetricsRole textSizeRole = MetricsRole::TextColumn;
                std::string sizeString;

                glm::vec2 textSize = glm::vec2(0.F, 0.F);
                std::future<glm::vec2> textSizeFuture;
                glm::vec2 sizeStringSize = glm::vec2(0.F, 0.F);
                std::future<glm::vec2> sizeStringFuture;
                glm::vec2 widgetSize = glm::vec2(0.F, 0.F);
                float viewOffset = 0.F;
                size_t cursorPos = 0;
                size_t selectionAnchor = std::string::npos;
                std::future<std::vector<Math::BBox2f> > glyphGeomFuture;
                std::vector<Math::BBox2f> glyphGeom;
                std::vector<std::shared_ptr<Render2D::Font::Glyph> > glyphs;
                std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > glyphsFuture;
                bool cursorBlink = false;
                System::Event::PointerID pressedID = System::Event::invalidID;

                std::shared_ptr<System::Timer> cursorBlinkTimer;

                std::function<void(std::string)> textChangedCallback;
                std::function<void(const std::string&, TextEditReason)> textEditCallback;
                std::function<void(bool)> focusCallback;
            };

            void LineEditBase::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::LineEditBase");
                setVAlign(VAlign::Center);
                setPointerEnabled(true);
                setBackgroundColorRole(UI::ColorRole::Trough);

                p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();

                p.cursorBlinkTimer = System::Timer::create(context);
                p.cursorBlinkTimer->setRepeating(true);

                _textUpdate();
            }

            LineEditBase::LineEditBase() :
                _p(new Private)
            {}

            LineEditBase::~LineEditBase()
            {}

            std::shared_ptr<LineEditBase> LineEditBase::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<LineEditBase>(new LineEditBase);
                out->_init(context);
                return out;
            }

            const std::string& LineEditBase::getText() const
            {
                return _p->text;
            }

            void LineEditBase::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.text)
                    return;
                p.text = value;
                p.utf32 = _toUtf32(p.text);
                p.cursorPos = p.utf32.size();
                p.selectionAnchor = p.cursorPos;
                _textUpdate();
                _cursorUpdate();
                _viewUpdate();
            }

            const std::string& LineEditBase::getFont() const
            {
                return _p->font;
            }

            const std::string& LineEditBase::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole LineEditBase::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void LineEditBase::setFont(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.font)
                    return;
                p.font = value;
                _textUpdate();
                _cursorUpdate();
                _viewUpdate();
            }

            void LineEditBase::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFace)
                    return;
                p.fontFace = value;
                _textUpdate();
                _cursorUpdate();
                _viewUpdate();
            }

            void LineEditBase::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontSizeRole)
                    return;
                p.fontSizeRole = value;
                _textUpdate();
                _cursorUpdate();
                _viewUpdate();
            }

            ColorRole LineEditBase::getTextColorRole() const
            {
                return _p->textColorRole;
            }

            void LineEditBase::setTextColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textColorRole)
                    return;
                p.textColorRole = value;
                _redraw();
            }

            MetricsRole LineEditBase::getTextSizeRole() const
            {
                return _p->textSizeRole;
            }

            const std::string& LineEditBase::getSizeString() const
            {
                return _p->sizeString;
            }

            void LineEditBase::setTextSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textSizeRole)
                    return;
                p.textSizeRole = value;
                _resize();
            }

            void LineEditBase::setSizeString(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.sizeString)
                    return;
                p.sizeString = value;
                _textUpdate();
            }

            void LineEditBase::setTextChangedCallback(const std::function<void(const std::string&)>& value)
            {
                _p->textChangedCallback = value;
            }

            void LineEditBase::setTextEditCallback(const std::function<void(const std::string&, TextEditReason)>& value)
            {
                _p->textEditCallback = value;
            }

            void LineEditBase::setFocusCallback(const std::function<void(bool)>& value)
            {
                _p->focusCallback = value;
            }

            bool LineEditBase::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void LineEditBase::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float tc = style->getMetric(p.textSizeRole);
                const glm::vec2 size(p.sizeString.empty() ? tc : p.sizeStringSize.x, p.fontMetrics.lineHeight);
                _setMinimumSize(size + m * 2.F + getMargin().getSize(style));
            }

            void LineEditBase::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const Math::BBox2f& g = getGeometry();
                const glm::vec2 size = g.getSize();
                if (size != p.widgetSize)
                {
                    p.widgetSize = size;
                    p.viewOffset = 0.F;
                }
                _viewUpdate();
            }

            void LineEditBase::_clipEvent(System::Event::Clip& event)
            {
                if (isClipped())
                {
                    releaseTextFocus();
                    _p->viewOffset = 0.F;
                }
            }

            void LineEditBase::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const glm::vec2 c = g.getCenter();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);

                // Draw the selection.
                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Checked));
                const size_t glyphGeomSize = p.glyphGeom.size();
                if (p.cursorPos != p.selectionAnchor)
                {
                    float x0 = 0.F;
                    float x1 = 0.F;
                    if (glyphGeomSize)
                    {
                        if (p.cursorPos < glyphGeomSize)
                        {
                            x0 = p.glyphGeom[p.cursorPos].min.x;
                        }
                        else
                        {
                            const Math::BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                            x0 = geom.min.x + geom.w();
                        }
                        if (p.selectionAnchor < glyphGeomSize)
                        {
                            x1 = p.glyphGeom[p.selectionAnchor].min.x;
                        }
                        else
                        {
                            const Math::BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                            x1 = geom.min.x + geom.w();
                        }
                    }
                    if (x1 < x0)
                    {
                        const float tmp = x0;
                        x0 = x1;
                        x1 = tmp;
                    }
                    render->drawRect(Math::BBox2f(
                        g.min.x + m + x0 - p.viewOffset,
                        g.min.y + m,
                        x1 - x0,
                        g.h() - m * 2.F));
                }

                // Draw the text.
                if (p.glyphs.size())
                {
                    render->setFillColor(style->getColor(p.textColorRole));
                    glm::vec2 pos = g.min;
                    pos += m;
                    pos.x -= p.viewOffset;
                    pos.y = c.y - p.textSize.y / 2.F;
                    //! \bug Why the extra subtract by one here?
                    render->drawText(p.glyphs, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.F)));
                }

                // Draw the cursor.
                if (p.cursorBlink)
                {
                    float x = 0.F;
                    if (glyphGeomSize)
                    {
                        if (p.cursorPos < glyphGeomSize)
                        {
                            x = p.glyphGeom[p.cursorPos].min.x;
                        }
                        else
                        {
                            const Math::BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                            x = geom.min.x + geom.w();
                        }
                    }
                    render->setFillColor(style->getColor(p.textColorRole));
                    render->drawRect(Math::BBox2f(
                        g.min.x + m + x - p.viewOffset,
                        g.min.y + m,
                        b,
                        g.h() - m * 2.F));
                }
            }

            void LineEditBase::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                if (!event.isRejected())
                {
                    event.accept();
                }
            }

            void LineEditBase::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                event.accept();
            }

            void LineEditBase::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    const auto& style = _getStyle();
                    const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    float x = event.getPointerInfo().projectedPos.x - g.min.x - m + p.viewOffset;
                    size_t cursorPos = 0;
                    if (x >= 0.F)
                    {
                        for (const auto& i : p.glyphGeom)
                        {
                            if (x >= i.min.x && x <= i.max.x)
                            {
                                break;
                            }
                            ++cursorPos;
                        }
                    }
                    if (cursorPos != p.cursorPos)
                    {
                        p.cursorPos = cursorPos;
                        _cursorUpdate();
                        _viewUpdate();
                    }
                }
            }

            void LineEditBase::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID || !isEnabled(true))
                    return;
                event.accept();
                takeTextFocus();
                const auto& pointerInfo = event.getPointerInfo();
                p.pressedID = pointerInfo.id;
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                float x = event.getPointerInfo().projectedPos.x - g.min.x - m + p.viewOffset;
                size_t cursorPos = 0;
                for (const auto& i : p.glyphGeom)
                {
                    if (x >= i.min.x && x <= i.max.x)
                    {
                        break;
                    }
                    ++cursorPos;
                }
                p.cursorPos = cursorPos;
                p.selectionAnchor = cursorPos;
                _cursorUpdate();
                _viewUpdate();
            }

            void LineEditBase::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = System::Event::invalidID;
                }
            }

            void LineEditBase::_keyPressEvent(System::Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    if (!event.isAccepted() && hasTextFocus())
                    {
                        const size_t size = p.utf32.size();
                        const int modifiers = event.getKeyModifiers();
                        switch (event.getKey())
                        {
                        case GLFW_KEY_BACKSPACE:
                        {
                            event.accept();
                            const auto& selection = _getSelection();
                            if (size > 0 && (p.cursorPos > 0 || selection.getMin() != selection.getMax()))
                            {
                                if (selection.getMin() != selection.getMax())
                                {
                                    p.utf32.erase(selection.getMin(), std::min(selection.getMax() - selection.getMin(), size));
                                    p.cursorPos = selection.getMin();
                                }
                                else
                                {
                                    p.utf32.erase(selection.getMin() - 1, 1);
                                    --p.cursorPos;
                                }
                                p.text = _fromUtf32(p.utf32);
                                p.selectionAnchor = p.cursorPos;
                                _textUpdate();
                                _cursorUpdate();
                                _viewUpdate();
                                _doTextChangedCallback();
                            }
                            break;
                        }
                        case GLFW_KEY_DELETE:
                        {
                            event.accept();
                            const auto& selection = _getSelection();
                            if (size > 0 && (p.cursorPos < size || selection.getMin() != selection.getMax()))
                            {
                                if (selection.getMin() != selection.getMax())
                                {
                                    p.utf32.erase(selection.getMin(), std::min(selection.getMax() - selection.getMin(), size));
                                    p.cursorPos = selection.getMin();
                                }
                                else
                                {
                                    p.utf32.erase(p.cursorPos, 1);
                                }
                                p.text = _fromUtf32(p.utf32);
                                p.selectionAnchor = p.cursorPos;
                                _textUpdate();
                                _cursorUpdate();
                                _viewUpdate();
                                _doTextChangedCallback();
                            }
                            break;
                        }
                        case GLFW_KEY_ENTER:
                            event.accept();
                            _doTextEditCallback(TextEditReason::Accepted);
                            break;
                        case GLFW_KEY_LEFT:
                            event.accept();
                            if (p.cursorPos > 0)
                            {
                                --p.cursorPos;
                                if (!(modifiers & GLFW_MOD_SHIFT))
                                {
                                    p.selectionAnchor = p.cursorPos;
                                }
                                _cursorUpdate();
                                _viewUpdate();
                            }
                            break;
                        case GLFW_KEY_RIGHT:
                            event.accept();
                            if (p.cursorPos < size)
                            {
                                ++p.cursorPos;
                                if (!(modifiers & GLFW_MOD_SHIFT))
                                {
                                    p.selectionAnchor = p.cursorPos;
                                }
                                _cursorUpdate();
                                _viewUpdate();
                            }
                            break;
                        case GLFW_KEY_HOME:
                            event.accept();
                            if (p.cursorPos > 0)
                            {
                                p.cursorPos = 0;
                                if (!(modifiers & GLFW_MOD_SHIFT))
                                {
                                    p.selectionAnchor = p.cursorPos;
                                }
                                _cursorUpdate();
                                _viewUpdate();
                            }
                            break;
                        case GLFW_KEY_END:
                            event.accept();
                            if (size > 0 && p.cursorPos != size)
                            {
                                p.cursorPos = size;
                                if (!(modifiers & GLFW_MOD_SHIFT))
                                {
                                    p.selectionAnchor = p.cursorPos;
                                }
                                _cursorUpdate();
                                _viewUpdate();
                            }
                            break;
                        case GLFW_KEY_ESCAPE:
                            event.accept();
                            releaseTextFocus();
                            break;
                        case GLFW_KEY_UP:
                        case GLFW_KEY_DOWN:
                        case GLFW_KEY_PAGE_UP:
                        case GLFW_KEY_PAGE_DOWN:
                            break;
                        case GLFW_KEY_A:
                            if (modifiers & getSystemModifier())
                            {
                                event.accept();
                                p.cursorPos = 0;
                                p.selectionAnchor = p.utf32.size();
                                _textUpdate();
                                _cursorUpdate();
                                _viewUpdate();
                            }
                            else if (!event.getKeyModifiers())
                            {
                                event.accept();
                            }
                            break;
                        case GLFW_KEY_X:
                            if (modifiers & getSystemModifier())
                            {
                                event.accept();
                                const auto& selection = _getSelection();
                                if (selection.getMin() != selection.getMax())
                                {
                                    const auto utf32 = p.utf32.substr(selection.getMin(), selection.getMax() - selection.getMin());
                                    p.utf32.erase(selection.getMin(), selection.getMax() - selection.getMin());
                                    if (auto eventSystem = _getEventSystem().lock())
                                    {
                                        eventSystem->setClipboard(_fromUtf32(utf32));
                                    }
                                    p.text = _fromUtf32(p.utf32);
                                    p.cursorPos = selection.getMin();
                                    p.selectionAnchor = p.cursorPos;
                                    _textUpdate();
                                    _cursorUpdate();
                                    _viewUpdate();
                                    _doTextChangedCallback();
                                }
                            }
                            else if (!event.getKeyModifiers())
                            {
                                event.accept();
                            }
                            break;
                        case GLFW_KEY_C:
                            if (modifiers & getSystemModifier())
                            {
                                event.accept();
                                const auto& selection = _getSelection();
                                if (selection.getMin() != selection.getMax())
                                {
                                    const auto utf32 = p.utf32.substr(selection.getMin(), selection.getMax() - selection.getMin());
                                    if (auto eventSystem = _getEventSystem().lock())
                                    {
                                        eventSystem->setClipboard(_fromUtf32(utf32));
                                    }
                                }
                            }
                            else if (!event.getKeyModifiers())
                            {
                                event.accept();
                            }
                            break;
                        case GLFW_KEY_V:
                            if (modifiers & getSystemModifier())
                            {
                                event.accept();
                                if (auto eventSystem = _getEventSystem().lock())
                                {
                                    const auto utf32 = _toUtf32(eventSystem->getClipboard());
                                    const auto& selection = _getSelection();
                                    if (selection.getMin() != selection.getMax())
                                    {
                                        p.utf32.replace(selection.getMin(), selection.getMax() - selection.getMin(), utf32);
                                        p.cursorPos = selection.getMin() + utf32.size();
                                    }
                                    else
                                    {
                                        p.utf32.insert(p.cursorPos, utf32);
                                        p.cursorPos += utf32.size();
                                    }
                                    p.text = _fromUtf32(p.utf32);
                                    p.selectionAnchor = p.cursorPos;
                                    _textUpdate();
                                    _cursorUpdate();
                                    _viewUpdate();
                                    _doTextChangedCallback();
                                }
                            }
                            else if (!event.getKeyModifiers())
                            {
                                event.accept();
                            }
                            break;
                        default:
                            if (!event.getKeyModifiers())
                            {
                                event.accept();
                            }
                            break;
                        }
                    }
                }
            }

            void LineEditBase::_textFocusEvent(System::Event::TextFocus& event)
            {
                event.accept();
                _cursorUpdate();
                _redraw();
                _doFocusCallback(true);
            }

            void LineEditBase::_textFocusLostEvent(System::Event::TextFocusLost& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                p.cursorBlinkTimer->stop();
                p.cursorBlink = false;
                _redraw();
                _doTextEditCallback(TextEditReason::LostFocus);
                _doFocusCallback(false);
            }

            void LineEditBase::_textInputEvent(System::Event::TextInput& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto& selection = _getSelection();
                const auto& utf32 = event.getUtf32();
                if (selection.getMin() != selection.getMax())
                {
                    p.utf32.replace(selection.getMin(), selection.getMax() - selection.getMin(), utf32);
                    p.cursorPos = selection.getMin() + utf32.size();
                }
                else
                {
                    p.utf32.insert(p.cursorPos, utf32);
                    p.cursorPos += utf32.size();
                }
                p.text = _fromUtf32(p.utf32);
                p.selectionAnchor = p.cursorPos;
                _textUpdate();
                _cursorUpdate();
                _viewUpdate();
                _doTextChangedCallback();
            }

            void LineEditBase::_initEvent(System::Event::Init& event)
            {
                if (event.getData().resize || event.getData().font)
                {
                    _p->viewOffset = 0.F;
                    _textUpdate();
                    _cursorUpdate();
                    _viewUpdate();
                }
            }

            void LineEditBase::_updateEvent(System::Event::Update& event)
            {
                DJV_PRIVATE_PTR();
                if (p.fontMetricsFuture.valid() &&
                    p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.fontMetrics = p.fontMetricsFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
                if (p.textSizeFuture.valid() &&
                    p.textSizeFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.textSize = p.textSizeFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
                if (p.sizeStringFuture.valid() &&
                    p.sizeStringFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.sizeStringSize = p.sizeStringFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
                if (p.glyphGeomFuture.valid() &&
                    p.glyphGeomFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.glyphGeom = p.glyphGeomFuture.get();
                        _viewUpdate();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
                if (p.glyphsFuture.valid() &&
                    p.glyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.glyphs = p.glyphsFuture.get();
                        _redraw();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
            }

            std::string LineEditBase::_fromUtf32(const std::basic_string<djv_char_t>& value)
            {
                std::string out;
                try
                {
                    out = _p->utf32Convert.to_bytes(value);
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Error converting string: " << e.what();
                    _log(ss.str(), System::LogLevel::Error);
                }
                return out;
            }

            std::basic_string<djv_char_t> LineEditBase::_toUtf32(const std::string& value)
            {
                std::basic_string<djv_char_t> out;
                try
                {
                    std::string tmp = value;
                    for (auto i = tmp.begin(); i != tmp.end(); ++i)
                    {
                        if ('\n' == *i)
                        {
                            *i = ' ';
                        }
                        else if ('\r' == *i)
                        {
                            *i = ' ';
                        }
                    }
                    out = _p->utf32Convert.from_bytes(tmp);
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Error converting string" << " '" << value << "': " << e.what();
                    _log(ss.str(), System::LogLevel::Error);
                }
                return out;
            }

            Math::SizeTRange LineEditBase::_getSelection() const
            {
                DJV_PRIVATE_PTR();
                return Math::SizeTRange(p.cursorPos, p.selectionAnchor);
            }

            void LineEditBase::_textUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const auto fontInfo = p.font.empty() ?
                    style->getFontInfo(p.fontFace, p.fontSizeRole) :
                    style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
                p.textSizeFuture = p.fontSystem->measure(p.text, fontInfo);
                if (!p.sizeString.empty())
                {
                    p.sizeStringFuture = p.fontSystem->measure(p.sizeString, fontInfo);
                }
                if (!p.text.size())
                {
                    p.glyphGeom.clear();
                    p.glyphs.clear();
                }
                p.glyphGeomFuture = p.fontSystem->measureGlyphs(p.text, fontInfo);
                p.glyphsFuture = p.fontSystem->getGlyphs(p.text, fontInfo);
            }

            void LineEditBase::_cursorUpdate()
            {
                DJV_PRIVATE_PTR();

                const size_t size = p.utf32.size();
                if (size)
                {
                    p.cursorPos = Math::clamp(p.cursorPos, size_t(0), size);
                    p.selectionAnchor = Math::clamp(p.selectionAnchor, size_t(0), size);
                }
                else
                {
                    p.cursorPos = 0;
                    p.selectionAnchor = 0;
                }
                _redraw();

                if (hasTextFocus())
                {
                    p.cursorBlink = true;
                    auto weak = std::weak_ptr<LineEditBase>(std::dynamic_pointer_cast<LineEditBase>(shared_from_this()));
                    p.cursorBlinkTimer->start(
                        cursorTimeout,
                        [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->cursorBlink = !widget->_p->cursorBlink;
                                widget->_redraw();
                            }
                        });
                }
            }

            void LineEditBase::_viewUpdate()
            {
                DJV_PRIVATE_PTR();
                const size_t glyphGeomSize = p.glyphGeom.size();
                if (glyphGeomSize)
                {
                    float viewOffset = p.viewOffset;

                    const auto& style = _getStyle();
                    const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    float xMin = 0.F;
                    float xMax = 0.F;
                    if (p.cursorPos < glyphGeomSize)
                    {
                        xMin = p.glyphGeom[p.cursorPos].min.x;
                        xMax = p.glyphGeom[p.cursorPos].max.x;
                    }
                    else if (glyphGeomSize > 0)
                    {
                        xMin = p.glyphGeom[glyphGeomSize - 1].min.x;
                        xMax = p.glyphGeom[glyphGeomSize - 1].max.x;
                    }
                    const float viewWidth = g.w() - m * 2.F;
                    if (viewWidth > 0.F)
                    {
                        const size_t size = p.utf32.size();
                        if (0 == size)
                        {
                            viewOffset = 0.F;
                        }
                        else if (size > 0 && p.cursorPos >= size && xMax >= viewWidth)
                        {
                            viewOffset = xMax - viewWidth;
                        }
                        else if (xMax > (viewOffset + viewWidth))
                        {
                            viewOffset = xMax - viewWidth;
                        }
                        else if (xMin < viewOffset)
                        {
                            viewOffset = xMin;
                        }
                    }

                    if (viewOffset != p.viewOffset)
                    {
                        p.viewOffset = viewOffset;
                        _redraw();
                    }
                }
            }

            void LineEditBase::_doTextChangedCallback()
            {
                DJV_PRIVATE_PTR();
                if (p.textChangedCallback)
                {
                    p.textChangedCallback(p.text);
                }
            }

            void LineEditBase::_doTextEditCallback(TextEditReason reason)
            {
                DJV_PRIVATE_PTR();
                if (p.textEditCallback)
                {
                    p.textEditCallback(p.text, reason);
                }
            }

            void LineEditBase::_doFocusCallback(bool value)
            {
                DJV_PRIVATE_PTR();
                if (p.focusCallback)
                {
                    p.focusCallback(value);
                }
            }

        } // namespace Text
    } // namespace UI
} // namespace djv
