# API 设计 方案一

## json_new :ok_hand:

返回值：堆分配的 JSON 值

参数：JSON 值的类型

功能：创建特定类型的 JSON，需要为 JSON 分配堆内存。

注意事项：

1. 在函数内部判断内存分配是否成功

## json_add_member :ok_hand:

返回值：成功时返回 JSON*，失败时返回 NULL

参数：json JSON 对象，key 键名，val JSON对象类型的键值

功能：向 json 对象中添加一个键值对，键名为 key，键值为 val

注意事项：

1. val 是堆分配的内存

2. 参数检查

3. val 的所有权会转移给函数，因此需要在函数中妥善处理 val，若函数内部发生失败，则需要释放 val

4. 如果存在键名相同的情况，则用新的键值覆盖旧的键值

5. 要支持以下写法

   ```c
   json_add_member(json,"post",json_new_num(80));
   ```

   所以要允许 val 为 NULL。当 val 为 NULL 时，分配失败。



JSON 允许键名为空 `“”`，不允许为 null，允许为数字，此时数字被当做字符串看待。同样允许为 undefined 和 NaN，此时依旧当作字符串看待。使用对象名作为 JSON 的键名时，会将对象名视为字符串处理，如果使用赋值的方式将对象设置为键名，则会将对象转换为字符串 `[object object]`。JSON 中相同键名的成员会相互覆盖。

## json_new_bool :ok_hand:

功能：新建一个 BOOL 类型的 JSON 值

参数：val 无符号整数

返回值：成功则返回 JSON*，失败则返回 NULL

注意事项：

1. 调用 json_new 后检查返回值

## json_new_num :ok_hand:

功能：新建一个数字类型的 JSON 值

参数：val double 类型的值

返回值：成功时返回 JSON*，失败时返回 NULL

注意事项：

1. 调用 json_new 后检查返回值

## json_new_str :ok_hand:

功能：创建一个字符串类型的 JSON 对象

参数：str 指向字符数组的常指针

返回值：成功时返回 JSON*，失败时返回 NULL

注意事项：

1. 参数检查。assert(str) 确保 str 不是空指针
2. 调用 json_new 后检查返回值
3. 调用 strdup 后检查返回值，strdup 失败后要释放 json_new 申请的内存

## json_obj_get_str :ok_hand:

功能：获取 JSON 对象中键名为 key 的值

参数：json JSON 对象、key 键名、def 缺省值

返回值：获取成功时，返回获取到的字符串，失败时，返回缺省值

注意事项：

1. 使用 get_child 获取键名为 key 的 JSON 对象的子节点
2. 检查 get_child 的返回结果

## get_child :ok_hand:

功能：获取键名为 key，类型为 expect_type 的子节点

参数：json JSON 对象，key 键名，expect_type 期望类型

返回值：成功时，返回 JSON 值，失败时返回 NULL

注意事项：

1. 在 json_get_member 中进行参数检查
2. 检查 json_get_member 的返回值

## json_get_member :ok_hand:

功能：从 JSON 对象中获取键名为 key 的子成员

参数：json JSON 对象、key 键值

返回值：成功时返回 JSON*，失败时返回 NULL

注意事项：

1. 参数检查。json、json 的类型要为 JSON_OBJ、json 的 obj 属性正常、key 不是 NULL 也不是空串
2. 遍历 json 的 obj 属性的键值对数组，寻找匹配的子成员。使用 strcmp 函数。

## json_arr_add_str :ok_hand:

功能：向数组类型的 JSON 对象中添加字符串数据

参数：json JSON 对象、val 待添加的值

返回值：成功时返回 1，失败时返回 -1

注意事项：

1. 调用 json_new_str 将 val 转换为字符串类型的 JSON 对象
2. 调用 json_add_element 函数向数组中添加一个元素
3. 对 val 进行参数检查
4. 检查 json_new_str 和 json_add_element 的返回值

## json_arr_add_num :ok_hand:

功能：向数组型 JSON 对象中添加数字类型的数据

参数：json JSON 对象，val 待添加的数据

返回值：成功时返回 1，失败时返回 -1

注意事项：

1. 调用 json_new_num 将 val 转换为 JSON 对象

## json_arr_count :ok_hand:

功能：获取数组型 JSON 对象中元素的个数

参数：json JSON 对象

返回值：成功时返回元素个数，失败时返回 -1

注意事项：

1. 参数检查

## json_add_element :ok_hand:

功能：向数组型 JSON 对象中添加新元素

参数：json JSON 对象、val 要添加的 JSON 对象

返回值：成功时返回 val，失败时返回 NULL

注意事项：

1. 该函数会转移 val 的所有权，当函数执行失败时，要释放 val 的内存
2. 允许 val 为 NULL，当 val == NULL 时，函数执行失败。
3. 检查 json 是否为 NULL 和 json 的类型是否为 JSON_ARR

## json_free :ok_hand:

返回值：void

参数：JSON *json

功能：释放 JSON 占用的内存

注意事项：

1. 参数检查，可以释放空指针。
2. 要释放 json 中所有键值对的内存
3. 检查 JSON 的类型，JSON_NUM、JSON_BOL 类型直接释放 JSON 指针
4. JSON_STR 类型则应先释放 char* 指针
5. JSON_ARR 类型则遍历数组，递归释放内存
6. JSON_OBJ 类型则遍历每一个属性，递归释放内存，如果没有属性，则直接释放 obj 的 kvs 指针。属性的键名占用的内存需要释放，属性值递归调用 json_free。

## json_save :ok_hand:

功能：将 JSON 以 YAML 格式输出，保存到名字为 fname 的文件之

参数：json 要输出的 JSON 对象、fname 输出文件名

返回值：成功时返回 0，失败时返回一个负数

## json_type​ :ok_hand:

功能：判断 JSON 对象的类型

参数：json 要判断的 JSON 对象

返回值：json 对象的类型

注意事项：

1. 使用断言建立契约，让调用者保证 json 不为 NULL

## json_num :ok_hand:

功能：获取 JSON_NUM 类型的 JSON 对象的数值

参数：json 数值类型的 JSON 对象、def 类型不匹配是的返回值

返回值：获取成功时返回 JSON 对象的值，失败时返回 def

## json_bool :ok_hand:

功能：获取 JSON_BOL 类型的 JSON 对象的值

参数：json JSON_BOL 类型的 JSON 对象

返回值：如果json是合法的JSON_BOL类型，返回其数值，否则返回FALSE

## json_str :ok_hand:

## json_get_element :ok_hand:

## json_obj_get_num :ok_hand:

## json_obj_get_bool :ok_hand:

## json_arr_get_num :ok_hand:

## json_arr_get_bool :ok_hand:

## json_arr_get_str :ok_hand:

## json_obj_set_num :ok_hand:

## json_obj_set_bool :ok_hand:

## json_obj_set_str :ok_hand:



# 单元测试

## json_new :ok_hand:

1. 创建 JSON_BOL 类型的 JSON 对象，初始化为 false
2. 创建 JSON_NUM 类型的 JSON 对象，初始化为 0
3. 创建 JSON_STR 类型的 JSON 对象，初始化为 NULL
4. 创建 JSON_ARR 类型的 JSON 对象，初始化为空数组，count 为 0，size 为 1
5. 创建 JSON_OBJ 类型的 JSON 对象，初始化为空对象，对象的键值对数组的 count 为 0，size 为 1

## json_add_member :ok_hand:

1. 通过断言规定契约，保证 json 不为 NULL 且类型为 JSON_OBJ 且格式正确，key 不为 NULL 且不为空串。
2. 键名不重复，未触发扩容机制
3. 键名不重复，触发扩容机制
4. 键名重复
5. val 为 NULL

## json_add_element :ok_hand:

1. 通过断言规定契约，保证 json 不为 NULL 且类型为 JSON_ARR 且格式正确
2. val 不为 NULL，未触发扩容机制
3. val 不为 NULL，触发扩容机制
4. val 为 NULL

## json_free :ok_hand:

1. 允许 json 为 NULL
2. 释放 JSON_BOL 类型的 JSON 对象
3. 释放 JSON_NUM 类型的 JSON 对象
4. 释放 JSON_STR 类型的 JSON 对象
5. 释放 JSON_ARR 类型的 JSON 对象，json 中包含 JSON_BOL、JSON_NUM、JSON_STR、JSON_ARR 和 JSON_OBJ，其中 JSON_ARR 和 JSON_OBJ 为空。
6. 释放 JSON_OBJ 类型的 JSON 对象，json 中包含 JSON_BOL、JSON_NUM、JSON_STR、JSON_ARR 和 JSON_OBJ，其中 JSON_ARR 和 JSON_OBJ 为空。
7. 释放 JSON_ARR 类型的 JSON 对象，json 中包含 JSON_BOL、JSON_NUM、JSON_STR、JSON_ARR 和 JSON_OBJ，其中 JSON_ARR 和 JSON_OBJ 不为空。
8. 释放 JSON_OBJ 类型的 JSON 对象，json 中包含 JSON_BOL、JSON_NUM、JSON_STR、JSON_ARR 和 JSON_OBJ，其中 JSON_ARR 和 JSON_OBJ 不为空。

## json_new_bool :ok_hand:

1. val = 0
3. val = 333333333333

## json_new_num :ok_hand:

2. val = 1.3
3. val = 333333333333332222

## json_new_str :ok_hand:

1. 用断言建立契约，确保 str 不为 NULL
2. str = “aa”
3. str = “”
4. str = “\0”

## json_obj_get_str :ok_hand:

1. 键名存在
2. 键名不存在

## get_child :ok_hand:

1. 通过 json_obj_get_str 测试
2. 键名存在，类型匹配
3. 键名存在，类型不匹配
4. 键名不存在

## json_get_member :ok_hand:

1. 使用断言建立契约，让调用者保证 json 不为 NULL 且类型为 JSON_OBJ 且格式正确，key 不为 NULL 且不为空串
2. 键名存在
3. 键名不存在

## json_arr_add_str :ok_hand:

1. 使用断言建立契约，由调用者保证 json 不为 NULL 且类型为 JSON_ARR 且格式正确，val 不为 NULL
2. val = “he”
3. val = “”

## json_arr_add_num :ok_hand:

1. 使用断言建立契约，由调用者保证 json 不为 NULL 且类型为 JSON_ARR 且格式正确
2. val = 100

## json_arr_add_bool :ok_hand:

1. 使用断言建立契约，由调用者保证 json 不为 NULL 且类型为 JSON_ARR 且格式正确
2. val = 100

## json_arr_count :ok_hand:

1. 测试非数组类型的 JSON 对象
2. 测试空数组
3. 测试非空数组

## json_save :ok_hand:

1. 使用断言创建契约，让用户保证 json 不为 NULL，fname 不为 NULL 且不为空串。

2. 普通的 JSON_STR JSON 对象

   ```c
   // 输入
   JSON *json=json_new_str("hello world");
   // 输出
   const char *expect="hello world";
   ```

3. 含特殊符号的 JSON_STR 对象

   ```c
   // 输入
   JSON *json=json_new_str("hello\nworld");
   // 输出，将 str 中的换行符转意
   const char *expect="hello\\nworld";
   ```

4. 普通的 JSON_NUM 对象

   ```c
   // input
   JSON *json=json_new_num(10);
   // output
   const char *expect="10";
   ```

5. 普通的 JSON_BOL 对象

6. 普通的 JSON_OBJ 对象

7. 普通的 JSON_ARR 对象

8. 包含 JSON_OBJ 和 JSON_ARR 的 JSON_OBJ 对象

9. 包含 JSON_OBJ 和 JSON_ARR 的 JSON_ARR 对象

## json_type :ok_hand:    

1. 用断言确保 json 不为 NULL

## json_num :ok_hand:

1. json = NULL,0
2. json = json_new_str(“he”),0
3. json = json_new_num(10),100

## json_bool :ok_hand:

1. json = NULL
2. json 为 JSON_OBJ 类型
3. json = json_new_bool(10)

## json_str :ok_hand:

1. json = NULL,”hao”
2. json = json_new_str(“he”),”hao”
3. json = json_new_num(10),”hao”

## json_get_element :ok_hand:

1. 使用断言建立契约，让调用者保证 json 不为 NULL 且类型为 JSON_ARR 且格式正确
2. idx ≥ json->arr.count
3. idx ≥ 0 && idx ＜ json->arr.count

## json_obj_get_num :ok_hand:

1. 用断言简历契约，让调用者保证 json 不为 NULL 且为 JSON_OBJ 类型
2. json 为 JSON_OBJ，key 存在
3. json 为 JSON_OBJ，key 不存在
4. json 为 JSON_OBJ，key 存在，但是对应的值不是 JOSN_NUM 类型

## json_obj_get_bool :ok_hand:

1. 用断言简历契约，让调用者保证 json 不为 NULL 且为 JSON_OBJ 类型
2. json 为 JSON_OBJ，key 存在
3. json 为 JSON_OBJ，key 不存在
4. json 为 JSON_OBJ，key 存在，但是对应的值不是 JOSN_BOL 类型

## json_arr_get_num :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且 类型为 JSON_ARR 且格式正确。idx 会被当作无符号整数处理，所以不用担心会小于 0
2. idx 在正确范围内
3. idx 超出下标表示范围
4. idx 对应的数据不是 JSON_NUM 类型数据

## json_arr_get_bool :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且 类型为 JSON_ARR 且格式正确。idx 会被当作无符号整数处理，所以不用担心会小于 0
2. idx 在正确范围内
3. idx 超出下标表示范围
4. idx 对应的数据不是 JSON_BOL 类型数据

## json_arr_get_str :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且 类型为 JSON_ARR 且格式正确。idx 会被当作无符号整数处理，所以不用担心会小于 0
2. idx 在正确范围内
3. idx 超出下标表示范围
4. idx 对应的数据不是 JSON_STR 类型数据

## json_obj_set_num :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且类型为 JSON_OBJ 且格式正确，key 不为 NULL 且不为空串
2. 键名存在
3. 键名不存在
4. 键名存在但对应的值的类型不是 JSON_NUM

## json_obj_set_bool :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且类型为 JSON_OBJ 且格式正确，key 不为 NULL 且不为空串
2. 键名存在
3. 键名不存在
4. 键名存在但对应的值的类型不是 JSON_BOL

## json_obj_set_str :ok_hand:

1. 用断言建立契约，保证 json 不为 NULL 且类型为 JSON_OBJ 且格式正确，key 不为 NULL 且不为空串
2. 键名存在
3. 键名不存在
4. 键名存在但对应的值的类型不是 JSON_STR

# 问答

1. 为何要将结构体的定义放在 .c 文件中？

   为了实现数据封装。C 语言结构体的属性都是 public 的。要实现对结构体的封装，只允许用户通过 API 操作结构体，需要在 .h 文件中声明结构体和 AIP 函数，在 .c 文件中实现结构体和 API 函数。这样，用户无法获取结构体的定义，不知道结构体中具体有哪些属性，只能通过 API 操作结构体。

   当在其他文件中 include “.h” 文件时，只能定义 .h 文件中声明的结构体类型的指针，而不能定义结构体类型的变量。因为在定义变量时，编译器需要给变量分配内存空间，而编译器并不知道该结构体的具体结构，自然也无法分配空间，定义结构体的变量会导致编译错误。而定义指针就很简单了，指针就是一个四字节的无符号整数。同时，在这种情况下也无法通过指针访问结构体的字段，强行访问会导致编译错误，因为编译器不知道结构体的实现细节。

   通过上述方法很好的实现了数据封装，在 .c 文件之外都只能通过 API 访问结构体，只是需要写很多操作函数，包括为结构体指针变量分配空间的函数。
   
2. 全局指针在释放后要置为 NULL

3. 保证函数满足事务原则，要么函数执行成功，要么相当于没有执行。

4. 内存分配时要保证分配成功后再赋值给别人，否则会造成原来的内存泄露。

5. 断言是契约，调用者要保证断言通过，单元测试时可以默认断言都通过了。

6. 根据经验，当一次内存分配失败后，接下来的内存分配一般都会失败。可以只检查最后一次内存分配，若失败，则可视为前面的分配都失败，记得一个一个的释放内存。

7. 文档内部的函数，可以用 static 修饰，将函数的使用范围限制在文档内部。当单元测试该函数时，可以通过 include .c 文件来使用该函数。

8. 内存分配借鉴智能指针的思想。

9. 实现一个函数之前先写单测，再写函数。一个函数一个函数的来，不要所有函数一起来。

10. LINXU 下函数返回 0 为成功，其他值都是失败

11. stderr 是没有缓冲的，内容会立即输出到目标位置，具有实时性。stdout 是有缓冲的，没有实时性。