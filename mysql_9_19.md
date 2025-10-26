##### 初识：

1. mysql -h 127.0.0.1 -P 3306 -u root -p	-h 指明登录部署了mysql服务的主机 -P指明端口号 -u 指明登录用户 -p 指明登陆密码
2. mysql为客户端，mysqld为服务端（一般绑定为3306端口号）
3. mysql本质：基于CS模式的一种网络服务，数据库一般指的是，在磁盘或者内存中存储的特定结构组织的数据
4. 建立数据库，其实就是在Linux目录下建立一个目录，在数据库内建立表，就是在LInux下创建对应的文件，
5. 主流数据库：redis，sql sever ，h2

MySQL 数据逻辑存储：

MySQL 数据的逻辑存储是分层级的抽象结构，从高到低依次为：数据库（Database）表（Table）行（Row）/ 记录（Record）列（Column）/ 字段（Field）索引（Index）视图（View）

MySQL 架构

MySQL 采用客户端 - 服务器（C/S）架构，整体分为四层（从客户端到存储介质）：

连接层（Connection Layer）

负责客户端与服务器的连接管理（TCP 连接）、身份认证（用户名 / 密码验证）、权限检查（如用户是否有某库的访问权限）。

服务层（Service Layer）

MySQL 的核心层，负责 SQL 语句的处理：解析器：将 SQL 解析为语法树（检查语法正确性）。优化器：对 SQL 进行优化（如选择最优索引、调整 Join 顺序），生成执行计划。缓存（Query Cache）：缓存查询结果（MySQL 8.0 已移除，因维护成本高）。内置函数：提供字符串处理、日期计算等函数支持。

存储引擎层（Storage Engine Layer）

负责数据的物理存储和读取，采用插件式架构（可动态切换）。存储引擎不影响 SQL 语法，但决定了数据的存储格式、事务支持、锁机制等（如 **InnoDB、MyISAM** 等）。

文件系统层（File System Layer）

将数据持久化到磁盘，存储形式包括：数据库目录、表数据文件、索引文件、日志文件（如 binlog、redo log）等。

SQL 分类

DDL（数据定义语言，Data Definition Language）

用于创建 / 修改 / 删除数据库对象（数据库、表、索引等），操作会自动提交，不可回滚。

示例：CREATE DATABASE（建库）、CREATE TABLE（建表）、ALTER TABLE（修改表）、DROP TABLE（删表）、CREATE INDEX（建索引）。

DML（数据操纵语言，Data Manipulation Language）

用于操作表中的数据（增删改），需手动提交（或自动提交，取决于事务设置）。

示例：INSERT（插入）、UPDATE（更新）、DELETE（删除）。

DCL（数据控制语言，Data Control Language）

用于管理数据库权限（用户创建、授权、撤销权限）。

示例：CREATE USER（创建用户）、GRANT（授权）、REVOKE（撤销权限）、DROP USER（删除用户）。

存储引擎

存储引擎是 MySQL 处理数据的 “底层引擎”，决定了数据的存储方式、支持的功能（如事务、锁）等。常见存储引擎及特点：

InnoDB（MySQL 5.5+ 默认）

支持事务（ACID 特性）、行级锁（高并发场景友好）、外键约束。

数据存储在 .ibd 文件中（表数据 + 索引），通过 redo log 和 undo log 保证事务安全。

适用场景：需要事务支持的业务（如电商订单、金融交易）。

MyISAM

不支持事务和外键，支持表级锁（并发写入性能差），但查询速度快，支持全文索引。

数据存储在 .MYD（数据）和 .MYI（索引）文件中。

适用场景：只读或读写少的业务（如博客文章、日志存储）。

##### 库的操作：

1. 创建数据库：create database db\_name --本质就是在服务器/var/lib/mysql创建一个目录；
2. 删除数据库：drop database  db\_name --删除目录（不要随意删除数据库！）
3. 创建数据库的时候，有两个编码集：数据库编码集--存储数据编码，数据库检验集--支持数据库进行字段比较使用的编码，本质也是一种读取数据库数据采用的编码格式（前者可以理解为存储编码，后者可以理解为查找编码）
4. 查看系统默认字符集以及校验规则：show variables like 'character\_set\_database';show variables like 'collation\_database';
5. 查看数据库支持的字符集以及校验规则：show charset;show collation;
6. 创建数据库时，默认采用utf8格式1编码，可以显示指定：create database 'your database name' charset=utf8 collate utf8\_general\_ci;（不区分大小写）（在/var/lib/mysql目录下找到创建的数据库目录，在mysql5.7版本中可以找到.opt文件，记录了两个编码格式）
7. 如果存储编码相同，但是查找编码不同，那么查找的方式就会有区别，查找的结果也不一样，就比如utf8\_general\_ci和utf8\_bin就有是否区分大小写的不同
8. 如果想要修改数据库的编码格式：alter database 'your database name' charset=utf8 collate utf8\_general\_ci;
9. 查看数据库创建指令：show create database 'your database name'
10. 使用数据库：use 'your database name'
11. 查看当前数据库：select database();
12. 数据库的备份与还原：mysqldump -P 3306 -u root -p -B 'your database name' > '希望保存路径' （注意这不是SQL语句！） eg.mysqldump -P 3306 -u root -p -B test1 > /home/drw1/mysql.sql（需要保存在.sql文件中，文件记录了数据库进行的所有操作，而非数据，如果备份时没有带上-B参数， 在恢复数据库时，需要先创建空数据库，然后使用数据库，再使用source来还原）；source '保存路径' （这是恢复语句，是SQL语句！）
13. 查看连接情况：show processlist;

##### 表的操作：

1. 创建表：create table table\_name (
   field1 datatype,
   field2 datatype,
   field3 datatype
   ) character set 字符集 collate 校验规则 engine 存储引擎;
   field 表示列名；datatype 表示列的类型；character set 字符集，如果没有指定字符集，则以所在数据库的字符集为准；collate 校验规则，如果没有指定校验规则，则以所在数据库的校验规则为准
   eg.create table if not exists user1(
   id int,
   name varchar(20) comment '用户名',
   address char(32) comment '地址',
   birthday date comment '生日'
   ) charset=utf8 collate=utf8\_general\_ci engine=InnoDB;
2. 查看表：show tables; 查看表结构：desc 表名;或者show create table 表名 \\G（查看详细信息）
3. 删除表：**drop** table 表名;
4. 打印表中数据：select \* from 表名;
5. 修改表：修改表名： alter table 原表名 **rename** to 新表名;
   增加表项：alter table 表名 **add** assets varchar(100) comment '路径' after 某个列;（在user表添加一个字段，用于保存路径，插入后对原数据没有影响）
   修改数据类型：alter table 表名 **modify** 对象名 varchar(60) comment '用户名';（修改信息是覆盖，如果不带上comment原来的comment会被覆盖！）
   删除表列：alter table 表名 **drop** 列名;（慎用！）
   表项修改：alter table 表名 change 要修改的对象 新对象 char(32) comment '图片路径';（最好记得带上comment）

##### 数据类型：

1. 数值类型：
   类型		字节		范围（有符号）范围（无符号，UNSIGNED）	说明
   TINYINT	1	-128 ~ 127	0 ~ 255	微小型整数（如状态标记）
   SMALLINT	2	-32768 ~ 32767	0 ~ 65535	小型整数（如数量）
   MEDIUMINT	3	-8388608 ~ 8388607	0 ~ 16777215	中型整数
   INT/INTEGER	4	-2147483648 ~ 2147483647	0 ~ 4294967295	常用整数（如 ID、年龄）
   BIGINT	8	-9223372036854775808 ~ 9223372036854775807	0 ~ 18446744073709551615	大型整数（如大数量、时间戳）
2. 当插入的数据不符合数据类型范围时，mysql会报错，这实际上是一种约束，也保证了存储的数据都是完整且符合范围的
3. bit类型：
   bit\[(M)] : 位字段类型。M表示每个值的位数，范围从1到64。如果M被忽略，默认为1。字段在显示时，是按照ASCII码对应的值显示。如果是1位，那么只能存储0或1.（如果不是字母可能显示不出来）
4. 小数类型：
   float\[(m, d)] \[unsigned] : M指定显示长度，d指定小数位数，占用空间4个字节。eg：float(4,2)表示的范围是-99.99 ~ 99.99，MySQL在保存值时会进行四舍五入，如果要插入100则不行，因为强制精度存储两位，3+2>4；如果带上unsigned选项，那么范围是0~99.99 ；
   double 则于float大相径庭，不过多赘述；
   decimal(m, d) \[unsigned] : 定点数m指定长度，d表示小数点的位数，float表示的精度大约是7位。decimal整数最大位数m为65。支持小数最大位数d是30。如果d被省略，默认为0.如果m被省略，默认是10。如果小数的精度高并且希望准确存储，推荐使用decimal。
5. 字符串类型：
   char(L): 固定长度字符串，L是可以存储的长度，单位为字符，最大长度值可以为255，char(2) 表示可以存放两个字符，可以是字母或汉字，但是不能超过2个， 最多只能是255，一个英文字母和一个汉字没有区别都是一个字符
   varchar(L): 可变长度字符串，L表示字符长度，最大长度65535个字节，varchar长度可以指定为0到65535之间的值，但是有1 - 3 个字节用于记录数据大小，所以说有效字节数是65532。当我们的表的编码是utf8时，varchar(n)的参数n最大值是65532/3=21844\[因为utf中，一个汉字占用3个字节]，如果编码是gbk，varchar(n)的参数n最大是65532/2=32766（因为gbk中，一个汉字占用2字节）
6. 如何选择定长或变长字符串？
   如果数据确定长度都一样，就使用定长（char），比如：身份证，手机号，md5
   如果数据长度有变化,就使用变长(varchar), 比如：名字，地址，但是你要保证最长的能存的进去。
   定长的磁盘空间比较浪费，但是效率高。
   变长的磁盘空间比较节省，但是效率低。
   定长的意义是，直接开辟好对应的空间
   变长的意义是，在不超过自定义范围的情况下，用多少，开辟多少。
7. 日期和时间类型：
   常用的日期有如下三个：
   date :日期'yyyy-mm-dd' ，占用三字节
   datetime 时间日期格式 'yyyy-mm-dd HH:ii:ss' 表示范围从1000 到9999 ，占用八字节
   timestamp ：时间戳，从1970年开始的 yyyy-mm-dd HH:ii:ss 格式和datetime 完全一致，占用四字节，由于时间戳会自动生成，不需要传值，在插入表中时要带上赋值的表项：insert into usr (name, address, date) values('文', '区', '2006-08-21');
8. enum和set类型：
   语法：
   enum：枚举，“单选”类型；
   enum('选项1','选项2','选项3',...);
   该设定只是提供了若干个选项的值，最终一个单元格中，实际只存储了其中一个值；而且出于效率考虑，这些值实际存储的是“数字”，因为这些选项的每个选项值依次对应如下数字：1,2,3,....最多65535个；当我们添加枚举值时，也可以添加对应的数字编号。
   set：集合，“多选”类型；
   set('选项值1','选项值2','选项值3', ...);
   该设定只是提供了若干个选项的值，最终一个单元格中，设计可存储了其中任意多个值；而且出于效率考虑，这些值实际存储的是比特位，从第一个元素开始依次对应1248...的值，填入的是数字代表对应的所有元素的和，如果填入0就没有数据（与NULL区别开来）
   说明：不建议在添加枚举值，集合值的时候采用数字的方式，因为不利于阅读。
9. 集合查询使用select \* from usr where 性别='女';一比一匹配，set或者enum都适用，也可以使用数字也全都适用，
   也可以使用find\_ in\_ set函数：
   find\_in\_set(sub,str\_list) ：如果sub 在str\_list 中，则返回下标；如果不在，返回0；str\_list 用逗号分隔的字符串，select \* from usr where find\_in\_set('足球',爱好);

##### 表的约束：

1. 空属性：
   两个值：null（默认的）和not null(不为空)
   数据库默认字段基本都是字段为空，但是实际开发时，尽可能保证字段不为空，因为数据为空没办法参与运算。
   insert into testnull values(NULL)对不许为空的数据传参空会报错，可以在数据类型后设置not null
2. 默认值
   默认值：某一种数据会经常性的出现某个具体的值，可以在一开始就指定好，在需要真实数据的时候，用户可以选择性的使用默认值，可以在数据类型后设置default 默认值来设置，如果默认值设定了，那么用户将来插入，有具体的数据就用用户的，没有则会设定为默认值（tips:default和not null并不冲突，mysql允许同时设置这两个，如果用户插入数值则不允许为NULL，否则只能插入合法数据）（大多数数据类型默认设置为default null，如果设置了not null，mysql就不会再有default null了）
3. 列描述
   列描述：comment，没有实际含义，专门用来描述字段，会根据表创建语句保存，用来给程序员或DBA来进行了解。（过于简单不进行演示）
4. zerofill：设置格式化输出
   补充： int(11)  / int(10) unsigned，int类型后面的默认数字实质是二进制位数，而如果存储的数字位数<设定的位数并设定了zerofill，高位次会用0补充，但如果>=设定位数则正常保存数字
5. 主键：primary key用来唯一的约束该字段里面的数据，**不能重复，不能为空，一张表中最多只能有一个主键**；主键所在的列通常是整数类型。
   案例：创建表的时候直接在字段上指定主键，主键约束：主键对应的字段中不能重复，一旦重复，操作失败。
   当表创建好以后但是没有主键的时候，可以再次追加主键：alter table 表名 add primary key(字段列表)，也可以删除主键：alter table 表名 drop primary key;
6. 复合主键：
   在创建表的时候，在所有字段之后，使用primary key(主键字段列表)来创建主键，如果有多个字段作为主键，可以使用复合主键：（一种复合字段只能存在一个）
   alter table testnull add primary key(id,gender);//追加
   alter table testnull drop primary key;//删除
   primary key(id, course)//在建表时设置复合主键
7. 自增长：
   auto\_increment：当对应的字段，不给值，会自动的被系统触发，系统会从当前字段中已经有的最大值+1操作，得到一个新的不同的值。通常和主键搭配使用，作为逻辑主键。
   自增长的特点:
   任何一个字段要做自增长，前提是本身是一个索引（key一栏有值）；
   自增长字段必须是整数；
   一张表最多只能有一个自增长；
   在建表时设置自增长，或者通过alter table modify修改属性设置，可以带上起始值
   create table if not exists t1(
   -> id int primary key auto\_increment
   -> )auto\_increment=2;（默认从2开始自增长）
   如果不按照顺序插入，则从最近一个整数开始自增长；select last\_insert\_id();可以获取最近插入的一个整数
8. 唯一键：
   一张表中有往往有很多字段需要唯一性，数据不能重复，但是一张表中只能有一个主键：唯一键就可以解决表中有多个字段需要唯一性约束的问题。唯一键的本质和主键差不多，**但是唯一键允许为空，而且可以多个为空，空字段不做唯一性比较。**
   关于唯一键和主键的区别：
   我们可以简单理解成，主键更多的是标识唯一性的。而唯一键更多的是保证在业务上，不要和别的信息出现重复。
   在建表时设置unique，或者通过alter table modify修改属性设置：alter table t1 modify name char(32) unique after id;
   create table users (
   id int primary key,
   username varchar(50) UNIQUE,  -- 用户名唯一
   email varchar(100) UNIQUE     -- 邮箱唯一
   );
9. 外键：
   外键用于定义**主表和从表**之间的关系：外键约束主要定义在从表上，主表则必须是有主键约束或unique约束。当定义外键后，要求外键列数据必须在主表的主键列存在或为null。
10. 外键约束是一种用于维护两个表之间关联关系的规则，它通过强制引用完整性，确保从表（包含外键的表）中的数据必须与主表（被引用的表）中的数据保持一致，避免出现无效或孤立的数据。
11. 外键约束的核心作用是保证数据的参照完整性，具体表现为：防止在从表中插入主表中不存在的记录；防止删除主表中被从表引用的记录；防止修改主表中被引用的主键值（或按规则同步修改从表的外键值）。
    mysql> create table if not exists class(
    -> id int primary key auto\_increment comment '班级',
    -> number int comment '人数'
    -> )charset=utf8 collate=utf8\_general\_ci engine=InnoDB;
    Query OK, 0 rows affected (0.02 sec)

    mysql> create table if not exists student(
    -> id int primary key auto\_increment,
    -> name char(20) comment '姓名',
    -> class\_id int,
    -> **foreign key (class\_id) references class(id) #关联时数据类型必须完全一致**
    -> )charset=utf8 collate=utf8\_general\_ci engine=InnoDB;
    Query OK, 0 rows affected (0.02 sec)

##### 表的增删查改：

1. Create：
   INSERT INTO students (id, sn, name) VALUES
   (102, 20001, '曹孟德'),
   (103, 20002, '孙仲谋');
   可以省略values左侧的列属性，那么就是全列插入，也可以指定要插入的数据，指定列插入，也可以一次插入多条数据；
2. 插入否则更新：
   insert into class values(1,32) on duplicate key update id=1,number=1024;这代表如果插入的数据有冲突，那么就更改为id=1,number=1024
   -- 0 row affected: 表中有冲突数据，但冲突数据的值和 update 的值相等
   -- 1 row affected: 表中没有冲突数据，数据被插入
   -- 2 row affected: 表中有冲突数据，并且数据已经被更新
3. 替换：
   -- 主键 或者 唯一键 没有冲突，则直接插入；
   -- 主键 或者 唯一键 如果冲突，则删除后再插入
   replace into class(number) values (999);
   -- 1 row affected: 表中没有冲突数据，数据被插入
   -- 2 row affected: 表中有冲突数据，删除后重新插入
4. 查询：
   全列查询：select \* from 表名;通常不建议遍历全列查询，不然会消耗大量网络资源
   指定列查询：select 列属性 from class;
   查询字段为表达式：
   -- 表达式不包含字段
   SELECT id, name, 10 FROM exam\_result;
   -- 表达式包含一个字段
   SELECT id, name, english + 10 FROM exam\_result;
   -- 表达式包含多个字段
   SELECT id, name, chinese + math + english FROM exam\_result；
   为查询结果指定别名：
   SELECT id, name, chinese + math + english 总分 FROM exam\_result;（可以as别名，也可不带as）
   结果去重：
   select distinct 列属性 from table;
5.  | 运算符               | 说明                                                                |
   | -------------------- | ------------------------------------------------------------------- |
   | `>`, `>=`, `<`, `<=` | 大于，大于等于，小于，小于等于                                      |
   | `=`                  | 等于，NULL不安全，例如`NULL = NULL` 的结果是 `NULL`                 |
   | `<=>`                | 等于，NULL安全，例如`NULL <=> NULL` 的结果是 `TRUE(1)`              |
   | `!=`, `<>`           | 不等于                                                              |
   | `BETWEEN a0 AND a1`  | 范围匹配，`[a0, a1]`，如果 `a0 <= value <= a1`，返回 `TRUE(1)`      |
   | `IN (option, ...)`   | 如果是`option` 中的任意一个，返回 `TRUE(1)`                         |
   | `IS NULL`            | 是`NULL`                                                            |
   | `IS NOT NULL`        | 不是`NULL`                                                          |
   | `LIKE`               | 模糊匹配。`%` 表示任意多个（包括0个）任意字符；`_` 表示任意一个字符 |

-- 基本比较：select math,chinese from exam\_result where english>66;

-- 使用 AND 进行条件连接：select math,chinese from exam\_result where english>66 and english<90;

-- 使用 BETWEEN ... AND ... 条件：（左闭右闭） select math,chinese from exam\_result where english>66 and english<90;

-- 使用 OR 进行条件连接：select math,chinese from exam\_result where math>60 or english<90;

-- 使用 IN 条件：select name, math from exam\_result where math in (58, 59, 98, 99);

-- % 匹配任意多个（包括 0 个）任意字符 ： select name from exam\_result where name like '孙%';

-- \_ 匹配严格的一个任意字符： select name from exam\_result where name like '孙\_';

-- WHERE 条件中比较运算符两侧都是字段：select name from exam\_result where english>math;

-- 别名不能用在 WHERE 条件中：select name, chinese + math + english 总分 from exam\_result WHERE chinese + math + english < 200;（这与sql语句的执行顺序密切相关）

如果条件很多又怕搞错，可以加上（）。

--专门查询是否为NULL：SELECT \* FROM test WHERE age <=> NULL; 或者使用 is NULL /is not NULL
6. 结果排序：
语法：
-- ASC 为升序（从小到大）
-- DESC 为降序（从大到小）
-- 默认为 ASC
SELECT ... FROM table\_name \[WHERE ...]
ORDER BY column \[ASC|DESC], \[...];
eg. select name from exam\_result order by math desc;  select name from exam\_result order by math asc;
查询同学各门成绩，依次按 数学降序，英语升序，语文升序的方式显示：SELECT name, math, english, chinese FROM exam\_result ORDER BY math DESC, english, chinese;
排序中可以使用别名：SELECT name, chinese + english + math 总分 FROM exam\_result ORDER BY 总分 Desc;
7. 筛选分页结果：
select \* from exam\_result limit 1,4;代表从第一个开始查询四个，也支持select \* from exam\_result limit 4 offset 1;这样使用offset进行筛选
建议：对未知表进行查询时，最好加一条 LIMIT 1，避免因为表中数据过大，查询全表数据导致数据库卡死
8. Update：
语法：
UPDATE table\_name SET column = expr \[, column = expr ...] \[WHERE ...] \[ORDER BY ...] \[LIMIT ...]
-- 更新值为具体值：update exam\_result set math=80 where name = '孙悟空';
-- 一次更新多个列：update exam\_result set math=60,chinese=70 where name='曹孟德';
-- 更新值为原值基础上变更：update exam\_result set math=math+30 order by chinese+math+english limit 3;
-- 没有 WHERE 子句，则更新全表：
9. Delete：
语法：DELETE FROM table\_name \[WHERE ...] \[ORDER BY ...] \[LIMIT ...]
-- 删除数据： delete from exam\_result where name='哈哈';
-- 删除整表数据： delete from testnull;（注意这里auto\_increment不会被删除，插入后保持自增）
10. 截断表：
语法：TRUNCATE \[TABLE] table\_name
truncate testnull;
注意：1. 只能对整表操作，不能像 DELETE 一样针对部分数据操作；2. 实际上 MySQL 不对数据操作，所以比 DELETE 更快，但是TRUNCATE在删除数据的时候，并不经过真正的事物，所以无法回滚 3. 会重置 AUTO\_INCREMENT 项
11. 插入查询结果：
语法：INSERT INTO table\_name \[(column \[, column ...])] SELECT ...
案例：删除表中的的重复记录，重复的数据只能有一份（创建新表将原表无重复数据插入，重命名）
create table if not exists copy like exam\_result;
insert into copy select distinct \* from exam\_result;
alter table copy rename to exam\_result;
12. 聚合函数：
函数说明
COUNT(\[DISTINCT] expr) 返回查询到的数据的数量
SUM(\[DISTINCT] expr) 返回查询到的数据的总和，不是数字没有意义
AVG(\[DISTINCT] expr) 返回查询到的数据的平均值，不是数字没有意义
MAX(\[DISTINCT] expr) 返回查询到的数据的最大值，不是数字没有意义
MIN(\[DISTINCT] expr) 返回查询到的数据的最小值，不是数字没有意义
--统计多少条数据： select count(\*) from exam\_result;/ select count(1) from exam\_result;
--统计指定数据的个数：select count(math) from exam\_result;
--统计指定去重数据的个数： select count(distinct math) from exam\_result;
--统计总分： select sum(math) from exam\_result;/ select avg(math) 平均总分 from exam\_result;
--统计平均分：select sum(math)/count(1) from exam\_result;
--条件统计：select sum(math)/count(math) from exam\_result where math>60;
--查找最高最低：select max(math) from exam\_result;/  select min(math) from exam\_result;
13. group by子句的使用（在scott\_data中）
在select中使用group by 子句可以对指定列进行分组查询：select column1, column2, .. from table group by column;（这里只有在group by后面出现的内容才可以在select后出现，除此之外就只有聚合函数能出现）
--显示每个部门的平均工资和最高工资：select deptno,max(sal) 最高工资,avg(sal) 平均工资 from emp group by deptno;
--显示每个部门的每种岗位的平均工资和最低工资：select deptno,job,min(sal) 最低工资,avg(sal) 平均工资 from emp group by deptno,job;
--显示平均工资低于2000的部门和它的平均工资：select deptno,avg(sal) 平均工资 from emp group by deptno having 平均工资<2000;（having和group by配合使用，对group by结果进行过滤）
--where与group by搭配使用：select deptno,avg(sal) 平均工资 from emp where empno!=7566 group by deptno having 平均工资<2000;
14. SQL查询中各个关键字的执行先后顺序 from > on> join > where > group by > with > having > select > distinct > order by > limit（初步认定select中的as命名在having之前where之后）

##### 内置函数：

1. 日期类：
   - current\_date()|当前日期
   - current\_time()|当前时间
   - current\_timestamp()|当前时间戳
   - date(datetime)|返回datetime参数的日期部分
   - date\_add(date, interval d\_value\_type)|在date中添加日期或时间 interval后的数值单位可以是：year minute second day
   - date\_sub(date, interval d\_value\_type)|在date中减去日期或时间 interval后的数值单位可以是：year minute second day
   - datediff(date1, date2)|两个日期的差，单位是天，date1减去date2
   - now()|当前日期时间
2. 字符串类：
   - charset(str) 返回字符串字符集
   - concat(string2 \[,...]) 连接字符串
   - instr(string, substring) 返回substring在string中出现的位置，没有返回0（返回第一次出现的位置）
   - ucase(string2) 转换成大写
   - lcase(string2) 转换成小写
   - left(string2, length) 从string2中的左边起取length个字符
   - right (string2, length) 从 string2 中的右边起取 length 个字符
   - length(string) string的长度
   - replace(str, search\_str, replace\_str) 在str中用replace\_str替换search\_str（将所有的search\_str全部替换）
   - strcmp(string1, string2) 逐字符比较两字符串大小
   - substring(str, position \[,length]) 从str的position开始，取length个字符
   - ltrim(string)、rtrim(string)、trim(string) 去除前空格或后空格
     获取emp表的ename列的字符集：select charset(ename) from emp;
     要求显示exam\_result表中的信息，显示格式：“XXX的语文是XXX分，数学XXX分，英语XXX分”： select concat(name,'的语文是',chinese,'分,数学',math,'分,英语',english,'分') as 总分 from exam\_result;
     求学生表中学生姓名占用的字节数：select length(name) from exam\_result;
     将表中所有名字中有孙的部分替换成'haha'：select replace(name,'孙','haha') from exam\_result;
     截取表中name字段的第二个到第三个字符：select substring(name,2,2) from exam\_result;
3. 数学函数：
   - abs(number) 绝对值函数
   - bin(decimal\_number) 十进制转换二进制
   - hex(decimalNumber) 转换成十六进制
   - conv(number,from\_base,to\_base) 进制转换
   - ceiling(number) 向上去整（3.1->4 -3.1->-3）
   - floor(number) 向下去整（3.1->3 -3.1->-4）
   - format(number, decimal\_places) 格式化，保留小数位数
   - hex(decimalNumber) 转换成十六进制
   - rand() 返回随机浮点数，范围\[0.0,1.0)
   - mod(number, denominator) 取模，求余
4. 其它函数
   - user() 查询当前用户
   - md5(str)对一个字符串进行md5摘要，摘要后得到一个32位字符串
   - database()显示当前正在使用的数据库
   - password()函数，MySQL数据库使用该函数对用户加密
   - ifnull（val1， val2） 如果val1为null，返回val2，否则返回val1的值
5. 复合查询（重点）：
   多表查询：不加任何关联条件的多表查询，会返回两个表所有行的组合（即「笛卡尔积」），结果行数 = 表 1 行数 × 表 2 行数。
   显示雇员名、雇员工资以及所在部门的名字：select emp.ename,emp.sal,dept.dname from emp,dept where emp.deptno=dept.deptno;
   显示部门号为10的部门名，员工名和工资：select emp.deptno,ename,sal from emp,dept where emp.deptno=dept.deptno and emp.deptno=10;
6. 自连接：自连接是指在同一张表连接查询
   使用多表查询（自查询）：select leader.empno,leader.ename from emp leader, emp worker where leader.empno = worker.mgr and worker.ename='FORD';（对表可以进行重命名，并且由于在from阶段进行，后面都可以使用重命名）
7. 子查询：子查询是指嵌入在其他sql语句中的select语句，也叫嵌套查询。
8. in关键字；查询和10号部门的工作岗位相同的雇员的名字，岗位，工资，部门号，但是不包含10自己的：select ename,job,sal,deptno from emp where job in (select distinct job from emp where deptno=10) and deptno!=10;（**not in的使用**：select emp\_no from employees where emp\_no not in (select emp\_no from dept\_manager);这样可以查询这张表中存在而另一张不存在的情况）
   还可以将查询信息作为新表： select \* from (select ename,job,sal,deptno from emp where job in (select distinct job from emp where deptno=10) and deptno!=10) as tmp,dept where tmp.deptno=dept.deptno;
9. all关键字；显示工资比部门30的所有员工的工资高的员工的姓名、工资和部门号：select ename, sal, deptno from EMP where sal > all(select sal from EMP where deptno=30);或者select \* from emp where sal>(select max(sal) from emp where deptno=30);
10. any关键字；显示工资比部门30的任意员工的工资高的员工的姓名、工资和部门号（包含自己部门的员工）：select \* from emp where sal>any(select sal from emp where deptno=30);（任意不是指比最大的那一个高）
11. 多列子查询：
    单行子查询是指子查询只返回单列，单行数据；多行子查询是指返回单列多行数据，都是针对单列而言的，而多列子查询则是指查询返回多个列数据的子查询语句：
    案例：查询和SMITH的部门和岗位完全相同的所有雇员，不含SMITH本人：select ename from emp where (deptno,job) in (select deptno,job from emp where ename='SMITH') and ename!='SMITH';
12. 在from子句中使用子查询：
    - 显示每个高于自己部门平均工资的员工的姓名、部门、工资、平均工资：select \* from emp,(select deptno,avg(sal) asal from emp group by deptno) tmp where emp.deptno=tmp.deptno and emp.sal>tmp.asal;
    - 查找每个部门工资最高的人的姓名、工资、部门、最高工资： select ename,sal,deptno,msal from emp,(select deptno dt,max(sal) msal from emp group by deptno) tmp where emp.sal=tmp.msal;
    - 显示每个部门的信息（部门名，编号，地址）和人员数量：select emp.deptno,dept.loc,count(1) from emp,dept where emp.deptno=dept.deptno group by deptno;也可以是： select deptno,loc,num from dept,(select deptno dt,count(1) num from emp group by deptno) tmp where dept.deptno=tmp.dt;
    - 合并查询：在实际应用中，为了合并多个select的执行结果，可以使用集合操作符 union，union all
      - union：该操作符用于取得两个结果集的并集。当使用该操作符时，会自动去掉结果集中的重复行。
      - union all：该操作符用于取得两个结果集的并集。当使用该操作符时，不会去掉结果集中的重复行。
13. 表的内连和外连：
14. 内连接：语法：select 字段 from 表1 inner join 表2 on 连接条件 and 其他条件；
15. select \* from emp,dept where emp.deptno=dept.deptno;这是前面的笛卡尔积的写法实际上就等于：select \* from emp inner join dept where emp.deptno=dept.deptno;
16. 外连接：左外/右外连接： select \* from stu **left join** exam on stu.id=exam.id;//实际等同于 select \* from exam **right join** stu on stu.id=exam.id;左侧的表信息完全显示是左外连接，如果另一张表中没有匹配的信息显示NULL，同理右侧的表信息完全显示是右外连接，如果另一张表中没有匹配的信息显示NULL

##### 索引：

1. MySQL 与磁盘交互基本单位：为了提高基本的IO效率， MySQL 进行IO的基本单位是16KB (后面统一使用InnoDB 存储引擎讲解)，磁盘这个硬件设备的基本单位是512 字节，而MySQL InnoDB引擎使用16KB 进行IO交互。即， MySQL 和磁盘进行数据交互的基本单位是**16KB** 。这个基本数据单元在MySQL 这里叫做page（注意和系统的page区分）
2. 建立共识：
   MySQL 中的数据文件，是以page为单位保存在磁盘当中的。
   MySQL 的CURD 操作，都需要通过计算，找到对应的插入位置，或者找到对应要修改或者查询的数据。而只要涉及计算，就需要CPU参与，而为了便于CPU参与，一定要能够先将数据移动到内存当中。
   所以在特定时间内，数据一定是磁盘中有，内存中也有。后续操作完内存数据之后，以特定的刷新策略，刷新到磁盘。而这时，就涉及到磁盘和内存的数据交互，也就是IO了。而此时IO的基本单位就是Page。（说白了根据局部性原理，page也是为了提高IO效率）
   为了更好的进行上面的操作， MySQL 服务器在内存中运行的时候，在服务器内部，就申请了被称
   为 **Buffer Pool** 的的大内存空间，来进行各种缓存。其实就是很大的内存空间，来和磁盘数据进
   行IO交互。为何更高的效率，一定要尽可能的减少系统和磁盘IO的次数
3. 向一个具有主键的表中，乱序插入数据，发现数据会自动排序：如何理解mysql中page的概念？
   mysql内部，一定需要并且会存在大量的page，也就决定了，mysql必须要将多个同时存在的page管理起来！要管理所有的mysql内的page，需要 先描述，再组织！所以，不要简单的将page认为是一个内存块，page内部也必须写入对应的管理信息

```c
* struct page
  {
  struct page \*next;
  string page \*prev;
  char buffer\[NUM];
* }; --- 16KB ， new page, 将所有的page用“链表”的形式管理起来 --- 在 **buffer pool** 内部，对mysql中的page进行了一个建模！
```

4. 索引的理解：在表中添加主键时，默认生成主键索引，不同的Page ，在MySQL 中，都是16KB ，使用prev 和next 构成双向链表，而且每个Page内部的数据也是基于链表的，因为有主键的问题， MySQL 会默认按照主键给我们的数据进行排序，排序的目的是为了优化查找效率，为了更加高效的查找，page中存在页目录：即只保存部分表中数据，形成目录，查找目录可以一次性跳过多个不需要的数据，加快了查找效率，但是如果一个表很大，需要多个page存储，page之间通过链表连接效率还是太低，为此为page增加一个索引（原来的链表结构不变，方便线性范围查找），给Page也带上目录，使用一个目录项来指向某一页，而这个目录项存放的就是将要指向的页中存放的最小数据的键值（只存储目录不存储数据，所以可以管理非常多的叶子page），和页内目录不同的地方在于，这种目录管理的级别是页，而页内目录管理的级别是行，其中，每个目录项的构成是：键值+指针，这样在查找时只需要根据目录去指定的page往下继续查找即可，不用加载所有的page也不用线性遍历，效率拉满（Page分为目录页和数据页。目录页只放各个下级Page的最小键值，查找的时候，自定向下找，只需要加载部分目录页到内存，即可完成算法的整个查找过程，大大减少了IO次数），**这所有的前提都是以主键作为索引的，如果没有指定主键，InnoDB会采用隐藏主键，事务部分继续**
5. 为何选择B+：与B相比较节点不存储data，这样一个节点就可以存储更多的key。可以使得树更矮，所以IO操作次数更少，叶子节点相连，更便于进行范围查找
6. 其中， InnoDB 这种用户数据与索引数据在一起索引方案，叫做聚簇索引，MyISAM叫做非聚簇索引，当然， MySQL 除了默认会建立主键索引外，也有可能建立按照其他列信息建立辅助（普通）索引，对于MyISAM ,建立辅助（普通）索引和主键索引没有差别，无非就是主键不能重复，而非主键可重复，叶子结点中存的是地址，但是InnoDB辅助索引的叶子节点中存的是对于的主键值。
7. 索引操作：创建主键索引（前面讲过）唯一索引的创建（前面也讲过），指定主键时就会创建主键索引，指定唯一键时就会创建唯一索引
8. 普通索引的创建

   - 第一种方式：index(name) --在表的定义最后，指定某列为索引
   - 第二种方式：alter table user9 add index(name); --创建完表以后指定某列为普通索引
   - 第三种方式：create index idx\_name on user10(name); -- 创建一个索引名为 idx\_name 的索引
     普通索引的特点：如果某列需要创建索引，但是该列有重复的值，应该使用普通索引
9. 查看索引：show keys from 表名 / show index from 表名;
10. 删除索引：
    第一种方法-删除主键索引： alter table 表名 drop primary key;
    第二种方法-其他索引的删除： alter table 表名 drop index 索引名； 索引名是Key\_name 字段
11. 拓展：
12. 复合索引：由多个列联合组成的索引（如 (col1, col2, col3)），本质是将多个列的取值组合作为索引键。索引的排序规则是 “先按第一列排序，第一列相同再按第二列排序，以此类推”（类似字典的排序逻辑）。叶子节点存储的是 “索引键（多列组合）+ 指向数据行的指针（非聚簇索引）” 或 “数据行本身（聚簇索引）”。
    示例：对表 user 建立复合索引 (age, name)，则索引会先按 age 升序排列，相同 age 的记录再按 name 升序排列。当查询 WHERE age = 20 AND name = '张三' 时，可直接通过该索引快速定位。
13. 索引最左匹配原则：复合索引只有查询条件包含复合索引的 “最左前缀” 时，索引才会被部分或全部使用。“最左前缀” 指从索引的第一列开始，连续的若干列（如索引 (a,b,c) 的最左前缀可以是 a、a+b、a+b+c）。
    例：索引 (a,b,c)，查询 WHERE a = 1 → 用到 a 列；查询 WHERE a = 1 AND b = 2 → 用到 a+b 列；查询 WHERE a = 1 AND b = 2 AND c = 3 → 用到全部三列。
    若跳过最左列，索引完全不生效。
    例：查询 WHERE b = 2 或 WHERE b = 2 AND c = 3 → 无法使用 (a,b,c) 索引（因为索引先按 a 排序，没有 a 的条件，无法定位 b 和 c 的位置）。
    若最左列使用范围查询（如 a > 1），则范围查询右侧的列无法使用索引。
    例：查询 WHERE a > 1 AND b = 2 → 仅 a 列生效（a 范围查询后，b 的排序在 a 内部，无法继续通过 b 过滤）。
    本质原因：复合索引的排序是 “先第一列、再第二列……”，索引结构本身依赖最左列的顺序，跳过最左列会导致索引的有序性被破坏，无法高效定位。
14. 索引覆盖：若查询所需的所有列（包括 SELECT 后的列和 WHERE 条件中的列）都包含在索引中，则无需回表查询数据行，直接从索引中获取结果，避免了回表操作，显著提升查询效率。
    示例：
    表 user 有列 id, age, name, address，建立复合索引 (age, name)。
    若查询 SELECT age, name FROM user WHERE age = 20：
    索引 (age, name) 已包含查询所需的 age（条件列）和 name（结果列），直接从索引获取数据，会返回所有 age=20 对应的 name，无需回表 → 触发索引覆盖。
    如何利用：设计复合索引时，可将查询中频繁用到的列（包括结果列）加入索引，形成 “覆盖索引”，尤其适合高频、简单的查询（如列表查询）。
15. 索引创建原则：比较频繁作为查询条件的字段应该创建索引；；唯一性太差的字段不适合单独创建索引，即使频繁作为查询条件；；更新非常频繁的字段不适合作创建索引；；不会出现在where子句中的字段不该创建索引
16. 全文索引的创建（拓中拓）
    当对文章字段或有大量文字的字段进行检索时，会使用到全文索引。MySQL提供全文索引机制，但要求表的存储引擎必须是MyISAM，而且默认的全文索引支持英文，不支持中文。
    创建全文索引：
    -- 对 title 和 content 列创建全文索引 FULLTEXT INDEX ft\_article (title, content)
    -- 索引名 ft\_article，包含 title 和 content
    使用全文索引：-- 搜索 title 或 content 中包含 "数据库" 或 "索引" 的记录
    SELECT \* FROM articles WHERE MATCH(title, content) AGAINST('数据库 索引');

##### 事务：

1. 什么是 MySQL 事务？
   事务（Transaction）是数据库中一组不可分割的操作集合，**这组操作要么全部成功执行，要么全部失败回滚**，最终保证数据从一个一致性状态切换到另一个一致性状态。
   事务的核心特性由 ACID 定义：

   - 原子性（Atomicity）：事务中的所有操作要么全做，要么全不做（例如转账时 “扣钱” 和 “加钱” 必须同时成功或同时失败）。
   - 一致性（Consistency）：事务执行前后，数据必须满足预设的业务规则（例如转账后总金额不变）。
   - 隔离性（Isolation）：多个事务并发执行时，彼此的操作应相互隔离，避免干扰（例如防止 “脏读”“不可重复读” 等问题）。
   - 持久性（Durability）：事务一旦提交，其结果会永久保存到数据库（即使宕机也不会丢失）。
2. \- 一致性的核心定义：“状态的合法跃迁”
   事务执行的结果，必须让数据库从一个合法的一致性状态，转变为另一个合法的一致性状态。
   举个例子：银行转账时，A 向 B 转 100 元。事务的预期是 “A 的余额减 100，B 的余额加 100，总资金不变”。如果事务成功提交，数据库就从 “转账前的合法状态” 变为 “转账后的合法状态”；如果事务中途中断（比如系统崩溃），A 的钱扣了但 B 没收到，数据库就处于 “资金不守恒” 的不一致状态。
   这里，原子性（Atomicity）是一致性的直接保障—— 原子性确保事务 “要么全做，要么全不做”，避免了 “做一半” 导致的不一致。

   * 一致性与 “业务逻辑强相关”
     一致性的 “合法性” 是**由用户的业务规则定义的，数据库本身无法凭空判断业务是否合理**。
     比如：业务规则要求 “用户年龄不能为负数”，MySQL 可以通过字段类型（如INT）或约束（CHECK age >= 0）提供技术支持，但 “为什么年龄不能为负” 是业务逻辑决定的，**数据库只是工具，最终由用户的业务需求来定义 “什么样的状态是一致的”**。
   * 技术上通过 AID 保障 C（原子性、隔离性、持久性共同支撑一致性）
     原子性（A）：避免事务 “做一半” 导致数据残缺，确保状态跃迁的 “完整性”。
     隔离性（I）：避免多事务并发时互相干扰（如脏读、幻读），确保每个事务看到的是 “合法的中间状态”。
     持久性（D）：确保事务提交后数据不会丢失，保证 “合法状态的长期有效性”。
3. 事务的版本支持：在 MySQL 中只有使用了 Innodb 数据库引擎的数据库或表才支持事务， MyISAM 不支持，show engines可以查看版本支持
4. 事务提交方式：事务的提交方式常见的有两种：自动提交/手动提交，查看事务提交方式：show variables like 'autocommit';用SET 来改变MySQL 的自动提交模式：
5. 查询事务隔离级别：select @@transaction\_isolation;
6. 事务的开始与回滚：开始前将mysql的默认隔离级别设置成读未提交，方便查看，启动事务：start transaction或者begin; （begin属于手动开启事务，与自动提交是否打开没有关系）设置一个保存点save1：savepoint save1;使用rollback to save1可以回到保存点，save1后续的操作还原，也可以使用rollback直接回到最开始，但是！如果已经进行了commit那么无法回滚；；如果mysql意外终止或退出，会自动回退
7. 结论：
   只要输入begin或者start transaction，事务便必须要通过commit提交，才会持久化，与是否设置set autocommit无关。事务可以手动回滚，同时，当操作异常，MySQL会自动回滚；对于InnoDB 每一条SQL 语言都默认封装成事务，自动提交。（select有特殊情况，因为MySQL 有MVCC ）从上面的例子，我们能看到事务本身的原子性(回滚)，持久性(commit)
8. 隔离级别：

   * 读未提交【Read Uncommitted】： 在该隔离级别，**所有的事务都可以看到其他事务没有提交的
     执行结果**。（实际生产中不可能使用这种隔离级别的），但是相当于没有任何隔离性，也会有很多
     并发问题，如脏读，幻读，不可重复读等，我们上面为了做实验方便，用的就是这个隔离性。（一个事务在执行中，读到另一个执行中事务的更新(或其他操作)但是未commit的数据，这种现象叫做脏读(dirty read)）
   * 读提交【Read Committed】 ：该隔离级别是大多数数据库的默认的隔离级别（不是 MySQL 默
     认的）。它满足了隔离的简单定义:**一个事务只能看到其他的已经提交的事务所做的改变**。这种隔离
     级别会引起不可重复读，即一个事务执行时，如果多次 select， 可能得到不同的结果。
   * 可重复读【Repeatable Read】： 这是 MySQL 默认的隔离级别，它确保同一个事务，在执行
     中，多次读取操作数据时，会看到同样的数据行。（两个事务都进行提交后才可以看到改变的结果）但是会有幻读问题。（幻读：其他事务对范围外的行执行了 INSERT 或 DELETE 并提交，不可重复读：其他事务对已有行执行了 UPDATE 并提交，本人认为差别不大，mysql解决了幻读问题）
   * 串行化【Serializable】: 这是事务的最高隔离级别，**它通过强制事务排序，使之不可能相互冲突**，
     从而解决了幻读的问题。它在每个读的数据行上面加上共享锁，。但是可能会导致超时和锁竞争
     （这种隔离级别太极端，实际生产基本不使用）
   * 隔离级别如何实现：隔离，基本都是通过锁实现的，不同的隔离级别，锁的使用是不同的。常见有，表锁，行锁，读锁，写锁，间隙锁(GAP),Next-Key锁(GAP+行锁)等。不过，我们目前现有这个认识就行，先关注上层使用。
9. 查看与设置隔离性：

   查全局隔离级别：

```sql
SELECT @@global.transaction\_isolation;  -- 全局
```

- 查当前会话隔离级别：

```sql
SELECT @@session.transaction\_isolation;  -- 或简写为 SELECT @@transaction\_isolation;设置隔离性：SET \[SESSION | GLOBAL] TRANSACTION ISOLATION LEVEL {READ UNCOMMITTED | READ COMMITTED | REPEATABLE READ | SERIALIZABLE
```

##### 隔离性：

1. 数据库并发的场景有三种：

   * 读-读：不存在任何问题，也不需要并发控制
   * 读-写：有线程安全问题，可能会造成事务隔离性问题，可能遇到脏读，幻读，不可重复读（重点）
   * 写-写：有线程安全问题，可能会存在更新丢失问题，比如第一类更新丢失，第二类更新丢失(后面补充)
2. 事务拥有ID决定先后顺序，并用结构体组织，数据结构管理，3个记录隐藏列字段：

   * DB\_TRX\_ID ：6 byte，最近修改( 修改/插入)事务ID，记录创建这条记录/最后一次修改该记录的事务ID
   * DB\_ROLL\_PTR : 7 byte，回滚指针，指向这条记录的上一个版本（简单理解成，指向历史版本就行，这些数据一般在undo log 中）
   * DB\_ROW\_ID : 6 byte，隐含的自增ID（隐藏主键），如果数据表没有主键， InnoDB 会自动以DB\_ROW\_ID 产生一个聚簇索引
   * 补充：实际还有一个删除flag隐藏字段, 既记录被更新或删除并不代表真的删除，而是删除flag变了
3. 问题 1：undo 储存在哪里？
   在 MySQL 的 InnoDB 存储引擎中，undo 日志存储在 “回滚段（Rollback Segment）” 中。InnoDB 会为每个数据库实例维护若干个回滚段，每个回滚段又包含多个 undo 日志段，用于管理和存储 undo 日志的内容。（初步理解为存储在mysql开辟的内存中）
4. 问题 2：undo 的作用是什么？
   事务回滚（保障原子性）：当事务执行失败（如触发约束、系统崩溃）时，数据库可通过 undo 日志反向执行操作，将数据恢复到事务开始前的状态。
   支持多版本并发控制（MVCC）：为事务提供数据的历史版本，让不同事务能在并发时读取到 “一致性快照”，避免锁竞争，提升并发性能。
5. 模拟MVCC：插入数据时**版本链**的形成：（undo log中的数据可以称为快照）
   在InnoDB的MVCC机制中，每行数据包含三个隐藏列：

   * DB\_TRX\_ID：记录最后一次修改该数据的事务ID；
   * DB\_ROLL\_PTR：回滚指针，指向该数据版本对应的undo log（旧版本数据）；
   * DB\_ROW\_ID：行唯一标识（若表无主键，InnoDB会自动生成，修改时不变）。

   场景说明：

   * 表t初始数据：(id=1, name='A')
   * 事务 T1（ID=100）：第一次修改该数据（修改后提交）
   * 事务 T2（ID=200）：第二次修改该数据（T1 已提交，T2 正常执行）

   阶段 1：初始状态（未被任何事务修改）

   此时数据为原始版本，未产生任何修改操作，因此：

   * **数据行**：id=1, name='A'
   * **隐藏列**：
     * DB\_TRX\_ID=0（初始状态，无事务修改过，默认用 0 表示）；
     * DB\_ROLL\_PTR=NULL（无旧版本数据，无需指向 undo log）；
     * DB\_ROW\_ID=1（InnoDB 自动生成的行唯一标识，后续修改不变）。
   * **undo log**：无（未发生修改，无需记录旧版本快照）。

   阶段 2：第一次修改（事务 T1，ID=100）

   T1 执行命令：UPDATE t SET name='B' WHERE id=1;

   InnoDB 的处理逻辑：

   1. 不直接覆盖原始数据，而是将原始版本（name='A'）标记为 “旧版本”，并拷贝到**update undo log**中（作为快照，用于回滚或 MVCC 读取）；
   2. 生成新的当前版本数据（name='B'），更新隐藏列以关联旧版本。

   * **数据行（当前版本）**：id=1, name='B'
   * **隐藏列（当前版本）**：

     * DB\_TRX\_ID=100（标记最后修改该数据的事务为 T1）；
     * DB\_ROLL\_PTR = 指向 T1 生成的 undo log（通过该指针可找到旧版本name='A'）；
     * DB\_ROW\_ID=1（保持不变）。
   * **undo log 变化**：

     T1 生成一条 update undo log，内容为：

     * 旧值：name='A'；
     * 上一个版本指针 = NULL（因为这是第一个旧版本，没有更早的快照）；
     * 事务 ID=100（标记该 undo log 由 T1 生成）。
   * **版本链形成**：

     当前版本（B） → T1 的 undo log（A） → NULL

   阶段 3：第二次修改（事务 T2，ID=200，T1 已提交）

   T2 执行命令：UPDATE t SET name='C' WHERE id=1;

   此时 T1 已提交，其修改的版本（name='B'）可被 T2 修改，InnoDB 处理逻辑：

   1. 将 T1 修改后的版本（name='B'）标记为 “旧版本”，拷贝到**新的 update undo log**中；
   2. 生成最新的当前版本数据（name='C'），更新隐藏列关联最新的旧版本。

   * **数据行（当前版本）**：id=1, name='C'
   * **隐藏列（当前版本）**：

     * DB\_TRX\_ID=200（标记最后修改该数据的事务为 T2）；
     * DB\_ROLL\_PTR = 指向 T2 生成的 undo log（通过该指针可找到旧版本name='B'）；
     * DB\_ROW\_ID=1（保持不变）。
   * **undo log 变化**：

     T2 生成一条新的 update undo log，内容为：

     * 旧值：name='B'；
     * 上一个版本指针 = 指向 T1 生成的 undo log（即通过该指针可找到更早的旧版本name='A'）；
     * 事务 ID=200（标记该 undo log 由 T2 生成）。
   * **版本链形成**：

     当前版本（C） → T2 的 undo log（B） → T1 的 undo log（A） → NULL
6. （如果当前只有一个事务修改后提交，undo log会被清空）如果有多个事务，某个事务执行插入并且commit了，那么undo log会被清空，但如果是update或者delete则不一定，上面是update的版本链，如果是delete，那么修改的是flag仍然有版本链，如果是insert，那么不存在版本链，但是undo log中会插入delete 这条数据供事务回滚时删除这条记录

   - 当前读：读取数据的 “最新版本”，并且会对读取的记录加锁（共享锁或排他锁），确保其他事务无法同时修改该记录，从而保证读取到的数据是 “当前时刻最新的、未被其他事务干扰的”，增删改都是当前读，select也可以当前读，
   - 快照读：读取数据的历史版本，通过数据库的多版本控制（MVCC，Multi-Version Concurrency Control）机制实现，不加锁，因此不会阻塞其他事务的读写操作，能显著提升并发效率。为什么可以读写并发？读写根据的是不同版本，隔离级别可以决定具体可以看到哪些版本，看到不同版本就体现出来隔离性。
     Read View：是事务进行快照读操作的时候生产的读视图(Read View)，在该事务执行的快照读的那一刻，会生成数据库系统当前的一个快照，记录并维护系统当前活跃事务的ID(当每个事务开启时，都会被分配一个ID, 这个ID是递增的，所以最新的事务，ID值越大),Read View 在MySQL 源码中,就是一个类，本质是用来进行可见性判断的。 即当我们某个事务执行快照读的时候，对该记录创建一个Read View 读视图，把它比作条件,用来判断当前事务能够看到哪个版本的数据，既可能是当前最新的数据，也有可能是该行记录的undo log 里面的某个版本的数据。

   * \- - m\_ids：是一个列表，用于维护Read View 生成时刻，系统中正在活跃的事务 ID。换句话说，就是在创建这个 Read View 时，所有还没提交的事务的 ID 都会被记录在这里。
   * \- - up\_limit\_id：记录m\_ids列表中事务 ID 最小的值。它用于界定 “活跃事务中最早启动的那个”，是判断数据版本可见性的边界之一。
   * \- - low\_limit\_id：表示Read View 生成时刻系统尚未分配的下一个事务 ID，也就是 “目前已出现过的事务 ID 的最大值 + 1”。可以理解为 “未来事务 ID 的起始值”，用于区分 “生成Read View 之后才启动的事务”。
   * \- - creator\_trx\_id：指创建该 Read View 的事务自身的 ID。用于判断当前事务修改的数据对自身的可见性（事务内的修改对自己是可见的）。
7. 以 “时间轴” 为核心，将事务分为三类：
   已经提交的事务：在 Read View 生成前就已提交的事务，其修改对当前事务可能可见。
   正在操作的事务（活跃事务）：Read View 生成时还在执行、未提交的事务，其修改对当前事务不可见。
   快照后新来的事务：Read View 生成后才启动的事务，其修改对当前事务不可见。
8. 可见性判断规则：
   通过数据版本的事务 ID（DB\_TRX\_ID） 与 Read View 字段对比，确定是否可见：

   * 已经提交的事务：
     若 creator\_trx\_id == DB\_TRX\_ID（自身修改），或 DB\_TRX\_ID < up\_limit\_id（快照前已提交的事务）则数据可见。
   * 正在操作的事务：
     所有活跃事务的 ID 都在m\_ids中，若DB\_TRX\_ID在m\_ids里，说明事务未提交，数据不可见。
     （思维误区：m\_ids里的事务 ID 不一定连续，比如示例中 11、13、15 号事务活跃，12、14 号已提交，以m\_ids是 {11,13,15}。只要DB\_TRX\_ID不在m\_ids中，就说明事务已提交，数据可见）
   * 快照后新来的事务：
     若 DB\_TRX\_ID >= low\_limit\_id，说明是快照后才启动的事务，数据不可见。
9. RR 与 RC 的本质区别：（select \* from user lock in share mode ,以加共享锁方式进行读取，对应的就是当前读）当前读和快照读在RR级别下的区别：
   结论：事务中快照读的结果是非常依赖该事务**首次出现快照读**的地方，即某个事务中首次出现快照读，决定该事务后续快照读结果的能力，delete同样如此。正是**Read View生成时机的不同**，从而造成RC,RR级别下快照读的结果的不同：在RR级别下的某个事务的对某条记录的**第一次快照读**会创建一个快照及Read View, 将当前系统活跃的其他事务记录起来，此后在调用快照读的时候，**还是使用的是同一个Read View**，所以只要当前事务在其他事务提交更新之前使用过快照读，那么之后的快照读使用的都是同一个Read View，所以对之后的修改不可见；即RR级别下，快照读生成Read View时，Read View会记录此时所有其他活动事务的快照，这些事务的修改对于当前事务都是不可见的。而早于Read View创建的事务所做的修改均是可见，而在RC级别下的，事务中，每次快照读都会**新生成一个快照和Read View**, 在RC隔离级别下，每个快照读都会生成并获取最新的Read View；而在RR隔离级别下，则是同一个事务中的第一个快照读才会创建Read View, 之后的快照读获取的都是同一个Read View，正是RC每次快照读，都会形成Read View，所以，RC才会有不可重复读问题。

##### 视图：

1. 视图的概念：视图(View)是基于一个或多个表的虚拟表，本质上是一个预定义的SQL查询。视图并不存储实际数据，而是存储查询逻辑，当查询视图时，数据库会动态执行定义视图的SQL语句，返回结果集。同真实的表一样，视图包含一系列带有名称的列和行数据。视图的数据变化会影响到基表，基表的数据变化也会影响到视图。
2. 基本使用：
   - 创建视图：create view 视图名 as select语句；修改了视图，对基表数据有影响，修改了基表，对视图有影响
   - 删除视图：drop view ;
3. 视图规则和限制：
   - 与表一样，必须唯一命名（不能出现同名视图或表名）
   - 创建视图数目无限制，但要考虑复杂查询创建为视图之后的性能影响视图不能添加索引，也不能有关联的触发器或者默认值
   - 视图可以提高安全性，必须具有足够的访问权限
   - order by 可以用在视图中，但是如果从该视图检索数据select中也含有order by,那么该视图中的order by将被覆盖yo图可以和表一起使用

##### MySQL用户管理：

1. 用户：MySQL中的用户，都存储在系统数据库mysql的user表,可以初步查看一下：

```sql
+-----------+------------------+------------------------------------------------------------------------+
| host      | user             | authentication_string                                                  |
+-----------+------------------+------------------------------------------------------------------------+
| localhost | debian-sys-maint | $A$005$LC(kHWSYr]OxNmtRIt5zFa6Yv5hQTllu4hxXLoM3wKCzIM/KSIbYiBQIL9 |
| localhost | mysql.infoschema | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED |
| localhost | mysql.session    | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED |
| localhost | mysql.sys        | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED |
| localhost | root             | *4AF64A40B8DF72D7902B0AE596600F448B09D81C                              |
+-----------+------------------+------------------------------------------------------------------------+
```

- 字段解释：
  - host： 表示这个用户可以从哪个主机登陆，如果是localhost，表示只能从本机登陆
  - user： 用户名
  - authentication_string： 用户密码通过password函数加密后的
  - *_priv： 用户拥有的权限

2.创建用户：

语法：create user '用户名'@'登陆主机/ip' identified by '密码';（登录主机/ip不能够省略，由于涉及到密码所以不能回滚）

案例：

```sql
mysql> create user 'drw'@'localhost' identified by 'dairenwen1092'; #这样创建的是本地用户，不能使用该用户远程登陆，关于新增用户这里，不要轻易添加一个可以从任意地方登陆的user。
mysql> create user 'drw'@'%' identified by 'dairenwen1092'; #这是为了方便从windows登录服务器的mysql，允许drw用户从任意地方登录，不建议将mysql暴露在公网中！
```

3.删除用户

语法：drop user '用户名'@'主机名';(主机名不允许省略)

案例：

```sql
drop user 'drw'@'127.0.0.1';
```

请注意，如果是windows远程登陆Linux MySQL需要更改配置文件：

在 Linux 系统（如 Ubuntu/Debian）中，MySQL 的核心配置文件之一是 `/etc/mysql/mysql.conf.d/mysql.cnf`

- 核心改动：允许远程 IP 连接

默认情况下，MySQL 仅监听本地回环地址（`127.0.0.1`），拒绝远程主机连接。需修改 `bind-address` 参数：

1. 打开配置文件（需 root 权限）：

   ```bash
   sudo vim /etc/mysql/mysql.conf.d/mysql.cnf
   ```
2. 找到并修改 `bind-address`：

   * 原配置（默认只允许本地连接）：

     ```ini
     [mysqld]
     bind-address = 127.0.0.1  # 仅监听本地 IP
     ```
   * 修改为允许所有 IP 连接（测试环境可用，生产环境不建议）：

     ```ini
     [mysqld]
     bind-address = 0.0.0.0  # 允许所有远程 IP 连接
     ```
   * 或限制为特定远程 IP（生产环境推荐，如只允许 `192.168.1.100` 连接）：

     ```ini
     [mysqld]
     bind-address = 192.168.1.100  # 仅允许指定 IP 连接
     ```

- 检查并禁用 `skip-networking`（若存在）

若配置文件中存在 `skip-networking` 参数，需注释或删除它 —— 该参数会完全禁用 MySQL 的网络连接功能（仅允许本地 socket 连接），导致远程登录失败。

若有如下配置：

```ini
skip-networking  # 禁用网络连接
```

需修改为：

```ini
# skip-networking  # 注释掉，启用网络连接
```

- 修改后生效步骤

1. **重启 MySQL 服务**，使配置生效：

   ```bash
   sudo systemctl restart mysql  # 或 mysqld，根据系统服务名调整
   ```
2. **验证监听状态**，确认 MySQL 已监听修改后的 IP：

   ```bash
   sudo netstat -tulpn | grep mysql
   ```

   若输出中包含 `0.0.0.0:3306` 或指定 IP，则说明配置生效。

- 额外必要操作（远程登录缺一不可）

修改配置文件后，还需确保：

1. **授权远程用户访问**：

   即使 MySQL 允许远程连接，若用户权限未开放，仍无法登录。需登录 MySQL 终端，为用户授予远程访问权限：

   ```sql
   -- 示例：允许用户 'username' 从任意 IP（%）登录，密码为 'password'
   GRANT ALL PRIVILEGES ON *.* TO 'username'@'%' IDENTIFIED BY 'password' WITH GRANT OPTION;
   FLUSH PRIVILEGES;  # 刷新权限
   ```

   （生产环境建议将 `%` 替换为具体远程 IP，如 `192.168.1.0/24`，限制访问范围）
2. **开放防火墙端口**：

   确保服务器防火墙允许 3306 端口（MySQL 默认端口）的入站连接：

   ```bash
   # UFW 防火墙示例（Ubuntu/Debian）
   sudo ufw allow 3306/tcp
   sudo ufw reload

   # firewalld 防火墙示例（CentOS/RHEL）
   sudo firewall-cmd --add-port=3306/tcp --permanent
   sudo firewall-cmd --reload
   ```

4. 修改账户密码：

自己改自己密码：set password=password('新的密码');

root用户修改指定用户的密码：set password for '用户名'@'主机名'=password('新的密码')；

5. 数据库的权限:


| 权限                    | 列                       | 上下文                 |
| ----------------------- | ------------------------ | ---------------------- |
| CREATE                  | Create\_priv             | 数据库、表或索引       |
| DROP                    | Drop\_priv               | 数据库或表             |
| GRANT OPTION            | Grant\_priv              | 数据库、表或保存的程序 |
| REFERENCES              | References\_priv         | 数据库或表             |
| ALTER                   | Alter\_priv              | 表                     |
| DELETE                  | Delete\_priv             | 表                     |
| INDEX                   | Index\_priv              | 表                     |
| INSERT                  | Insert\_priv             | 表                     |
| SELECT                  | Select\_priv             | 表                     |
| UPDATE                  | Update\_priv             | 表                     |
| CREATE VIEW             | Create\_view\_priv       | 视图                   |
| SHOW VIEW               | Show\_view\_priv         | 视图                   |
| ALTER ROUTINE           | Alter\_routine\_priv     | 保存的程序             |
| CREATE ROUTINE          | Create\_routine\_priv    | 保存的程序             |
| EXECUTE                 | Execute\_priv            | 保存的程序             |
| FILE                    | File\_priv               | 服务器主机上的文件访问 |
| CREATE TEMPORARY TABLES | Create\_tmp\_table\_priv | 服务器管理             |
| LOCK TABLES             | Lock\_tables\_priv       | 服务器管理             |
| CREATE USER             | Create\_user\_priv       | 服务器管理             |
| PROCESS                 | Process\_priv            | 服务器管理             |
| RELOAD                  | Reload\_priv             | 服务器管理             |
| REPLICATION CLIENT      | Repl\_client\_priv       | 服务器管理             |
| REPLICATION SLAVE       | Repl\_slave\_priv        | 服务器管理             |
| SHOW DATABASES          | Show\_db\_priv           | 服务器管理             |
| SHUTDOWN                | Shutdown\_priv           | 服务器管理             |
| SUPER                   | Super\_priv              | 服务器管理             |


**给用户授权**:

刚创建的用户默认无任何权限，需通过授权操作赋予权限。

**1. 授权语法**

```sql
grant 权限列表 on 库.对象名 to '用户名'@'登陆位置' [identified by '密码'];
```

**2. 语法说明**

* **权限列表**：可指定单个或多个权限（用逗号分隔），如`select`、`select, delete`；若要赋予所有权限，用`all [privileges]`。
* **库。对象名**：
  * `*.*`：代表系统中所有数据库的所有对象（表、视图、存储过程等）；
  * `库.*`：代表某个数据库中的所有对象（如`test.*`表示`test`库的所有表）。
* **' 用户名 '@' 登陆位置'**：指定被授权的用户及允许登录的位置（如`'whb'@'localhost'`表示用户`whb`仅允许本地登录）。
* [identified by ' 密码 ']（可选）：
  * 若用户已存在，可同时修改其密码；
  * 若用户不存在，则直接创建该用户并设置密码。

**3. 案例**

* **场景**：root 用户给`whb`用户授权`test`数据库所有对象的`select`权限。

  ```sql
  -- 终端A（root身份）：给whb授权
  mysql> grant select on test.* to 'whb'@'localhost';
  Query OK, 0 rows affected (0.01 sec)
  ```

**查看用户现有权限**

通过`show grants for '用户名'@'登陆位置'`命令查看：

```sql
-- 查看whb的权限
mysql> show grants for 'whb'@'%';
+-----------------------------------------------+
| Grants for whb@%                              |
+-----------------------------------------------+
| GRANT USAGE ON *.* TO 'whb'@'%'               |
| GRANT ALL PRIVILEGES ON `test`.* TO 'whb'@'%' |
+-----------------------------------------------+

-- 查看root的权限（默认有所有权限）
mysql> show grants for 'root'@'%';
+-------------------------------------------------------------+
| Grants for root@%                                           |
+-------------------------------------------------------------+
| GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' WITH GRANT OPTION |
+-------------------------------------------------------------+
```



**回收权限**:

当用户权限不再需要时，可通过`revoke`命令回收。

**1. 回收语法**

```sql
revoke 权限列表 on 库.对象名 from '用户名'@'登陆位置';
```

**2. 案例**

* 场景：root 用户回收`whb`对`test`数据库的所有权限。

  ```sql
  -- 终端A（root身份）：回收权限
  mysql> revoke all on test.* from 'whb'@'localhost';
  Query OK, 0 rows affected (0.00 sec)
  ```

  * **回收后验证（终端 B，whb 身份）**：

    ```sql
    -- 回收后：无法再看到test库
    mysql> show databases;
    +--------------------+
    | Database           |
    +--------------------+
    | information_schema |
    +--------------------+
    ```

**如果没有生效，需要flush privileges来刷新一下**！</mark>
