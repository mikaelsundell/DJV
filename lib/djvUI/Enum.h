// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/UI.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides orientations.
        enum class Orientation
        {
            Horizontal,
            Vertical,

            Count,
            First = Horizontal
        };
        DJV_ENUM_HELPERS(Orientation);
        Orientation getOpposite(Orientation);

        //! This enumeration provides sides.
        enum class Side
        {
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };
        DJV_ENUM_HELPERS(Side);

        //! This enumeration provides corners.
        enum class Corner
        {
            UpperLeft,
            UpperRight,
            LowerRight,
            LowerLeft,

            Count,
            First = UpperLeft
        };
        DJV_ENUM_HELPERS(Corner);

        //! This enumeration provides layout expansion directions.
        enum class Expand
        {
            None,
            Horizontal,
            Vertical,
            Both,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(Expand);

        //! This enumeration provides horizontal alignment.
        enum class HAlign
        {
            Center,
            Left,
            Right,
            Fill,
            
            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(HAlign);

        //! This enumeration provides vertical alignment.
        enum class VAlign
        {
            Center,
            Top,
            Bottom,
            Fill,

            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(VAlign);

        //! This enumeration provides horizontal text alignment.
        enum class TextHAlign
        {
            Center,
            Left,
            Right,
            
            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(TextHAlign);

        //! This enumeration provides vertical text alignment.
        enum class TextVAlign
        {
            Center,
            Top,
            Bottom,
            Baseline,

            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(TextVAlign);

        //! This enumeration provides selection types.
        enum class SelectionType
        {
            None,
            Single,
            Radio,
            Multiple,

            Count,
            First = Single
        };
        DJV_ENUM_HELPERS(SelectionType);

        //! This enumeration provides sort orders.
        enum class SortOrder
        {
            Forward,
            Reverse,

            Count,
            First = Forward
        };
        DJV_ENUM_HELPERS(SortOrder);

        //! This enumeration provides button types.
        enum class ButtonType
        {
            Push,
            Toggle,
            Radio,
            Exclusive,

            Count,
            First = Push
        };
        DJV_ENUM_HELPERS(ButtonType);

        //! This enumeration provides item view types.
        enum class ViewType
        {
            Tiles,
            List,

            Count,
            First = Tiles
        };

        //! This enumeration provides callback options.
        enum class Callback
        {
            Trigger,
            Suppress
        };

        //! This enumeration provides the color roles.
        enum class ColorRole
        {
            None,
            Background,
            BackgroundHeader,
            BackgroundBellows,
            BackgroundToolBar,
            Foreground,
            ForegroundDim,
            Border,
            BorderButton,
            Trough,
            Button,
            Hovered,
            Pressed,
            Checked,
            TextFocus,
            TooltipBackground,
            TooltipForeground,
            Overlay,
            OverlayLight,
            Shadow,
            Handle,
            Cached,
            Warning,
            Error,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ColorRole);

        //! This enumeration provides the metrics roles.
        enum class MetricsRole
        {
            None,
            Border,
            BorderTextFocus,
            Margin,
            MarginSmall,
            MarginLarge,
            MarginInside,
            MarginDialog,
            Spacing,
            SpacingSmall,
            SpacingLarge,
            Drag,
            Icon,
            IconSmall,
            IconMini,
            FontSmall,
            FontMedium,
            FontLarge,
            FontHeader,
            FontTitle,
            Swatch,
            SwatchSmall,
            Slider,
            ScrollArea,
            ScrollAreaSmall,
            ScrollBar,
            ScrollBarSmall,
            Menu,
            TextColumn,
            TextColumnLarge,
            SearchBox,
            Dialog,
            Shadow,
            ShadowSmall,
            TooltipOffset,
            Handle,
            Move,
            Scrub,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(MetricsRole);

        //! This enumeration provides information about text editing.
        enum class TextEditReason
        {
            Accepted,
            LostFocus
        };

        //! This enumeration provides the text focus navigation direction.
        enum class TextFocusDirection
        {
            Next,
            Prev
        };

        //! This enumeration provides image rotations.
        enum class ImageRotate
        {
            _0,
            _90,
            _180,
            _270,

            Count,
            First = _0
        };
        DJV_ENUM_HELPERS(ImageRotate);
        float getImageRotate(ImageRotate);

        //! This enumeration provides image aspect ratios.
        enum class ImageAspectRatio
        {
            Unscaled,
            FromSource,
            _16_9,
            _1_85,
            _2_35,
            _2_39,

            Count,
            First = Unscaled
        };
        DJV_ENUM_HELPERS(ImageAspectRatio);
        float getImageAspectRatio(ImageAspectRatio);
        float getPixelAspectRatio(ImageAspectRatio, float pixelAspectRatio);
        float getAspectRatioScale(ImageAspectRatio, float aspectRatio);

        //! This enumeration provides menu popup directions.
        enum class Popup
        {
            BelowRight,
            BelowLeft,
            AboveRight,
            AboveLeft,

            Count,
            First = BelowRight
        };
        DJV_ENUM_HELPERS(Popup);

        //! This enum provides menu button styles.
        enum class MenuButtonStyle
        {
            Flat,
            Tool,
            ComboBox
        };

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Orientation);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Side);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Corner);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Expand);
    DJV_ENUM_SERIALIZE_HELPERS(UI::HAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::VAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextHAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextVAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SelectionType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SortOrder);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ButtonType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ViewType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ColorRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::MetricsRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageRotate);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageAspectRatio);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Popup);

    rapidjson::Value toJSON(UI::ViewType, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageRotate, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageAspectRatio, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ViewType&);
    void fromJSON(const rapidjson::Value&, UI::ImageRotate&);
    void fromJSON(const rapidjson::Value&, UI::ImageAspectRatio&);

} // namespace djv

