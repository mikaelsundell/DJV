//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#pragma once

#include <djvGraphics/ImageIO.h>
#include <djvGraphics/LUT.h>

namespace djv
{
    namespace Graphics
    {
        //! \class LUTPlugin
        //!
        //! This plugin provides support for two-dimensional lookup table file formats.
        //!
        //! File extensions: .lut, .1dl
        //!
        //! Supported features:
        //!
        //! - Inferno and Kodak formats
        //! - 8-bit, 16-bit, Luminance, Luminance Alpha, RGB, RGBA; 10-bit RGB
        //!
        //! \todo Add support for detecting whether the input LUT file is horizontal
        //! or vertical.
        class LUTPlugin : public ImageIO
        {
        public:
            explicit LUTPlugin(djvCoreContext *);

            virtual QString pluginName() const;
            virtual QStringList extensions() const;

            virtual QStringList option(const QString &) const;
            virtual bool setOption(const QString &, QStringList &);
            virtual QStringList options() const;

            virtual void commandLine(QStringList &) throw (QString);
            virtual QString commandLineHelp() const;

            virtual ImageLoad * createLoad() const;
            virtual ImageSave * createSave() const;

        private:
            LUT::Options _options;
        };

    } // namespace Graphics
} // namespace djv
