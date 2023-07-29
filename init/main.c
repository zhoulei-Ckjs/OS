void main(void)
{
    char *p = "[init::main.c] : in main ...";
    /// 显示在第三行，0xb8000 是显存和内存的映射位置，每行 80 字符，每个字符 2 字节。
    char* video = (char*)0xb8000 + 80 * 2 * 4;
    while(*p != '\0')
    {
        *video = *p;
        ++video;
        *video = 12;
        ++video;
        p++;
    }
}