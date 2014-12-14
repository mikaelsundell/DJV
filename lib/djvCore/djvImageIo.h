//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvImageIo.h

#ifndef DJV_IMAGE_IO_H
#define DJV_IMAGE_IO_H

#include <djvImageTags.h>
#include <djvPixelData.h>
#include <djvPlugin.h>
#include <djvSequence.h>
#include <djvSystem.h>

#include <QObject>
#include <QStringList>

class djvAbstractPrefsWidget;

class djvFileInfo;
class djvImage;

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvImageIoInfo
//!
//! This class provides image I/O information.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageIoInfo : public djvPixelDataInfo
{
public:

    //! Constructor.

    djvImageIoInfo();

    //! Constructor.

    djvImageIoInfo(const djvPixelDataInfo &);

    //! Add a layer.

    void addLayer(const djvPixelDataInfo &);

    //! Get the number of layers.

    int layerCount() const;

    //! Set the number of layers.

    void setLayerCount(int);

    //! Remove all the layers.

    void clearLayers();

    //! The image tags.

    djvImageTags tags;

    //! The frame sequence.

    djvSequence sequence;

    djvPixelDataInfo & operator [] (int);

    const djvPixelDataInfo & operator [] (int) const;

private:

    QVector<djvPixelDataInfo> _info;
};

//------------------------------------------------------------------------------
//! \class djvImageIoFrameInfo
//!
//! This struct provides image I/O frame information.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvImageIoFrameInfo
{
    //! Constructor.

    djvImageIoFrameInfo(
        qint64                  frame = -1,
        int                     layer = 0,
        djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE);

    //! The frame number.

    qint64 frame;

    //! The image layer.

    int layer;

    //! The proxy scale.

    djvPixelDataInfo::PROXY proxy;
};

//------------------------------------------------------------------------------
//! \class djvImageLoad
//!
//! This class provides the base functionality for image loading.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageLoad
{
public:

    //! Destructor.
    
    virtual ~djvImageLoad() = 0;

    //! Open an image.

    virtual void open(
        const djvFileInfo & fileInfo,
        djvImageIoInfo &    imageIoInfo)
        throw (djvError) = 0;

    //! Load an image.

    virtual void read(
        djvImage &                  image,
        const djvImageIoFrameInfo & frameInfo = djvImageIoFrameInfo())
        throw (djvError) = 0;
  
    //! Close the image.
  
    virtual void close() throw (djvError);
};

//------------------------------------------------------------------------------
//! \class djvImageSave
//!
//! This class provides the base functionality for image saving.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageSave
{
public:

    //! Destructor.

    virtual ~djvImageSave() = 0;

    //! Open an image.

    virtual void open(
        const djvFileInfo &    fileInfo,
        const djvImageIoInfo & imageIoInfo)
        throw (djvError) = 0;

    //! Save an image.

    virtual void write(
        const djvImage &            image,
        const djvImageIoFrameInfo & frameInfo = djvImageIoFrameInfo())
        throw (djvError) = 0;

    //! Close the image.

    virtual void close() throw (djvError);
};

//------------------------------------------------------------------------------
//! \class djvImageIo
//!
//! This class provides the base functionality for image I/O plugins.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageIo : public QObject, public djvPlugin
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    djvImageIo();
    
    //! Destructor.

    virtual ~djvImageIo() = 0;
    
    //! Get the list of supported file extensions.

    virtual QStringList extensions() const;

    //! Does the plugin use file sequences?

    virtual bool isSequence() const;

    //! Get an option.

    virtual QStringList option(const QString &) const;

    //! Set an option.

    virtual bool setOption(const QString &, QStringList &);
    
    //! Get the list of options.

    virtual QStringList options() const;

    //! Parse the command line.

    virtual void commandLine(QStringList &) throw (djvError);

    //! Get the command line help.

    virtual QString commandLineHelp() const;
    
    //! Get an image loader.
    
    virtual djvImageLoad * createLoad() const;
    
    //! Get an image saver.
    
    virtual djvImageSave * createSave() const;

    //! Create a widget.
    
    virtual djvAbstractPrefsWidget * createWidget();

    //! Throw an error that the image is unrecognized.

    static void throwUnrecognized(
        const QString & plugin,
        const QString & fileName) throw (djvError);

    //! Throw an error that the image is unsupported.

    static void throwUnsupported(
        const QString & plugin,
        const QString & fileName) throw (djvError);

    //! Throw an error that the image could not be opened.

    static void throwErrorOpen(
        const QString & plugin,
        const QString & fileName) throw (djvError);

    //! Throw an error that the image could not be read.

    static void throwErrorRead(
        const QString & plugin,
        const QString & fileName) throw (djvError);

    //! Throw an error that the image could not be written.

    static void throwErrorWrite(
        const QString & plugin,
        const QString & fileName) throw (djvError);

Q_SIGNALS:

    //! This signal is emitted when an option is changed.
    
    void optionChanged(const QString &);
};

//------------------------------------------------------------------------------
//! \class djvImageIoFactory
//!
//! This class provides a factory for image I/O plugins.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageIoFactory : public djvPluginFactory
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvImageIoFactory(
        const QStringList & searchPath = djvSystem::searchPath(),
        QObject *           parent     = 0)
        throw (djvError);

    //! Destructor.

    virtual ~djvImageIoFactory();

    //! Get a plugin option.

    QStringList option(const QString & name, const QString &) const;

    //! Set a plugin option.

    bool setOption(const QString & name, const QString &, QStringList &);
    
    //! Open an image for loading.
  
    djvImageLoad * load(
        const djvFileInfo & fileInfo,
        djvImageIoInfo &    imageIoInfo) const throw (djvError);
    
    //! Open an image for saving.
  
    djvImageSave * save(
        const djvFileInfo &    fileInfo,
        const djvImageIoInfo & imageIoInfo) const throw (djvError);

    //! Get the global factory.

    static djvImageIoFactory * global();

Q_SIGNALS:

    //! This signal is emitted when a plugin option is changed.
    
    void optionChanged();
    
private Q_SLOTS:

    void pluginOptionCallback(const QString &);

private:

    DJV_PRIVATE_COPY(djvImageIoFactory);
    DJV_PRIVATE_IMPLEMENTATION();
};

//------------------------------------------------------------------------------

DJV_CORE_EXPORT bool operator == (
    const djvImageIoInfo &,
    const djvImageIoInfo &);
DJV_CORE_EXPORT bool operator == (
    const djvImageIoFrameInfo &,
    const djvImageIoFrameInfo &);

DJV_CORE_EXPORT bool operator != (
    const djvImageIoInfo &,
    const djvImageIoInfo &);
DJV_CORE_EXPORT bool operator != (
    const djvImageIoFrameInfo &,
    const djvImageIoFrameInfo &);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvImageIoInfo);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvImageIoFrameInfo);

//@} // djvCoreImage

#endif // DJV_IMAGE_IO_H

