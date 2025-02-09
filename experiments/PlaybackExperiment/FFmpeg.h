// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

extern "C"
{
#include <libavutil/samplefmt.h>

} // extern "C"

struct AVPacket;
struct AVRational;

namespace IO
{
    class FFmpegRead : public IRead
    {
        DJV_NON_COPYABLE(FFmpegRead);

    protected:
        void _init(
            const djv::System::File::Info&,
            const std::shared_ptr<djv::System::LogSystem>&);
        FFmpegRead();

    public:
        ~FFmpegRead() override;

        static std::shared_ptr<FFmpegRead> create(
            const djv::System::File::Info&,
            const std::shared_ptr<djv::System::LogSystem>&);

        std::future<std::shared_ptr<Info> > getInfo() override;

        void seek(Timestamp) override;

    private:
        void _init(const djv::System::File::Info&);
        void _work();
        void _cleanup();

        void _seek(
            AVPacket*,
            Timestamp seekTimestamp,
            std::queue<VideoFrame>&,
            std::queue<AudioFrame>&);
        void _read(
            AVPacket*,
            Timestamp seekTimestamp,
            std::queue<VideoFrame>&,
            std::queue<AudioFrame>&);
        int _readVideo(AVPacket*, Timestamp seek, std::queue<VideoFrame>&);
        int _readAudio(AVPacket*, Timestamp seek, std::queue<AudioFrame>&);

        DJV_PRIVATE();
    };

    class FFmpegPlugin : public IPlugin
    {
        DJV_NON_COPYABLE(FFmpegPlugin);

    protected:
        FFmpegPlugin(const std::shared_ptr<djv::System::LogSystem>&);

    public:
        ~FFmpegPlugin() override;

        static std::shared_ptr<FFmpegPlugin> create(const std::shared_ptr<djv::System::LogSystem>&);

        bool canRead(const djv::System::File::Info&) override;
        std::shared_ptr<IRead> read(const djv::System::File::Info&) override;
    };
    
    AVRational toFFmpeg(const djv::Math::IntRational& value);

    djv::Math::IntRational fromFFmpeg(const AVRational& value);

    djv::Audio::Type fromFFmpeg(AVSampleFormat value);

    void extractAudio(
        uint8_t** inData,
        int inFormat,
        uint8_t inChannelCount,
        std::shared_ptr<djv::Audio::Data>);

} // namespace IO
