-- =============================================================================
-- OCILIB tutorials — capstone "empdir" extra schema objects
--
-- Run AFTER tutorial-schema.sql, as the same user the samples connect with:
--
--     sqlplus scott/tiger@//localhost:1521/XEPDB1 @empdir-schema.sql
--
-- Provides:
--   * employees.profile  — JSON column used by `empdir profile`
--   * job_t + job_queue  — AQ type/queue used by `empdir jobs submit|work`
-- =============================================================================

alter table employees add (profile json);

create or replace type job_t as object
(
    title   varchar2(64),
    content varchar2(200)
);
/

begin
    dbms_aqadm.create_queue_table('job_queue_table', 'job_t');
    dbms_aqadm.create_queue('job_queue', 'job_queue_table');
    dbms_aqadm.start_queue('job_queue');
end;
/

-- =============================================================================
-- Seed data
-- =============================================================================

-- JSON profiles on a few employees (others stay NULL so `empdir profile`
-- shows both paths)

update employees set profile = json('{
    "email":   "smith@empdir.local",
    "phone":   "+1-555-0101",
    "skills":  ["SQL", "PL/SQL"],
    "bio":     "Junior clerk, learning the ropes."
}') where empno = 7369;

update employees set profile = json('{
    "email":   "ford@empdir.local",
    "phone":   "+1-555-0108",
    "skills":  ["Analytics", "C++", "OCILIB"],
    "bio":     "Analyst and driver developer."
}') where empno = 7902;

update employees set profile = json('{
    "email":   "king@empdir.local",
    "skills":  ["Management"],
    "bio":     "President."
}') where empno = 7839;

commit;

-- Two queued jobs so `empdir jobs work` has something to pull

declare
    v_enq_opts dbms_aq.enqueue_options_t;
    v_msg_prop dbms_aq.message_properties_t;
    v_msgid    raw(16);
begin
    dbms_aq.enqueue('job_queue', v_enq_opts, v_msg_prop,
                    job_t('REVIEW PR',  'Review pull request #42'), v_msgid);
    dbms_aq.enqueue('job_queue', v_enq_opts, v_msg_prop,
                    job_t('RUN REPORT', 'Nightly employees report'), v_msgid);
end;
/

commit;

-- -- Cleanup ------------------------------------------------------------------
-- begin
--     dbms_aqadm.stop_queue('job_queue');
--     dbms_aqadm.drop_queue('job_queue');
--     dbms_aqadm.drop_queue_table('job_queue_table');
-- end;
-- /
-- drop type job_t;
-- alter table employees drop column profile;
