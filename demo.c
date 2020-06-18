#include "json.h"
#include <stdio.h>

#if ACTIVE_PLAN == 1
//---------------------------------------------------------------------------
//  方案1
//---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    JSON *json = json_new(JSON_OBJ);
    if (!json)
        goto failed_;

    // basic
    JSON *basic = json_new(JSON_OBJ);
    if (!basic)
        goto failed_;
    json_add_member(json, "basic", basic);

    json_add_member(basic, "enable", json_new_bool(TRUE));

    // 添加 ip 属性
    if (!json_add_member(basic, "ip", json_new_str("200.200.3.61")))
        goto failed_;
    // 检查是否成功添加
    const char *ip = json_obj_get_str(basic, "ip", NULL);
    if (!ip)
        goto failed_;

    json_add_member(basic, "port", json_new_num(389));

    json_add_member(basic, "timeout", json_new_num(10));

    json_add_member(basic, "basedn", json_new_str("aaa"));

    json_add_member(basic, "fd", json_new_num(-1));

    json_add_member(basic, "maxcnt", json_new_num(133333333333));

    JSON *dns = json_new(JSON_ARR);
    if (!dns)
        goto failed_;
    json_add_member(basic, "dns", dns);

    if (json_arr_add_str(dns, "200.200.0.1") == -1)
        goto failed_;
    if (json_arr_add_str(dns, "200.0.0.254") == -1)
        goto failed_;
    if (json_arr_count(dns) != 2)
        goto failed_;

    // advance
    JSON *advance = json_new(JSON_OBJ);
    if (!advance)
        goto failed_;
    json_add_member(json, "advance", advance);

    dns = json_new(JSON_ARR);
    if (!dns)
        goto failed_;
    json_add_member(advance, "dns", dns);

    JSON *portpool = json_new(JSON_ARR);
    if (!portpool)
        goto failed_;
    json_add_member(advance, "portpool", portpool);

    json_add_member(advance, "url", json_new_str("http://200.200.0.4/main"));

    json_add_member(advance, "path", json_new_str("/etc/sinfors"));

    json_add_member(advance, "value", json_new_num(3.14));

    // 在 portpool 中添加内容
    if (json_arr_add_num(portpool, 130) == -1)
        goto failed_;
    if (json_arr_add_num(portpool, 131) == -1)
        goto failed_;
    if (json_arr_add_num(portpool, 132) == -1)
        goto failed_;
    if (json_arr_count(portpool) != 3)
        goto failed_;

    // 在 dns 中添加内容
    JSON *temp = json_new(JSON_OBJ);
    if (!temp)
        goto failed_;
    json_add_member(temp, "name", json_new_str("huanan"));
    json_add_member(temp, "ip", json_new_str("200.200.0.1"));
    json_add_element(dns, temp);

    temp = json_new(JSON_OBJ);
    if (!temp)
        goto failed_;
    json_add_member(temp, "name", json_new_str("huabei"));
    json_add_member(temp, "ip", json_new_str("200.0.0.254"));
    json_add_element(dns, temp);

    temp = json_new(JSON_ARR);
    if (!temp)
        goto failed_;
    json_add_element(dns, temp);

    json_arr_add_str(temp, "he");
    json_arr_add_str(temp, "bo");

    JSON *tempObj = json_new(JSON_OBJ);
    if (!tempObj)
        goto failed_;
    json_add_element(temp, tempObj);
    json_add_member(tempObj, "name", json_new_str("tao"));

    if (json_arr_count(dns) != 3)
        goto failed_;

    //...
    int ret = json_save(json, "./test.yml");
    json_free(json);
    return ret < 0 ? 1 : 0;
failed_:
    json_free(json);
    return 1;
}

#elif ACTIVE_PLAN == 2
//---------------------------------------------------------------------------
//  方案2
//---------------------------------------------------------------------------

int main()
{
    int ret = 0;
    JSON *json = json_new(JSON_OBJ);

    json_set(json, "basic", json_new(JSON_OBJ));
    json_set(json, "basic.ip", json_new_str("200.200.3.61"));
    json_set(json, "basic.enable", json_new_bol(true));
    json_set(json, "basic.port", json_new_int(389));
    json_set(json, "basic.timeout", json_new_int(10));
    json_set(json, "basic.dns[0]", json_new_str("200.200.0.1"));
    ret = json_set(json, "basic.dns[1]", json_new_str("200.0.0.254"));
    if (ret < 0)
        goto failed_;

    const JSON *val = json_get(json, "basic.dns");

    int port = json_int(json_get(json, "basic.port"), 80);
    bool enable = json_bool(json_get(json, "basic.enable"));
    const char *ip = json_str(json_get(json, "basic.ip"), "127.0.0.1");
    const char *dns0 = json_str(json_get(val, "[0]"), "192.168.1.1");
    //...
    ret = json_save(json, "./test.yml");
    json_free(json);
    return 0;
failed_:
    json_free(json);
    return 1;
}

#endif //ACTIVE_PLAN

#if 0
//---------------------------------------------------------------------------
//  方案3
//---------------------------------------------------------------------------

int main()
{
    JSON *json = json_new(JSON_OBJ);

    json_eval(json, "basic = {}");
    json_eval(json, "basic.ip = \"200.200.3.61\"");
    json_eval(json, "basic.enable = true");
    json_eval(json, "basic.port = 389");
    json_eval(json, "basic.timeout = 10");
    json_eval(json, "basic.dns[0] = \"200.200.0.1\"");
    json_eval(json, "basic.dns[1] = \"200.0.0.254\"");

...
    JSON *val = json_get(json, "basic.port");
    int port = json_int(json, "basic.port");
    bool enable = json_bool(json, "basic.enable");
    const char *ip = json_str(json, "basic.ip");

...
    json_free(json);
    return 0;
}

#endif
