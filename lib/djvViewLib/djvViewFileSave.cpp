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

#include <djvViewFileSave.h>

#include <djvViewContext.h>
#include <djvViewUtil.h>

#include <djvProgressDialog.h>

#include <djvError.h>
#include <djvImage.h>
#include <djvPixelDataUtil.h>

#include <QApplication>
#include <QDir>

//------------------------------------------------------------------------------
// djvViewFileSaveInfo
//------------------------------------------------------------------------------

djvViewFileSaveInfo::djvViewFileSaveInfo(
    const djvFileInfo &           inputFile,
    const djvFileInfo &           outputFile,
    const djvPixelDataInfo &      info,
    const djvSequence &           sequence,
    int                           layer,
    djvPixelDataInfo::PROXY       proxy,
    bool                          u8Conversion,
    bool                          colorProfile,
    const djvOpenGlImageOptions & options) :
    inputFile   (inputFile),
    outputFile  (outputFile),
    info        (info),
    sequence    (sequence),
    layer       (layer),
    proxy       (proxy),
    u8Conversion(u8Conversion),
    colorProfile(colorProfile),
    options     (options)
{}

//------------------------------------------------------------------------------
// djvViewFileSave::Private
//------------------------------------------------------------------------------

struct djvViewFileSave::Private
{
    Private(djvViewContext * context) :
        dialog (0),
        context(context)
    {}
    
    djvViewFileSaveInfo          info;
    djvSequence                  saveSequence;
    QScopedPointer<djvImageLoad> load;
    QScopedPointer<djvImageSave> save;
    djvProgressDialog *          dialog;
    djvViewContext *             context;
};

//------------------------------------------------------------------------------
// djvViewFileSave
//------------------------------------------------------------------------------

djvViewFileSave::djvViewFileSave(djvViewContext * context, QObject * parent) :
    QObject(parent),
    _p(new Private(context))
{
    _p->dialog = new djvProgressDialog;

    connect(
        _p->dialog,
        SIGNAL(progressSignal(int)),
        SLOT(callback(int)));
    connect(
        _p->dialog,
        SIGNAL(finishedSignal()),
        SLOT(finishedCallback()));
}

djvViewFileSave::~djvViewFileSave()
{
    //DJV_DEBUG("djvViewFileSave::~djvViewFileSave");
    delete _p->dialog;
}

void djvViewFileSave::save(const djvViewFileSaveInfo & info)
{
    //DJV_DEBUG("djvViewFileSave::save");
    //DJV_DEBUG_PRINT("input = " << info.inputFile);
    //DJV_DEBUG_PRINT("output = " << info.outputFile);
    //DJV_DEBUG_PRINT("sequence = " << info.sequence);

    cancel();

    _p->info = info;
    if (_p->info.outputFile.isSequenceValid())
    {
        _p->saveSequence = djvSequence(
            _p->info.outputFile.sequence().frames[0],
            _p->info.outputFile.sequence().frames[0] + info.sequence.frames.count() - 1,
            info.sequence.pad,
            info.sequence.speed);
    }
    else
    {
        _p->saveSequence = info.sequence;
    }
    
    //! \todo Why do we need to reverse the rotation here?
    _p->info.options.xform.rotate = -_p->info.options.xform.rotate;
    const djvBox2f bbox =
        djvOpenGlImageXform::xformMatrix(_p->info.options.xform) *
        djvBox2f(_p->info.info.size * djvPixelDataUtil::proxyScale(_p->info.info.proxy));
    //DJV_DEBUG_PRINT("bbox = " << bbox);
    _p->info.options.xform.position = -bbox.position;
    _p->info.info.size = bbox.size;

    // Open input.
    djvImageIoInfo loadInfo;
    try
    {
        _p->load.reset(
            _p->context->imageIoFactory()->load(_p->info.inputFile, loadInfo));
    }
    catch (djvError error)
    {
        error.add(
            djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
            arg(QDir::toNativeSeparators(_p->info.inputFile)));
        _p->context->printError(error);
        return;
    }

    // Open output.
    djvImageIoInfo saveInfo(_p->info.info);
    saveInfo.tags     = loadInfo.tags;
    saveInfo.sequence = _p->saveSequence;
    try
    {
        _p->save.reset(
            _p->context->imageIoFactory()->save(_p->info.outputFile, saveInfo));
    }
    catch (djvError error)
    {
        error.add(
            djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
            arg(QDir::toNativeSeparators(_p->info.outputFile)));
        
        _p->context->printError(error);

        return;
    }

    // Start...
    _p->dialog->setLabel(qApp->translate("djvViewFileSave", "Saving \"%1\":").
        arg(QDir::toNativeSeparators(_p->info.outputFile)));
    _p->dialog->start(
        _p->info.sequence.frames.count() ? _p->info.sequence.frames.count() : 1);
    _p->dialog->show();
}

void djvViewFileSave::cancel()
{
    //DJV_DEBUG("djvViewFileSave::cancel");
    if (_p->dialog->isVisible())
    {
        _p->dialog->reject();
    }
    if (_p->save.data())
    {
        try
        {
            //DJV_DEBUG_PRINT("close");

            _p->save->close();
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_WRITE_IMAGE].
                arg(QDir::toNativeSeparators(_p->info.outputFile)));

            _p->context->printError(error);
        }
    }
    _p->info = djvViewFileSaveInfo();
    _p->load.reset();
    _p->save.reset();
}

void djvViewFileSave::callback(int in)
{
    //DJV_DEBUG("djvViewFileSave::callback");
    //DJV_DEBUG_PRINT("in = " << in);

    // Load the frame.
    djvImage image;
    try
    {
        //DJV_DEBUG_PRINT("load");
        _p->load->read(
            image,
            djvImageIoFrameInfo(
                in < _p->info.sequence.frames.count() ? _p->info.sequence.frames[in] : -1,
                _p->info.layer,
                _p->info.proxy));

        //DJV_DEBUG_PRINT("image = " << image);
    }
    catch (djvError error)
    {
        error.add(
            djvViewUtil::errorLabels()[djvViewUtil::ERROR_READ_IMAGE].
            arg(QDir::toNativeSeparators(_p->info.inputFile)));
        _p->context->printError(error);
        cancel();
        return;
    }

    // Process the frame.
    djvImage * p = &image;
    djvImage tmp;
    djvOpenGlImageOptions options(_p->info.options);
    if (_p->info.u8Conversion || _p->info.colorProfile)
    {
        options.colorProfile = image.colorProfile;
    }
    //DJV_DEBUG_PRINT("convert = " << (p->info() != _p->info.info));
    //DJV_DEBUG_PRINT("options = " << (options != djvOpenGlImageOptions()));
    //DJV_DEBUG_PRINT("options = " << options);
    //DJV_DEBUG_PRINT("def options = " << djvOpenGlImageOptions());
    if (p->info() != _p->info.info ||
        options != djvOpenGlImageOptions())
    {
        tmp.set(_p->info.info);
        try
        {
            //DJV_DEBUG_PRINT("process");
            djvOpenGlImage::copy(image, tmp, options);
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_WRITE_IMAGE].
                arg(QDir::toNativeSeparators(_p->info.outputFile)));
            _p->context->printError(error);
            cancel();
            return;
        }
        p = &tmp;
    }
    tmp.tags = image.tags;

    // Save the frame.
    try
    {
        //DJV_DEBUG_PRINT("save");
        _p->save->write(
            tmp,
            djvImageIoFrameInfo(
                in < _p->saveSequence.frames.count() ? _p->saveSequence.frames[in] : -1));
    }
    catch (djvError error)
    {
        error.add(
            djvViewUtil::errorLabels()[djvViewUtil::ERROR_WRITE_IMAGE].
            arg(QDir::toNativeSeparators(_p->info.outputFile)));
        _p->context->printError(error);
        cancel();
        return;
    }
    if ((_p->saveSequence.frames.count() - 1) == in)
    {
        try
        {
            //DJV_DEBUG_PRINT("close");
            _p->save->close();
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_WRITE_IMAGE].
                arg(QDir::toNativeSeparators(_p->info.outputFile)));
            _p->context->printError(error);
        }
    }
}

void djvViewFileSave::finishedCallback()
{
    //DJV_DEBUG("djvViewFileSave::finishedCallback");
    try
    {
        //DJV_DEBUG_PRINT("close");
        _p->save->close();
    }
    catch (djvError error)
    {
        error.add(
            djvViewUtil::errorLabels()[djvViewUtil::ERROR_WRITE_IMAGE].
            arg(QDir::toNativeSeparators(_p->info.outputFile)));
        _p->context->printError(error);
    }
    Q_EMIT finished();
}
