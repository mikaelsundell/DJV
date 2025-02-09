// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/JPEG.h>

#include <djvSystem/File.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace JPEG
        {
            class Read::File
            {
                DJV_NON_COPYABLE(File);

                File()
                {
                    memset(&jpeg, 0, sizeof(jpeg_decompress_struct));
                }

            public:
                ~File()
                {
                    if (jpegInit)
                    {
                        jpeg_destroy_decompress(&jpeg);
                        jpegInit = false;
                    }
                    if (f)
                    {
                        fclose(f);
                        f = nullptr;
                    }
                }

                static std::shared_ptr<File> create()
                {
                    return std::shared_ptr<File>(new File);
                }

                FILE*                  f         = nullptr;
                jpeg_decompress_struct jpeg;
                bool                   jpegInit  = false;
                JPEGErrorStruct        jpegError;
            };

            Read::Read()
            {}

            Read::~Read()
            {
                _finish();
            }

            std::shared_ptr<Read> Read::create(
                const System::File::Info& fileInfo,
                const IO::ReadOptions& readOptions,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                auto out = std::shared_ptr<Read>(new Read);
                out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                return out;
            }

            IO::Info Read::_readInfo(const std::string& fileName)
            {
                auto f = File::create();
                return _open(fileName, f);
            }

            namespace
            {
                bool jpegScanline(
                    jpeg_decompress_struct* jpeg,
                    uint8_t*                out,
                    JPEGErrorStruct*        error)
                {
                    if (::setjmp(error->jump))
                    {
                        return false;
                    }
                    JSAMPROW p[] = { (JSAMPLE*)(out) };
                    if (!jpeg_read_scanlines(jpeg, p, 1))
                    {
                        return false;
                    }
                    return true;
                }

                bool jpegEnd(
                    jpeg_decompress_struct* jpeg,
                    JPEGErrorStruct*        error)
                {
                    if (::setjmp(error->jump))
                    {
                        return false;
                    }
                    jpeg_finish_decompress(jpeg);
                    return true;
                }

            } // namespace

            std::shared_ptr<Image::Data> Read::_readImage(const std::string& fileName)
            {
                // Open the file.
                auto f = File::create();
                const auto info = _open(fileName, f);

                // Read the file.
                auto out = Image::Data::create(info.video[0]);
                out->setPluginName(pluginName);
                for (uint16_t y = 0; y < info.video[0].size.h; ++y)
                {
                    if (!jpegScanline(&f->jpeg, out->getData(y), &f->jpegError))
                    {
                        std::vector<std::string> messages;
                        messages.push_back(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_read_scanline"))));
                        for (const auto& i : f->jpegError.messages)
                        {
                            messages.push_back(i);
                        }
                        throw System::File::Error(String::join(messages, ' '));
                    }
                }
                if (!jpegEnd(&f->jpeg, &f->jpegError))
                {
                    std::vector<std::string> messages;
                    messages.push_back(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_file_close"))));
                    for (const auto& i : f->jpegError.messages)
                    {
                        messages.push_back(i);
                    }
                    throw System::File::Error(String::join(messages, ' '));
                }

                // Log any warnings.
                for (const auto& i : f->jpegError.messages)
                {
                    _logSystem->log(
                        pluginName,
                        String::Format("{0}: {1}").arg(fileName).arg(i),
                        System::LogLevel::Warning);
                }

                return out;
            }

            namespace
            {
                bool jpegInit(
                    jpeg_decompress_struct* jpeg,
                    JPEGErrorStruct*        error)
                {
                    if (::setjmp(error->jump))
                    {
                        return false;
                    }
                    jpeg_create_decompress(jpeg);
                    return true;
                }

                bool jpegOpen(
                    FILE*                   f,
                    jpeg_decompress_struct* jpeg,
                    JPEGErrorStruct*        error)
                {
                    if (::setjmp(error->jump))
                    {
                        return false;
                    }
                    jpeg_stdio_src(jpeg, f);
                    jpeg_save_markers(jpeg, JPEG_COM, 0xFFFF);
                    if (!jpeg_read_header(jpeg, static_cast<boolean>(1)))
                    {
                        return false;
                    }
                    if (!jpeg_start_decompress(jpeg))
                    {
                        return false;
                    }
                    return true;
                }

            } // namespace

            IO::Info Read::_open(const std::string& fileName, const std::shared_ptr<File>& f)
            {
                f->jpeg.err = jpeg_std_error(&f->jpegError.pub);
                f->jpegError.pub.error_exit = djvJPEGError;
                f->jpegError.pub.emit_message = djvJPEGWarning;
                if (!jpegInit(&f->jpeg, &f->jpegError))
                {
                    std::vector<std::string> messages;
                    messages.push_back(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                    for (const auto& i : f->jpegError.messages)
                    {
                        messages.push_back(i);
                    }
                    throw System::File::Error(String::join(messages, ' '));
                }
                f->jpegInit = true;
                f->f = System::File::fopen(fileName, "rb");
                if (!f->f)
                {
                    throw System::File::Error(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                }
                if (!jpegOpen(f->f, &f->jpeg, &f->jpegError))
                {
                    std::vector<std::string> messages;
                    messages.push_back(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_file_open"))));
                    for (const auto& i : f->jpegError.messages)
                    {
                        messages.push_back(i);
                    }
                    throw System::File::Error(String::join(messages, ' '));
                }

                Image::Type imageType = Image::getIntType(f->jpeg.out_color_components, 8);
                if (Image::Type::None == imageType)
                {
                    throw System::File::Error(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_unsupported_color_components"))));
                }
                IO::Info info;
                info.fileName = fileName;
                info.videoSpeed = _speed;
                info.videoSequence = _sequence;
                info.video.push_back(Image::Info(f->jpeg.output_width, f->jpeg.output_height, imageType));

                const jpeg_saved_marker_ptr marker = f->jpeg.marker_list;
                if (marker)
                {
                    info.tags.set("Description", std::string((const char*)marker->data, marker->data_length));
                }

                return info;
            }

        } // namespace JPEG
    } // namespace AV
} // namespace djv

