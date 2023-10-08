;// 本文件由洪旭耀设计，而使用权分发权自由属于你！
;// 可以通过QQ联系本人：26750452【24小时在线守候】
;///////////////////////////////////////////////////////////////////////////////
            AREA    UENTRY, CODE, READONLY
            ARM

            IMPORT  Reset_Handler

            ;// 本代码模块仅由FEL加载，用于快速调试！
            ;// 由"sunxi-fel uboot file.bin"命令加载并运行后，SP被设置为0x00005E08
            B       Reset_Handler   ;// 跳转到startup代码
            B       Reset_Handler   ;// 跳转到startup代码
            B       Reset_Handler   ;// 跳转到startup代码
            B       Reset_Handler   ;// 跳转到startup代码

;///////////////////////////////////////////////////////////////////////////////
            END

