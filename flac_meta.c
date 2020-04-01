/**
 * flac_meta.c 解析flac格式文件的META_BLOCK结构
 * @description 解析flac的META_BLOCK列表，追加图片信息到META_BLOCK列表中
 * @author zhaohehe
 */ 

#include "flac_meta.h"

/**
 * 校验flac文件的文件标记
 * @param fp flac文件指针
 */ 
void check_flac_marker(FILE *fp)
{
    char tag[5];
    int tag_size = sizeof(FLAC_FILE_TAG) - 1;    // 减去'\0'
    if (fread(tag, sizeof(char), tag_size, fp) < tag_size)
    {
        printf("can't read flac file\n");
        exit(EXIT_FAILURE);  
    }
    tag[4] = '\0';
    if (strcmp(tag, FLAC_FILE_TAG) != 0)
    {
        printf("is not a flac file\n");
        exit(EXIT_FAILURE);  
    }
}

/**
 * 解析flac文件的META_BLOCK列表
 * @param fp flac文件指针
 * @return int META_BLOCK数量
 */ 
int parse_flac_meta(FILE *fp) 
{
    char *file_content;

    // 检查flac标志
    check_flac_marker(fp);

    // 解析 STREAMINFO
    unsigned char tmp_header[4];
    int is_fanial_block = 0;
    int i = 0;
   
    while (!is_fanial_block)
    {
        insert_position = ftell(fp);

        fread(tmp_header, sizeof(char), 4, fp);

        // 设置METADATA_BLOCK_HEADER
        meta_block[i].block_header.is_final   = is_fanial_block = tmp_header[0] >> 7;
        meta_block[i].block_header.block_type = tmp_header[0] << 1 >> 1;
        meta_block[i].block_header.block_size = tmp_header[1] * 256 * 256 + tmp_header[2] * 256 + tmp_header[3];

        // 为METADATA_BLOCK_DATA分配内存
        meta_block[i].block_data_ptr = malloc(meta_block[i].block_header.block_size);
        fread(meta_block[i].block_data_ptr, meta_block[i].block_header.block_size, 1, fp);
        
        i ++;
    }
   
    return i;
}

/**
 * 解析图片文件生成METADATA_BLOCK_PICTURE结构
 * @param fp 图片文件指针
 * @return flac_meta_block_picture METADATA_BLOCK_PICTURE
 */ 
flac_meta_block_picture generate_picture_meta_block(FILE *fp)
{
    flac_meta_block_picture picture;

    // 初始化METADATA_BLOCK_PICTURE结构, 这些字段可以允许用户输入或读取图片元数据，目前先写死
    picture.picture_type = PICTURE_TYPE_FRONT_COVER;
    picture.description = "test description";
    picture.mime = "image/jpeg";
    picture.index_color_count = 0;
    picture.color_depth = 24;
    picture.width = 240;
    picture.height = 240;

    picture.mime_length = strlen(picture.mime);
    picture.description_length = strlen(picture.description);

    // 读取图片内容
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    char *image_data = malloc(size);
    rewind(fp);
    if (fread(image_data, sizeof(char), size, fp) < size)
    {
        printf("read picture file failed\n");
        exit(EXIT_FAILURE);  
    }

    picture.image_data = image_data;
    picture.image_data_length = size;

    return picture;
}

/**
 * 写入一个32位证书到文件中
 * 由于flac文件的字节序为大端序，为保证写入的字节顺序正确，将32位整数分解为字节数组逐字节写入
 * @param fp 要写入的文件指针
 * @param number 要写入的数字
 */ 
FILE* write_int_32(FILE *fp, unsigned int number)
{
    char char_array[4];

    char_array[0] = number >> 24;
    char_array[1] = number << 8 >> 24;
    char_array[2] = number << 16 >> 24;
    char_array[3] = number << 24 >> 24;

    fwrite(char_array, 1, 4, fp);

    return fp;
}

/**
 * 插入一个METADATA_BLOCK_PICTURE结构到flac文件中
 * @param fp falc文件指针
 * @param picture METADATA_BLOCK_PICTURE
 * @return 写入成功后的新文件指针
 */ 
FILE* append_meta_block(FILE *fp, flac_meta_block_picture picture)
{
    FILE *new_flac = fopen("./music_out.flac", "wb+");
 
    // 写入原始flac文件前半部的内容
    fseek(fp, 0, SEEK_END);
    long old_flac_length = ftell(fp);

    rewind(fp);
    char *before_insert_position = (char*) malloc(old_flac_length);
    fread(before_insert_position, 1, old_flac_length, fp);

    rewind(new_flac);
    fwrite(before_insert_position, 1, insert_position, new_flac);

    int block_size = 32 + picture.mime_length + picture.description_length + picture.image_data_length;

    char block_header[4];
    block_header[0] = BLOCK_TYPE_PICTURE;
    block_header[1] = block_size << 8 >> 24;
    block_header[2] = block_size << 16 >> 24;
    block_header[3] = block_size << 24 >> 24;
    fwrite(&block_header, sizeof(char), 4, new_flac);

    // 写METADATA_BLOCK_PICTURE todo
    write_int_32(new_flac, picture.picture_type);
    write_int_32(new_flac, picture.mime_length);
    fwrite(picture.mime, sizeof(char), picture.mime_length, new_flac);
    write_int_32(new_flac, picture.description_length);
    fwrite(picture.description, sizeof(char), picture.description_length, new_flac);
    write_int_32(new_flac, picture.width);
    write_int_32(new_flac, picture.height);
    write_int_32(new_flac, picture.color_depth);
    write_int_32(new_flac, picture.index_color_count);
    write_int_32(new_flac, picture.image_data_length);
    fwrite(picture.image_data, sizeof(char), picture.image_data_length, new_flac);

    // 写入原始flac文件后半部的内容
    fseek(fp, insert_position, SEEK_SET);
    long after_insert_position_length = old_flac_length - insert_position;
    char *after_insert_postion = (char*) malloc(after_insert_position_length);
    fread(after_insert_postion, 1, after_insert_position_length, fp);

    fwrite(after_insert_postion, 1, after_insert_position_length, new_flac);

    rewind(new_flac);
    return new_flac;
}