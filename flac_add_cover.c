/**
 * flac_add_cover.c 
 * @description 给flac文件添加封面
 * @author zhaohehe
 */ 

#include <stdio.h>
#include "flac_meta.h"

/**
 * 检查文件是否已经包含有封面图片信息
 * @param flac_meta_block 文件对应的META_BLOCK列表
 * @param meta_block_count META_BLOCK列表长度
 * @return has_picture
 */ 
short int check_picture(flac_meta_block *meta_block, int meta_block_count)
{
    short int has_picture = 0;

    for (int i = 0; i < meta_block_count; i++)
    {
        if (meta_block[i].block_header.block_type == BLOCK_TYPE_PICTURE)
        {
            has_picture = 1;
        }    
    }

    return has_picture;
}


int main(int argc, char *args[]) 
{   
    if (argc < 3)
    {
        printf("params error\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp_flac, *fp_pic;

    char *flac_file = args[1];    // flac文件路径
    char *pic_file  = args[2];    // 要设置的封面图片路径

    // 打开flac文件
    if ((fp_flac = fopen(flac_file, "rb+")) == NULL)
    {
        printf("can't open %s\n", flac_file);
        exit(EXIT_FAILURE);
    }

    // 打开图片文件
    if ((fp_pic = fopen(pic_file, "r+")) == NULL)
    {
        printf("can't open %s\n", pic_file);
        exit(EXIT_FAILURE);
    }
    
    // 解析
    int meta_block_count = parse_flac_meta(fp_flac);  

    if (!check_picture(meta_block, meta_block_count))
    {
        flac_meta_block_picture picture = generate_picture_meta_block(fp_pic);
        fp_flac = append_meta_block(fp_flac, picture);

        // 重新解析
        int meta_block_count = parse_flac_meta(fp_flac);  

        if (!check_picture(meta_block, meta_block_count))
        {
            printf("add picture failed\n");
            exit(EXIT_FAILURE);
        }   
        else
        {
            printf("add picture success !!\n");
        } 
    }

    fclose(fp_flac);
    fclose(fp_pic);

    return 0;
}