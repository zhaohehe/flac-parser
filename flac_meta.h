/**
 * flac_meta.h 解析flac格式文件的META_BLOCK结构
 * @description 定义了flac文件结构的结构体、常量已经相关函数的声明
 * @author zhaohehe
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* META_BLOCK最大数量 */
#define META_BLOCK_MAX_SIZE 128

/* META_BLOCK的类型 */
#define BLOCK_TYPE_STREAMINFO 1
#define BLOCK_TYPE_PICTURE 6

/* METADATA_BLOCK_PICTURE的类型 */
#define PICTURE_TYPE_FRONT_COVER 3
#define PICTURE_TYPE_BAND 10

/* flac格式文件的文件标记，即文件的前4个字符 */
#define FLAC_FILE_TAG "fLaC"

/**
 * METADATA_BLOCK_HEADER
 */
typedef struct _flac_meta_block_header
{
    unsigned int is_final : 1;
    unsigned int block_type : 7;
    unsigned int block_size : 24;
} flac_meta_block_header;

/**
 * METADATA_BLOCK 
 */
typedef struct _flac_meta_block
{
    struct _flac_meta_block_header block_header;
    void *block_data_ptr;
    long position;    // 当前META在文件中的偏移量
} flac_meta_block;

/**
 *  METADATA_BLOCK_PICTURE
 */
typedef struct _flac_meta_block_picture
{
    unsigned int picture_type;
    unsigned int mime_length;
    char *mime;
    unsigned int description_length;
    char *description;
    unsigned int width;
    unsigned int height;
    unsigned int color_depth;
    unsigned int index_color_count;
    unsigned int image_data_length;
    char *image_data;
} flac_meta_block_picture;

// 插入图片block的位置，即最后一个METADATA_BLOCK的起始位置
long insert_position;

// flac文件解析后得到的METADATA_BLOCK数组，全局变量
struct _flac_meta_block meta_block[META_BLOCK_MAX_SIZE];

// 解析falc文件，返回METADATA_BLOCK的实际数量
int parse_flac_meta(FILE *fp);

// 解析图片成METADATA_BLOCK_PICTURE结构
flac_meta_block_picture generate_picture_meta_block(FILE *fp);

// 将一个METADATA_BLOCK_PICTURE结构追加到falc文件的METADATA_BLOCK中
FILE* append_meta_block(FILE *fp, flac_meta_block_picture picture);