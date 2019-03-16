set feedback off
set heading off
set trimspool on
set newpage none
set linesize 200
set pagesize 0

spool &1

select 
	'#define OCI_SFC_' || RPAD(regexp_replace(name, ' /-', '_'), 40, ' ') || '         ' || action
from
	audit_actions 
order by 
	action;


select 
	CHR(10) || '#define OCI_SQLCMD_COUNT                '  ||  count(*) || CHR(10)
from
	audit_actions;

spool off

spool &2


select 'const OCI_SQLCmdInfo SQLCmds[OCI_SQLCMD_COUNT] ='  from dual;
select '{' from dual;

select 
    '    {OCI_SFC_' || RPAD(REPLACE(name, ' /-', '_'), 40, ' ') || ', TEXT("' || RPAD(name || '")', 40, ' ') ||  ' },'
from
	audit_actions 
order by 
	action;

select '};' from dual;

spool off

exit;
/

