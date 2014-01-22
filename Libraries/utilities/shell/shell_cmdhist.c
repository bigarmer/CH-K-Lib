/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "shell.h"

/*******************************************************************************
 * Defination
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
 
 /*******************************************************************************
 * Code
 ******************************************************************************/
#ifdef SHELL_CONFIG_USE_HIST
static int DoHist(int argc, char * const argv[])
{
    uint8_t num, cur_index;
    uint8_t i = 0;
    uint8_t index = 0;
    char ** pplist = shell_get_hist_data_list(&num, &cur_index);
    shell_printf("history: MAX:%d CUR:%d\r\n", num, cur_index);
    if (cur_index >= num)
    {
        return CMD_RET_SUCCESS;
    }
    for (i = cur_index; i < num; i++)
    {
        if(pplist[i] != NULL && pplist[i][0] != '\0')
        {
            shell_printf("(%d) %s\r\n", i - cur_index, pplist[i]); 
        }       
    }
    for (i = 0; i < cur_index; i++)
    {
        if(pplist[i] != NULL && pplist[i][0] != '\0')
        {
            shell_printf("(%d) %s\r\n", num - cur_index + i, pplist[i]);  
        }
    }
    return CMD_RET_SUCCESS;
}

const cmd_tbl_t CommandFun_Hist = 
{
    .name = "history",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = DoHist,
    .usage = "print history",
    .complete = NULL,
    .help = NULL,
};
#endif

 /*******************************************************************************
 * EOF
 ******************************************************************************/

