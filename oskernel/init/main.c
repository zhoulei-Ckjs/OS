void kernel_main(void)
{
    char *p = "[init::main.c] : in kernel_main ...";
    char* video = (char*)0xb8000 + 80 * 2 * 2;       //显示在第三行，0xb8000是显存和内存的映射位置，每行80字符，每个字符2字节
    while(*p != '\0')
    {
        *video = *p;
        ++video;
        *video = 12;
        ++video;
        p++;
    }
}