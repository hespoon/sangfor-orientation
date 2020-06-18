#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "json.h"

typedef struct array array;
typedef struct object object;
typedef struct value value;
typedef struct keyvalue keyvalue;

/**
 *  想想：这些结构体定义在.c是为什么？
 */
/**
 *  想想：如果要提升内存分配效率，这个结构体该作什么变化？
 */
struct array
{
    value **elems; /* 想想: 这里如果定义为'value *elems'会怎样？ 会导致数组中无法添加新元素*/
    U32 count;     //elems中有多少个value*
    U32 size;      // 数组 array 的容量
};

/**
 * @brief 对象的键值对
 */
struct keyvalue
{
    char *key;  //键名
    value *val; //值
};

/**
 *  想想：如果要提升内存分配效率，这个结构体该作什么变化？
 */
struct object
{
    keyvalue *kvs; //这是一个keyvalue的数组，可以通过realloc的方式扩充的动态数组
    U32 count;     //数组kvs中有几个键值对
    U32 size;      // 数组 kvs 的容量
};

/**
 * @brief JSON值
 */
struct value
{
    json_e type;    //JSON值的具体类型
    union {         //匿名 union，其中的属性可以当作 value 的属相直接访问
        double num; //数值，当type==JSON_NUM时有效
        BOOL bol;   //布尔值，当type==JSON_BOL时有效
        char *str;  //字符串值，堆中分配的一个字符串，当type==JSON_STR时有效
        array arr;  //值数组，当type==JSON_ARR时有效
        object obj; //对象，当type==JSON_OBJ时有效
    };
};

/**
 *  @brief 新建一个type类型的JSON值，采用缺省值初始化
 *  
 *  @param [in] type JSON值的类型，见json_e的定义
 *  @return 堆分配的JSON值
 *  
 *  @details 
 *  1. 对于数值，初始化为0
 *  2. 对于BOOL，初始化为FALSE
 *  3. 对于字符串，初始化为NULL
 *  4. 对于OBJ，初始化为空对象
 *  5. 对于ARR，初始化为空数组
 */
JSON *json_new(json_e type)
{
    JSON *json = (JSON *)calloc(1, sizeof(JSON));
    if (!json)
    {
        //想想：为什么输出到stderr，不用printf输出到stdout？
        fprintf(stderr, "json_new: calloc(%lu) failed\n", sizeof(JSON));
        return NULL;
    }
    json->type = type;
    switch (type)
    {
    case JSON_STR:
        json->str = NULL;
        break;
    case JSON_ARR:

        json->arr.elems = (value **)calloc(1, sizeof(value *));
        if (!json->arr.elems)
        {
            free(json);
            fprintf(stderr, "json_new: calloc(%lu) failed\n", sizeof(value *));
            return NULL;
        }
        json->arr.count = 0;
        json->arr.size = 1;
        break;
    case JSON_OBJ:

        json->obj.kvs = (keyvalue *)calloc(1, sizeof(keyvalue));
        if (!json->obj.kvs)
        {
            free(json);
            fprintf(stderr, "json_new: calloc(%lu) failed\n", sizeof(keyvalue));
            return NULL;
        }
        json->obj.count = 0;
        json->obj.size = 1;
        break;
    default:
        break;
    }
    return json;
}
/**
 * @brief 释放一个JSON值
 * @param json json值
 * @details
 * 该JSON值可能含子成员，也要一起释放
 * 可以接受 json 为 NULL
 */
void json_free(JSON *json)
{
    if (!json)
    {
        return;
    }

    switch (json->type)
    {
    case JSON_STR:
        free(json->str);
        free(json);
        break;
    case JSON_ARR:
        for (int i = 0; i < json->arr.count; i++)
        {
            json_free(json->arr.elems[i]);
        }
        free(json->arr.elems);
        free(json);
        break;
    case JSON_OBJ:
        for (int i = 0; i < json->obj.count; i++)
        {
            free(json->obj.kvs[i].key);
            json_free(json->obj.kvs[i].val);
        }
        free(json->obj.kvs);
        free(json);
        break;
    default:
        free(json);
        break;
    }
}
/**
 * @brief 获取JSON值json的类型
 * @param json json值
 * @return json的实际类型
 */
json_e json_type(const JSON *json)
{
    assert(json);
    return json ? json->type : JSON_NONE;
}
/**
 * @brief 新建一个BOOL类型的JSON值
 * @param val 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_bool(BOOL val)
{
    JSON *json = json_new(JSON_BOL);
    if (!json)
        return NULL;
    json->bol = val;
    return json;
}
/**
 * @brief 新建一个数字类型的JSON值
 * @param val 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_num(double val)
{

    JSON *json = json_new(JSON_NUM);
    if (!json)
        return NULL;
    json->num = val;
    return json;
}
/**
 * @brief 新建一个字符串类型的JSON值
 * @param str 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_str(const char *str)
{
    JSON *json;
    assert(str);

    json = json_new(JSON_STR);
    if (!json)
        return json;
    json->str = strdup(str);
    if (!json->str)
    {
        fprintf(stderr, "json_new_str: strdup(%s) failed", str);
        json_free(json);
        return NULL;
    }
    return json;
}
//想想：json_num和json_str为什么带一个def参数？ 方便用户自定义函数执行失败时的返回值，同时防止固定的错误返回值与 JSON_NUM 的内容一致导致误判
/**
 * @brief 获取JSON_NUM类型JSON值的数值
 * 
 * @param json 数值类型的JSON值
 * @param def   类型不匹配时返回的缺省值
 * @return double 如果json是合法的JSON_NUM类型，返回其数值，否则返回缺省值def
 */
double json_num(const JSON *json, double def)
{
    //想想：为什么这里不assert(json)? 在 return 中会判断
    return json && json->type == JSON_NUM ? json->num : def;
}
/**
 * @brief 获取JSON_BOOL类型JSON值的布尔值
 * 
 * @param json 布尔值类型的JSON值
 * @return BOOL 如果json是合法的JSON_BOL类型，返回其数值，否则返回FALSE
 */
BOOL json_bool(const JSON *json)
{
    //想想：为什么这里不assert(json)? 在 return 中会判断
    return json && json->type == JSON_BOL ? json->bol : FALSE;
}
/**
 * @brief 获取JSON_STR类型JSON值的字符串值
 * 
 * @param json 字符串类型的JSON值
 * @param def   类型不匹配时返回的缺省值
 * @return const char* 如果json是合法的JSON_STR类型，返回其字符串值，否则返回def
 */
const char *json_str(const JSON *json, const char *def)
{
    //想想：为什么这里不assert(json)? 在 return 中会判断
    return json && json->type == JSON_STR ? json->str : def;
}
/**
 * @brief 从对象类型的JSON值中获取名字为key的成员(JSON值)
 * @param json 对象类型的JSON值
 * @param key  成员的键名
 * @return 找到的成员
 * @details 要求json是个对象类型
 */
const JSON *json_get_member(const JSON *json, const char *key)
{
    U32 i;
    assert(json);
    assert(json->type == JSON_OBJ);
    assert(!(json->obj.count > 0 && json->obj.kvs == NULL));
    assert(key);
    assert(key[0]);

    for (i = 0; i < json->obj.count; ++i)
    {
        if (strcmp(json->obj.kvs[i].key, key) == 0)
            return json->obj.kvs[i].val;
    }
    return NULL;
}
/**
 * 从数组类型的JSON值中获取第idx个元素(子JSON值)
 * @param json 数组类型的JSON值
 * @param idx  元素的索引值
 * @return 找到的元素(JSON值的指针)
 * @details 要求json是个数组
 */
const JSON *json_get_element(const JSON *json, U32 idx)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));

    if (idx >= json->arr.count) // idx 为无符号整数，因此一定大于等于零
        return NULL;
    return json->arr.elems[idx];
}
/**
 * @brief 将 str 中的特殊字符全部转意，并在末尾添加换行符
 * @param str 要处理的字符串
 * @return 处理好的字符串
 */
static char *transfer_str(const char *str)
{
    char *ret = (char *)malloc((strlen(str) * 2 + 2) * sizeof(char));
    if (!ret)
    {
        fprintf(stderr, "transfer_str: malloc failed!\n");
        return NULL;
    }
    int i = 0, len = strlen(str), j = 0;
    while (i < len)
    {
        switch (str[i])
        {
        case '\n':
            ret[j++] = '\\';
            ret[j++] = 'n';
            i++;
            break;
        case '\a':
            ret[j++] = '\\';
            ret[j++] = 'a';
            i++;
            break;
        case '\b':
            ret[j++] = '\\';
            ret[j++] = 'b';
            i++;
            break;
        case '\f':
            ret[j++] = '\\';
            ret[j++] = 'f';
            i++;
            break;
        case '\r':
            ret[j++] = '\\';
            ret[j++] = 'r';
            i++;
            break;
        case '\t':
            ret[j++] = '\\';
            ret[j++] = 't';
            i++;
            break;
        case '\v':
            ret[j++] = '\\';
            ret[j++] = 'v';
            i++;
            break;
        default:
            ret[j++] = str[i++];
            break;
        }
    }
    ret[j++] = '\n';
    ret[j] = '\0';
    return ret;
}
/**
 * @brief 将双精度浮点数转换为字符串，并去除多余的 0
 * @param num 要转换的数字
 * @param buf 存放字符串的缓冲区
 * @return 返回值为空
 */
void JSON_NUM_to_string(double num, char *const buf)
{
    sprintf(buf, "%f", num);
    for (int i = strlen(buf) - 1; i >= 0; i--)
    {
        if (buf[i] == '.')
        {
            buf[i] = '\n';
            buf[i + 1] = '\0';
            break;
        }
        else if (buf[i] != '0')
        {
            buf[i + 1] = '\n';
            buf[i + 2] = '\0';
            break;
        }
    }
}
/**
 * @brief 将 JSON 对象转换为 YAML 格式并存储在 fp 打开的文件中
 * @param space_num 当前 json 对象转换为 YAML 格式时需要缩进的空格数
 * @param falg 记录了上一层 JSON 对象的类型
 * @param fp 指向存放 YAML 格式内容的文件的指针
 * @param json 要转换的 JSON 对象
 * @param error 常指针，用于记录函数执行过程中是否出错，*error=0 时未出错，非 0 时表示出错了
 * @return 转换成功时返回 0，失败时返回非 0 整数
 */
static int json_to_yaml(const JSON *json, FILE *fp, int space_num, json_e flag, int *const error)
{
    if (*error != 0)
    {
        return *error;
    }
    switch (json->type)
    {
    case JSON_NUM:
    {
        char *buf = malloc(320 * sizeof(char));
        if (!buf)
        {
            fprintf(stderr, "json_to_yaml: JSON_NUM malloc failed\n");
            *error = -1;
            return -1;
        }
        JSON_NUM_to_string(json->num, buf); // 添加了换行符
        int num_sucess = fwrite(buf, sizeof(char), strlen(buf), fp);
        if (num_sucess != strlen(buf))
        {
            fprintf(stderr, "json_to_yaml: write JSON_NUM into file failed!\n");
            *error = -1;
            free(buf);
            return -1;
        }
        free(buf);
        return 0;
    }

    case JSON_BOL:
    {
        char buf[7];
        if (json->bol != 0)
        {
            strcpy(buf, "true");
        }
        else
        {
            strcpy(buf, "false");
        }
        strcat(buf, "\n");
        int num_sucess = fwrite(buf, sizeof(char), strlen(buf), fp);
        if (num_sucess != strlen(buf))
        {
            fprintf(stderr, "json_to_yaml: write JSON_BOL into file failed!\n");
            *error = -1;
            return -1;
        }
        return 0;
    }

    case JSON_STR:
    {
        char *t_str = transfer_str(json->str); // 已经加好换行符了
        if (!t_str)
        {
            fprintf(stderr, "json_to_yaml: transfer string [%s] failed!\n", json->str);
            *error = -1;
            return -1;
        }
        int len = strlen(t_str);
        int num_sucess = fwrite(t_str, sizeof(char), len, fp);
        free(t_str);
        if (num_sucess != len)
        {
            fprintf(stderr, "json_to_yaml: write JSON_STR into file failed!\n");
            *error = -1;

            return -1;
        }
        return 0;
    }

    case JSON_ARR:
    {
        for (int i = 0; i < json->arr.count; i++)
        {
            char *buf = (char *)malloc((space_num + 3) * sizeof(char));
            buf[0] = '\0';
            if (!(flag == JSON_ARR && i == 0))
            {
                for (int j = 0; j < space_num; j++)
                {
                    buf[j] = ' ';
                }
                buf[space_num] = '\0';
            }

            strcat(buf, "- ");
            int len = strlen(buf);
            int num_sucess = fwrite(buf, sizeof(char), len, fp);
            free(buf);
            if (num_sucess != len)
            {
                fprintf(stderr, "json_to_yaml: write JSON_ARR prefix into file failed!\n");
                *error = -1;
                return -1;
            }

            json_to_yaml(json->arr.elems[i], fp, space_num + 2, JSON_ARR, error);
        }
        return 0;
    }

    case JSON_OBJ:
    {
        for (int i = 0; i < json->obj.count; i++)
        {
            char *buf = (char *)malloc((space_num + strlen(json->obj.kvs[i].key) + 4) * sizeof(char));
            buf[0] = '\0';
            if (!(flag == JSON_ARR && i == 0))
            {
                for (int j = 0; j < space_num; j++)
                {
                    buf[j] = ' ';
                }
                buf[space_num] = '\0';
            }

            strcat(buf, json->obj.kvs[i].key);
            strcat(buf, ": ");
            if (json->obj.kvs[i].val->type == JSON_ARR || json->obj.kvs[i].val->type == JSON_OBJ)
            {
                strcat(buf, "\n");

                int num_sucess = fwrite(buf, sizeof(char), strlen(buf), fp);

                if (num_sucess != strlen(buf))
                {
                    fprintf(stderr, "json_to_yaml: write JSON_OBJ prefix into file failed!\n");
                    free(buf);
                    *error = -1;
                    return -1;
                }
                free(buf);
            }
            else
            {
                int num_sucess = fwrite(buf, sizeof(char), strlen(buf), fp);

                if (num_sucess != strlen(buf))
                {
                    fprintf(stderr, "json_to_yaml: write into file failed!\n");
                    free(buf);
                    *error = -1;
                    return -1;
                }
                free(buf);
            }
            json_to_yaml(json->obj.kvs[i].val, fp, space_num + 2, JSON_OBJ, error);
        }
        return 0;
    }

    default:
        return -1;
    }
}
/**
 * @brief 把JSON值json以YAML格式输出，保存到名字为fname的文件中
 * 
 * @param json  JSON值
 * @param fname 输出文件名
 * @return int 0表示成功，非 0 表示失败
 */
int json_save(const JSON *json, const char *fname)
{
    assert(json);
    assert(fname);
    assert(fname[0]);
    //TODO:
    FILE *fp;
    fp = fopen(fname, "w");
    if (!fp)
    {
        fprintf(stderr, "json_save: open file [%s] failed!\n", fname);
        return -1;
    }
    int *error = (int *)malloc(sizeof(int));
    if (!error)
    {
        fclose(fp);
        fprintf(stderr, "json_save: malloc failed!\n");
        return -1;
    }
    *error = 0;
    int ans = json_to_yaml(json, fp, 0, JSON_NONE, error);
    free(error);
    fclose(fp);
    return ans;
}

/**
 * @brief 扩容函数，将 json 对象的容量扩大一倍
 * @param json 要扩容的 JSON 对象
 * @return 扩容成功返回 json，失败返回 NULL
 * @details 保证 json 的类型为 JSON_OBJ 或 JSON_ARR；json 不为 NULL
 */
JSON *expand(JSON *json)
{
    assert(json);
    assert((json->type == JSON_OBJ || json->type == JSON_ARR));
    switch (json->type)
    {
    case JSON_ARR:
    {
        // realloc 在内存申请成功后自动释放旧内存
        value **temp = (value **)realloc(json->arr.elems, json->arr.size * 2 * sizeof(value *));
        if (!temp)
        {
            fprintf(stderr, "expand: expand array size failed!\n");
            return NULL;
        }
        json->arr.size *= 2;
        json->arr.elems = temp;
        return json;
    }

    case JSON_OBJ:
    {
        keyvalue *temp = (keyvalue *)realloc(json->obj.kvs, json->obj.size * 2 * sizeof(keyvalue));
        if (!temp)
        {
            fprintf(stderr, "expand: expand object size failed!\n");
            return NULL;
        }
        json->obj.size *= 2;
        json->obj.kvs = temp;
        return json;
    }
    default:
        return NULL;
    }
}
//  想想：json_add_member和json_add_element中，val应该是堆分配，还是栈分配？堆分配的
//  想想：如果json_add_member失败，应该由谁来释放val？在函数中释放
/**
 * @brief 往对象类型的json中增加一个键值对，键名为key，值为val
 * 
 * @param json JSON对象
 * @param key 键名，符合正则：[a-zA-Z_][a-zA-Z_-0-9]*
 * @param val 键值，必须是堆分配拥有所有权的JSON值
 * @return JSON* 成功返回val，失败返回NULL
 * @details
 *  json_add_member会转移val的所有权，所以调用json_add_member之后不用考虑释放val的问题
 * 因为需要支持如下写法：
 *  json_add_member(json, "port", json_new_num(80));
 * 所以需要做到：
 *  1) 允许val为NULL；
 *  2) 当json_add_member内部发生失败时，需要释放val，满足将val的所有权转让给json_add_member的语义设定
 */
JSON *json_add_member(JSON *json, const char *key, JSON *val)
{
    assert(json->type == JSON_OBJ);
    assert(!(json->obj.count > 0 && json->obj.kvs == NULL));
    assert(key);
    assert(key[0]);
    //想想: 为啥不用assert检查val？因为允许 val 为 NULL。
    //想想：如果json中已经存在名字为key的成员，怎么办？覆盖旧成员的值。
    //TODO:
    if (val == NULL)
    {
        return NULL;
    }
    else
    {
        // 查找键名是否存在
        for (int i = 0; i < json->obj.count; i++)
        {
            if (strcmp(json->obj.kvs[i].key, key) == 0)
            {
                json_free(json->obj.kvs[i].val);
                json->obj.kvs[i].val = val;
                return val;
            }
        }
        // 键名不存在，则向 json 中添加新的键值对
        // 需要扩容
        if (json->obj.count == json->obj.size)
        {
            if (!expand(json))
            {
                fprintf(stderr, "json_add_member: expand capacity failed!\n");
                json_free(val);
                val = NULL;
                return NULL;
            }
        }
        json->obj.kvs[json->obj.count].key = strdup(key);
        if (json->obj.kvs[json->obj.count].key == NULL)
        {
            fprintf(stderr, "json_add_member: strdup(%s) failed!\n", key);
            json_free(val);
            val = NULL;
            return NULL;
        }
        json->obj.kvs[json->obj.count].val = val;
        json->obj.count++;
        return val;
    }
}
/**
 * @brief 往数组类型的json中追加一个元素
 * 
 * @param json JSON数组
 * @param val 加入到数组的元素，必须是堆分配拥有所有权的JSON值
 * @details
 *  json_add_element会转移val的所有权，所以调用json_add_element之后不用考虑释放val的问题
 *  val 允许为 NULL 用以支持 json_add_element(json,json_new(JSON_OBJ)) 的写法
 */
JSON *json_add_element(JSON *json, JSON *val)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));

    //想想：为啥不用assert检查val？ 因为 val 允许为 NULL
    //TODO:
    if (val == NULL)
        return NULL;

    // 判断是否需要扩容
    if (json->arr.count == json->arr.size)
    {
        if (!expand(json))
        {
            fprintf(stderr, "json_add_element: expand capacity failed!\n");
            json_free(val);
            val = NULL;
            return NULL;
        }
    }

    json->arr.elems[json->arr.count] = val;
    json->arr.count++;
    return val;
}

#if ACTIVE_PLAN == 1
/**
 * @brief 获取名字为key，类型为expect_type的子节点（JSON值）
 * @param json 对象类型的JSON值
 * @param key   键名
 * @param expect_type 期望类型
 * @return 找到的JSON值
 */
static const JSON *get_child(const JSON *json, const char *key, json_e expect_type)
{
    const JSON *child;

    child = json_get_member(json, key);
    if (!child)
        return NULL;
    if (child->type != expect_type)
        return NULL;
    return child;
}
/**
 * 获取JSON对象中键名为key的数值，如果获取不到，或者类型不对，返回def
 * @param json json对象
 * @param key  成员键名
 * @param def  取不到结果时返回的默认值
 * @return double 获取到的数值
 */
double json_obj_get_num(const JSON *json, const char *key, double def)
{
    const JSON *child = get_child(json, key, JSON_NUM);
    if (!child)
        return def;
    return child->num;
}
/**
 * 获取JSON对象中键名为key的BOOL值，如果获取不到，或者类型不对，返回false
 * @param json json对象
 * @param key  成员键名
 * @return BOOL 获取到的键值
 */
BOOL json_obj_get_bool(const JSON *json, const char *key)
{
    const JSON *child = get_child(json, key, JSON_BOL);
    if (!child)
        return FALSE;
    return child->bol;
}
/**
 * @brief 获取JSON对象中键名为key的值，如果获取不到，则返回缺省值def
 * @param json 对象类型的JSON值
 * @param key  键名
 * @param def  找不到时返回的缺省值
 * @return 获取到的字符串结果
 * @details
 * 如果json不是对象类型，则返回def
 * 如果对应的值不是字符串类型，则返回def
 * 如: 
 *  json: {"key": "str"}
 *  json_obj_get_str(json, "key", NULL) = "str"
 *  json_obj_get_str(json, "noexist", NULL) = NULL
 *  json_obj_get_str(json, "noexist", "") = ""
 *  
 */
const char *json_obj_get_str(const JSON *json, const char *key, const char *def)
{
    const JSON *child = get_child(json, key, JSON_STR);
    if (!child)
        return def;
    return child->str;
}

/**
 * @brief 在 json 中找到键名为 key 类型为 type 的元素
 * @param json 在该对象中寻找
 * @param key 键名
 * @param type 期望的 JSON 类型
 * @return 成功时返回对应元素的 JSON 指针，失败时返回 NULL
 * @details 调用者可以通过返回的指针修改 JSON 对象
 *          用断言建立契约
 */
static JSON *find_child(JSON *json, const char *key, json_e type)
{
    U32 i;
    assert(json);
    assert(json->type == JSON_OBJ);
    assert(!(json->obj.count > 0 && json->obj.kvs == NULL));
    assert(key);
    assert(key[0]);
    for (i = 0; i < json->obj.count; ++i)
    {
        if (strcmp(json->obj.kvs[i].key, key) == 0)
        {
            if (json->obj.kvs[i].val->type == type)
            {
                return json->obj.kvs[i].val;
            }
        }
    }
    return NULL;
}
/**
 * @brief 根据键名设置键值
 * @param json JSON_OBJ 类型的 JSON 对象
 * @param key 键名
 * @param val 要设置的键值
 * @return 成功时返回 0，失败时返回 -1
 * @details 不能调用 json_get_member，因为该函数返回的指针是常指针
 */
int json_obj_set_num(JSON *json, const char *key, double val)
{
    //TODO:
    JSON *ret = find_child(json, key, JSON_NUM);
    if (ret)
    {
        ret->num = val;
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 根据键名设置键值
 * @param json JSON_OBJ 类型的 JSON 对象
 * @param key 键名
 * @param val 要设置的键值
 * @return 成功时返回 0，失败时返回 -1
 */
int json_obj_set_bool(JSON *json, const char *key, BOOL val)
{
    //TODO:
    JSON *ret = find_child(json, key, JSON_BOL);
    if (ret)
    {
        ret->bol = val;
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 根据键名设置键值
 * @param json JSON_OBJ 类型的 JSON 对象
 * @param key 键名
 * @param val 要设置的键值
 * @return 成功时返回 0，失败时返回 -1
 */
int json_obj_set_str(JSON *json, const char *key, const char *val)
{
    //TODO:
    JSON *ret = find_child(json, key, JSON_STR);
    if (ret)
    {
        ret->str = strdup(val);
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 获取数组型 JSON 中元素的个数
 * @param json JSON 对象
 * @return 成功时返回元素个数，失败时返回 -1
 */
int json_arr_count(const JSON *json)
{
    if (!json || json->type != JSON_ARR)
        return -1;
    return json->arr.count;
}

/**
 * @brief 根据下标获取 JSON_ARR 型 json 中的 JSON_NUM 元素的值
 * @param json JSON_ARR 型 json
 * @param idx 数组下标
 * @param def 获取失败时返回的缺省值
 * @return 成功时返回获取到的值，失败时返回 def
 * @details 调用 json_get_element
 */
double json_arr_get_num(const JSON *json, int idx, double def)
{
    //TODO:
    return json_num(json_get_element(json, idx), def);
}

BOOL json_arr_get_bool(const JSON *json, int idx)
{
    //TODO:
    return json_bool(json_get_element(json, idx));
}

const char *json_arr_get_str(const JSON *json, int idx, const char *def)
{
    //TODO:
    return json_str(json_get_element(json, idx), def);
}

/**
 * @brief 向数组型 JSON 对象中添加 JSON_NUM 型数据
 * @param json 数组型 JSON 对象
 * @param val 要添加的数据
 * @return 添加成功返回 1，失败返回 -1
 */
int json_arr_add_num(JSON *json, double val)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));
    //TODO:
    if (!json_add_element(json, json_new_num(val)))
    {
        fprintf(stderr, "json_arr_add_num: add failed!\n");
        return -1;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief 向数组型 JSON 对象中添加 JSON_BOL 型数据
 * @param json 数组型 JSON 对象
 * @param val 要添加的数据
 * @return 添加成功返回 1，失败返回 -1
 */
int json_arr_add_bool(JSON *json, BOOL val)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));
    //TODO:
    if (!json_add_element(json, json_new_bool(val)))
    {
        fprintf(stderr, "json_arr_add_bool: add failed!\n");
        return -1;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief 向数组型 JSON 对象中添加 JSON_STR 型数据
 * @param json 数组型 JSON 对象
 * @param val 要添加的字符串
 * @return 添加成功返回 1，失败返回 -1
 */
int json_arr_add_str(JSON *json, const char *val)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));
    assert(val);
    //TODO:
    if (!json_add_element(json, json_new_str(val)))
    {
        fprintf(stderr, "json_arr_add_str: add failed!\n");
        return -1;
    }
    else
    {
        return 1;
    }
}

#elif ACTIVE_PLAN == 2
/*
json_get和json_get所使用的路径表达式语法：

root ::= member | index;
member ::= <name> child;
index ::= '[' <number> ']' child;
child ::= dot_member | index | EOF;
dot_member ::= '.' member;
 */

/**
 * @brief 交换两个json值的内容
 * 
 * @param lhs 左手侧JSON值
 * @param rhs 右手侧JSON值
 */
static void json_swap(JSON *lhs, JSON *rhs)
{
    JSON tmp;
    memcpy(&tmp, lhs, sizeof(tmp));
    memcpy(lhs, rhs, sizeof(*lhs));
    memcpy(rhs, &tmp, sizeof(tmp));
}
/**
 * @brief 路径解析的上下文
 */
typedef struct query_ctx
{
    JSON *root;       //待查找的根JSON值
    JSON *val;        //待替换的JSON值
    const char *path; //原始路径
} query_ctx;

/**
 * @brief 处理查询结果
 * 
 * @param json  查找到的JSON值
 * @param val   需要替换的JSON值，如果val不为空，表示需要将json替换为val
 * @return const JSON* 查找结果
 */
static const JSON *deal_query_result(JSON *json, JSON *val)
{
    assert(json);

    if (val)
    {
        json_swap(json, val);
        json_free(val);
    }
    return json;
}
/**
 * @brief 报告路径解析过程发现的语法错误
 * 
 * @param ctx   路径解析的上下文
 * @param info  错误说明
 * @param cur   出错位置
 */
static void report_syntax_error(const query_ctx *ctx, const char *info, const char *cur)
{
    fprintf(stderr, "%s\n", info);
    fprintf(stderr, "path: %s\n", ctx->path);
    fprintf(stderr, "%*s^\n", (int)(cur - ctx->path + 6), " ");
}

static const JSON *query_child(const query_ctx *ctx, JSON *json, const char *cur);

/**
 * @brief 期待解析结束，即希望接下来的是结束符
 * 
 * @param json  JSON值
 * @param cur   当前解析位置
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_eof(const query_ctx *ctx, JSON *json, const char *cur)
{
    assert(ctx);
    assert(json);
    assert(cur);

    if (*cur == '\0')
    {
        return deal_query_result(json, ctx->val);
    }
    else
    {
        report_syntax_error(ctx, "JSON path invalid", cur);
        json_free(ctx->val);
        return NULL;
    }
}
/**
 * @brief 在JSON值json中查询MEMBER表达式cur对应的子孙成员
 * 
 * @param json  JSON值
 * @param cur  MEMBER表达式
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_member(const query_ctx *ctx, JSON *json, const char *cur)
{
    //TODO:
    return NULL;
}
/**
 * @brief 在JSON值json中查询DOT_MEMBER表达式cur对应的子孙成员
 * 
 * @param json  JSON值
 * @param cur  DOT_MEMBER表达式
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_dot_member(const query_ctx *ctx, JSON *json, const char *cur)
{
    assert(ctx);
    assert(json);
    assert(json->type == JSON_OBJ);
    assert(cur);

    if (*cur == '.')
    {
        if (cur[1] == '\0')
        {
            report_syntax_error(ctx, "unexpected end", cur);
            json_free(ctx->val);
            return NULL;
        }
        return query_member(ctx, json, cur + 1);
    }
    else
    {
        return query_eof(ctx, json, cur);
    }
}
/**
 * @brief 在JSON值json中查询INDEX表达式cur对应的子孙成员
 * 
 * @param json  JSON值
 * @param cur  INDEX表达式
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_index(const query_ctx *ctx, JSON *json, const char *cur)
{
    //TODO:
    return NULL;
}
/**
 * @brief 在JSON值json中查询CHILD表达式cur对应的子孙成员
 * 
 * @param json  JSON值
 * @param cur  CHILD表达式
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_child(const query_ctx *ctx, JSON *json, const char *cur)
{
    assert(ctx);
    assert(json);
    assert(cur);

    switch (json_type(json))
    {
    case JSON_NUM:
    case JSON_STR:
    case JSON_BOL:
    case JSON_NONE:
        return query_eof(ctx, json, cur);
    case JSON_ARR:
        return query_index(ctx, json, cur);
    case JSON_OBJ:
        return query_dot_member(ctx, json, cur);
    default:
        assert(!"dead code");
        return NULL;
    }
}

/**
 * @brief 在JSON值json中查询ROOT表达式cur对应的子孙成员
 * 
 * @param json  JSON值
 * @param cur  ROOT表达式
 * @param val   待替换的JSON值。val为NULL，表示只查询，否则将查到的子孙成员值替换为val
 * @return const JSON* 查找到的子孙成员
 */
static const JSON *query_root(JSON *json, const char *path, JSON *val)
{
    query_ctx ctx = {0};

    assert(json);
    assert(path);

    ctx.root = json;
    ctx.path = path;
    ctx.val = val;

    if (json_type(json) == JSON_OBJ)
    {
        return query_member(&ctx, json, path);
    }
    else
    {
        return query_child(&ctx, json, path);
    }
}
/**
 * 在JSON值json中找到路径为path的成员，将其值修改为val
 * @param json JSON值
 * @param path 待修改成员的路径，如：basic.dns[1]，空串表示本身
 * @param val 新的值
 * @return <0表示失败，否则表示成功
 */
int json_set(JSON *json, const char *path, JSON *val)
{
    assert(json);
    assert(path);

    if (!val)
        return -1;
    if (query_root(json, path, val))
        return 0;
    return -1;
}
/**
 * 在JSON值json中找到路径为path的成员
 * @param json JSON值
 * @param path 路径表达式，待查找成员的路径，如：basic.dns[1]，空串表示本身
 * @return 路径path指示的成员值，不存在则返回NULL
 */
const JSON *json_get(const JSON *json, const char *path)
{
    assert(json);
    assert(path);

    return query_root((JSON *)json, path, NULL);
}
#elif ACTIVE_PLAN == 3
/**
 * @brief 设置json成员的值
 * 
 * @param json JSON值
 * @param path 待操作的子成员在json中的位置，如：basic.dns[0]。NULL表示json本身
 * @param value 子成员的新值，以字符串形式表示，五种可能，字符串："200.200.0.1"，数值：8080，BOOL值：true/false，空数组：[], 空对象：{}
 * @return JSON* path指向的子成员JSON值
 * @details
 *  如果path表示的子成员不存在，将自动创建，如果存在，将替换成新值。
 *  两种情况下不会自动创建，第一种是父对象不存在，第二种是数组成员的前一个兄弟不存在。
 */
JSON *json_set_value(JSON *json, const char *path, const char *value)
{
    assert(json);
    assert(value);
    //TODO:
    return NULL;
}
/**
 * @brief 从JSON值json中获取一个子成员的值，子成员所在位置由路径path标识
 * 
 * @param json JSON值
 * @param path 待操作的子成员在json中的位置，如：basic.dns[0]。NULL表示json本身
 * @return const JSON* path指向的子成员
 */
const JSON *json_get_value(const JSON *json, const char *path)
{
    //TODO:
    return NULL;
}
/**
 * @brief 从JSON值json中读取一个INT类型配置项的值，配置项的位置由路径path标识
 * 
 * @param json JSON值
 * @param path 待操作的子成员在json中的位置，如：basic.dns[0]。NULL表示json本身
 * @param def 如果配置项不存在或类型不匹配，返回该值作为缺省值
 * @return int 配置项的值
 */
int json_get_int(const JSON *json, const char *path, int def)
{
    const JSON *child = json_get_value(json, path);
    if (!child)
        return def;
    return (int)json_num(child, def);
}
/**
 * @brief 从JSON值json中读取一个BOOL类型配置项的值，配置项的位置由路径path标识
 * 
 * @param json JSON值
 * @param path 待操作的子成员在json中的位置，如：basic.dns[0].enable。NULL表示json本身
 * @return BOOL 配置项的值
 * @details 如果配置项不存在或类型不匹配，返回FALSE，当作不启用的意思
 */
BOOL json_get_bool(const JSON *json, const char *path)
{
    const JSON *child = json_get_value(json, path);
    if (!child)
        return FALSE;
    return json_bool(child);
}
/**
 * @brief 从JSON值json中读取一个字符串类型配置项的值，配置项的位置由路径path标识
 * 
 * @param json JSON值
 * @param path 待操作的子成员在json中的位置，如：basic.dns[0].ip。NULL表示json本身
 * @return const char* 如果配置项不存在或类型不匹配，返回该值作为缺省值
 */
const char *json_get_str(const JSON *json, const char *path, const char *def)
{
    const JSON *child = json_get_value(json, path);
    if (!child)
        return FALSE;
    return json_str(child, def);
}

#endif //ACTIVE_PLAN
