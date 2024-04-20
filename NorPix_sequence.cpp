#include "NorPix_sequence.h"
#include <string>
#include <ctime>
#include <time.h>
#include <iomanip>
//#include "opencv2/opencv.hpp"

#define HEADER_SIZE 1024
#define ALIGN_BOUNDAYRY_BYTES 8192

//using namespace cv;

NorPix_sequence::NorPix_sequence()
{

}

NorPix_sequence::~NorPix_sequence()
{
    if(m_read_file.is_open())
    {
        m_read_file.close();
    }

    if(m_write_file.is_open())
    {
        m_write_file.close();
    }
}

void NorPix_sequence::open_for_read(std::string filename)
{
    if(m_read_file.is_open())
    {
        m_read_file.close();

        m_read_file.open(filename, std::ios::binary);
    }
    else
    {
        m_read_file.open(filename, std::ios::binary);
        if(m_read_file.is_open())
        {
            std::cout<<"File is successfully opened!"<<std::endl;
        }
    }

    switch (errno)
    {
    case ENOENT: // Case for file not existing.
        std::cout<<"Error: the file doesn's exist."<<std::endl;
        break;
    case EACCES: // Case for access denied.
        std::cout<<"Error: Permission denied for the file."<<std::endl;
        break;
    default:    // Default case for other errors.
        std::cout<<"Error opening the file. errno = "<<std::to_string(errno)<<std::endl;
        break;
    }

    parse_header();
}

void NorPix_sequence::open_for_write(std::string filename)
{
    if(m_write_file.is_open())
    {
        m_write_file.close();

        m_write_file.open(filename, std::ios::out | std::ios::binary);
    }
    else
    {
        m_write_file.open(filename, std::ios::out | std::ios::binary);
        if(m_write_file.is_open())
        {
            std::cout<<"File is successfully opened!"<<std::endl;
        }
    }

    switch (errno)
    {
    case ENOENT: // Case for file not existing.
        std::cout<<"Error: the file doesn's exist."<<std::endl;
        break;
    case EACCES: // Case for access denied.
        std::cout<<"Error: Permission denied for the file."<<std::endl;
        break;
    default:    // Default case for other errors.
        std::cout<<"Error opening the file. errno = "<<std::to_string(errno)<<std::endl;
        break;
    }
}


void NorPix_sequence::close_for_read()
{
    if(m_read_file.is_open())
    {
        m_read_file.close();
    }
}


void NorPix_sequence::close_for_write()
{
    if(m_write_file.is_open())
    {
        m_write_file.close();
    }
}


void NorPix_sequence::parse_header()
{
    if(m_read_file.is_open())
    {
        m_read_file.seekg(0, std::ios::beg);

        char buffer[HEADER_SIZE];
        if (m_read_file.read(buffer, HEADER_SIZE)) {
            std::cout << "Sequence file header read successfully." << std::endl;
        }

        memset((void *)&m_read_file_header, 0, HEADER_SIZE);
        memcpy((void *)&m_read_file_header, (void*)buffer, HEADER_SIZE);
        m_read_image_numbers = m_read_file_header.AllocatedFrames;

        std::cout<<"MagicNumber: "<<m_read_file_header.MagicNumber<<std::endl;
        std::cout<<"TrueImageSize: "<<std::to_string(m_read_file_header.TrueImageSize)<<std::endl;
        std::cout<<"AllocatedFrames: "<<m_read_file_header.AllocatedFrames<<std::endl;

        std::cout<<"ImageWidth: "<<std::to_string(m_read_file_header.ImageInfo.ImageWidth)<<std::endl;
        std::cout<<"ImageHeight: "<<std::to_string(m_read_file_header.ImageInfo.ImageHeight)<<std::endl;
        std::cout<<"ImageBitDepth: "<<std::to_string(m_read_file_header.ImageInfo.ImageBitDepth)<<std::endl;
        std::cout<<"ImageBitDepthReal: "<<std::to_string(m_read_file_header.ImageInfo.ImageBitDepthReal)<<std::endl;
        std::cout<<"ImageSizeBytes: "<<std::to_string(m_read_file_header.ImageInfo.ImageSizeBytes)<<std::endl;
        std::cout<<"ImageFormat: "<<std::to_string(m_read_file_header.ImageInfo.ImageFormat)<<std::endl;
    }
    else
    {
        std::cout<<"File does not open!"<<std::endl;
    }
}


int NorPix_sequence::read_image(int index, void *buffer)
{
    if(m_read_file.is_open())
    {
        if(index >= m_read_image_numbers)
        {
            std::cout<<"image index is out of range......"<<std::endl;
            return 1;
        }

        size_t offset = ALIGN_BOUNDAYRY_BYTES + index * m_read_file_header.TrueImageSize;
        m_read_file.seekg(offset, std::ios::beg);

        m_read_file.read((char *)buffer, m_read_file_header.TrueImageSize);

        time_t timestamp_time_t;
        unsigned short timestamp_ms = 0;
        unsigned short timestamp_us = 0;

        memset(&timestamp_time_t, 0, 8);

        unsigned long ImageSizeBytes = m_read_file_header.ImageInfo.ImageSizeBytes;
        memcpy(&timestamp_time_t,  (char *)buffer + ImageSizeBytes, 4);
        memcpy(&timestamp_ms,  (char *)buffer + ImageSizeBytes + 4, 2);
        memcpy(&timestamp_us,  (char *)buffer + ImageSizeBytes + 6, 2);

        std::cout<<std::endl;

        char str[26];
        ctime_s(str,sizeof str,&timestamp_time_t);
        std::cout<<"ctime_s is: "<<str<<std::endl;
        std::cout<<"Milliseconds: "<<timestamp_ms<<std::endl;
        std::cout<<"Microseconds: "<<timestamp_us<<std::endl;

        return 0;
    }
    else
    {
        std::cout<<"File does not open!"<<std::endl;

        return 1;
    }
}

void NorPix_sequence::save_to_images(std::string saving_directory)
{
    if(m_read_file.is_open())
    {
        size_t offset;
        char *buffer = new char[m_read_file_header.TrueImageSize];

        unsigned long image_width;
        unsigned long image_height;

        image_width = m_read_file_header.ImageInfo.ImageWidth;
        image_height = m_read_file_header.ImageInfo.ImageHeight;

        std::string img_name_sequence = "";

        for(int i=0; i< m_read_image_numbers; i++)
        {
            memset(buffer, 0, m_read_file_header.TrueImageSize);
            offset = ALIGN_BOUNDAYRY_BYTES + i * m_read_file_header.TrueImageSize;
            m_read_file.seekg(offset, std::ios::beg);
            m_read_file.read(buffer, m_read_file_header.TrueImageSize);

            std::cout<<"Beginning......"<<i<<std::endl;

            //Mat img(image_height, image_width, CV_8UC1);
            //img.data = (unsigned char*)buffer;
            //img_name_sequence = saving_directory + "/img" + std::to_string(i) + ".png";
            //std::cout<<img_name_sequence<<std::endl;
            //imwrite(img_name_sequence, img);
        }

        std::cout<<"Save sequence file complete......"<<std::endl;

        delete []buffer;
    }
    else
    {
        std::cout<<"File does not open!"<<std::endl;
    }
}


void NorPix_sequence::write_header()
{
    m_write_file_header.MagicNumber = 0xFEED;

    std::wstring name(L"Norpix seq");
    const wchar_t* szName = name.c_str();
    std::cout<<"sizeof(wchar_t) is "<<sizeof(wchar_t)<<std::endl;
    std::cout<<"wcslen(szName) is "<<std::wcslen(szName)<<std::endl;
    memset(m_write_file_header.Name, 0, 12*sizeof(wchar_t));
    memcpy(m_write_file_header.Name, szName, 2*std::wcslen(szName));

    m_write_file_header.Version = 5;
    m_write_file_header.HeaderSize = 1024;
    m_write_file_header.Description[0] = 'S';

    //later write m_seq_write_file_header.ImageInfo
    //later write m_seq_write_file_header.AllocatedFrames
    m_write_file_header.Origin = 0;
    //later write m_seq_write_file_header.TrueImageSize
    //later write m_seq_write_file_header.FrameRate
    m_write_file_header.DescriptionFormat = 0;
    m_write_file_header.ReferenceFrame = 0;
    m_write_file_header.FixedSize = 0;
    m_write_file_header.Flags = 16;
    m_write_file_header.BayerPattern = 0;
    m_write_file_header.TimeOffsetUS = 0;
    m_write_file_header.ExtendedHeaderSize = 0;
    m_write_file_header.CompressionFormat = H_COMPRESSION_NONE;
    m_write_file_header.ReferenceTime = 0;
    m_write_file_header.ReferenceTimeMS = 0;
    m_write_file_header.ReferenceTimeUS = 0;
    m_write_file_header.H264GOP = 30;
    m_write_file_header.H264Bitrate = 5000000;
    m_write_file_header.JPEGQualityInfo = 2147483648;
    m_write_file_header.H264DecodeFormat = H_IMAGE_UNKNOWN;
    m_write_file_header.IndexOffset = 0;
    m_write_file_header.OldestFrameIndex = 0;
    m_write_file_header.BytesAlignment = ALIGN_BOUNDAYRY_BYTES;

    char header_data[ALIGN_BOUNDAYRY_BYTES];
    memset((void*)header_data, 0, ALIGN_BOUNDAYRY_BYTES);
    memcpy((void*)header_data, (void*)&m_write_file_header, sizeof(m_write_file_header));
    m_write_file.write(header_data, ALIGN_BOUNDAYRY_BYTES);
}

void NorPix_sequence::write_image(void *buffer)
{
    m_write_file.write(reinterpret_cast<char*>(buffer), m_write_file_header.TrueImageSize);
}


void NorPix_sequence::write_images()
{
    char* buffer;
    m_write_image_numbers = image_data_ptr_queue.size();
    for(int i = 0; i < m_write_image_numbers; i ++)
    {
        buffer = image_data_ptr_queue.front();
        m_write_file.write(buffer, m_write_file_header.TrueImageSize);
        image_data_ptr_queue.pop();
        delete buffer;
    }
}

unsigned long NorPix_sequence::calculate_TrueImageSize(unsigned long ImageSizeBytes)
{
    unsigned long TrueImageSize;

    TrueImageSize = ImageSizeBytes + 8;
    while((TrueImageSize % 8192) != 0)
    {
        TrueImageSize++;
    }

    return TrueImageSize;
}

void NorPix_sequence::clear_image_data_ptr_queue()
{
    while (!image_data_ptr_queue.empty())
    {
        image_data_ptr_queue.pop();
    }
}


