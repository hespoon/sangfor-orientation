#include "json.h"
#include "xtest.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>

//  完整使用场景的测试
TEST(test, scene)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    JSON *basic = json_new(JSON_OBJ);
    ASSERT_TRUE(basic != NULL);

    ASSERT_TRUE(NULL != json_add_member(json, "basic", basic));

    ASSERT_TRUE(NULL != json_add_member(basic, "enable", json_new_bool(TRUE)));
    EXPECT_EQ(TRUE, json_obj_get_bool(basic, "enable"));

    ASSERT_TRUE(NULL != json_add_member(basic, "port", json_new_num(389)));
    EXPECT_EQ(389, json_obj_get_num(basic, "port", 0));
    //...
    ASSERT_TRUE(NULL != json_add_member(basic, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(basic, "ip", NULL);
    ASSERT_STRCASEEQ("200.200.3.61", ip);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_new
//----------------------------------------------------------------------------------------------------

// 测试创建 JSON_BOL 类型的 JSON 对象
TEST(json_new, JSON_BOL)
{
    JSON *json = json_new(JSON_BOL);
    ASSERT_TRUE(json != NULL);
    json_free(json);
}

// 测试创建 JSON_NUM 类型的 JSON 对象
TEST(json_new, JSON_NUM)
{
    JSON *json = json_new(JSON_NUM);
    ASSERT_TRUE(json != NULL);
    json_free(json);
}

// 测试创建 JSON_STR 类型的 JSON 对象
TEST(json_new, JSON_STR)
{
    JSON *json = json_new(JSON_STR);
    ASSERT_TRUE(json != NULL);
    json_free(json);
}

// 测试创建 JSON_ARR 类型的 JSON 对象
TEST(json_new, JSON_ARR)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json != NULL);
    json_free(json);
}

// 测试创建 JSON_OBJ 类型的 JSON 对象
TEST(json_new, JSON_OBJ)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_add_member
//----------------------------------------------------------------------------------------------------

// 测试键名不存在，且未触发扩容机制
TEST(json_add_member, nonexist_nonexpand)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    JSON *temp = json_new(JSON_STR);
    ASSERT_TRUE(temp != NULL);
    ASSERT_TRUE(json_add_member(json, "he", temp) != NULL);
    json_free(json);
}

// 测试键名不存在，且触发扩容机制
TEST(json_add_member, nonexist_expand)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    JSON *temp1 = json_new(JSON_OBJ);
    ASSERT_TRUE(temp1 != NULL);
    ASSERT_TRUE(json_add_member(json, "a1", temp1) != NULL);
    JSON *temp2 = json_new(JSON_OBJ);
    ASSERT_TRUE(temp2 != NULL);
    ASSERT_TRUE(json_add_member(json, "a2", temp2) != NULL); // 触发扩容机制
    json_free(json);
}

// 测试键名存在
TEST(json_add_member, exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    JSON *temp = json_new(JSON_OBJ);
    ASSERT_TRUE(temp != NULL);
    ASSERT_TRUE(json_add_member(json, "a1", temp) != NULL);
    temp = json_new(JSON_OBJ);
    ASSERT_TRUE(temp != NULL);
    ASSERT_TRUE(json_add_member(json, "a1", temp) != NULL); // 键名重复
    json_free(json);
}

// 测试 val 为 NULL
TEST(json_add_member, val_is_null)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    ASSERT_TRUE(json_add_member(json, "a", NULL) == NULL);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_add_element
//----------------------------------------------------------------------------------------------------

// 测试 val 不为 NULL 且未触发扩容机制
TEST(json_add_element, nonexpand)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    JSON *val = json_new(JSON_BOL);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    json_free(json);
}

// 测试 val 不为 NULL 且触发扩容机制
TEST(json_add_element, expand)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    JSON *val = json_new(JSON_BOL);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    val = json_new(JSON_NUM);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    val = json_new(JSON_STR);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    val = json_new(JSON_ARR);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    val = json_new(JSON_OBJ);
    ASSERT_TRUE(val);
    ASSERT_TRUE(json_add_element(json, val) != NULL);

    json_free(json);
}

// 测试 val 为 NULL
TEST(json_add_element, is_null)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_add_element(json, NULL) == NULL);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_new_bool
//----------------------------------------------------------------------------------------------------

// // 测试 val 为正常值
TEST(json_new_bool, normal)
{
    JSON *json = json_new_bool(10);
    ASSERT_TRUE(json);
    json_free(json);
}

// // 测试 val 为异常值 333333333333332222
TEST(json_new_bool, abnormal)
{
    JSON *json = json_new_bool(333333333333332222);
    ASSERT_TRUE(json);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_new_num
//----------------------------------------------------------------------------------------------------

// // 测试 val 为正常值
TEST(json_new_num, normal)
{
    JSON *json = json_new_num(1.3);
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 val 为异常值 333333333333332222
TEST(json_new_num, abnormal)
{
    JSON *json = json_new_num(333333333333332222);
    ASSERT_TRUE(json);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_new_str
//----------------------------------------------------------------------------------------------------

// 测试 val 为正常值
TEST(json_new_str, normal)
{
    JSON *json = json_new_str("he");
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 val 为空串
TEST(json_new_str, empty_string1)
{
    JSON *json = json_new_str("");
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 val 为空串
TEST(json_new_str, empty_string2)
{
    JSON *json = json_new_str("\0");
    ASSERT_TRUE(json);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_add_str
//----------------------------------------------------------------------------------------------------

// // 测试 val 为正常数据
// TEST(json_arr_add_str, val_he)
// {
//     JSON *json = json_new(JSON_ARR);
//     ASSERT_TRUE(json);
//     ASSERT_TRUE(json_arr_add_str(json, "he") == 1);
//     json_free(json);
// }

// 测试 val 为空串
TEST(json_arr_add_str, val_empty)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_arr_add_str(json, "") == 1);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_add_num
//----------------------------------------------------------------------------------------------------

// 测试 val 为正常数据
TEST(json_arr_add_num, val_100)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_arr_add_num(json, 100) == 1);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_add_bool
//----------------------------------------------------------------------------------------------------

// 测试 val 为正常数据
TEST(json_arr_add_bool, val_100)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_arr_add_bool(json, 100) == 1);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_free
//----------------------------------------------------------------------------------------------------

// 测试 json 为 NULL
TEST(json_free, json_null)
{
    json_free(NULL);
}

// 测试 JSON_BOL 类型的 json
TEST(json_free, JSON_BOL)
{
    JSON *json = json_new(JSON_BOL);
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 JSON_NUM 类型的 json
TEST(json_free, JSON_NUM)
{
    JSON *json = json_new(JSON_NUM);
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 JSON_STR 类型的 json
TEST(json_free, JSON_STR)
{
    JSON *json = json_new(JSON_STR);
    ASSERT_TRUE(json);
    json_free(json);
}

// 测试 JSON_ARR 类型的 JSON，其中的 JSON_ARR 和 JSON_OBJ 为空
TEST(json_free, JSON_ARR_empty)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    json_add_element(json, json_new_bool(1));
    json_add_element(json, json_new_num(10));
    json_add_element(json, json_new_str("he"));
    json_add_element(json, json_new(JSON_ARR));
    json_add_element(json, json_new(JSON_OBJ));
    json_free(json);
}

// 测试 JSON_ARR 类型的 JSON，其中的 JSON_ARR 和 JSON_OBJ 不为空
TEST(json_free, JSON_ARR_nonempty)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    json_arr_add_bool(json, 1);
    json_arr_add_num(json, 10);
    json_arr_add_str(json, "he");

    JSON *arr = json_new(JSON_ARR);
    ASSERT_TRUE(arr);
    json_add_element(json, arr);
    json_arr_add_bool(arr, 0);
    json_arr_add_num(arr, 100);
    json_arr_add_str(arr, "bo");
    json_add_element(arr, json_new(JSON_ARR));
    json_add_element(arr, json_new(JSON_OBJ));

    JSON *obj = json_new(JSON_OBJ);
    ASSERT_TRUE(obj);
    json_add_element(json, obj);
    json_add_member(obj, "1", json_new_bool(0));
    json_add_member(obj, "2", json_new_num(10));
    json_add_member(obj, "3", json_new_str("tao"));
    json_add_member(obj, "4", json_new(JSON_ARR));
    json_add_member(obj, "5", json_new(JSON_OBJ));

    json_free(json);
}

// 测试 JSON_OBJ 类型的 JSON 对象，其中的 JSON_ARR 和 JSON_OBJ 为空
TEST(json_free, JSON_OBJ_empty)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    json_add_member(json, "1", json_new_bool(1));
    json_add_member(json, "2", json_new_num(10));
    json_add_member(json, "3", json_new_str("he"));
    json_add_member(json, "4", json_new(JSON_ARR));
    json_add_member(json, "5", json_new(JSON_OBJ));

    json_free(json);
}

// 测试 JSON_OBJ 类型的 JSON 对象，其中的 JSON_ARR 和 JSON_OBJ 不为空
TEST(json_free, JSON_OBJ_nonempty)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    json_add_member(json, "1", json_new_bool(1));
    json_add_member(json, "2", json_new_num(10));
    json_add_member(json, "3", json_new_str("he"));

    JSON *arr = json_new(JSON_ARR);
    ASSERT_TRUE(arr);
    json_add_member(json, "4", arr);
    json_arr_add_bool(arr, 0);
    json_arr_add_num(arr, 100);
    json_arr_add_str(arr, "bo");
    json_add_element(arr, json_new(JSON_ARR));
    json_add_element(arr, json_new(JSON_OBJ));

    JSON *obj = json_new(JSON_OBJ);
    ASSERT_TRUE(obj);
    json_add_member(json, "5", obj);
    json_add_member(obj, "5.1", json_new_bool(0));
    json_add_member(obj, "5.2", json_new_num(10));
    json_add_member(obj, "5.3", json_new_str("tao"));
    json_add_member(obj, "5.4", json_new(JSON_ARR));
    json_add_member(obj, "5.5", json_new(JSON_OBJ));

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_count
//----------------------------------------------------------------------------------------------------

// 测试非 JSON_ARR 类型的 JSON
TEST(json_arr_count, type_error)
{
    JSON *json = json_new(JSON_BOL);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_arr_count(json) == -1);
    json_free(json);
}

// 测试 JSON_ARR 类型的 JSON，数组内容为空
TEST(json_arr_count, empty_array)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_arr_count(json) == 0);
    json_free(json);
}

// 测试 JSON_ARR 类型的 JSON，数组内容非空
TEST(json_arr_count, nonempty_array)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    json_arr_add_bool(json, 10);
    json_arr_add_num(json, 100);
    json_arr_add_str(json, "he");
    json_add_element(json, json_new(JSON_ARR));
    json_add_element(json, json_new(JSON_OBJ));
    ASSERT_TRUE(json_arr_count(json) == 5);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_get_member
//----------------------------------------------------------------------------------------------------

// 测试键名存在的情况
TEST(json_get_member, exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);
    json_add_member(json, "he", json_new_str("botao"));
    ASSERT_TRUE(json_get_member(json, "he"));
    json_free(json);
}

// 测试键名不存在的情况
TEST(json_get_member, nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);
    json_add_member(json, "he", json_new_str("botao"));
    ASSERT_TRUE(!json_get_member(json, "shi"));
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_get_str
//----------------------------------------------------------------------------------------------------

// 测试键值对存在的情况
TEST(json_obj_get_str, exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    ASSERT_TRUE(NULL != json_add_member(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip", NULL);
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("200.200.3.61", ip);

    json_free(json);
}

//  测试键值对不存在的情况
TEST(json_obj_get_str, notexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    ASSERT_TRUE(NULL != json_add_member(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip2", NULL);
    ASSERT_TRUE(ip == NULL);

    ip = json_obj_get_str(json, "ip3", "default");
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("default", ip);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_save
//----------------------------------------------------------------------------------------------------

// 字符缓冲区
typedef struct buf_t
{
    char *str;
    unsigned int size;
} buf_t;

/**
 * @brief 将 fname 标识的文件读入字符缓冲区 buf 中
 * @param buf 存放文件内容的缓冲区
 * @param fname 被读取的文件的文件名
 * @return 读取成功返回 0，读取失败返回 -1
 */
int read_file(buf_t *buf, const char *fname)
{
    FILE *fp;
    long len;
    long realsize;

    assert(buf);
    assert(fname);
    assert(fname[0]);

    fp = fopen(fname, "rb"); // 以只读的方式打开文件，以二进制的格式读取数据
    if (!fp)
    {
        fprintf(stderr, "open file [%s] failed\n", fname);
        return -1;
    }
    fseek(fp, 0, SEEK_END); // 将文件指针指向文件尾
    len = ftell(fp);        // 获取文件指针当前的位置，即文件长度
    if (len <= 0)
    {
        fclose(fp);
        fprintf(stderr, "ftell failed, errno: %d\n", errno);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);             // 将文件指针指向文件开始位置
    buf->str = (char *)malloc(len + 1); // 为缓冲区分配内存
    if (!buf->str)
    {
        fclose(fp);
        buf->size = 0;
        fprintf(stderr, "malloc(%ld) failed\n", len + 1);
        return -1;
    }
    buf->size = len + 1;
    realsize = fread(buf->str, 1, len, fp); // 以字节为单位读取数据，并返回成功读取到的字节数
    fclose(fp);

    buf->str[realsize] = '\0';
    return 0;
}

// 测试普通的 JSON_STR 对象
TEST(json_save, str)
{
    JSON *json;
    buf_t result;
    const char *expect = "hello world\n";

    json = json_new_str("hello world");
    ASSERT_TRUE(json);

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试含特殊符号的 JSON_STR 对象
TEST(json_save, special_str)
{
    JSON *json;
    buf_t result;
    const char *expect = "hello\\njson\\v\\t\\a\\f\\b\\r\n";

    json = json_new_str("hello\njson\v\t\a\f\b\r");
    ASSERT_TRUE(json);

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试普通的 JSON_NUM 对象，整数
TEST(json_save, integer)
{
    JSON *json;
    buf_t result;
    const char *expect = "10\n";

    json = json_new_num(10);
    ASSERT_TRUE(json);

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试普通的 JSON_NUM 对象，小数
TEST(json_save, decimal)
{
    JSON *json;
    buf_t result;
    const char *expect = "1.58\n";

    json = json_new_num(1.58);
    ASSERT_TRUE(json);

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试普通的 JSON_BOL 对象
TEST(json_save, bol)
{
    JSON *json;
    buf_t result;
    const char *expect = "false\n";

    json = json_new_bool(0);
    ASSERT_TRUE(json);

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试普通的 JSON_OBJ 对象
TEST(json_save, obj)
{
    JSON *json;
    buf_t result;
    const char *expect = "key: hello\nname: world\n";

    json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "key", json_new_str("hello")));
    ASSERT_TRUE(json_add_member(json, "name", json_new_str("world")));

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试普通的 JSON_ARR 对象
TEST(json_save, arr)
{
    JSON *json;
    buf_t result;
    const char *expect = "- he\n- bo\n- tao\n";

    json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_str(json, "he"));
    ASSERT_TRUE(json_arr_add_str(json, "bo"));
    ASSERT_TRUE(json_arr_add_str(json, "tao"));

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试嵌套 JSON_OBJ 和 JSON_ARR 的 JSON_OBJ 对象
TEST(json_save, obj_embed_obj_arr)
{

    buf_t result;
    const char *expect = "1: he\n2: 1\n3: \n  3.1: bo\n  3.2: tao\n4: \n  - hello\n  - world\n";

    JSON *json;
    json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);
    JSON *obj = json_new(JSON_OBJ);
    ASSERT_TRUE(obj);
    JSON *arr = json_new(JSON_ARR);
    ASSERT_TRUE(arr);

    ASSERT_TRUE(json_add_member(obj, "3.1", json_new_str("bo")));
    ASSERT_TRUE(json_add_member(obj, "3.2", json_new_str("tao")));

    ASSERT_TRUE(json_arr_add_str(arr, "hello"));
    ASSERT_TRUE(json_arr_add_str(arr, "world"));

    ASSERT_TRUE(json_add_member(json, "1", json_new_str("he")));
    ASSERT_TRUE(json_add_member(json, "2", json_new_num(1)));
    ASSERT_TRUE(json_add_member(json, "3", obj));
    ASSERT_TRUE(json_add_member(json, "4", arr));

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试嵌套 JSON_OBJ 的 JSON_ARR 对象
TEST(json_save, obj_embed_obj)
{

    buf_t result;
    const char *expect = "- 1.1: bo\n  1.2: tao\n";

    JSON *json;
    json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    JSON *obj = json_new(JSON_OBJ);
    ASSERT_TRUE(obj);

    ASSERT_TRUE(json_add_member(obj, "1.1", json_new_str("bo")));
    ASSERT_TRUE(json_add_member(obj, "1.2", json_new_str("tao")));

    ASSERT_TRUE(json_add_element(json, obj));

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

// 测试嵌套 JSON_OBJ 和 JSON_ARR 的 JSON_ARR 对象
TEST(json_save, arr_embed_obj_arr)
{

    buf_t result;
    const char *expect = "- he\n- 1\n- 1.1: bo\n  1.2: tao\n- - hello\n  - world\n";

    JSON *json;
    json = json_new(JSON_ARR);
    ASSERT_TRUE(json);
    JSON *obj = json_new(JSON_OBJ);
    ASSERT_TRUE(obj);
    JSON *arr = json_new(JSON_ARR);
    ASSERT_TRUE(arr);

    ASSERT_TRUE(json_add_member(obj, "1.1", json_new_str("bo")));
    ASSERT_TRUE(json_add_member(obj, "1.2", json_new_str("tao")));

    ASSERT_TRUE(json_arr_add_str(arr, "hello"));
    ASSERT_TRUE(json_arr_add_str(arr, "world"));

    ASSERT_TRUE(json_add_element(json, json_new_str("he")));
    ASSERT_TRUE(json_add_element(json, json_new_num(1)));
    ASSERT_TRUE(json_add_element(json, obj));
    ASSERT_TRUE(json_add_element(json, arr));

    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_type
//----------------------------------------------------------------------------------------------------

TEST(json_type, JSON_OBJ)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);
    ASSERT_TRUE(json_type(json) == JSON_OBJ);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_num
//----------------------------------------------------------------------------------------------------

// 测试正常情况
TEST(json_num, normal)
{
    JSON *json = json_new_num(100);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_num(json, 10) == 100);
    json_free(json);
}

// 测试 json 为  NULL
TEST(json_num, json_null)
{
    ASSERT_TRUE(json_num(NULL, 10) == 10);
}

// 测试 json 类型不为 JSON_NUM
TEST(json_num, json_obj)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_num(json, 10) == 10);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_bool
//----------------------------------------------------------------------------------------------------

// 测试正常情况
TEST(json_bool, normal)
{
    JSON *json = json_new_bool(10);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_bool(json) == 10);
    json_free(json);
}

// 测试 json 为 NULL
TEST(json_bool, json_null)
{
    ASSERT_TRUE(json_bool(NULL) == FALSE);
}

// 测试 json 为非 JSON_BOL 类型
TEST(json_bool, JSON_OBJ)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_bool(json) == FALSE);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_str
//----------------------------------------------------------------------------------------------------

// 测试正常情况
TEST(json_str, normal)
{
    JSON *json = json_new_str("he");
    ASSERT_TRUE(json);

    ASSERT_TRUE(strcmp(json_str(json, "hao"), "he") == 0);
    json_free(json);
}

// 测试 json 为  NULL
TEST(json_str, json_null)
{
    ASSERT_TRUE(strcmp(json_str(NULL, "hao"), "hao") == 0);
}

// 测试 json 类型不为 JSON_NUM
TEST(json_str, json_obj)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(strcmp(json_str(json, "hao"), "hao") == 0);
    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_get_element
//----------------------------------------------------------------------------------------------------

// 测试 idx 正常
TEST(json_get_element, normal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    json_arr_add_num(json, 10);

    const JSON *item = json_get_element(json, 0);
    ASSERT_TRUE(item);
    ASSERT_TRUE(json_num(item, 100) == 10);

    json_free(json);
}

// 测试 idx 异常
TEST(json_get_element, abnormal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    json_arr_add_num(json, 10);

    const JSON *item = json_get_element(json, 2);
    ASSERT_TRUE(!item);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_get_num
//----------------------------------------------------------------------------------------------------

// 测试正常情况
TEST(json_obj_get_num, normal)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_num(10)));

    ASSERT_TRUE(json_obj_get_num(json, "he", 100) == 10);

    json_free(json);
}

// 测试 key 不存在
TEST(json_obj_get_num, key_nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "bo", json_new_num(10)));

    ASSERT_TRUE(json_obj_get_num(json, "he", 100) == 100);

    json_free(json);
}

// 测试 key 存在，但是对应的值不是 JSON_NUM 类型
TEST(json_obj_get_num, key_exist_non_json_num)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_str("10")));

    ASSERT_TRUE(json_obj_get_num(json, "he", 100) == 100);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_get_bool
//----------------------------------------------------------------------------------------------------

// 测试正常情况
TEST(json_obj_get_bool, normal)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_bool(10)));

    ASSERT_TRUE(json_obj_get_bool(json, "he") == 10);

    json_free(json);
}

// 测试 key 不存在
TEST(json_obj_get_bool, key_nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "bo", json_new_bool(10)));

    ASSERT_TRUE(json_obj_get_bool(json, "he") == FALSE);

    json_free(json);
}

// 测试 key 存在，但是对应的值不是 JSON_BOL 类型
TEST(json_obj_get_bool, key_exist_non_json_bol)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_str("10")));

    ASSERT_TRUE(json_obj_get_bool(json, "he") == FALSE);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_get_num
//----------------------------------------------------------------------------------------------------

// 测试 idx 正常
TEST(json_arr_get_num, normal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_num(json, 10));
    ASSERT_TRUE(json_arr_add_num(json, 20));

    ASSERT_TRUE(json_arr_get_num(json, 0, 100) == 10);

    json_free(json);
}

// 测试 idx 超出正常范围
TEST(json_arr_get_num, abnormal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_num(json, 10));
    ASSERT_TRUE(json_arr_add_num(json, 20));

    ASSERT_TRUE(json_arr_get_num(json, 2, 100) == 100);

    json_free(json);
}

// 测试 idx 对应的数据不是 JSON_NUM 类型
TEST(json_arr_get_num, non_json_num)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_str(json, "he"));
    ASSERT_TRUE(json_arr_add_num(json, 20));

    ASSERT_TRUE(json_arr_get_num(json, 0, 100) == 100);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_get_bool
//----------------------------------------------------------------------------------------------------

// 测试 idx 正常
TEST(json_arr_get_bool, normal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_bool(json, 10));
    ASSERT_TRUE(json_arr_add_bool(json, 20));

    ASSERT_TRUE(json_arr_get_bool(json, 0) == 10);

    json_free(json);
}

// 测试 idx 超出正常范围
TEST(json_arr_get_bool, abnormal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_bool(json, 10));
    ASSERT_TRUE(json_arr_add_bool(json, 20));

    ASSERT_TRUE(json_arr_get_bool(json, 2) == FALSE);

    json_free(json);
}

// 测试 idx 对应的数据不是 JSON_NUM 类型
TEST(json_arr_get_bool, non_json_bol)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_str(json, "he"));
    ASSERT_TRUE(json_arr_add_num(json, 20));

    ASSERT_TRUE(json_arr_get_bool(json, 0) == FALSE);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_arr_get_str
//----------------------------------------------------------------------------------------------------

// 测试 idx 正常
TEST(json_arr_get_str, normal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_str(json, "he"));
    ASSERT_TRUE(json_arr_add_str(json, "bo"));

    ASSERT_TRUE(strcmp(json_arr_get_str(json, 0, "nan"), "he") == 0);

    json_free(json);
}

// 测试 idx 超出正常范围
TEST(json_arr_get_str, abnormal)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_str(json, "he"));
    ASSERT_TRUE(json_arr_add_str(json, "bo"));

    ASSERT_TRUE(strcmp(json_arr_get_str(json, 2, "nan"), "nan") == 0);

    json_free(json);
}

// 测试 idx 对应的数据不是 JSON_STR 类型
TEST(json_arr_get_str, non_json_str)
{
    JSON *json = json_new(JSON_ARR);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_arr_add_bool(json, 10));
    ASSERT_TRUE(json_arr_add_num(json, 20));

    ASSERT_TRUE(strcmp(json_arr_get_str(json, 0, "nan"), "nan") == 0);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_set_num
//----------------------------------------------------------------------------------------------------

// 测试键名存在
TEST(json_obj_set_num, key_exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new(JSON_NUM)));

    ASSERT_TRUE(json_obj_set_num(json, "he", 20) == 0);

    ASSERT_TRUE(json_obj_get_num(json, "he", 100) == 20);

    json_free(json);
}

// 测试键名不存在
TEST(json_obj_set_num, key_nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "bo", json_new_num(10)));

    ASSERT_TRUE(json_obj_set_num(json, "he", 20) != 0);

    ASSERT_TRUE(json_obj_get_num(json, "bo", 100) == 10);

    json_free(json);
}

// 测试键名存在但对应的值的类型不是 JSON_NUM
TEST(json_obj_set_num, key_exist_no_json_num)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_str("tao")));

    ASSERT_TRUE(json_obj_set_num(json, "he", 20) != 0);

    ASSERT_TRUE(strcmp(json_obj_get_str(json, "he", "nan"), "tao") == 0);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_set_bool
//----------------------------------------------------------------------------------------------------

// 测试键名存在
TEST(json_obj_set_bool, key_exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new(JSON_BOL)));

    ASSERT_TRUE(json_obj_set_bool(json, "he", 20) == 0);

    ASSERT_TRUE(json_obj_get_bool(json, "he") == 20);

    json_free(json);
}

// 测试键名不存在
TEST(json_obj_set_bool, key_nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "bo", json_new_bool(10)));

    ASSERT_TRUE(json_obj_set_bool(json, "he", 20) != 0);

    ASSERT_TRUE(json_obj_get_bool(json, "bo") == 10);

    json_free(json);
}

// 测试键名存在但对应的值的类型不是 JSON_BOL
TEST(json_obj_set_bool, key_exist_no_json_bol)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_str("tao")));

    ASSERT_TRUE(json_obj_set_bool(json, "he", 20) != 0);

    ASSERT_TRUE(strcmp(json_obj_get_str(json, "he", "nan"), "tao") == 0);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_obj_set_str
//----------------------------------------------------------------------------------------------------

// 测试键名存在
TEST(json_obj_set_str, key_exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new(JSON_STR)));

    ASSERT_TRUE(json_obj_set_str(json, "he", "bo") == 0);

    ASSERT_TRUE(strcmp(json_obj_get_str(json, "he", "nan"), "bo") == 0);

    json_free(json);
}

// 测试键名不存在
TEST(json_obj_set_str, key_nonexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "bo", json_new_str("tao")));

    ASSERT_TRUE(json_obj_set_str(json, "he", "hello") != 0);

    ASSERT_TRUE(strcmp(json_obj_get_str(json, "bo", "nan"), "tao") == 0);
    ASSERT_TRUE(strcmp(json_obj_get_str(json, "he", "nan"), "nan") == 0);

    json_free(json);
}

// 测试键名存在但对应的值的类型不是 JSON_STR
TEST(json_obj_set_str, key_exist_no_json_str)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json);

    ASSERT_TRUE(json_add_member(json, "he", json_new_num(10)));

    ASSERT_TRUE(json_obj_set_str(json, "he", "hello") != 0);

    ASSERT_TRUE(json_obj_get_num(json, "he", 100) == 10);
    ASSERT_TRUE(strcmp(json_obj_get_str(json, "he", "nan"), "nan") == 0);

    json_free(json);
}

//----------------------------------------------------------------------------------------------------
//  json_get
//----------------------------------------------------------------------------------------------------

// TEST(json_get, self)
// {
//     JSON *json = json_new_num(20);
//     const JSON *ret;

//     ASSERT_TRUE(json != NULL);

//     ret = json_get(json, "");
//     ASSERT_TRUE(json == ret);

//     ret = json_get(json, "hello");
//     ASSERT_TRUE(ret == NULL);

//     json_free(json);
// }

int main(int argc, char **argv)
{
    return xtest_start_test(argc, argv);
}
