;// 本文件由洪旭耀设计，而使用权分发权自由属于你！
;// 可以通过QQ联系本人：26750452【24小时在线守候】
;///////////////////////////////////////////////////////////////////////////////
            AREA    UHEAD, DATA, READONLY

            IMPORT  ||Load$$EXE_CODE$$Length||
            IMPORT  ||Image$$EXE_CODE$$Base||
            IMPORT  Reset_Handler

            ;// 以下DCD定义的32位常数都要靠定制版mksunxi来交换为大端字节序！

            DCD     0x27051956                      ;// ih_magic
            DCD     0                               ;// ih_hcrc
            DCD     0                               ;// ih_time
            DCD     ||Load$$EXE_CODE$$Length||      ;// ih_size
            DCD     ||Image$$EXE_CODE$$Base||       ;// ih_load
            DCD     Reset_Handler                   ;// ih_ep
            DCD     0                               ;// ih_dcrc
            DCD     0x11020500                      ;// ih_os etc.
            DCB     "DESIGNER: Xuyao Hong, QQ26750452"

;///////////////////////////////////////////////////////////////////////////////
            END

