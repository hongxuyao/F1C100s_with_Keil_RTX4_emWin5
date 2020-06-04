;// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
;///////////////////////////////////////////////////////////////////////////////
            AREA    SPLFILE, CODE, READONLY

            ALIGN   4
            IF      :DEF:PCB_TINY200V2
            INCBIN  ..\output\f1c100s-spl_uart1.bin
            ELSE
            INCBIN  ..\output\f1c100s-spl_uart0.bin
            ENDIF

;///////////////////////////////////////////////////////////////////////////////
            END

