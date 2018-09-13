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
#include <djvGraphics/OpenEXR.h>

namespace djv
{
    namespace Graphics
    {
        //! \class OpenEXRPlugin
        //!
        //! This plugin provides support for the Industrial Light and Magic OpenEXR
        //! image file format.
        //!
        //! Requires:
        //!
        //! - OpenEXR - http://www.openexr.com
        //!
        //! File extensions: .exr
        //!
        //! Supported features:
        //!
        //! - 16-bit float, 32-bit float, Luminance, Luminance Alpha, RGB, RGBA
        //! - Image layers
        //! - Display and data windows
        //! - File compression
        //!
        //! \todo Add support for writing luminance/chroma images.
        //! \todo Add support for OpenEXR aspect ratios.
        //! \todo Add better support for tiled images.
        class OpenEXRPlugin : public ImageIO
        {
        public:
            explicit OpenEXRPlugin(djvCoreContext *);

            virtual void initPlugin() throw (djvError);
            virtual void releasePlugin();
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
            void threadsUpdate();

            OpenEXR::Options _options;
        };

    } // namespace Graphics
} // namespace djv
