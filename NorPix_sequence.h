#ifndef NORPIX_SEQUENCE_H
#define NORPIX_SEQUENCE_H

#include <iostream>
#include <fstream>
#include <queue>

enum eHCompression
{
    H_COMPRESSION_NONE          =   0,
    H_COMPRESSION_JPEG,
    H_COMPRESSION_RLE,
    H_COMPRESSION_HUFFMAN,
    H_COMPRESSION_LZ,
    H_COMPRESSION_RLE_FAST,
    H_COMPRESSION_HUFFMAN_FAST,
    H_COMPRESSION_LZ_FAST,
    H_COMPRESSION_H264,
    H_COMPRESSION_WAVELET
};

enum eHMetadataFormat
{
    H_METADATA_UNKNOWN  = 0,
    H_METADATA_BOOL,            //bool (1 byte)
    H_METADATA_BYTE,            //byte (1 byte)
    H_METADATA_SHORT,           //short (2 bytes)
    H_METADATA_USHORT,          //unsigned short (2 bytes)
    H_METADATA_INT,             //int (4 bytes)
    H_METADATA_UINT,            //unsigned int (4 bytes)
    H_METADATA_DOUBLE,          //double (8 bytes)
    H_METADATA_STRING,          //wchar_t[] (variable)
    H_METADATA_BINARY,          //BYTE[] (variable)
    H_METADATA_INT64,           //__int64 (8 bytes)
    H_METADATA_UINT64           //unsigned __int64 (8 bytes)
};

enum eHImageFormat
{
    H_IMAGE_UNKNOWN                 = 0,    //Unknown format
    H_IMAGE_MONO                    = 100,  //Monochrome Image (LSB)
    H_IMAGE_MONO_BAYER              = 101,  //Raw Bayer Image (treated as H_IMAGE_MONO)
    H_IMAGE_BGR                     = 200,  //BGR Color Image
    H_IMAGE_PLANAR                  = 300,  //Planar Color Image
    H_IMAGE_RGB                     = 400,  //RGB Color Image
    H_IMAGE_BGRx                    = 500,  //BGRx Color Image
    H_IMAGE_YUV422                  = 600,  //YUV422
    H_IMAGE_YUV422_20               = 610,
    H_IMAGE_UVY422                  = 700,  //UVY422
    H_IMAGE_UVY411                  = 800,  //UVY411
    H_IMAGE_UVY444                  = 900,  //UVY444
    H_IMAGE_BGR555_PACKED           = 905,  //PhynxRGB
    H_IMAGE_BGR565_PACKED           = 906,
    H_IMAGE_MONO_MSB                = 112,  //Only for > 8 bit per pixel, MSB align litle endian 10 bit: JIHGFEDC BA000000
    H_IMAGE_MONO_BAYER_MSB          = 113,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_MONO_MSB_SWAP           = 114,  //Only for > 8 bit per pixel, MSB align big endian 10 bit: BA000000 JIHGFEDC
    H_IMAGE_MONO_BAYER_MSB_SWAP     = 115,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_BGR10_PPACKED           = 123,  //Only for 10 bit per pixel, LSB align
    H_IMAGE_BGR10_PPACKED_PHOENIX   = 124,  //Only for 10 bit per pixel, LSB align, RRRRRRRR RR00GGGG GGGGGGBB BBBBBBBB
    H_IMAGE_RGB10_PPACKED_PHOENIX   = 125,  //Only for 10 bit per pixel, LSB align, BBBBBBBB BB00GGGG GGGGGGRR RRRRRRRR
    H_IMAGE_MONO_PPACKED            = 131,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_MONO_BAYER_PPACKED      = 132,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_MONO_PPACKED_8448       = 133,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_MONO_BAYER_PPACKED_8448 = 134,  //Only for > 8 bit per pixel, MSB align
    H_IMAGE_GVSP_BGR10V1_PACKED     = 135,  //Only for 10 bit per pixel(From Gige Vision) BBBBBBBB GGGGGGGG RRRRRRRR 00BBGGRR
    H_IMAGE_GVSP_BGR10V2_PACKED     = 136,  //Only for 10 bit per pixel(From Gige Vision)00BBBBBB BBGGGGGG GGGGRRRR RRRRRRRR
    H_IMAGE_BASLER_VENDOR_SPECIFIC  = 1000,
    H_IMAGE_EURESYS_JPEG            = 1001,
    H_IMAGE_ISG_JPEG                = 1002
};

typedef struct{
    unsigned long ImageWidth;
    unsigned long ImageHeight;
    unsigned long ImageBitDepth;
    unsigned long ImageBitDepthReal;
    unsigned long ImageSizeBytes;
    eHImageFormat ImageFormat;
}CImageInfo;

typedef struct{
    long MagicNumber;
    wchar_t Name[12];
    long Version;
    long HeaderSize;
    unsigned char Description[512];
    CImageInfo ImageInfo;
    unsigned long AllocatedFrames;
    unsigned long Origin;
    unsigned long TrueImageSize;
    double FrameRate;
    long DescriptionFormat;
    unsigned long ReferenceFrame;
    unsigned long FixedSize;
    unsigned long Flags;
    long BayerPattern;
    long TimeOffsetUS;
    long ExtendedHeaderSize;
    eHCompression CompressionFormat;
    long ReferenceTime;
    unsigned short ReferenceTimeMS;
    unsigned short ReferenceTimeUS;
    unsigned long H264GOP;
    unsigned long H264Bitrate;
    unsigned long JPEGQualityInfo;
    eHImageFormat H264DecodeFormat;
    long long IndexOffset;
    unsigned long OldestFrameIndex;
    unsigned long BytesAlignment;
    unsigned char Padding[360];
}NORPIX_SEQUENCE_FILE_HEADER_STRUCT;


class NorPix_sequence
{
public:
    NorPix_sequence();
    ~NorPix_sequence();

    void open_for_read(std::string filename);
    void open_for_write(std::string filename);

    void close_for_read();
    void close_for_write();

    void parse_header();

    int read_image(int index, void *buffer);
    void read_images();

    void read_timestamp();

    void write_header();
    unsigned long calculate_TrueImageSize(unsigned long ImageSizeBytes);
    void clear_image_data_ptr_queue();
    void write_image(void *buffer);
    void write_images();

    void save_to_images(std::string saving_directory);
private:
    std::ifstream m_read_file;
    std::ofstream m_write_file;

public:
    NORPIX_SEQUENCE_FILE_HEADER_STRUCT m_read_file_header;
    NORPIX_SEQUENCE_FILE_HEADER_STRUCT m_write_file_header;

    std::queue<char*> image_data_ptr_queue;

    unsigned int m_read_image_numbers;
    unsigned int m_write_image_numbers;
};

#endif // NORPIX_SEQUENCE_H
