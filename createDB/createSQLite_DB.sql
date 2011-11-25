
------------------------------------------------------------------------- Создание Sequence 

--Примечание: в SQLite нет Sequence. нужно создавать таблицу и запихивать их туда.
CREATE TABLE Sequence (
	name text,
	curr_val integer
);

INSERT INTO Sequence VALUES ('seq_addr_id', 1);
/*CREATE Sequence seq_addr_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('seq_attachment_id', 1);
/*CREATE Sequence seq_attachment_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('jobs_queue_job_id_seq', 1);
/*CREATE Sequence jobs_queue_job_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('seq_mail_id', 1);
/*CREATE Sequence seq_mail_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('seq_thread_id', 1);
/*CREATE Sequence seq_thread_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('seq_word_id', 1);
/*CREATE Sequence seq_word_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/
INSERT INTO Sequence VALUES ('seq_tag_id', 1);
/*CREATE Sequence seq_tag_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;
*/

------------------------------------------------------------------------- Закончили с Sequence 










------------------------------------------------------------------------- Создание ТАБЛИЦ!!! 

CREATE TABLE addresses (
    addr_id integer PRIMARY KEY,
    email_addr character varying(300),
    name character varying(300),
    nickname character varying(300),
    last_sent_to timestamp with time zone,
    last_recv_from timestamp with time zone,
    notes text,
    invalid integer DEFAULT 0,
    recv_pri integer DEFAULT 0,
    nb_sent_to integer,
    nb_recv_from integer,
    CONSTRAINT addresses_email_addr_key UNIQUE (email_addr)
);

CREATE TRIGGER autoincrement_addresses AFTER INSERT ON addresses 
  BEGIN
    UPDATE addresses SET addr_id=(select curr_val+1 from Sequence where name='seq_addr_id') where addr_id is null;
    UPDATE Sequence SET curr_val=curr_val+1 where             		 name='seq_addr_id';
  END;

/*
ALTER TABLE ONLY addresses
    ADD CONSTRAINT addresses_email_addr_key UNIQUE (email_addr);
*/

CREATE TABLE attachment_contents (
    attachment_id integer,
    content oid,
    fingerprint text
);

CREATE TABLE attachments (
    attachment_id integer PRIMARY KEY, --DEFAULT nextval('seq_attachment_id'::regclass) NOT NULL,
    mail_id integer,
    content_type character varying(300),
    content_size integer,
    filename character varying(300),
    charset character varying(30),
    mime_content_id text
);

CREATE TRIGGER autoincrement_attachments AFTER INSERT ON attachments 
       BEGIN 
         	UPDATE attachments SET attachment_id=(select curr_val+1 from Sequence where name='seq_attachment_id') where attachment_id is null; 
        	UPDATE Sequence SET curr_val=curr_val+1 where           		    name='seq_attachment_id'; 
       END;

CREATE TABLE body (
    mail_id integer,
    bodytext text,
    bodyhtml text
);


CREATE TABLE config (
    conf_key character varying(100) NOT NULL,
    value text,
    conf_name character varying(100),
    date_update timestamp with time zone
);



CREATE TABLE files (
    mail_id integer,
    filename character varying(300)
);



CREATE TABLE filter_action (
    expr_id integer,
    action_order smallint,
    action_arg text,
    action_type character varying(100)
);


CREATE TABLE filter_expr (
    expr_id integer PRIMARY KEY NOT NULL,
    name character varying(100),
    user_lastmod integer,
    last_update timestamp with time zone DEFAULT CURRENT_TIMESTAMP,
    expression text,
    direction character(1) DEFAULT 'I'
);

CREATE TABLE filter_log (
    expr_id integer,
    mail_id integer,
    hit_date timestamp with time zone DEFAULT CURRENT_TIMESTAMP
);


CREATE TABLE forward_addresses (
    to_email_addr character varying(300),
    forward_to text
);


CREATE TABLE global_notepad (
    contents text,
    last_modified timestamp with time zone
);


CREATE TABLE header (
    mail_id integer,
    lines text
);


CREATE TABLE identities (
    email_addr character varying(200) NOT NULL,
    username character varying(200),
    xface text,
    signature text
);


CREATE TABLE inverted_word_index (
    word_id integer,
    part_no integer,
    mailvec bytea,
    nz_offset integer DEFAULT 0
);


CREATE TABLE jobs_queue (
    job_id integer PRIMARY KEY, --DEFAULT nextval('jobs_queue_job_id_seq'::regclass) NOT NULL,
    mail_id integer,
    job_type character varying(4),
    job_args text
);

CREATE TRIGGER autoincrement_jobs_queue AFTER INSERT ON jobs_queue 
       BEGIN 
         	UPDATE jobs_queue SET job_id=(select curr_val+1 from Sequence where name='jobs_queue_job_id_seq') where job_id is null; 
        	UPDATE Sequence SET curr_val=curr_val+1 where           		 name='jobs_queue_job_id_seq'; 
       END;

CREATE TABLE mail (
    mail_id integer PRIMARY KEY,
    sender text,
    toname text,
    sender_fullname text,
    subject text,
    msg_date timestamp with time zone DEFAULT CURRENT_TIMESTAMP,
    sender_date timestamp with time zone,
    mbox_id integer,
    status integer,
    mod_user_id integer,
    thread_id integer,
    message_id text,
    in_reply_to integer,
    date_processed timestamp with time zone,
    priority integer DEFAULT 0,
    flags integer DEFAULT 0
);

CREATE TRIGGER autoincrement_mail AFTER INSERT ON mail 
       BEGIN 
         	UPDATE mail SET mail_id=(select curr_val+1 from Sequence where name='seq_mail_id') where mail_id is null; 
        	UPDATE Sequence SET curr_val=curr_val+1 where           		 name='seq_mail_id'; 
       END;


CREATE TABLE mail_addresses (
    mail_id integer,
    addr_id integer,
    addr_type smallint,
    addr_pos smallint
);


CREATE TABLE mail_status (
    mail_id integer,
    status integer
);


CREATE TABLE mail_tags (
    mail_id integer,
    tag integer,
    agent integer,
    date_insert timestamp with time zone DEFAULT CURRENT_TIMESTAMP
);


CREATE TABLE mailboxes (
    mbox_id integer,
    name text
);


CREATE TABLE mime_types (
    suffix character varying(20) NOT NULL,
    mime_type character varying(100) NOT NULL
);


CREATE TABLE non_indexable_words (
    wordtext character varying(50)
);


CREATE TABLE notes (
    mail_id integer,
    note text,
    last_changed timestamp with time zone DEFAULT CURRENT_TIMESTAMP
);


CREATE TABLE programs (
    program_name character varying(256),
    content_type character varying(256),
    conf_name character varying(100)
);

CREATE TABLE raw_mail (
    mail_id integer,
    mail_text oid
);


CREATE TABLE runtime_info (
    rt_key character varying(100) NOT NULL,
    rt_value text
);


CREATE TABLE tags (
    tag_id integer PRIMARY KEY, --DEFAULT nextval('seq_tag_id'::regclass),
    name character varying(300),
    parent_id integer
);

CREATE TRIGGER autoincrement_tags AFTER INSERT ON tags 
       BEGIN 
         	UPDATE tags SET tag_id=(select curr_val+1 from Sequence where name='seq_tag_id') where tag_id is null; 
        	UPDATE Sequence SET curr_val=curr_val+1 where           		 name='seq_tag_id'; 
       END;

CREATE TABLE tags_words (
    tag_id integer,
    word_id integer,
    counter integer
);



CREATE TABLE user_queries (
    title text NOT NULL,
    sql_stmt text
);


CREATE TABLE users (
    user_id integer PRIMARY KEY NOT NULL,
    fullname character varying(300),
    login character varying(80),
    email text,
    custom_field1 text,
    custom_field2 text,
    custom_field3 text,
    current_user text,	-- TODO: ХАК! Проверить и желательно исправить
    CONSTRAINT users_user_id_check CHECK ((user_id > 0))
);


CREATE TABLE words (
    word_id integer PRIMARY KEY NOT NULL,
    wordtext character varying(50)
);

------------------------------------------------------------------------- Закончили с таблицами






------------------------------------------------------------------------- Создание функций и Тригеров.
-- TODO: их нужно распихать по коду, всё что нельзя оставить тут.


--\/--------------------------------- ТРИГЕРЫ ---------------------------------------\/--


-- TODO: SQLite3 этот тригер проглатывает, но нужно проверить работоспособность.
CREATE TRIGGER delete_mail
    AFTER DELETE ON mail
    FOR EACH ROW
    BEGIN
        DELETE FROM mail_status WHERE mail_id=OLD.mail_id;
    END;




/*
CREATE FUNCTION insert_mail() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF NEW.status&(256+32+16)=0 THEN
	  -- The message is not yet sent, archived, or trashed
	  INSERT INTO mail_status(mail_id,status) VALUES(new.mail_id,new.status);
	END IF;
	RETURN new;
END;
$$;

CREATE TRIGGER insert_mail
    AFTER INSERT ON mail
    FOR EACH ROW
    EXECUTE PROCEDURE insert_mail();




CREATE FUNCTION update_note_flag() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
  IF (TG_OP = 'DELETE') THEN
    UPDATE mail SET flags=flags&(~2) WHERE mail_id=OLD.mail_id;
    RETURN OLD;
  ELSIF (TG_OP = 'INSERT') THEN
    UPDATE mail SET flags=flags|2 WHERE mail_id=NEW.mail_id;
    RETURN NEW;
  END IF;
END;
$$;


CREATE TRIGGER update_note
    AFTER INSERT OR DELETE ON notes
    FOR EACH ROW
    EXECUTE PROCEDURE update_note_flag();




CREATE FUNCTION update_mail() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
 rc int4;
BEGIN
   IF new.status!=old.status THEN
	IF NEW.status&(256+32+16)=0 THEN
	  -- The message is not yet sent, archived, or trashed
	  UPDATE mail_status
	    SET status = new.status
	   WHERE mail_id = new.mail_id;
	  GET DIAGNOSTICS rc = ROW_COUNT;
	  if rc=0 THEN
	    INSERT INTO mail_status(mail_id,status) VALUES(new.mail_id,new.status);
	  END IF;
	ELSE
	  -- The mail has been "processed"
	  DELETE FROM mail_status
	   WHERE mail_id = new.mail_id;
	END IF;
   END IF;
   RETURN new;
END;
$$;

CREATE TRIGGER update_mail
    AFTER UPDATE ON mail
    FOR EACH ROW
    EXECUTE PROCEDURE update_mail();
*/

--/\---------------------------------- ТРИГЕРЫ ---------------------------------------/\--


--\/---------------------------------- Хранимые процедуры ---------------------------------------\/--

-- TODO эта функция используется в delete_msg_set
/*
CREATE FUNCTION delete_msg(integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
	id ALIAS FOR $1;
	attch RECORD;
	cnt integer;
	o oid;
BEGIN
  DELETE FROM notes WHERE mail_id=id;
  DELETE FROM mail_addresses WHERE mail_id=id;
  DELETE FROM header WHERE mail_id=id;
  DELETE FROM body WHERE mail_id=id;
  DELETE FROM mail_tags WHERE mail_id=id;

  FOR attch IN SELECT a.attachment_id,c.content,c.fingerprint FROM attachments a, attachment_contents c WHERE a.mail_id=id AND c.attachment_id=a.attachment_id LOOP
    cnt=0;
    IF attch.fingerprint IS NOT NULL THEN
      -- check if that content is shared with another message's attachment
      SELECT count(*) INTO cnt FROM attachment_contents WHERE fingerprint=attch.fingerprint AND attachment_id!=attch.attachment_id;
    END IF;
    IF (cnt=0) THEN
      PERFORM lo_unlink(attch.content);
    END IF;
    DELETE FROM attachment_contents WHERE attachment_id=attch.attachment_id;
  END LOOP;

  DELETE FROM attachments WHERE mail_id=id;
  UPDATE mail SET in_reply_to=NULL WHERE in_reply_to=id;

  SELECT mail_text INTO o FROM raw_mail WHERE mail_id=id;
  IF FOUND THEN
     PERFORM lo_unlink(o);
     DELETE FROM raw_mail WHERE mail_id=id;
  END IF;

  DELETE FROM mail WHERE mail_id=id;
  IF (FOUND) THEN
	  RETURN 1;
  ELSE
	  RETURN 0;
  END IF;
END;
$_$;

CREATE FUNCTION delete_msg_set(in_array_mail_id integer[]) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
 cnt int;
BEGIN
 cnt:=0;
 FOR idx IN array_lower(in_array_mail_id,1)..array_upper(in_array_mail_id,1) LOOP
   cnt:=cnt + delete_msg(in_array_mail_id[idx]);
 END LOOP;
 RETURN cnt;
END;
$$;


--
-- Name: trash_msg(integer, integer); Type: FUNCTION; Schema: public; Owner: ivan
--

CREATE FUNCTION trash_msg(in_mail_id integer, in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
new_status int;
BEGIN
  UPDATE mail SET status=status|16,mod_user_id=in_op WHERE mail_id=in_mail_id;
  SELECT INTO new_status status FROM mail WHERE mail_id=in_mail_id;
  RETURN new_status;
END;
$$;

CREATE FUNCTION trash_msg_set(in_array_mail_id integer[], in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
cnt int;
BEGIN
  UPDATE mail SET status=status|16, mod_user_id=in_op WHERE mail_id=any(in_array_mail_id);
  GET DIAGNOSTICS cnt=ROW_COUNT;
  return cnt;
END;
$$;

CREATE FUNCTION untrash_msg(in_mail_id integer, in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
new_status int;
BEGIN
  UPDATE mail SET status=status&(~16),mod_user_id=in_op WHERE mail_id=in_mail_id;
  SELECT INTO new_status status FROM mail WHERE mail_id=in_mail_id;
  RETURN new_status;
END;
$$;
*/
--/\---------------------------------- Хранимые процедуры ---------------------------------------/\--


------------------------------------------------------------------------- Закончили с функциями











-------------------------------- Остальная настройка 

/*
--
-- Name: addresses_email_addr_key; Type: CONSTRAINT; Schema: public; Owner: ivan; Tablespace: 
--




--
-- Name: attachment_contents_attachment_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY attachment_contents
    ADD CONSTRAINT attachment_contents_attachment_id_fkey FOREIGN KEY (attachment_id) REFERENCES attachments(attachment_id);


--
-- Name: attachments_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY attachments
    ADD CONSTRAINT attachments_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: body_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY body
    ADD CONSTRAINT body_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: filter_action_expr_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY filter_action
    ADD CONSTRAINT filter_action_expr_id_fkey FOREIGN KEY (expr_id) REFERENCES filter_expr(expr_id);


--
-- Name: header_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY header
    ADD CONSTRAINT header_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: inverted_word_index_word_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY inverted_word_index
    ADD CONSTRAINT inverted_word_index_word_id_fkey FOREIGN KEY (word_id) REFERENCES words(word_id);


--
-- Name: mail_addresses_addr_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY mail_addresses
    ADD CONSTRAINT mail_addresses_addr_id_fkey FOREIGN KEY (addr_id) REFERENCES addresses(addr_id);


--
-- Name: mail_addresses_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY mail_addresses
    ADD CONSTRAINT mail_addresses_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: mail_mbox_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY mail
    ADD CONSTRAINT mail_mbox_id_fkey FOREIGN KEY (mbox_id) REFERENCES mailboxes(mbox_id);


--
-- Name: mail_tags_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY mail_tags
    ADD CONSTRAINT mail_tags_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: mail_tags_tag_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY mail_tags
    ADD CONSTRAINT mail_tags_tag_fkey FOREIGN KEY (tag) REFERENCES tags(tag_id);


--
-- Name: notes_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY notes
    ADD CONSTRAINT notes_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: parent_tag_fk; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY tags
    ADD CONSTRAINT parent_tag_fk FOREIGN KEY (parent_id) REFERENCES tags(tag_id);


--
-- Name: raw_mail_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY raw_mail
    ADD CONSTRAINT raw_mail_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: tags_words_tag_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY tags_words
    ADD CONSTRAINT tags_words_tag_id_fkey FOREIGN KEY (tag_id) REFERENCES tags(tag_id);


--
-- Name: tags_words_word_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: ivan
--

ALTER TABLE ONLY tags_words
    ADD CONSTRAINT tags_words_word_id_fkey FOREIGN KEY (word_id) REFERENCES words(word_id);




------------------------- Создание индекса. TODO по идее не надо, но нужно уточнить .

CREATE INDEX attach_ct_fp_idx ON attachment_contents USING btree (fingerprint);


CREATE UNIQUE INDEX attch_ct_idx ON attachment_contents USING btree (attachment_id);


CREATE UNIQUE INDEX expr_idx ON filter_expr USING btree (name);


CREATE UNIQUE INDEX filter_action_idx ON filter_action USING btree (expr_id, action_order);


CREATE UNIQUE INDEX fwa_idx ON forward_addresses USING btree (to_email_addr);


CREATE INDEX idx_attachments_mail_id ON attachments USING btree (mail_id);


CREATE UNIQUE INDEX idx_raw_mail ON raw_mail USING btree (mail_id);


CREATE UNIQUE INDEX iwi_idx ON inverted_word_index USING btree (word_id, part_no);


CREATE UNIQUE INDEX jobs_pk_idx ON jobs_queue USING btree (job_id);


CREATE INDEX mail_addresses_addrid_idx ON mail_addresses USING btree (addr_id);


CREATE INDEX mail_addresses_mailid_idx ON mail_addresses USING btree (mail_id);


CREATE INDEX mail_date_idx ON mail USING btree (msg_date);


CREATE INDEX mail_in_replyto_idx ON mail USING btree (in_reply_to);


CREATE INDEX mail_message_id_idx ON mail USING btree (message_id);


CREATE UNIQUE INDEX mail_tags_idx ON mail_tags USING btree (mail_id, tag);


CREATE INDEX mail_thread_idx ON mail USING btree (thread_id);


CREATE UNIQUE INDEX mailboxes_mbox_idx ON mailboxes USING btree (mbox_id);


CREATE UNIQUE INDEX mailboxes_mbox_name ON mailboxes USING btree (name);


CREATE UNIQUE INDEX notes_idx ON notes USING btree (mail_id);


CREATE UNIQUE INDEX pk_body_idx ON body USING btree (mail_id);


CREATE UNIQUE INDEX pk_header_idx ON header USING btree (mail_id);


CREATE UNIQUE INDEX pk_mail_idx ON mail USING btree (mail_id);


CREATE UNIQUE INDEX pk_mail_status_idx ON mail_status USING btree (mail_id);


CREATE UNIQUE INDEX runtime_info_pk ON runtime_info USING btree (rt_key);


CREATE UNIQUE INDEX tag_id_pk ON tags USING btree (tag_id);


CREATE INDEX tags_words_idx ON tags_words USING btree (word_id);


CREATE UNIQUE INDEX user_queries_idx ON user_queries USING btree (title);


CREATE UNIQUE INDEX users_login_idx ON users USING btree (login);


CREATE UNIQUE INDEX wordtext_idx ON words USING btree (wordtext);

*/

