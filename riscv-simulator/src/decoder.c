#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "alu.h"
#include "decoder.h"

typedef struct 
{
    const char *name;
    int index;
} RegAlias;

static const RegAlias reg_aliases[] = {
    {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4},
    {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"fp", 8},
    {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13},
    {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17},
    {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22},
    {"s7", 23}, {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27},
    {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31},
    {NULL, -1}
};

static void trim_inplace(char *s)
{
    if(!s) 
        return;

    char *start = s;
    while (*start && isspace((unsigned char)*start)) 
        start++;

    if(start != s) 
        memmove(s, start, strlen(start) + 1);

    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) 
        s[--len] = '\0';
}

static void to_lower_inplace(char *s)
{
    for (char *p = s; *p; ++p) 
        *p = (char)tolower((unsigned char)*p);
}

int reg_index(const char *name) 
{
    if(!name)
        return -1;
    
    char tmp[64];
    strncpy(tmp, name, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    trim_inplace(tmp);
    if(tmp[0] == '\0')
        return -1;

    // handle xn or Xn
    if(tmp[0] == 'x' || tmp[0] == 'X')
    { 
        const char *num = tmp + 1;
        if(*num == '\0') 
            return -1;
        
        char *endp = NULL;
        long v = strtol(num, &endp, 10);
        if(*endp != '\0') 
            return -1;
        if(v < 0 || v > 31) 
            return -1;
        return (int)v;
    }

    // case-sensitive compare for aliases
    to_lower_inplace(tmp);
    for(int i = 0; reg_aliases[i].name; i++) 
    {
        if(strcmp(tmp, reg_aliases[i].name) == 0)
            return reg_aliases[i].index;
    }
    return -1;
}

int32_t parse_immediate(const char *str)
{
    if(!str || strlen(str) == 0)
        return 0;

    const char *start = str;

    if(str[0] == '#')
        start = str + 1;

    return (int32_t)strtol(start, NULL, 0);
}

int parse_memory_operand(const char *operand, int32_t *out_offset, int *out_reg)
{
    if(!operand || !out_offset || !out_reg)
        return -1;

    char *paren = strchr(operand, '(');
    if(!paren)
    {
        printf("[ERROR] Invalid memory operand format (expected 'offset(register)'): %s\n", operand);
        return -1;
    }

    char offset_str[32];
    strncpy(offset_str, operand, paren - operand);
    offset_str[paren - operand] = '\0';
    *out_offset = parse_immediate(offset_str);

    char *close_paren = strchr(paren, ')');
    if(!close_paren)
    {
        printf("[ERROR] Missing closing parenthesis in memory operand: %s\n", operand);
        return -1;
    }

    char reg_str[32];
    strncpy(reg_str, paren + 1, close_paren - paren - 1);
    reg_str[close_paren - paren - 1] = '\0';

    *out_reg = reg_index(reg_str);
    if(*out_reg < 0)
    {
        printf("[ERROR] Invalid register in memory operand: %s\n", reg_str);
        return -1;
    }

    return 0;
}
