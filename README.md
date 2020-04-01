## 说明
一个简单的小程序，主要用来给flac格式的音乐文件添加自己喜欢的封面图片。

## 使用
```
make
./add_cover "path/to/flac" "path/to/image"
```
会在当前目录产生一个新的flac文件，即是添加了封面后的flac文件

## 待完成
功能很单一，只处理了flac的METADATA_BLOCK_PICTURE结构，其实有很多可以扩展的功能

## 参考
(FLAC格式说明文档)[https://xiph.org/flac/format.html#metadata_block_picture]
