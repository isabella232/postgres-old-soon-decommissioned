drop table 计算机术语;
create table 计算机术语(术语 text, 分类号 varchar, 备注1A char(16));
create index 计算机术语index1 on 计算机术语 using btree(术语);
create index 计算机术语index2 on 计算机术语 using btree(分类号);
insert into 计算机术语 values('电脑显示屏','机A01上');
insert into 计算机术语 values('电脑图形','分B01中');
insert into 计算机术语 values('电脑程序员','人Z01下');
vacuum 计算机术语;
select * from 计算机术语;
select * from 计算机术语 where 分类号 = '人Z01下';
select * from 计算机术语 where 分类号 ~* '人z01下';
select * from 计算机术语 where 分类号 like '_Z01_';
select * from 计算机术语 where 分类号 like '_Z%';
select * from 计算机术语 where 术语 ~ '电脑[显图]';
select * from 计算机术语 where 术语 ~* '电脑[显图]';
