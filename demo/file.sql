-- Initialization

-- replace 'TEMP_FOLDER' value with a local path on your server
create table files (id int, item bfile);
create directory TEMP_FOLDER as 'c:\temp';

-- Cleanup

drop table files;
drop directory TEMP_FOLDER;

