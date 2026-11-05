/*
  cJSON implementation for GemTrio Mod Loader
  License: MIT
*/
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *global_error = NULL;

const char *cJSON_GetErrorPtr(void)
{
    return global_error;
}

static void *cJSON_malloc(size_t sz)
{
    return malloc(sz);
}

static void cJSON_free(void *ptr)
{
    free(ptr);
}

static cJSON *cJSON_New_Item(void)
{
    cJSON *node = (cJSON*)cJSON_malloc(sizeof(cJSON));
    if (node)
        memset(node, 0, sizeof(cJSON));
    return node;
}

void cJSON_Delete(cJSON *c)
{
    cJSON *next;
    while (c)
    {
        next = c->next;
        if (!(c->type & cJSON_IsReference) && c->child)
            cJSON_Delete(c->child);
        if (!(c->type & cJSON_IsReference) && c->valuestring)
            cJSON_free(c->valuestring);
        if (!(c->type & cJSON_StringIsConst) && c->string)
            cJSON_free(c->string);
        cJSON_free(c);
        c = next;
    }
}

static const char *skip_whitespace(const char *in)
{
    while (in && *in && (unsigned char)*in <= 32)
        in++;
    return in;
}

static const char *parse_value(cJSON *item, const char *value);

static const char *parse_string(cJSON *item, const char *str)
{
    const char *ptr = str + 1;
    char *ptr2;
    char *out;
    int len = 0;

    if (*str != '\"')
    {
        global_error = str;
        return NULL;
    }

    while (*ptr != '\"' && *ptr && ++len)
    {
        if (*ptr++ == '\\')
            ptr++;
    }

    out = (char*)cJSON_malloc(len + 1);
    if (!out) return NULL;

    ptr = str + 1;
    ptr2 = out;
    while (*ptr != '\"' && *ptr)
    {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else
        {
            ptr++;
            switch (*ptr)
            {
                case 'b': *ptr2++ = '\b'; break;
                case 'f': *ptr2++ = '\f'; break;
                case 'n': *ptr2++ = '\n'; break;
                case 'r': *ptr2++ = '\r'; break;
                case 't': *ptr2++ = '\t'; break;
                case '\"': case '\\': case '/': *ptr2++ = *ptr; break;
                default: *ptr2++ = *ptr; break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"') ptr++;
    item->valuestring = out;
    item->type = cJSON_String;
    return ptr;
}

static const char *parse_number(cJSON *item, const char *num)
{
    double n = 0;
    char *endptr;
    n = strtod(num, &endptr);
    if (endptr == num)
    {
        global_error = num;
        return NULL;
    }
    item->valuedouble = n;
    item->valueint = (int)n;
    item->type = cJSON_Number;
    return endptr;
}

static const char *parse_array(cJSON *item, const char *value)
{
    cJSON *child;
    if (*value != '[')
    {
        global_error = value;
        return NULL;
    }

    item->type = cJSON_Array;
    value = skip_whitespace(value + 1);
    if (*value == ']') return value + 1;

    item->child = child = cJSON_New_Item();
    if (!item->child) return NULL;
    value = skip_whitespace(parse_value(child, skip_whitespace(value)));
    if (!value) return NULL;

    while (*value == ',')
    {
        cJSON *new_item = cJSON_New_Item();
        if (!new_item) return NULL;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip_whitespace(parse_value(child, skip_whitespace(value + 1)));
        if (!value) return NULL;
    }

    if (*value == ']') return value + 1;
    global_error = value;
    return NULL;
}

static const char *parse_object(cJSON *item, const char *value)
{
    cJSON *child;
    if (*value != '{')
    {
        global_error = value;
        return NULL;
    }

    item->type = cJSON_Object;
    value = skip_whitespace(value + 1);
    if (*value == '}') return value + 1;

    item->child = child = cJSON_New_Item();
    if (!item->child) return NULL;
    value = skip_whitespace(parse_string(child, skip_whitespace(value)));
    if (!value) return NULL;
    child->string = child->valuestring;
    child->valuestring = NULL;
    if (*value != ':')
    {
        global_error = value;
        return NULL;
    }
    value = skip_whitespace(parse_value(child, skip_whitespace(value + 1)));
    if (!value) return NULL;

    while (*value == ',')
    {
        cJSON *new_item = cJSON_New_Item();
        if (!new_item) return NULL;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip_whitespace(parse_string(child, skip_whitespace(value + 1)));
        if (!value) return NULL;
        child->string = child->valuestring;
        child->valuestring = NULL;
        if (*value != ':')
        {
            global_error = value;
            return NULL;
        }
        value = skip_whitespace(parse_value(child, skip_whitespace(value + 1)));
        if (!value) return NULL;
    }

    if (*value == '}') return value + 1;
    global_error = value;
    return NULL;
}

static const char *parse_value(cJSON *item, const char *value)
{
    if (!value) return NULL;
    if (!strncmp(value, "null", 4))  { item->type = cJSON_NULL; return value + 4; }
    if (!strncmp(value, "false", 5)) { item->type = cJSON_False; return value + 5; }
    if (!strncmp(value, "true", 4))  { item->type = cJSON_True; item->valueint = 1; return value + 4; }
    if (*value == '\"')              { return parse_string(item, value); }
    if (*value == '-' || (*value >= '0' && *value <= '9')) { return parse_number(item, value); }
    if (*value == '[')              { return parse_array(item, value); }
    if (*value == '{')              { return parse_object(item, value); }
    global_error = value;
    return NULL;
}

cJSON *cJSON_Parse(const char *value)
{
    cJSON *c = cJSON_New_Item();
    global_error = NULL;
    if (!c) return NULL;
    if (!parse_value(c, skip_whitespace(value)))
    {
        cJSON_Delete(c);
        return NULL;
    }
    return c;
}

cJSON *cJSON_GetObjectItem(const cJSON * const object, const char * const string)
{
    cJSON *c = object ? object->child : NULL;
    while (c && c->string && strcasecmp(c->string, string))
        c = c->next;
    return c;
}

cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string)
{
    cJSON *c = object ? object->child : NULL;
    while (c && c->string && strcmp(c->string, string))
        c = c->next;
    return c;
}

int cJSON_GetArraySize(const cJSON *array)
{
    cJSON *c = array ? array->child : NULL;
    int i = 0;
    while (c) { i++; c = c->next; }
    return i;
}

cJSON *cJSON_GetArrayItem(const cJSON *array, int index)
{
    cJSON *c = array ? array->child : NULL;
    while (c && index > 0) { index--; c = c->next; }
    return c;
}

int cJSON_IsNumber(const cJSON * const item)
{
    if (!item) return 0;
    return (item->type & 0xFF) == cJSON_Number;
}

int cJSON_IsString(const cJSON * const item)
{
    if (!item) return 0;
    return (item->type & 0xFF) == cJSON_String;
}

int cJSON_IsArray(const cJSON * const item)
{
    if (!item) return 0;
    return (item->type & 0xFF) == cJSON_Array;
}

int cJSON_IsObject(const cJSON * const item)
{
    if (!item) return 0;
    return (item->type & 0xFF) == cJSON_Object;
}
