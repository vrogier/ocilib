-- Initialization

create table large_objects (code int, obj clob);
insert into large_objects values (1, '123456789');
insert into large_objects values (2, '987654321');
commit;

-- Cleanup

drop table large_objects;
